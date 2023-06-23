#pragma once
#pragma comment(lib,"RmlCore.lib")

#include "pch.h"
#include "RmlUi/Core/RenderInterface.h"
#include "../../DirectXTK-main/Inc/BufferHelpers.h"

using namespace Rml;


namespace YX {
	class DX11RmlRenderer : public Rml::RenderInterface
	{
	public:
		DX11RmlRenderer(ID3D11Device* pDev, ID3D11DeviceContext* pCtx);
		virtual void RenderGeometry(Vertex* vertices, int num_vertices, int* indices, int num_indices, TextureHandle texture, const Vector2f& translation);
		virtual CompiledGeometryHandle CompileGeometry(Vertex* vertices, int num_vertices, int* indices, int num_indices, TextureHandle texture);
		virtual void RenderCompiledGeometry(CompiledGeometryHandle geometry, const Vector2f& translation);
		virtual void ReleaseCompiledGeometry(CompiledGeometryHandle geometry);
		virtual void EnableScissorRegion(bool enable);
		virtual void SetScissorRegion(int x, int y, int width, int height);
		virtual bool LoadTexture(TextureHandle& texture_handle, Vector2i& texture_dimensions, const String& source);
		virtual bool GenerateTexture(TextureHandle& texture_handle, const byte* source, const Vector2i& source_dimensions);
		virtual void ReleaseTexture(TextureHandle texture);
		virtual void SetTransform(const Matrix4f* transform);
	protected:
		struct GeometryData
		{
		public:
			Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
			unsigned int vertexStride;
			unsigned int vertexCount;

			Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
			unsigned int indexCount;
			DXGI_FORMAT indexFormat;

			Rml::TextureHandle textureHandle;

			GeometryData(ID3D11Device* device,const Vertex* vertices, size_t verticesSize, int* indices, size_t indicesSize, Rml::TextureHandle texture)
				: vertexBuffer(nullptr), vertexStride(0), vertexCount(0), textureHandle(texture), indexBuffer(nullptr), indexCount(0), indexFormat(DXGI_FORMAT::DXGI_FORMAT_UNKNOWN)
			{
				DirectX::CreateStaticBuffer(device, (const char*)vertices, verticesSize, sizeof(Vertex), D3D11_BIND_VERTEX_BUFFER, vertexBuffer.GetAddressOf());
				vertexStride = sizeof(Vertex);
				vertexCount = verticesSize;

				DirectX::CreateStaticBuffer(device, (const char*)indices, indicesSize, sizeof(int), D3D11_BIND_INDEX_BUFFER, indexBuffer.GetAddressOf());
				indexCount = indicesSize;
				indexFormat = DXGI_FORMAT_R32_UINT;
			}
		};
	private:
		ID3D11Device* _pDev;
		ID3D11DeviceContext* _pCtx;
	};
}