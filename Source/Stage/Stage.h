#pragma once
#include "Graphics/Shader.h"
#include"Chara/Collision.h"
#include"Graphics/Model.h"

class Stage {
public:
	Stage() {};
	virtual ~Stage() {};


	//�X�V����
	virtual void Update(float elapsedTime) = 0;

	//�`�揈��
	virtual void Render(ID3D11DeviceContext* dc, Shader* shader) = 0;

	//���C�L���X�g
	virtual bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) = 0;

	virtual bool SphereCast(const DirectX::XMFLOAT3& start,const float radius , const DirectX::XMFLOAT3& end, HitResult& hit) = 0;

	virtual void DrawDebugGUI() = 0;
	virtual void DrawDebugPrimitive() = 0;

	Model* GetStageModel() { return model; }

	//�j��
	void Destroy();

protected:

	Model* model = nullptr;


};