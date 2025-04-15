#pragma once
#include "Graphics/Shader.h"
#include"Chara/Collision.h"

class Gimmick {
public:
	Gimmick() {};
	virtual ~Gimmick() {};


	//�X�V����
	virtual void Update(float elapsedTime) = 0;

	//�`�揈��
	virtual void Render(ID3D11DeviceContext* dc, Shader* shader) = 0;

	//���C�L���X�g
	virtual bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) = 0;

	//�ʒu�擾
	const DirectX::XMFLOAT3& GetPosition() const { return position; }
	//�ʒu�ݒ�
	void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }
	//��]�擾
	const DirectX::XMFLOAT3& GetAngle()const { return angle; }
	//��]�ݒ�
	void SetAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }
	//�X�P�[���擾
	const DirectX::XMFLOAT3& GetScale()const { return scale; }
	//�X�P�[���ݒ�
	void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }

	//�X�P�[���ݒ�
	void SetRadius(const float radius) { this->radius = radius; }


	//�s��X�V����

	void UpdateTransform();

	//���C�L���X�g
	//virtual bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) = 0;

	virtual void DrawDebugGUI() = 0;
	virtual void DrawDebugPrimitive() = 0;

	//�j��
	 void Destroy();

protected:
	DirectX::XMFLOAT3	position = { 0,0,0 };
	DirectX::XMFLOAT3	angle = { 0,0,0 };
	DirectX::XMFLOAT3	scale = { 1,1,1 };
	float radius = 0;
	DirectX::XMFLOAT4X4 transform = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};


};