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


	//パラメータ初期化
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

	//徘徊ステートへ遷移
	TransitionWanderState();

}

EnemySlime::~EnemySlime() {

	delete coinEffect;
	delete HitSE;
	delete model;

}

void EnemySlime::Update(float elapsedTime)
{

	//ステート毎の更新処理
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
	//ダメージを受けたらプレイヤーを狙う
	targetPosition = Player::Instance().GetPosition();
	//ダメージステートへ遷移
	TransitionDamageState();
}

void EnemySlime::OnDead() {
	//死亡ステートへ遷移
	TransitionDeathState();
}

//デバッグプリミティブ描画
void EnemySlime::DrawDebugPrimitive() {
	//基底クラスのデバッグプリミティブ描画
	Enemy::DrawDebugPrimitive();
	DebugRenderer* debugRender = Graphics::Instance().GetDebugRenderer();

	//縄張り範囲をデバッグ円柱描画
	debugRender->DrawCylinder(territoryOrigin, territoryRange, 1.0f, DirectX::XMFLOAT4(0, 1, 0, 1),{0,0,0});

	//ターゲット位置をデバッグ球描画
	debugRender->DrawSphere(targetPosition, radius, DirectX::XMFLOAT4(1, 1, 0, 1));

	//索敵範囲をデバッグ円柱描画
	debugRender->DrawCylinder(position, searchRange, 1.0f, DirectX::XMFLOAT4(0, 0, 1, 1),{0,0,0});

	//攻撃範囲をデバッグ円柱描画
	debugRender->DrawCylinder(position, attackRange, 1.0f, DirectX::XMFLOAT4(1, 0, 0, 1),{0,0,0});
}

void EnemySlime::SetTerritory(const DirectX::XMFLOAT3& origin, float range) {
	territoryOrigin = origin;
	territoryRange = range;
}

//ターゲット位置をランダム設定
void EnemySlime::SetRandomTargetPosition() {

	DirectX::XMFLOAT3 r = { Mathf::RandomRange(-territoryRange,territoryRange),0.0f , Mathf::RandomRange(-territoryRange,territoryRange) };
	float theta = Mathf::RandomRange(DirectX::XMConvertToRadians(-180), DirectX::XMConvertToRadians(180));
	targetPosition.x = territoryOrigin.x+r.x * cosf(theta);
	targetPosition.y = territoryOrigin.y ;
	targetPosition.z = territoryOrigin.z + r.z * sinf(theta);

}

//目標地点へ移動
void EnemySlime::MoveToTarget(float elapsedTime, float speedRate)
{
	//ターゲット方向への進行ベクトルを算出
	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	float dist = sqrtf(vx * vx + vz * vz);
	vx /= dist;
	vz /= dist;

	//移動処理
	Move(vx, vz, moveSpeed * speedRate);
	Turn(elapsedTime, vx, vz, turnSpeed * speedRate);
}

//プレイヤー索敵
bool EnemySlime::SearchPlayer() {

	//プレイヤーとの高低差を考慮して3Dでの距離判定をする
	const DirectX::XMFLOAT3& playerPosition = Player::Instance().GetPosition();
	float vx = playerPosition.x - position.x;
	float vy = playerPosition.y - position.y;
	float vz = playerPosition.z - position.z;
	float dist = sqrtf(vx * vx + vy * vy + vz * vz);

	if (dist < searchRange)
	{
		float distXZ = sqrtf(vx * vx + vz * vz);
		//単位ベクトル化
		vx /= distXZ;
		vz /= distXZ;
		//前方ベクトル
		float frontX = sinf(angle.y);
		float frontZ = cosf(angle.y);
		//2つのベクトルの内積値で前後判定
		float dot = (frontX * vx) + (frontZ * vz);
		if (dot > 0.0f)
		{
			return true;
		}

	}
	return false;
}

//ノードとプレイヤーの衝突処理
void EnemySlime::CollisionNodeVsPlayer(const char* nodeName, float nodeRadius) {

	//ノードの位置と当たり判定を行う
	Model::Node* node = model->FindNode(nodeName);
	if (node != nullptr) {
		//ノードのワールド座標
		DirectX::XMFLOAT3 nodePosition(
			node->worldTransform._41,
			node->worldTransform._42,
			node->worldTransform._43
		);

		//当たり判定表示
		Graphics::Instance().GetDebugRenderer()->DrawSphere(
			nodePosition, nodeRadius, DirectX::XMFLOAT4(0, 0, 1, 1)
		);

		//プレイヤーと当たり判定
		Player& player = Player::Instance();
		DirectX::XMFLOAT3 outPosition;
		if (Collision::IntersectSphereVsCylinder(
			nodePosition,
			nodeRadius,
			player.GetPosition(),
			player.GetRadius(),
			player.GetHeight(),
			outPosition)) {

			//ダメージを与える
			if (player.ApplyDamage(Mathf::RandomRange(DamageMin,DamageMax), 1.0f)) {
				//敵を吹っ飛ばすベクトルを算出
				DirectX::XMFLOAT3 vec;
				vec.x = outPosition.x - nodePosition.x;
				vec.z = outPosition.z - nodePosition.z;

				float length = sqrtf(vec.x * vec.x + vec.z * vec.z);

				vec.x /= length;
				vec.z /= length;

				//XZ平面に吹っ飛ばす力をかける
				float power = 10.0f;
				vec.x *= power;
				vec.z *= power;
				//Y方向にも力をかける
				vec.y = 0.0f;

				//吹っ飛ばす
				player.AddImpulse(vec);
				HitSE->Play(false);

			}

		}

	}
}


//徘徊ステートへ遷移
void EnemySlime::TransitionWanderState() {

	state = State::Wander;

	//目標地点設定
	SetRandomTargetPosition();

	lookPlayer = false;

	//タイマーをランダム設定
	stateTimer = Mathf::RandomRange(10.0f, 15.0f);

	//歩きアニメーションの再生
	model->PlayAnimation(Anim_WalkFWD, true);

}

//徘徊ステート更新処理
void EnemySlime::UpdateWanderState(float elapsedTime) {

	//目標地点までXZ平面での距離判定
	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	float distSq = vx * vx + vz * vz;

	//タイマー処理
	stateTimer -= elapsedTime;

	if (distSq < radius * radius|| stateTimer < 0.0f) {
		//待機ステートへ遷移
		TransitionIdleState();
	}

	//目標地点へ移動
	MoveToTarget(elapsedTime, 0.5f);

	//プレイヤー索敵
	if (SearchPlayer()) {
		//見つかったら追跡ステートへ遷移
		TransitionPursuitState();
	}
}

//待機ステートへ遷移
void EnemySlime::TransitionIdleState() {
	state = State::Idle;

	//タイマーをランダム設定
	stateTimer = Mathf::RandomRange(3.0f, 5.0f);

	lookPlayer = false;

	//待機アニメーション再生
	model->PlayAnimation(Anim_IdleNormal, true);
}

//待機ステート更新処理
void EnemySlime::UpdateIdleState(float elapsedTime) {
	//タイマー処理
	stateTimer -= elapsedTime;
	if (stateTimer < 0.0f) {
		//徘徊ステートへ遷移
		TransitionWanderState();
	}

	//プレイヤー索敵
	if (SearchPlayer()) {
		//見つかったら追跡ステートへ遷移
		TransitionPursuitState();
	}

}

//追跡ステートへ遷移
void EnemySlime::TransitionPursuitState() {
	state = State::Pursuit;

	//数秒間追跡するタイマーをランダム設定
	stateTimer = Mathf::RandomRange(3.0f, 5.0f);

	lookPlayer = true;

	//歩きアニメーション再生
	model->PlayAnimation(Anim_RunFWD, true);

}

//追跡ステート更新処理
void EnemySlime::UpdatePursuitState(float elapsedTime) {
	//目標地点をプレイヤー位置に設定
	targetPosition = Player::Instance().GetPosition();

	//目標地点へ移動
	MoveToTarget(elapsedTime, 1.0f);

	//タイマー処理
	stateTimer -= elapsedTime;
	if (stateTimer < 0.0f) {
		//待機ステートへ遷移
		TransitionIdleState();
	}

	//プレイヤーに近づくと攻撃ステートへ遷移
	float vx = targetPosition.x - position.x;
	float vy = targetPosition.y - position.y;
	float vz = targetPosition.z - position.z;
	float dist = sqrtf(vx * vx + vy * vy + vz * vz);
	if (dist < attackRange) {
		//攻撃ステートへ遷移
		TransitionAttackState();
	}

}

//攻撃ステートへ遷移
void EnemySlime::TransitionAttackState() {
	state = State::Attack;

	lookPlayer = true;

	//攻撃アニメーション再生
	model->PlayAnimation(Anim_Attack1, false);

}

void EnemySlime::UpdateAttackState(float elapsedTime) {
	//任意のアニメーション再生区間のみ衝突判定処理をする
	float animationTime = model->GetCurrentAnimationSeconds();
	if (animationTime >= 0.1f && animationTime < 0.35f) {
		//目玉ノードとプレイヤーの衝突処理
		CollisionNodeVsPlayer("EyeBall", 1.0f);
	}

	//攻撃アニメーションが終ったら戦闘待機ステートへ遷移
	if (!model->IsPlayAnimation()) {
		TransitionIdleBattleState();
	}
}

void EnemySlime::TransitionIdleBattleState() {
	state = State::IdleBattle;
	//数秒間待機するタイマーをランダム設定
	stateTimer = Mathf::RandomRange(0.5f, 1.5f);
	lookPlayer = true;
	//戦闘待機アニメーション再生
	model->PlayAnimation(Anim_IdleBattle, true);
}

//戦闘待機ステート更新処理
void EnemySlime::UpdateIdleBattleState(float elapsedTime) {
	//目標地点をプレイヤー位置に設定
	targetPosition = Player::Instance().GetPosition();

	MoveToTarget(elapsedTime, turnSpeed);

	//タイマー処理
	stateTimer -= elapsedTime;

	 if (stateTimer < 0.0f) {
		//プレイヤーに近づくと攻撃ステートへ遷移
		float vx = targetPosition.x - position.x;
		float vy = targetPosition.y - position.y;
		float vz = targetPosition.z - position.z;
		float dist = sqrtf(vx * vx + vy * vy + vz * vz);
		Turn(elapsedTime, vx, vz, 1.0f);
		if (dist < attackRange) {
			//攻撃ステートへ遷移
			TransitionAttackState();
		}
		else {
			//徘徊ステートへ遷移
			TransitionWanderState();
		}
	}

	MoveToTarget(elapsedTime, 0.0f);

}

//ダメージステートへ遷移
void EnemySlime::TransitionDamageState() {
	state = State::Damage;
	lookPlayer = true;
	//ダメージアニメーション再生
	model->PlayAnimation(Anim_GetHit, false);
}

//ダメージステート更新処理
void EnemySlime::UpdateDamageState(float elapsedTime) {
	//ダメージアニメーションが終ったら戦闘待機ステートへ遷移
	if (!model->IsPlayAnimation()) {
		TransitionIdleBattleState();
	}
}

//死亡ステートへ遷移
void EnemySlime::TransitionDeathState() {
	state = State::Death;

	//死亡アニメーション再生
	model->PlayAnimation(Anim_Die, false);
}

//死亡ステート更新処理
void EnemySlime::UpdateDeathState(float elapsedTime) {
	//死亡アニメーションが終ったら自分を破棄
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