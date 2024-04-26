#pragma once
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"RmlCore.lib")

#include "pch.h"
#include <memory>
#include <functional>
#include <unordered_map>
#include <wrl.h>
#include "RmlUi/Core/RenderInterface.h"
#include "../DirectXTK-main/Inc/BufferHelpers.h"
#include "../DirectXTK-main/Inc/SimpleMath.h"

using Microsoft::WRL::ComPtr;
using std::unordered_map;
using std::shared_ptr;
using std::function;

class SpriteResHandle;

namespace YX {
	// 加载图片功能强相关于应用资源管理，因此核心实现中开放外部注入实现
	
	// 从文件创建纹理
	typedef function<bool(Rml::TextureHandle& texture_handle, Rml::Vector2i& texture_dimensions, const Rml::String& source)> LoadTextureFunc;
	// 从像素创建纹理
	typedef function<bool(Rml::TextureHandle& texture_handle, const byte* source, const Rml::Vector2i& source_dimensions)> GenTextureFunc;
	typedef function<void(Rml::TextureHandle texture)> ReleaseTextureFunc;
	// 通过TextureHandle获取对应的SRV**
	typedef function<ID3D11ShaderResourceView**(Rml::TextureHandle& texture_handle)> GetTextureFunc;

	class DX11RmlRenderer : public Rml::RenderInterface
	{
	public:
		DX11RmlRenderer(ID3D11Device* pDev, ID3D11DeviceContext* pCtx);
		virtual ~DX11RmlRenderer() override;

		void SetWindowSize(uint32_t width, uint32_t height);
		inline void SetLoadTextureFunc(LoadTextureFunc func) { _loadTexFunc = func; }
		void SetGenTextureFunc(GenTextureFunc func);
		inline void SetReleaseTextureFunc(ReleaseTextureFunc func) { _releaseTexFunc = func; }
		inline void SetGetTextureFunc(GetTextureFunc func) { _getTexFunc = func; }
		//
		void OnBeginRender();
		void OnEndRender();
		// RenderInterface的虚函数实现
		virtual void RenderGeometry(Rml::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rml::TextureHandle texture, const Rml::Vector2f& translation) override;
		virtual Rml::CompiledGeometryHandle CompileGeometry(Rml::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rml::TextureHandle texture) override;
		virtual void RenderCompiledGeometry(Rml::CompiledGeometryHandle geometry, const Rml::Vector2f& translation) override;
		virtual void ReleaseCompiledGeometry(Rml::CompiledGeometryHandle geometry) override;
		virtual void EnableScissorRegion(bool enable) override;
		virtual void SetScissorRegion(int x, int y, int width, int height) override;
		virtual bool LoadTexture(Rml::TextureHandle& texture_handle, Rml::Vector2i& texture_dimensions, const Rml::String& source) override;
		virtual bool GenerateTexture(Rml::TextureHandle& texture_handle, const byte* source, const Rml::Vector2i& source_dimensions) override;
		virtual void ReleaseTexture(Rml::TextureHandle texture) override;
		virtual void SetTransform(const Rml::Matrix4f* transform) override;
	protected:
		class GeometryData
		{
		public:
			Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
			unsigned int vertexStride;
			unsigned int vertexCount;

			Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
			unsigned int indexCount;
			DXGI_FORMAT indexFormat;

			Rml::TextureHandle textureHandle;
			GeometryData():
				vertexBuffer{}, vertexStride(0), vertexCount(0),
				indexBuffer{}, indexCount(0), indexFormat(DXGI_FORMAT::DXGI_FORMAT_UNKNOWN),
				textureHandle{}
			{}
			GeometryData(ID3D11Device* device,const Rml::Vertex* vertices, int verticesSize, int* indices, int indicesSize, Rml::TextureHandle texture): 
				vertexBuffer(nullptr), vertexStride(0), vertexCount(verticesSize),
				indexBuffer(nullptr), indexCount(indicesSize), indexFormat(DXGI_FORMAT::DXGI_FORMAT_UNKNOWN),
				textureHandle(texture)
			{
				DirectX::CreateStaticBuffer(device, (const void*)vertices, verticesSize, sizeof(Rml::Vertex), D3D11_BIND_VERTEX_BUFFER, vertexBuffer.GetAddressOf());
				vertexStride = sizeof(Rml::Vertex);

				DirectX::CreateStaticBuffer(device, (const void*)indices, indicesSize, sizeof(int), D3D11_BIND_INDEX_BUFFER, indexBuffer.GetAddressOf());
				indexFormat = DXGI_FORMAT_R32_UINT;
			}
		};

	private:
		void SetupShader();
		void SetupState();
		void SetupShaderRes();

		HRESULT CompileShaderFromData(const void* data, int size, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);

		DirectX::SimpleMath::Matrix _transform;

		ID3D11Device* _pDev;
		ID3D11DeviceContext* _pCtx;
		ComPtr<ID3D11VertexShader> _vs;
		ComPtr<ID3D11PixelShader> _ps;
		ComPtr<ID3D11InputLayout> _inputLayout;
		ComPtr<ID3D11RasterizerState> _rsState;
		ComPtr<ID3D11RasterizerState> _srScissorState;
		ComPtr<ID3D11DepthStencilState> _depthState;
		ComPtr<ID3D11BlendState> _blendState;
		uint32_t _width;
		uint32_t _height;
		
		DirectX::SimpleMath::Matrix _world;
		DirectX::SimpleMath::Matrix _viewProj;
		ComPtr<ID3D11SamplerState> _sampler;
		DirectX::ConstantBuffer<DirectX::XMMATRIX> _worldCB;
		DirectX::ConstantBuffer<DirectX::XMMATRIX> _viewProjCB;

		Rml::TextureHandle _whiteTex;

		uint32_t _geometryIdGen;

		unordered_map<Rml::CompiledGeometryHandle, shared_ptr<GeometryData>> _geometries;

		LoadTextureFunc _loadTexFunc;
		GenTextureFunc _genTexFunc;
		ReleaseTextureFunc _releaseTexFunc;
		GetTextureFunc _getTexFunc;
	};
}