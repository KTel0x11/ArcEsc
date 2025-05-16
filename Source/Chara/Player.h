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


//�v���C���[
class Player : public Character
{
public:
	Player();
	~Player()override;

	//�X�V����
	void Update(float elapsedTime);

	//�`�揈��
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	//UI�`��
	void UIRender(ID3D11DeviceContext* dc);

	//�f�o�b�O�pGUI�`��
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
	//���n�����Ƃ��ɌĂ΂��
	void OnLanding() override;

	//�_���[�W���󂯂����ɌĂ΂��
	void OnDamaged() override;

	//���S�������ɌĂ΂��
	void OnDead() override;

private:

/*********************���͏���***********************/
	//�W�����v���͏���
	bool InputJump();

	//�ړ����͏���
	bool InputMove(float elapsedTime);

	//�e�ۓ��͏���
	void InputProjectile();

	//�U�����͏���
	bool InputAttack();

	//�K�[�h���͏���
	bool InputGuard();

	//�����͏���
	bool InputDodge();

	//�A�C�e���g�p���͏���
	void InputUseItem();

/***************************************************/

/********************�����蔻�菈��*****************/
	//�v���C���[�ƓG�̓����蔻��
	void CollisionPlayerVsEnemies();

	//�m�[�h�ƃG�l�~�[�̏Փˏ���
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

/************************�X�e�[�g����***************/
	//�ҋ@�X�e�[�g�֑J��
	void TransitionIdleState();

	//�ҋ@�X�e�[�g�X�V����
	void UpdateIdleState(float elapsedTime);

	//�ړ�(����)�X�e�[�g�֑J��
	void TransitionMoveWalkState();

	//�ړ�(����)�X�e�[�g�X�V����
	void UpdateMoveWalkState(float elapsedTime);

	//�ړ�(����)�X�e�[�g�֑J��
	void TransitionMoveRunState();

	//�ړ�(����)�X�e�[�g�X�V����
	void UpdateMoveRunState(float elapsedTime);

	//�W�����v�X�e�[�g�֑J��
	void TransitionJumpState();

	//�W�����v�X�e�[�g�X�V����
	void UpdateJumpState(float elapsedTime);

	//���n�X�e�[�g�֑J��
	void TransitionLowLandState();

	//���n�X�e�[�g�X�V����
	void UpdateLowLandState(float elapsedTime);

	//���n�X�e�[�g�֑J��
	void TransitionHighLandState();

	//���n�X�e�[�g�X�V����
	void UpdateHighLandState(float elapsedTime);

	//�U���X�e�[�g�֑J��
	void TransitionAttackState();

	//�U���X�e�[�g�X�V����
	void UpdateAttackState(float elapsedTime);

	//�_���[�W�X�e�[�g�֑J��
	void TransitionDamageState();

	//�_���[�W�X�e�[�g�X�V����
	void UpdateDamageState(float elapsedTime);

	//���S�X�e�[�g�֑J��
	void TransitionDeathState();

	//���S�X�e�[�g�X�V����
	void UpdateDeathState(float elapsedTime);

	//�����X�e�[�g�֑J��
	void TransitionReviveState();

	//�����X�e�[�g�X�V����
	void UpdateReviveState(float elapsedTime);

	//�K�[�h�X�e�[�g�֑J��
	void TransitionGuardState();

	//�K�[�h�X�e�[�g�X�V����
	void UpdateGuardState(float elapsedTime);

	//�ڋ߃X�e�[�g�֑J��
	void TransitionCloserState();

	//�ڋ߃X�e�[�g�X�V����
	void UpdateCloserState(float elapsedTime);

	//����X�e�[�g�֑J��
	void TransitionDodgeState();

	//����X�e�[�g�X�V����
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

	//�X�e�[�g
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

	//�A�j���[�V����
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

	//�X�e�B�b�N���͒l����ړ��x�N�g�����擾
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