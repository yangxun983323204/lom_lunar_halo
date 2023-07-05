// 关于RenderInterface的约定可在下面的网站查阅
// https://mikke89.github.io/RmlUiDoc/pages/cpp_manual/interfaces/render.html
#include "pch.h"
#include "DX11RmlRenderer.h"
#include <string>
#include <d3d11.h>
#include <d3dcompiler.h>

using namespace YX;
using std::string;

namespace {
	// hlsl shader
	const string SHADER_STRING = R"(
Texture2D tex : register( t0 );
SamplerState samLinear : register( s0 );

cbuffer cbNeverChanges : register( b0 )
{
    matrix VP;
};

cbuffer cbPerObject : register( b1 )
{
    matrix W;
};

struct VS_INPUT
{
	float2 position : POSITION;
	float4 color	: COLOR;
	float2 uv		: TEXCOORD0;
};

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float4 color	: COLOR;
	float2 uv		: TEXCOORD0;
};

PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;
	output.position = mul(float4(input.position,0,1), W);
	output.position = mul(output.position, VP);
	output.color = input.color;
	output.uv = input.uv;

	return output;
}

float4 PS(PS_INPUT input) : SV_Target
{
	return tex.Sample(samLinear, input.uv) * input.color;
}
)";
}

DX11RmlRenderer::DX11RmlRenderer(ID3D11Device* pDev, ID3D11DeviceContext* pCtx):
	_transform{},
	_pDev{pDev}, _pCtx{pCtx},
	_vs{}, _ps{}, _inputLayout{},
	_state{}, _scissorState{}, _depthState{}, _blendState{},
	_flip{}, _world{}, _viewProj{}, _worldCB{ pDev }, _viewProjCB{ pDev },
	_geometryIdGen{},
	_geometries{},
	_loadTexFunc{}, _genTexFunc{}, _releaseTexFunc{}
{
	SetupShader();
	SetupState();
	SetupShaderRes();
}
YX::DX11RmlRenderer::~DX11RmlRenderer()
{
}
void YX::DX11RmlRenderer::SetWindowSize(uint32_t width, uint32_t height)
{
	auto view = DirectX::SimpleMath::Matrix::CreateLookAt({ 0,0,-1 }, { 0,0,0 }, { 0,1,0 });
	auto proj = DirectX::SimpleMath::Matrix::CreateOrthographic((float)width, (float)height, 0.1, 10);
	_viewProj = view * proj;
	_flip = DirectX::SimpleMath::Matrix::CreateTranslation({ -(float)width / 2, -(float)height / 2, 0 }) *
		DirectX::SimpleMath::Matrix::CreateScale({ 1,-1,1 });
	_flip.Invert();
	_viewProjCB.SetData(_pCtx, XMMatrixTranspose(_viewProj));
}
void YX::DX11RmlRenderer::OnBeginRender()
{
	_pCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_pCtx->IASetInputLayout(_inputLayout.Get());
	_pCtx->VSSetShader(_vs.Get(), 0, 0);
	_pCtx->PSSetShader(_ps.Get(), 0, 0);
	_pCtx->OMSetBlendState(_blendState.Get(), 0, 0xffffffff);
	_pCtx->OMSetDepthStencilState(_depthState.Get(), 0);
	_pCtx->PSSetSamplers(0, 1, _sampler.GetAddressOf());

	auto vpCb = _viewProjCB.GetBuffer();
	_pCtx->VSSetConstantBuffers(0, 1, &vpCb);
}
void YX::DX11RmlRenderer::OnEndRender()
{
}
void DX11RmlRenderer::RenderGeometry(Rml::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rml::TextureHandle texture, const Rml::Vector2f& translation)
{
	// 使用RenderCompiledGeometry绘制，于是这里不做实现
}
Rml::CompiledGeometryHandle DX11RmlRenderer::CompileGeometry(Rml::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rml::TextureHandle texture)
{
	GeometryData geometry = { _pDev, vertices, num_vertices, indices, num_indices, texture };
	++_geometryIdGen;
	if (_geometryIdGen == 0)
		_geometryIdGen = 1;

	auto handle = _geometryIdGen;
	_geometries.insert({ handle,geometry });
	return handle;
}
void DX11RmlRenderer::RenderCompiledGeometry(Rml::CompiledGeometryHandle geometry, const Rml::Vector2f& translation)
{
	auto g = _geometries[geometry];
	_pCtx->IASetVertexBuffers(0, 0, g.vertexBuffer.GetAddressOf(), &g.vertexStride, 0);
	_pCtx->IASetIndexBuffer(g.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	// set texture,sampler,constbuffer
	if (_getTexFunc)
	{
		auto texSRV = _getTexFunc(g.textureHandle);
		if(texSRV)
			_pCtx->PSSetShaderResources(0, 0, texSRV);
	}
	// 更新cbuffer
	_world = _flip * DirectX::SimpleMath::Matrix::CreateTranslation(translation.x, translation.y, 0);
	_worldCB.SetData(_pCtx, XMMatrixTranspose(_world));// cpu是行主序，但是gpu是列主序。。。
	_viewProjCB.SetData(_pCtx, XMMatrixTranspose(_viewProj));
	auto wCb = _worldCB.GetBuffer();
	_pCtx->VSSetConstantBuffers(1, 1, &wCb);
	//
	_pCtx->DrawIndexed(g.indexCount, 0, 0);
}
void DX11RmlRenderer::ReleaseCompiledGeometry(Rml::CompiledGeometryHandle geometry)
{
	auto s = _geometries.find(geometry);
	if (s != _geometries.end())
	{
		_geometries.erase(s);
	}
}
void DX11RmlRenderer::EnableScissorRegion(bool enable)
{
	_pCtx->RSSetState(enable ? _scissorState.Get() : _state.Get());
}
void DX11RmlRenderer::SetScissorRegion(int x, int y, int width, int height)
{
	D3D11_RECT rect;
	rect.left = x;
	rect.right = x + width;
	rect.top = y;
	rect.bottom = y + height;
	_pCtx->RSSetScissorRects(1, &rect);
}
bool DX11RmlRenderer::LoadTexture(Rml::TextureHandle& texture_handle, Rml::Vector2i& texture_dimensions, const Rml::String& source)
{
	if (!_loadTexFunc)
		return false;

	return _loadTexFunc(texture_handle, texture_dimensions, source);
}
bool DX11RmlRenderer::GenerateTexture(Rml::TextureHandle& texture_handle, const byte* source, const Rml::Vector2i& source_dimensions)
{
	if (!_genTexFunc)
		return false;

	return _genTexFunc(texture_handle, source, source_dimensions);
}
void DX11RmlRenderer::ReleaseTexture(Rml::TextureHandle texture)
{
	if (!_releaseTexFunc)
		return;

	_releaseTexFunc(texture);
}
void DX11RmlRenderer::SetTransform(const Rml::Matrix4f* transform)
{
	if (transform == nullptr)
	{
		_transform = DirectX::SimpleMath::Matrix();
	}
	else {
		_transform = DirectX::SimpleMath::Matrix(transform->data());
		_transform.Transpose();// rml是列主序，dx是行主序，因此转置
	}
}

void YX::DX11RmlRenderer::SetupShader()
{
	{
		ComPtr<ID3DBlob> vsBlob = nullptr;
		auto hr = CompileShaderFromData(SHADER_STRING.c_str(), SHADER_STRING.size(), "VS", "vs_4_0", vsBlob.GetAddressOf());
		if (FAILED(hr))
		{
			MessageBox(nullptr,
				L"The SHADER_STRING VS cannot be compiled.", L"Error", MB_OK);
			return;
		}

		hr = _pDev->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, _vs.GetAddressOf());
		if (FAILED(hr))
			return;

		D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,		0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"COLOR",	 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 8,	D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		0, 20,	D3D11_INPUT_PER_VERTEX_DATA, 0},
		};

		_pDev->CreateInputLayout(inputDesc, 3, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), _inputLayout.GetAddressOf());
		if (FAILED(hr))
			return; 
	}

	{
		ComPtr<ID3DBlob> psBlob = nullptr;
		auto hr = CompileShaderFromData(SHADER_STRING.c_str(), SHADER_STRING.size(), "PS", "ps_4_0", psBlob.GetAddressOf());
		if (FAILED(hr))
		{
			MessageBox(nullptr,
				L"The SHADER_STRING PS cannot be compiled.", L"Error", MB_OK);
			return;
		}

		hr = _pDev->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, _ps.GetAddressOf());
		if (FAILED(hr))
			return;
	}
}

void YX::DX11RmlRenderer::SetupState()
{
	D3D11_BLEND_DESC blendDesc{};
	ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	_pDev->CreateBlendState(&blendDesc, _blendState.GetAddressOf());

	D3D11_DEPTH_STENCIL_DESC dsDesc{};
	dsDesc.DepthEnable = false;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;
	dsDesc.StencilEnable = false;
	dsDesc.StencilReadMask = 0;
	dsDesc.StencilWriteMask = 0;
	dsDesc.FrontFace = {};
	dsDesc.BackFace = {};
	_pDev->CreateDepthStencilState(&dsDesc, _depthState.GetAddressOf());

	D3D11_RASTERIZER_DESC rsDesc{};
	rsDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	rsDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
	rsDesc.FrontCounterClockwise = true;
	rsDesc.DepthClipEnable = true;
	rsDesc.ScissorEnable = false;
	rsDesc.MultisampleEnable = true;

	D3D11_RASTERIZER_DESC rsScissorDesc = rsDesc;
	rsDesc.ScissorEnable = true;

	_pDev->CreateRasterizerState(&rsDesc, _state.GetAddressOf());
	_pDev->CreateRasterizerState(&rsScissorDesc, _scissorState.GetAddressOf());
}

void YX::DX11RmlRenderer::SetupShaderRes()
{
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	auto hr = _pDev->CreateSamplerState(&sampDesc, _sampler.GetAddressOf());
	if (FAILED(hr))
		return;
}

HRESULT YX::DX11RmlRenderer::CompileShaderFromData(const void* data, int size, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	dwShaderFlags |= D3DCOMPILE_DEBUG;
	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	ID3DBlob* pErrorBlob = nullptr;
	hr = D3DCompile(data, size, nullptr, nullptr, nullptr, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
	if (FAILED(hr))
	{
		if (pErrorBlob)
		{
			OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
			pErrorBlob->Release();
		}
		return hr;
	}
	if (pErrorBlob) pErrorBlob->Release();

	return S_OK;
}