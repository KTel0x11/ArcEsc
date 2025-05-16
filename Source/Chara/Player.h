#pragma once

#include"Graphics/Shader.h"
#include"Graphics/Model.h"
#include"Graphics/Sprite.h"
#include"Character.h"
#include"ProjectileManager.h"
#include"Effect.h"
#include"Enemy.h"
#include"Audio/AudioSource.h"

const int MAX_COIN = 100;


//プレイヤー
class Player : public Character
{
public:
	Player();
	~Player()override;

	//更新処理
	void Update(float elapsedTime);

	//描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	//UI描画
	void UIRender(ID3D11DeviceContext* dc);

	//デバッグ用GUI描画
	void DrawDebugGUI();

	void DrawDebugPrimitive();


	static Player& Instance();

	Enemy* GetNearEnemy() { return nearEnemy; }

	void SetStartPosition(DirectX::XMFLOAT3 pos) { startPosition = pos; };


	bool SearchNearEnemy();

	bool AddCoin(int Coin);

	DirectX::XMINT3 GetAxis() { return axis; }

	void PlamReset();

protected:
	//着地したときに呼ばれる
	void OnLanding() override;

	//ダメージを受けた時に呼ばれる
	void OnDamaged() override;

	//死亡した時に呼ばれる
	void OnDead() override;

private:

/*********************入力処理***********************/
	//ジャンプ入力処理
	bool InputJump();

	//移動入力処理
	bool InputMove(float elapsedTime);

	//弾丸入力処理
	void InputProjectile();

	//攻撃入力処理
	bool InputAttack();

	//ガード入力処理
	bool InputGuard();

	//回避入力処理
	bool InputDodge();

	//アイテム使用入力処理
	void InputUseItem();

/***************************************************/

/********************当たり判定処理*****************/
	//プレイヤーと敵の当たり判定
	void CollisionPlayerVsEnemies();

	//ノードとエネミーの衝突処理
	void CollisionNodeVsEnemies(const char* nodeName, float nodeRadius,float elapsedTime);

/***************************************************/



	void AttackToTarget(float elapsedTime, float speedRate);

	void PlayerStates();

	void SetEnemyTargetPosition();

	void DodgeImpulse();

	void ResetVelocity();

	void UseItem();

	void SelectItem();

	void UpdatePowerUpTimer(float elapsedTime);

/************************ステート処理***************/
	//待機ステートへ遷移
	void TransitionIdleState();

	//待機ステート更新処理
	void UpdateIdleState(float elapsedTime);

	//移動(歩き)ステートへ遷移
	void TransitionMoveWalkState();

	//移動(歩き)ステート更新処理
	void UpdateMoveWalkState(float elapsedTime);

	//移動(走り)ステートへ遷移
	void TransitionMoveRunState();

	//移動(走り)ステート更新処理
	void UpdateMoveRunState(float elapsedTime);

	//ジャンプステートへ遷移
	void TransitionJumpState();

	//ジャンプステート更新処理
	void UpdateJumpState(float elapsedTime);

	//着地ステートへ遷移
	void TransitionLowLandState();

	//着地ステート更新処理
	void UpdateLowLandState(float elapsedTime);

	//着地ステートへ遷移
	void TransitionHighLandState();

	//着地ステート更新処理
	void UpdateHighLandState(float elapsedTime);

	//攻撃ステートへ遷移
	void TransitionAttackState();

	//攻撃ステート更新処理
	void UpdateAttackState(float elapsedTime);

	//ダメージステートへ遷移
	void TransitionDamageState();

	//ダメージステート更新処理
	void UpdateDamageState(float elapsedTime);

	//死亡ステートへ遷移
	void TransitionDeathState();

	//死亡ステート更新処理
	void UpdateDeathState(float elapsedTime);

	//復活ステートへ遷移
	void TransitionReviveState();

	//復活ステート更新処理
	void UpdateReviveState(float elapsedTime);

	//ガードステートへ遷移
	void TransitionGuardState();

	//ガードステート更新処理
	void UpdateGuardState(float elapsedTime);

	//接近ステートへ遷移
	void TransitionCloserState();

	//接近ステート更新処理
	void UpdateCloserState(float elapsedTime);

	//回避ステートへ遷移
	void TransitionDodgeState();

	//回避ステート更新処理
	void UpdateDodgeState(float elapsedTime);

/************************************************/

public:
	void CollisionProjectilesVsEnemies();


public:
	int coin;

	int lifeItem;

	int powerItem;

	//bool OnPoison = false;
	bool isDamaged = false;
	bool spawnKey = false;
	bool haveKey = false;
	bool clear = false;

private:

	//ステート
	enum class State
	{
		Idle,
		MoveWalk,
		MoveRun,
		Jump,
		LowLand,
		HighLand,
		Attack,
		Damage,
		Death,
		Revive,
		Guard,
		Closer,
		Dodge,
	};

	//アニメーション
	enum Animation
	{
		Anim_RunA_C_FL,
		Anim_RunA_C_FR,
		Anim_RunA,
		Anim_StandA,
		Anim_StandA_idleA,
		Anim_StandA_Potion,
		Anim_StandB,
		Anim_StandC,
		Anim_WalkA_C_FL,
		Anim_WalkA_C_FR,
		Anim_WalkA,
		Anim_AttackA,
		Anim_AttackB,
		Anim_AttackC,
		Anim_AttackD,
		Anim_Damaged,
		Anim_Defend,
		Anim_Die,
		Anim_Down_B,
		Anim_Down_F,
		Anim_Emotion_Defeat,
		Anim_Emotion_Victory,
		Anim_jump,
		Anim_Roll_F,
		Anim_Roll_FL,
		Anim_Roll_FR,
		Anim_WTD_Run_C_FL,
		Anim_WTD_Run_C_FR,
		Anim_WTD_Run,
		Anim_WTD_Run_Look_FL,
		Anim_WTD_Run_Look_FR,
		Anim_WTD_Run_To_StandA,
		Anim_WTD_Stand,
		Anim_WTD_Stand_Calm,
		Anim_WTD_Stand_Calm_To_Stand,
		Anim_WTD_Stand_Tired,
		Anim_WTD_Stand_To_StandA,
		Anim_WTD_Walk_C_FL,
		Anim_WTD_Walk_C_FR,
		Anim_WTD_Walk_C_F,
		Anim_WTD_Walk_Look_B,
		Anim_WTD_Walk_Look_BL,
		Anim_WTD_Walk_Look_BR,
		Anim_WTD_Walk_Look_FL,
		Anim_WTD_Walk_Look_FR,
		Anim_WTD_Walk_Look_L,
		Anim_WTD_Walk_Look_R,
		Anim_WTD_Walk_To_WalkA,
		Anim_Emotion_Clap,
	};

	enum Item {
		Life,
		Power,
		MAX
	};

	 const float MAX_NEAR_DIST = FLT_MAX;

	//スティック入力値から移動ベクトルを取得
	DirectX::XMFLOAT3 GetMoveVec() const;
	//Model* model = nullptr;

	Effect* hitEffect = nullptr;
	Effect* poisonEffect = nullptr;
	Effect* swordEffect = nullptr;

	DirectX::XMFLOAT3 swordPosition[2] = { {0,0,0},{0,0,0}};

	DirectX::XMFLOAT3 targetPosition = { 0,0,0 };
	DirectX::XMFLOAT3 nearEnemyPosition = { 0,0,0 };

	DirectX::XMINT3 axis = { 0,0,0 };

	float moveSpeed = 20.0f;
	float turnSpeed = DirectX::XMConvertToRadians(360);
	float attackSpeed = 50.0f;

	float jumpSpeed = 45.0f;
	int jumpCount = 0;
	int jumpLimit = 2;

	float WeaponRadius = 4.0f;
	float katanaHeight = 2.0f;
	int AttackState = 0;

	float neardist = 100.0f;

	bool attackCollisionFlag = false;
	bool nextAttackFlag = false;
	bool UpDownFlg = false;
	bool LandingFlag = false;
	bool Dead = false;

	

	

	DirectX::XMFLOAT4 HPColor = { 0,1.0f,0,1.0f };


	State state = State::Idle;

	ProjectileManager projectileManager;

	Sprite* spritePlayerHP = nullptr;
	Sprite* spriteHPBar = nullptr;
	Sprite* spriteHPBack = nullptr;
	
	Sprite* spriteItemFrame = nullptr;
	Sprite* spriteItemLife = nullptr;
	Sprite* spriteItemPower = nullptr;
	Sprite* spriteCoin = nullptr;
	Sprite* spriteKey = nullptr;
	Sprite* spriteUp = nullptr;

	DirectX::XMFLOAT4 keyColor = { 1,1,1,1 };


	Enemy* nearEnemy = nullptr;


	AudioSource* RunSE = nullptr;
	AudioSource* WalkSE = nullptr;
	AudioSource* DodgeSE = nullptr;
	AudioSource* HitSE = nullptr;


	int itemNum;
	float powerUpTime = 0.0f;


	//float gravity = -1.0f;
	//DirectX::XMFLOAT3 velocity = { 0,0,0 };

};