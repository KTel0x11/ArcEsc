#pragma once

#include <DirectXMath.h>
#include "Graphics/Model.h"

//キャラクター
class Character
{
public:
	Character() {}
	virtual ~Character() {}

	//行列更新処理

	void UpdateTransform();

	//位置取得
	const DirectX::XMFLOAT3& GetPosition() const { return position; }
	//位置設定
	void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }
	//回転取得
	const DirectX::XMFLOAT3& GetAngle()const { return angle; }
	//回転設定
	void SetAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }
	//スケール取得
	const DirectX::XMFLOAT3& GetScale()const { return scale; }
	//スケール設定
	void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }

	//カウンター設定
	void SetCounter(bool& CounterFlag) { this->CounterFlag = CounterFlag; }

	//半径取得
	float GetRadius() const { return radius; }

	//地面に設置しているか
	bool IsGround() const { return isGround; }

	//高さ取得
	float GetHeight() const { return height; }

	//ダメージを与える
	bool ApplyDamage(float damage,float invincibleTime);

	//衝撃を与える
	void AddImpulse(const DirectX::XMFLOAT3& impulse);

	//健康状態を取得
	float GetHealth() const { return health; }

	//最大健康状態を取得
	float GetMaxHealth() const { return maxHealth; }

	//重さ取得
	int GetWeight()const { return weight; }

	//ガードしているか
	bool GetGuard() { return isGuard; }

	bool GetDead() { return isDead; }

private:

	//垂直速力更新処理
	void UpdateVerticalVelocity(float elapsedTime);

	//垂直移動更新処理
	void UpdateVerticalMove(float elapsedTime);


	//水平速力更新処理
	void UpdateHorizontalVelocity(float elapsedTime);

	//水平移動更新処理
	void UpdateHorizontalMove(float elapsedTime);


protected:

	void Move(float vx, float vz, float speed);

	//ジャンプ処理
	void Jump(float speed);

	//速度処理更新
	void UpdateVelocity(float elapsedTime);

	void UpdateInvincibleTimer(float elapsedTime);

	void Turn(float elapsedTime, float vx, float vz, float speed);

	//着地したときに呼ばれる
	virtual void OnLanding(){}
	
	//ダメージを受けたときに呼ばれる
	virtual void OnDamaged(){}

	//死亡した時に呼ばれる
	virtual void OnDead() {};
	
	//ある一定の加速度で落下すれば位置リセット
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