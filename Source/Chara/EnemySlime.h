#pragma once

#include "Graphics/Model.h"
#include"Enemy.h"
#include"Effect.h"
#include"Audio/AudioSource.h"
#include"Audio/Audio.h"

//スライム
class EnemySlime : public Enemy {
public:
	EnemySlime();
	~EnemySlime() override;

	//更新処理
	void Update(float elapsedTime) override;
	//描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader)override;

	//デバックプリミティブ描画
	void DrawDebugPrimitive() override;

	//縄張り設定
	void SetTerritory(const DirectX::XMFLOAT3& origin, float range);

protected:
	//ダメージを受けた時に呼ばれる
	void OnDamaged()override;

private:

	//ターゲット位置をランダム設定
	void SetRandomTargetPosition();

	//目標地点へ移動
	void MoveToTarget(float elapsedTime, float speedRate);

	//プレイヤー索敵
	bool SearchPlayer();

	//ノードとプレイヤーの衝突処理
	void CollisionNodeVsPlayer(const char* nodeName, float boneRadius);

	
	//デバッグ用GUI描画
	void DrawDebugGUI();

	/**************************ステート********************************/
		//待機ステートへ遷移
	void TransitionIdleState();

	//待機ステート更新処理
	void UpdateIdleState(float elapsedTime);

	//徘徊ステートへ遷移
	void TransitionWanderState();

	//徘徊ステート更新処理
	void UpdateWanderState(float elapsedTime);

	//追跡ステート遷移
	void TransitionPursuitState();

	//追跡ステート更新処理
	void UpdatePursuitState(float elapsedTime);

	//攻撃ステートへ遷移
	void TransitionAttackState();

	//攻撃ステート更新処理
	void UpdateAttackState(float elapsedTime);

	//戦闘待機ステートへ遷移
	void TransitionIdleBattleState();

	//戦闘待機ステート更新処理
	void UpdateIdleBattleState(float elapsedTime);

	//ダメージステートへ遷移
	void TransitionDamageState();

	//ダメージステート更新処理
	void UpdateDamageState(float elapsedTime);

	//死亡ステートへ遷移
	void TransitionDeathState();

	//死亡ステート更新処理
	void UpdateDeathState(float elapsedTime);

	/***************************************************************/

	//デスポーン
	void Despawn(float elapsedTime);

private:
	//ステート
	enum class State
	{
		Wander,
		Idle,
		Pursuit,
		Attack,
		IdleBattle,
		Damage,
		Death
	};

	//アニメーション
	enum Animation
	{
		Anim_IdleNormal,
		Anim_IdleBattle,
		Anim_Attack1,
		Anim_Attack2,
		Anim_WalkFWD,
		Anim_WalkBWD,
		Anim_WalkLeft,
		Anim_WalkRight,
		Anim_RunFWD,
		Anim_SenseSomthingST,
		Anim_SenseSomthingPRT,
		Anim_Taunt,
		Anim_Victory,
		Anim_GetHit,
		Anim_Dizzy,
		Anim_Die
	};

private:
	Model* model = nullptr;
	State state = State::Wander;
	DirectX::XMFLOAT3 targetPosition = { 0,0,0 };
	DirectX::XMFLOAT3 territoryOrigin = { 0,0,0 };

	Effect* coinEffect = nullptr;

	float territoryRange = 20.0f;
	float moveSpeed = 3.0f;
	float turnSpeed = DirectX::XMConvertToRadians(360);
	float stateTimer = 0.0f;
	float searchRange = 25.0f;//索敵範囲
	float attackRange = 5.0f;//攻撃射程範囲
	
	float despawnTimer = 0.0f;

	bool lookPlayer = false;

	AudioSource* HitSE = nullptr;


protected:
	void OnDead() override;

};