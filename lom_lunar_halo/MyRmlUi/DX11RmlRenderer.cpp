#include "pch.h"
#include "DX11RmlRenderer.h"

using namespace YX;

DX11RmlRenderer::DX11RmlRenderer(ID3D11Device* pDev, ID3D11DeviceContext* pCtx)
{
	_pDev = pDev;
	_pCtx = pCtx;
}
void DX11RmlRenderer::RenderGeometry(Vertex* vertices, int num_vertices, int* indices, int num_indices, TextureHandle texture, const Vector2f& translation)
{
	// 使用RenderCompiledGeometry绘制，于是这里不做实现
}
CompiledGeometryHandle DX11RmlRenderer::CompileGeometry(Vertex* vertices, int num_vertices, int* indices, int num_indices, TextureHandle texture)
{
	return 0;
}
void DX11RmlRenderer::RenderCompiledGeometry(CompiledGeometryHandle geometry, const Vector2f& translation)
{}
void DX11RmlRenderer::ReleaseCompiledGeometry(CompiledGeometryHandle geometry)
{}
void DX11RmlRenderer::EnableScissorRegion(bool enable)
{}
void DX11RmlRenderer::SetScissorRegion(int x, int y, int width, int height)
{}
bool DX11RmlRenderer::LoadTexture(TextureHandle& texture_handle, Vector2i& texture_dimensions, const String& source)
{
	return true;
}
bool DX11RmlRenderer::GenerateTexture(TextureHandle& texture_handle, const byte* source, const Vector2i& source_dimensions)
{
	return true;
}
void DX11RmlRenderer::ReleaseTexture(TextureHandle texture)
{}
void DX11RmlRenderer::SetTransform(const Matrix4f* transform)
{}