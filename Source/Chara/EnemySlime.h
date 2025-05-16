#pragma once

#include "Graphics/Model.h"
#include"Enemy.h"
#include"Effect.h"
#include"Audio/AudioSource.h"
#include"Audio/Audio.h"

//�X���C��
class EnemySlime : public Enemy {
public:
	EnemySlime();
	~EnemySlime() override;

	//�X�V����
	void Update(float elapsedTime) override;
	//�`�揈��
	void Render(ID3D11DeviceContext* dc, Shader* shader)override;

	//�f�o�b�N�v���~�e�B�u�`��
	void DrawDebugPrimitive() override;

	//�꒣��ݒ�
	void SetTerritory(const DirectX::XMFLOAT3& origin, float range);

protected:
	//�_���[�W���󂯂����ɌĂ΂��
	void OnDamaged()override;

private:

	//�^�[�Q�b�g�ʒu�������_���ݒ�
	void SetRandomTargetPosition();

	//�ڕW�n�_�ֈړ�
	void MoveToTarget(float elapsedTime, float speedRate);

	//�v���C���[���G
	bool SearchPlayer();

	//�m�[�h�ƃv���C���[�̏Փˏ���
	void CollisionNodeVsPlayer(const char* nodeName, float boneRadius);

	
	//�f�o�b�O�pGUI�`��
	void DrawDebugGUI();

	/**************************�X�e�[�g********************************/
		//�ҋ@�X�e�[�g�֑J��
	void TransitionIdleState();

	//�ҋ@�X�e�[�g�X�V����
	void UpdateIdleState(float elapsedTime);

	//�p�j�X�e�[�g�֑J��
	void TransitionWanderState();

	//�p�j�X�e�[�g�X�V����
	void UpdateWanderState(float elapsedTime);

	//�ǐՃX�e�[�g�J��
	void TransitionPursuitState();

	//�ǐՃX�e�[�g�X�V����
	void UpdatePursuitState(float elapsedTime);

	//�U���X�e�[�g�֑J��
	void TransitionAttackState();

	//�U���X�e�[�g�X�V����
	void UpdateAttackState(float elapsedTime);

	//�퓬�ҋ@�X�e�[�g�֑J��
	void TransitionIdleBattleState();

	//�퓬�ҋ@�X�e�[�g�X�V����
	void UpdateIdleBattleState(float elapsedTime);

	//�_���[�W�X�e�[�g�֑J��
	void TransitionDamageState();

	//�_���[�W�X�e�[�g�X�V����
	void UpdateDamageState(float elapsedTime);

	//���S�X�e�[�g�֑J��
	void TransitionDeathState();

	//���S�X�e�[�g�X�V����
	void UpdateDeathState(float elapsedTime);

	/***************************************************************/

	//�f�X�|�[��
	void Despawn(float elapsedTime);

private:
	//�X�e�[�g
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

	//�A�j���[�V����
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
	float searchRange = 25.0f;//���G�͈�
	float attackRange = 5.0f;//�U���˒��͈�
	
	float despawnTimer = 0.0f;

	bool lookPlayer = false;

	AudioSource* HitSE = nullptr;


protected:
	void OnDead() override;

};