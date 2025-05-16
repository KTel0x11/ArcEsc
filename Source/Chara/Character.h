#pragma once

#include <DirectXMath.h>
#include "Graphics/Model.h"

//�L�����N�^�[
class Character
{
public:
	Character() {}
	virtual ~Character() {}

	//�s��X�V����

	void UpdateTransform();

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

	//�J�E���^�[�ݒ�
	void SetCounter(bool& CounterFlag) { this->CounterFlag = CounterFlag; }

	//���a�擾
	float GetRadius() const { return radius; }

	//�n�ʂɐݒu���Ă��邩
	bool IsGround() const { return isGround; }

	//�����擾
	float GetHeight() const { return height; }

	//�_���[�W��^����
	bool ApplyDamage(float damage,float invincibleTime);

	//�Ռ���^����
	void AddImpulse(const DirectX::XMFLOAT3& impulse);

	//���N��Ԃ��擾
	float GetHealth() const { return health; }

	//�ő匒�N��Ԃ��擾
	float GetMaxHealth() const { return maxHealth; }

	//�d���擾
	int GetWeight()const { return weight; }

	//�K�[�h���Ă��邩
	bool GetGuard() { return isGuard; }

	bool GetDead() { return isDead; }

private:

	//�������͍X�V����
	void UpdateVerticalVelocity(float elapsedTime);

	//�����ړ��X�V����
	void UpdateVerticalMove(float elapsedTime);


	//�������͍X�V����
	void UpdateHorizontalVelocity(float elapsedTime);

	//�����ړ��X�V����
	void UpdateHorizontalMove(float elapsedTime);


protected:

	void Move(float vx, float vz, float speed);

	//�W�����v����
	void Jump(float speed);

	//���x�����X�V
	void UpdateVelocity(float elapsedTime);

	void UpdateInvincibleTimer(float elapsedTime);

	void Turn(float elapsedTime, float vx, float vz, float speed);

	//���n�����Ƃ��ɌĂ΂��
	virtual void OnLanding(){}
	
	//�_���[�W���󂯂��Ƃ��ɌĂ΂��
	virtual void OnDamaged(){}

	//���S�������ɌĂ΂��
	virtual void OnDead() {};
	
	//������̉����x�ŗ�������Έʒu���Z�b�g
	void ResetPosition();

protected:
	DirectX::XMFLOAT3	startPosition = { 0,0,0 };
	DirectX::XMFLOAT3	position = { 0,0,0 };
	DirectX::XMFLOAT3	angle = { 0,0,0 };
	DirectX::XMFLOAT3	scale = { 1,1,1 };
	DirectX::XMFLOAT4X4 transform = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};

	DirectX::XMFLOAT3	rayEndPos = { 0,0,0 };

	Model* model=nullptr;

	float radius = 0.5f;
	float gravity = -1.0f;
	DirectX::XMFLOAT3 velocity = { 0,0,0 };
	float height = 2.0f;
	float speed = 2.0f;

	bool isGround = false;
	bool isGuard = false;
	bool CounterFlag = false;
	bool isDead = false;

	int weight = 50;

	float oldHealth = 0.0f;
	float health = 5.0f;
	float maxHealth = 5.0f;

	float Damage = 0.0f;
	float DamageMax = 50.0f;
	float DamageMin = 0.0f;

	float invincibleTimer = 1.0f;

	float friction = 0.5f;

	float acceleration = 1.0f;
	float maxMoveSpeed = 2.0f;
	float moveVecX = 0.0f;
	float moveVecZ = 0.0f;

	float airControl = 0.3f;

	float stepOffset = 5.0f;
	float slopeRate = 1.0f;
	float slopeSpeed = 5.0f;

	bool wallHit = false;
	bool OnPoison = false;
};