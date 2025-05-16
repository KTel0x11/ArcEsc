#include<imgui.h>
#include<time.h>
#include <stdlib.h>
#include <stdio.h>
#include "EnemySlime.h"
#include"Graphics/Graphics.h"
#include"Mathf.h"
#include"Player.h"
#include"Collision.h"
#include"EffectManager.h"

EnemySlime::EnemySlime() {
	model = new Model("Data/Model/Slime/Slime.mdl");
	scale.x = scale.y = scale.z = 0.07f;


	//�p�����[�^������
	radius = 2.5f;
	height = 5.0f;
	maxHealth = 100;
	health = maxHealth;
	DamageMax = 20;
	DamageMin = 10;
	targetPosition = position;

	despawnTimer = 60.0f;

	coinEffect = new Effect("Data/Effect/Coin.efk");

	HitSE = Audio::Instance().LoadAudioSource("Data/Audio/HitSE.wav");
	HitSE->SetVolume(0.1f);

	//�p�j�X�e�[�g�֑J��
	TransitionWanderState();

}

EnemySlime::~EnemySlime() {

	delete coinEffect;
	delete HitSE;
	delete model;

}

void EnemySlime::Update(float elapsedTime)
{

	//�X�e�[�g���̍X�V����
	switch (state)
	{
	case State::Wander:
		UpdateWanderState(elapsedTime);
		break;

	case State::Idle:
		UpdateIdleState(elapsedTime);
		break;

	case State::Pursuit:
		UpdatePursuitState(elapsedTime);
		break;
	case State::Attack:
		UpdateAttackState(elapsedTime);
		break;

	case State::IdleBattle:
		UpdateIdleBattleState(elapsedTime);
		break;

	case State::Damage:
		UpdateDamageState(elapsedTime);
		break;
	case State::Death:
		UpdateDeathState(elapsedTime);
		break;

	}


	UpdateVelocity(elapsedTime);

	UpdateTransform();

	UpdateInvincibleTimer(elapsedTime);

	Despawn(elapsedTime);

	model->UpdateAnimation(elapsedTime);

	model->UpdateTransform(transform);
}



void EnemySlime::Render(ID3D11DeviceContext* dc, Shader* shader) {
	shader->Draw(dc, model);

	//DrawDebugGUI();
}


void EnemySlime::OnDamaged() {
	//�_���[�W���󂯂���v���C���[��_��
	targetPosition = Player::Instance().GetPosition();
	//�_���[�W�X�e�[�g�֑J��
	TransitionDamageState();
}

void EnemySlime::OnDead() {
	//���S�X�e�[�g�֑J��
	TransitionDeathState();
}

//�f�o�b�O�v���~�e�B�u�`��
void EnemySlime::DrawDebugPrimitive() {
	//���N���X�̃f�o�b�O�v���~�e�B�u�`��
	Enemy::DrawDebugPrimitive();
	DebugRenderer* debugRender = Graphics::Instance().GetDebugRenderer();

	//�꒣��͈͂��f�o�b�O�~���`��
	debugRender->DrawCylinder(territoryOrigin, territoryRange, 1.0f, DirectX::XMFLOAT4(0, 1, 0, 1),{0,0,0});

	//�^�[�Q�b�g�ʒu���f�o�b�O���`��
	debugRender->DrawSphere(targetPosition, radius, DirectX::XMFLOAT4(1, 1, 0, 1));

	//���G�͈͂��f�o�b�O�~���`��
	debugRender->DrawCylinder(position, searchRange, 1.0f, DirectX::XMFLOAT4(0, 0, 1, 1),{0,0,0});

	//�U���͈͂��f�o�b�O�~���`��
	debugRender->DrawCylinder(position, attackRange, 1.0f, DirectX::XMFLOAT4(1, 0, 0, 1),{0,0,0});
}

void EnemySlime::SetTerritory(const DirectX::XMFLOAT3& origin, float range) {
	territoryOrigin = origin;
	territoryRange = range;
}

//�^�[�Q�b�g�ʒu�������_���ݒ�
void EnemySlime::SetRandomTargetPosition() {

	DirectX::XMFLOAT3 r = { Mathf::RandomRange(-territoryRange,territoryRange),0.0f , Mathf::RandomRange(-territoryRange,territoryRange) };
	float theta = Mathf::RandomRange(DirectX::XMConvertToRadians(-180), DirectX::XMConvertToRadians(180));
	targetPosition.x = territoryOrigin.x+r.x * cosf(theta);
	targetPosition.y = territoryOrigin.y ;
	targetPosition.z = territoryOrigin.z + r.z * sinf(theta);

}

//�ڕW�n�_�ֈړ�
void EnemySlime::MoveToTarget(float elapsedTime, float speedRate)
{
	//�^�[�Q�b�g�����ւ̐i�s�x�N�g�����Z�o
	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	float dist = sqrtf(vx * vx + vz * vz);
	vx /= dist;
	vz /= dist;

	//�ړ�����
	Move(vx, vz, moveSpeed * speedRate);
	Turn(elapsedTime, vx, vz, turnSpeed * speedRate);
}

//�v���C���[���G
bool EnemySlime::SearchPlayer() {

	//�v���C���[�Ƃ̍��፷���l������3D�ł̋������������
	const DirectX::XMFLOAT3& playerPosition = Player::Instance().GetPosition();
	float vx = playerPosition.x - position.x;
	float vy = playerPosition.y - position.y;
	float vz = playerPosition.z - position.z;
	float dist = sqrtf(vx * vx + vy * vy + vz * vz);

	if (dist < searchRange)
	{
		float distXZ = sqrtf(vx * vx + vz * vz);
		//�P�ʃx�N�g����
		vx /= distXZ;
		vz /= distXZ;
		//�O���x�N�g��
		float frontX = sinf(angle.y);
		float frontZ = cosf(angle.y);
		//2�̃x�N�g���̓��ϒl�őO�㔻��
		float dot = (frontX * vx) + (frontZ * vz);
		if (dot > 0.0f)
		{
			return true;
		}

	}
	return false;
}

//�m�[�h�ƃv���C���[�̏Փˏ���
void EnemySlime::CollisionNodeVsPlayer(const char* nodeName, float nodeRadius) {

	//�m�[�h�̈ʒu�Ɠ����蔻����s��
	Model::Node* node = model->FindNode(nodeName);
	if (node != nullptr) {
		//�m�[�h�̃��[���h���W
		DirectX::XMFLOAT3 nodePosition(
			node->worldTransform._41,
			node->worldTransform._42,
			node->worldTransform._43
		);

		//�����蔻��\��
		Graphics::Instance().GetDebugRenderer()->DrawSphere(
			nodePosition, nodeRadius, DirectX::XMFLOAT4(0, 0, 1, 1)
		);

		//�v���C���[�Ɠ����蔻��
		Player& player = Player::Instance();
		DirectX::XMFLOAT3 outPosition;
		if (Collision::IntersectSphereVsCylinder(
			nodePosition,
			nodeRadius,
			player.GetPosition(),
			player.GetRadius(),
			player.GetHeight(),
			outPosition)) {

			//�_���[�W��^����
			if (player.ApplyDamage(Mathf::RandomRange(DamageMin,DamageMax), 1.0f)) {
				//�G�𐁂���΂��x�N�g�����Z�o
				DirectX::XMFLOAT3 vec;
				vec.x = outPosition.x - nodePosition.x;
				vec.z = outPosition.z - nodePosition.z;

				float length = sqrtf(vec.x * vec.x + vec.z * vec.z);

				vec.x /= length;
				vec.z /= length;

				//XZ���ʂɐ�����΂��͂�������
				float power = 10.0f;
				vec.x *= power;
				vec.z *= power;
				//Y�����ɂ��͂�������
				vec.y = 0.0f;

				//������΂�
				player.AddImpulse(vec);
				HitSE->Play(false);

			}

		}

	}
}


//�p�j�X�e�[�g�֑J��
void EnemySlime::TransitionWanderState() {

	state = State::Wander;

	//�ڕW�n�_�ݒ�
	SetRandomTargetPosition();

	lookPlayer = false;

	//�^�C�}�[�������_���ݒ�
	stateTimer = Mathf::RandomRange(10.0f, 15.0f);

	//�����A�j���[�V�����̍Đ�
	model->PlayAnimation(Anim_WalkFWD, true);

}

//�p�j�X�e�[�g�X�V����
void EnemySlime::UpdateWanderState(float elapsedTime) {

	//�ڕW�n�_�܂�XZ���ʂł̋�������
	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	float distSq = vx * vx + vz * vz;

	//�^�C�}�[����
	stateTimer -= elapsedTime;

	if (distSq < radius * radius|| stateTimer < 0.0f) {
		//�ҋ@�X�e�[�g�֑J��
		TransitionIdleState();
	}

	//�ڕW�n�_�ֈړ�
	MoveToTarget(elapsedTime, 0.5f);

	//�v���C���[���G
	if (SearchPlayer()) {
		//����������ǐՃX�e�[�g�֑J��
		TransitionPursuitState();
	}
}

//�ҋ@�X�e�[�g�֑J��
void EnemySlime::TransitionIdleState() {
	state = State::Idle;

	//�^�C�}�[�������_���ݒ�
	stateTimer = Mathf::RandomRange(3.0f, 5.0f);

	lookPlayer = false;

	//�ҋ@�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_IdleNormal, true);
}

//�ҋ@�X�e�[�g�X�V����
void EnemySlime::UpdateIdleState(float elapsedTime) {
	//�^�C�}�[����
	stateTimer -= elapsedTime;
	if (stateTimer < 0.0f) {
		//�p�j�X�e�[�g�֑J��
		TransitionWanderState();
	}

	//�v���C���[���G
	if (SearchPlayer()) {
		//����������ǐՃX�e�[�g�֑J��
		TransitionPursuitState();
	}

}

//�ǐՃX�e�[�g�֑J��
void EnemySlime::TransitionPursuitState() {
	state = State::Pursuit;

	//���b�ԒǐՂ���^�C�}�[�������_���ݒ�
	stateTimer = Mathf::RandomRange(3.0f, 5.0f);

	lookPlayer = true;

	//�����A�j���[�V�����Đ�
	model->PlayAnimation(Anim_RunFWD, true);

}

//�ǐՃX�e�[�g�X�V����
void EnemySlime::UpdatePursuitState(float elapsedTime) {
	//�ڕW�n�_���v���C���[�ʒu�ɐݒ�
	targetPosition = Player::Instance().GetPosition();

	//�ڕW�n�_�ֈړ�
	MoveToTarget(elapsedTime, 1.0f);

	//�^�C�}�[����
	stateTimer -= elapsedTime;
	if (stateTimer < 0.0f) {
		//�ҋ@�X�e�[�g�֑J��
		TransitionIdleState();
	}

	//�v���C���[�ɋ߂Â��ƍU���X�e�[�g�֑J��
	float vx = targetPosition.x - position.x;
	float vy = targetPosition.y - position.y;
	float vz = targetPosition.z - position.z;
	float dist = sqrtf(vx * vx + vy * vy + vz * vz);
	if (dist < attackRange) {
		//�U���X�e�[�g�֑J��
		TransitionAttackState();
	}

}

//�U���X�e�[�g�֑J��
void EnemySlime::TransitionAttackState() {
	state = State::Attack;

	lookPlayer = true;

	//�U���A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Attack1, false);

}

void EnemySlime::UpdateAttackState(float elapsedTime) {
	//�C�ӂ̃A�j���[�V�����Đ���Ԃ̂ݏՓ˔��菈��������
	float animationTime = model->GetCurrentAnimationSeconds();
	if (animationTime >= 0.1f && animationTime < 0.35f) {
		//�ڋʃm�[�h�ƃv���C���[�̏Փˏ���
		CollisionNodeVsPlayer("EyeBall", 1.0f);
	}

	//�U���A�j���[�V�������I������퓬�ҋ@�X�e�[�g�֑J��
	if (!model->IsPlayAnimation()) {
		TransitionIdleBattleState();
	}
}

void EnemySlime::TransitionIdleBattleState() {
	state = State::IdleBattle;
	//���b�ԑҋ@����^�C�}�[�������_���ݒ�
	stateTimer = Mathf::RandomRange(0.5f, 1.5f);
	lookPlayer = true;
	//�퓬�ҋ@�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_IdleBattle, true);
}

//�퓬�ҋ@�X�e�[�g�X�V����
void EnemySlime::UpdateIdleBattleState(float elapsedTime) {
	//�ڕW�n�_���v���C���[�ʒu�ɐݒ�
	targetPosition = Player::Instance().GetPosition();

	MoveToTarget(elapsedTime, turnSpeed);

	//�^�C�}�[����
	stateTimer -= elapsedTime;

	 if (stateTimer < 0.0f) {
		//�v���C���[�ɋ߂Â��ƍU���X�e�[�g�֑J��
		float vx = targetPosition.x - position.x;
		float vy = targetPosition.y - position.y;
		float vz = targetPosition.z - position.z;
		float dist = sqrtf(vx * vx + vy * vy + vz * vz);
		Turn(elapsedTime, vx, vz, 1.0f);
		if (dist < attackRange) {
			//�U���X�e�[�g�֑J��
			TransitionAttackState();
		}
		else {
			//�p�j�X�e�[�g�֑J��
			TransitionWanderState();
		}
	}

	MoveToTarget(elapsedTime, 0.0f);

}

//�_���[�W�X�e�[�g�֑J��
void EnemySlime::TransitionDamageState() {
	state = State::Damage;
	lookPlayer = true;
	//�_���[�W�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_GetHit, false);
}

//�_���[�W�X�e�[�g�X�V����
void EnemySlime::UpdateDamageState(float elapsedTime) {
	//�_���[�W�A�j���[�V�������I������퓬�ҋ@�X�e�[�g�֑J��
	if (!model->IsPlayAnimation()) {
		TransitionIdleBattleState();
	}
}

//���S�X�e�[�g�֑J��
void EnemySlime::TransitionDeathState() {
	state = State::Death;

	//���S�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Die, false);
}

//���S�X�e�[�g�X�V����
void EnemySlime::UpdateDeathState(float elapsedTime) {
	//���S�A�j���[�V�������I�����玩����j��
	if (!model->IsPlayAnimation()) {
		srand((unsigned int)time(NULL));
		int rnd = rand() % 5;
		Player::Instance().AddCoin(rnd);
		coinEffect->Play(position, 2.0f);
		Destroy();
	}
}

void EnemySlime::Despawn(float elapsedTime) {
	if (!lookPlayer) {
		despawnTimer -= elapsedTime;
	}

	if (despawnTimer < 0.0f) {
		Destroy();
	}


}

void EnemySlime::DrawDebugGUI() {
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Character", nullptr, ImGuiWindowFlags_None)) {
		if (ImGui::CollapsingHeader("Slime", ImGuiTreeNodeFlags_DefaultOpen)) {
			if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::InputFloat3("Position", &position.x);

				DirectX::XMFLOAT3 a;
				a.x = DirectX::XMConvertToDegrees(angle.x);
				a.y = DirectX::XMConvertToDegrees(angle.y);
				a.z = DirectX::XMConvertToDegrees(angle.z);
				ImGui::InputFloat3("Angle", &a.x);
				angle.x = DirectX::XMConvertToRadians(a.x);
				angle.y = DirectX::XMConvertToRadians(a.y);
				angle.z = DirectX::XMConvertToRadians(a.z);

				ImGui::InputFloat3("Scale", &scale.x);
				ImGui::InputFloat3("Velocity", &velocity.x);
			}
			if (ImGui::CollapsingHeader("States", ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::InputFloat("HP", &health);

			}
		}

	}

	ImGui::End();
}