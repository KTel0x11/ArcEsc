#pragma once
#pragma once

#include <memory>
#include <wrl.h>
#include "Graphics/Shader.h"

class ToonShader : public Shader
{
public:
	ToonShader(ID3D11Device* device);
	~ToonShader() override {}

	void Begin(ID3D11DeviceContext* dc, const RenderContext& rc) override;
	void Draw(ID3D11DeviceContext* dc, const Model* model) override;
	void End(ID3D11DeviceContext* dc) override;

private:
	static const int MaxBones = 128;

	struct CbScene
	{
		DirectX::XMFLOAT4X4 viewProjection; // �r���[�E�v���W�F�N�V�����s��
		DirectX::XMFLOAT4   lightDirection; // ���s�����i���������C�g�j
		DirectX::XMFLOAT4   ambientColor;   // �����̐F�iRGB�j+ ���x
	};

	struct CbMesh
	{
		DirectX::XMFLOAT4X4	boneTransforms[MaxBones];
	};

	struct CbSubset
	{
		DirectX::XMFLOAT4	materialColor;
	};

	struct CbToonLighting
	{
		float stepThresholds[4]; // �g�D�[���̒i�K�i��F0.2, 0.5, 0.8, 1.0�j
	};


	Microsoft::WRL::ComPtr<ID3D11Buffer>			sceneConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			meshConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			subsetConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			toonLightingConstantBuffer;


	Microsoft::WRL::ComPtr<ID3D11VertexShader>		vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>		inputLayout;

	Microsoft::WRL::ComPtr<ID3D11BlendState>		blendState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	depthStencilState;

	Microsoft::WRL::ComPtr<ID3D11SamplerState>		samplerState;



};
