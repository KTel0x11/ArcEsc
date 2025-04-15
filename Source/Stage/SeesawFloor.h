#pragma once
#pragma once

#include"Graphics/Model.h"
#include"Stage.h"
#include "Chara/Player.h"

class SeesawFloor :public Stage
{
public:
	SeesawFloor();
	~SeesawFloor()override;


	//�X�V����
	void Update(float elapsedTime) override;

	//�`�揈��
	void Render(ID3D11DeviceContext* dc, Shader* shader) override;

	//���C�L���X�g
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) override;

	//�X�t�B�A�L���X�g
	bool SphereCast(const DirectX::XMFLOAT3& start,const float radius,const DirectX::XMFLOAT3& end, HitResult& hit) override;

	//ImGui�`��
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

	bool UpdateGroundTimer(float elapsedTime);

	float Lerp(float a, float b, float t) {
		return a + (b - a) * t;
	}


private:

	struct GimmickSwitch
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 angle;
		DirectX::XMFLOAT3 scale;
		float range;
		float height;
	};

	//Model* model = nullptr;


	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMFLOAT3 angle = { 0,0,0 };
	DirectX::XMFLOAT3 scale = { 1,1,1 };
	DirectX::XMFLOAT4X4 transform = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };

	DirectX::XMFLOAT3 HitPosition{0,0,0};
	bool hitFlg = false;
	bool groundFlg = false;

	float groundTimer = 0;

	float moveSpeed = 50.0f;
	float moveRate = 0.0f;

	DirectX::XMFLOAT3 currentNormal = { 0.0f, 1.0f, 0.0f }; // �����@��

	DirectX::XMFLOAT4X4 oldTransform = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };

	bool isPlayerOnSeesaw = false;        // �v���C���[���V�[�\�[�ɏ���Ă��邩
	float timeSincePlayerLeft = 0.0f;    // �v���C���[������Ă���̌o�ߎ���
	const float resetDelay = 5.0f;       // ���̈ʒu�ɖ߂�܂ł̎���
	const float resetSpeed = 0.2f;       // ���ɖ߂鑬���i��ԌW���j
	DirectX::XMFLOAT3 initialAngle = { 0.0f, 0.0f, 0.0f }; // �����p�x


};
