#pragma once

#include"Graphics/Model.h"
#include"Stage.h"




class StageMoveFloor :public Stage
{
public:
	StageMoveFloor();
	~StageMoveFloor()override;


	//�X�V����
	void Update(float elapsedTime) override;

	//�`�揈��
	void Render(ID3D11DeviceContext* dc, Shader* shader) override;

	//���C�L���X�g
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) override;

	//�X�t�B�A�L���X�g
	bool SphereCast(const DirectX::XMFLOAT3& start, const float radius, const DirectX::XMFLOAT3& end, HitResult& hit) override;

	//ImGui�`��
	void DrawDebugGUI() override;

	void DrawDebugPrimitive() override;

	//�X�^�[�g�ʒu�ݒ�
	void SetStartPoint(const DirectX::XMFLOAT3& start) { this->start = start; }

	//�S�[���ʒu�ݒ�
	void SetGoalPoint(const DirectX::XMFLOAT3& goal) { this->goal = goal; }

	void SetAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }

	//�g���N�ݒ�
	void SetTorque(const DirectX::XMFLOAT3& torque) { this->torque = torque; }

	void SetMoveSpeed(const float& speed) { this->moveSpeed = speed; }

private :
	//�s��X�V����
	void UpdateTransform();


private:
	//Model* model = nullptr;


	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMFLOAT3 angle = { 0,0,0 };
	DirectX::XMFLOAT3 scale = { 1,1,1 };
	DirectX::XMFLOAT4X4 transform = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };


	DirectX::XMFLOAT3 start = { 0,0,0 };
	DirectX::XMFLOAT3 goal = { 0,0,0 };
	DirectX::XMFLOAT3 torque = { 0,0,0 };
	DirectX::XMFLOAT3 oldAngle = { 0,0,0 };

	DirectX::XMFLOAT3 HitPosition{ 0,0,0 };
	bool hitFlg = false;

	float moveSpeed = 10.0f;
	float moveRate = 0.0f;


	DirectX::XMFLOAT4X4 oldTransform = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };

};
