#include "SimpleSpriteDraw.h"
#include <d3dcompiler.h>
#include <string>

using std::string;
using DirectX::XMFLOAT2;
using DirectX::XMFLOAT4;
using DirectX::SimpleMath::Color;
using DirectX::SimpleMath::Matrix;

namespace {
	// hlsl shader
	const string SHADER_STRING = R"(
Texture2D tex : register( t0 );
SamplerState samLinear : register( s0 );

cbuffer cbNeverChanges : register( b0 )
{
    matrix Matrix;
	float4 ShadowColor;
};

cbuffer cbPerObjChanges : register( b1 )
{
	int4 ShadowProjOrign;
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
	output.position = mul(float4(input.position,0,1), Matrix);
	output.color = input.color;
	output.uv = input.uv;

	return output;
}

PS_INPUT VS_ShadowProj(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;
	float2 p2 = input.position - ShadowProjOrign.xy;
	p2.x = p2.x + p2.y * 0.5;
	p2.y = p2.y * 0.5;
    p2 = ShadowProjOrign.xy + p2;
	output.position = mul(float4(p2,0,1), Matrix);
	output.color = input.color;
	output.uv = input.uv;

	return output;
}

float4 PS(PS_INPUT input) : SV_Target
{
	return tex.Sample(samLinear, input.uv) * input.color;
}

float4 PS_Shadow(PS_INPUT input) : SV_Target
{
	float4 c = tex.Sample(samLinear, input.uv);
	c.xyz = ShadowColor.xyz;
	c.w = c.w * ShadowColor.w;
	return c;
}
)";
}

namespace {
	struct SpriteVertex
	{
		XMFLOAT2 position;
		XMFLOAT4 color;
		XMFLOAT2 uv;
	};

	SpriteVertex _vertices[4];
}

static constexpr size_t MaxBufferSize = 2048;

SimpleSpriteDraw::SimpleSpriteDraw(ID3D11DeviceContext* context):
	_ctx{ context }, _readIdx{ 0 }, _writeIdx{ 0 }, _mode{DrawMode::Normal}
{
	_ctx->GetDevice(&_dev);
	_states = std::make_unique<CommonStates>(_dev);
	_cbuffer = DirectX::ConstantBuffer<CBNeverChanges>{ _dev };
	_cbShadowProjOrign = DirectX::ConstantBuffer<DirectX::XMINT4>{ _dev };
	_cbData.ShadowColor = { 0,0,0,0.7 };
	_dpiScale = 1;
	Init();
}

void SimpleSpriteDraw::Begin(DirectX::XMINT2 viewPoint, float dpiScale)
{
	_dpiScale = dpiScale;
	_ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_ctx->IASetInputLayout(_inputLayout.Get());
	_ctx->VSSetShader(_vs.Get(), nullptr, 0);
	_ctx->PSSetShader(_ps.Get(), nullptr, 0); 
	_mode = DrawMode::Normal;
	_ctx->RSSetState(_states->CullCounterClockwise());
	_ctx->OMSetBlendState(_states->AlphaBlend(), 0, 0xffffffff);
	_ctx->OMSetDepthStencilState(_states->DepthNone(), 0);
	auto sampler = _states->LinearClamp();
	_ctx->PSSetSamplers(0, 1, &sampler);

	D3D11_VIEWPORT vp;
	uint32_t i = 1;
	_ctx->RSGetViewports(&i, &vp);
	_width = vp.Width;
	_height = vp.Height;
	auto _halfW = _width / 2.0;
	auto _halfH = _height / 2.0;
	auto toNDC = Matrix::CreateTranslation(-viewPoint.x, -viewPoint.y, 0) *
		Matrix::CreateScale(1.0 / _halfW * _dpiScale, 1.0 / _halfH * _dpiScale, 1);

	_cbData.Matrix = toNDC.Transpose();
	_cbuffer.SetData(_ctx, _cbData);
	auto cb = _cbuffer.GetBuffer();
	_ctx->VSSetConstantBuffers(0, 1, &cb);
	_ctx->PSSetConstantBuffers(0, 1, &cb);
	_ctx->IASetIndexBuffer(_ib.Get(), DXGI_FORMAT_R32_UINT, 0);
}

void SimpleSpriteDraw::End()
{
}

void SimpleSpriteDraw::Draw(ID3D11ShaderResourceView* texture, DirectX::SimpleMath::Rectangle const& rect, DirectX::XMFLOAT4 color)
{
	_ctx->PSSetShaderResources(0, 1, &texture);
	UpdateVB(rect, color);
	_ctx->DrawIndexed(6, 0, _readIdx * 4);
}

void SimpleSpriteDraw::SetModeNormal()
{
	if (_mode == DrawMode::Normal) return;

	_mode = DrawMode::Normal;
	_ctx->VSSetShader(_vs.Get(), nullptr, 0);
	_ctx->PSSetShader(_ps.Get(), nullptr, 0);
}

void SimpleSpriteDraw::SetModeShadowOrth()
{
	if (_mode == DrawMode::ShadowOrth) return;

	_mode = DrawMode::ShadowOrth;
	_ctx->VSSetShader(_vs.Get(), nullptr, 0);
	_ctx->PSSetShader(_psShadow.Get(), nullptr, 0);
}

void SimpleSpriteDraw::SetModeShadowProj()
{
	if (_mode == DrawMode::ShadowOrth) return;

	_mode = DrawMode::ShadowOrth;
	_ctx->VSSetShader(_vsShadowProj.Get(), nullptr, 0);
	_ctx->PSSetShader(_psShadow.Get(), nullptr, 0);
}

void SimpleSpriteDraw::SetShadowColor(DirectX::XMFLOAT4 color)
{
	_cbData.ShadowColor = color;
	_cbuffer.SetData(_ctx, _cbData);
}
void SimpleSpriteDraw::SetShadowProjOrigin(DirectX::XMINT2 origin)
{
	_cbShadowProjOrign.SetData(_ctx, { origin.x, origin.y,0,0 });
	auto cb = _cbShadowProjOrign.GetBuffer();
	_ctx->VSSetConstantBuffers(1, 1, &cb);
}

HRESULT SimpleSpriteDraw::CompileShaderFromData(const void* data, int size, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
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

void SimpleSpriteDraw::Init()
{
	{
		ComPtr<ID3DBlob> vsBlob = nullptr;
		auto hr = CompileShaderFromData(SHADER_STRING.c_str(), SHADER_STRING.size(), "VS", "vs_4_0", vsBlob.GetAddressOf());
		if (FAILED(hr))
		{
			MessageBox(nullptr, L"The SHADER_STRING VS cannot be compiled.", L"Error", MB_OK);
			return;
		}

		hr = _dev->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, _vs.GetAddressOf());
		if (FAILED(hr))
			return;

		D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,		0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"COLOR",	 0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 8,	D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		0, 24,	D3D11_INPUT_PER_VERTEX_DATA, 0},
		};

		_dev->CreateInputLayout(inputDesc, 3, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), _inputLayout.GetAddressOf());
	}

	{
		ComPtr<ID3DBlob> vsBlob = nullptr;
		auto hr = CompileShaderFromData(SHADER_STRING.c_str(), SHADER_STRING.size(), "VS_ShadowProj", "vs_4_0", vsBlob.GetAddressOf());
		if (FAILED(hr))
		{
			MessageBox(nullptr, L"The SHADER_STRING VS cannot be compiled.", L"Error", MB_OK);
			return;
		}

		hr = _dev->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, _vsShadowProj.GetAddressOf());
		if (FAILED(hr))
			return;
	}

	{
		ComPtr<ID3DBlob> psBlob = nullptr;
		auto hr = CompileShaderFromData(SHADER_STRING.c_str(), SHADER_STRING.size(), "PS", "ps_4_0", psBlob.GetAddressOf());
		if (FAILED(hr))
		{
			MessageBox(nullptr, L"The SHADER_STRING PS cannot be compiled.", L"Error", MB_OK);
			return;
		}

		hr = _dev->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, _ps.GetAddressOf());
	}

	{
		ComPtr<ID3DBlob> psBlob = nullptr;
		auto hr = CompileShaderFromData(SHADER_STRING.c_str(), SHADER_STRING.size(), "PS_Shadow", "ps_4_0", psBlob.GetAddressOf());
		if (FAILED(hr))
		{
			MessageBox(nullptr, L"The SHADER_STRING PS cannot be compiled.", L"Error", MB_OK);
			return;
		}

		hr = _dev->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, _psShadow.GetAddressOf());
	}

	_vertices[0] = { {0,0},{0,0,0,0},{0,1} };
	_vertices[1] = { {0,0},{0,0,0,0},{0,0} };
	_vertices[2] = { {0,0},{0,0,0,0},{1,0} };
	_vertices[3] = { {0,0},{0,0,0,0},{1,1} };

	D3D11_BUFFER_DESC vbs{0};
	vbs.ByteWidth = MaxBufferSize * 4 * sizeof(SpriteVertex);
	vbs.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	vbs.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	vbs.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
	vbs.StructureByteStride = sizeof(SpriteVertex);
	_dev->CreateBuffer(&vbs, 0, _vb.GetAddressOf());

	uint32_t ids[] = { 0,1,2,0,2,3 };
	DirectX::CreateStaticBuffer(_dev, ids, 6, sizeof(uint32_t), D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER, _ib.GetAddressOf());
}

void SimpleSpriteDraw::UpdateVB(DirectX::SimpleMath::Rectangle const& rect, DirectX::XMFLOAT4 color)
{
	float left = (float)rect.x;
	float bottom = (float)rect.y;
	float right = left + rect.width;
	float top = bottom + rect.height;
	
	_vertices[0].position = { left, bottom };
	_vertices[0].color = color;

	_vertices[1].position = { left, top };
	_vertices[1].color = color;

	_vertices[2].position = { right, top };
	_vertices[2].color = color;

	_vertices[3].position = { right, bottom };
	_vertices[3].color = color;

	D3D11_MAPPED_SUBRESOURCE mappedBuffer{0};
	D3D11_MAP mapType = (_writeIdx == 0) ? D3D11_MAP_WRITE_DISCARD : D3D11_MAP_WRITE_NO_OVERWRITE;
	_ctx->Map(_vb.Get(), 0, mapType, 0, &mappedBuffer);
	auto p = (SpriteVertex*)mappedBuffer.pData + _writeIdx * 4;
	memcpy(p, _vertices, sizeof(_vertices));
	_ctx->Unmap(_vb.Get(), 0);
	_readIdx = _writeIdx;
	_writeIdx++;
	if (_writeIdx >= MaxBufferSize)
		_writeIdx = 0;

	uint32_t i = sizeof(SpriteVertex);
	uint32_t offset = 0;
	_ctx->IASetVertexBuffers(0, 1, _vb.GetAddressOf(), &i, &offset);
}