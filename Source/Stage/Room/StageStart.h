#pragma once
#pragma once

#include "Graphics/Model.h"
#include "Stage/Stage.h"
#include "Stage/Room.h"
//�X�e�[�W
class StageStart : public Stage
{
public:
	StageStart();
	~StageStart() override;


	//�X�V����
	void Update(float elapsedTime) override;

	//�`�揈��
	void Render(ID3D11DeviceContext* dc, Shader* shader) override;

	//���C�L���X�g
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) override;

	//�X�t�B�A�L���X�g
	bool SphereCast(const DirectX::XMFLOAT3& start, const float radius, const DirectX::XMFLOAT3& end, HitResult& hit) override;

	void DrawDebugGUI() override;

	void DrawDebugPrimitive() override;

	//�ʒu�ݒ�
	virtual void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; };

	//�傫���ݒ�
	virtual void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }

	//�p�x�ݒ�
	virtual void SetAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }



private:
	//�s��X�V����
	void UpdateTransform();

private:

	//Model* model = nullptr;

	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMFLOAT3 angle = { 0,0,0 };
	DirectX::XMFLOAT3 scale = { 1,1,1 };
	DirectX::XMFLOAT4X4 transform = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
};