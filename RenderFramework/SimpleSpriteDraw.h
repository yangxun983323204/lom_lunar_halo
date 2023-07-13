// 感觉用DXTK的SpriteBatch不满足人物阴影绘制的需求了，自行实现渲染
// 另外因为图都是分散的，也利用不到SpriteBatch的合批能力
#pragma once
#include <memory>
#include <d3d11.h>
#include <DirectXMath.h>
#include "../DirectXTK-main/Inc/CommonStates.h"
#include "../DirectXTK-main/Inc/SimpleMath.h"
#include "../DirectXTK-main/Inc/BufferHelpers.h"
#include <wrl.h>

using DirectX::FXMVECTOR;
using DirectX::CommonStates;
using Microsoft::WRL::ComPtr;

class SimpleSpriteDraw
{
public:
	SimpleSpriteDraw(ID3D11DeviceContext* context);
	void Begin();
	void End();
	void Draw(ID3D11ShaderResourceView* texture, RECT const& dstRect, FXMVECTOR color);
	void SetModeNormal();
	void SetModeShadow();
	void SetShadowOffset(int offsetX, int offsetY, int projX, int projY);

	HRESULT CompileShaderFromData(const void* data, int size, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
private:
	void Init();
	void UpdateVB(RECT const& dstRect, FXMVECTOR color);

	ID3D11Device* _dev;
	ID3D11DeviceContext* _ctx;
	std::unique_ptr<CommonStates> _states;
	long _width;
	long _height;
	DirectX::SimpleMath::Matrix _matrix;

	ComPtr<ID3D11VertexShader> _vs;
	ComPtr<ID3D11VertexShader> _vsShadow;
	ComPtr<ID3D11PixelShader> _psNomal;
	ComPtr<ID3D11PixelShader> _psShadow;
	ComPtr<ID3D11InputLayout> _inputLayout;

	struct CBNeverChanges
	{
		DirectX::XMMATRIX Matrix;
		DirectX::XMFLOAT4 ShadowColor;
	};

	CBNeverChanges _cbData;
	DirectX::ConstantBuffer<CBNeverChanges> _cbuffer;
	DirectX::ConstantBuffer<DirectX::XMINT4> _cbShadowInfo;
	
	ComPtr<ID3D11Buffer> _vb;
	ComPtr<ID3D11Buffer> _ib;

	int _readIdx;
	int _writeIdx;
	int _mode;
};

