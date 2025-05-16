#include "StageGoal.h"
#include"Graphics/Graphics.h"
#include"Scene/SceneManager.h"
#include"Scene/SceneTitle.h"
#include "Scene/SceneLoading.h"
#include"Chara/Collision.h"
#include"Chara/Player.h"
#define debug_new new(_NORMAL_BLOCK,__FILE__,__LINE__)

struct Spawn
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 angle;
	DirectX::XMFLOAT3 scale;
};


static Spawn Goal={ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) };

StageGoal::StageGoal() {

	scale = { 50.0f,50.0f,50.0f };


	model = new Model("Data/Model/StageModel/Room/DungeonGoal.mdl");


}

StageGoal::~StageGoal() {


	delete model;

}

void StageGoal::Update(float elapsedTime) {

	UpdateTransform();

	CollisionPlayerVsGoal();

	if (!OnGoal)
	{


		{
	
			//test[0]のローカル座標をワールド座標に変換し移動床スタート位置とする
			{
				DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&transform);
				DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(Goal.position.x, Goal.position.y, Goal.position.z);
				DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(Goal.angle.x, Goal.angle.y, Goal.angle.z);
				DirectX::XMMATRIX P = R * T * W;
				DirectX::XMStoreFloat4x4(&GoalGimmickTransform, P);
			}
			
			goalPosition={ GoalGimmickTransform._41, GoalGimmickTransform._42, GoalGimmickTransform._43 };
	
		}
		OnGoal = true;
	}
}

void StageGoal::Render(ID3D11DeviceContext* dc, Shader* shader) {

	if (model != nullptr) {
		shader->Draw(dc, model);
	}


}

bool StageGoal::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) {
	if (model != nullptr) {
		if (Collision::IntersectRayVsModel(start, end, model, hit)) {
			Player::Instance().SetAxis(axis);

			return true;
		}
		else {
			return false;
		}
	}
	else {
		return nullptr;
	}
}

//スフィアキャスト
bool StageGoal::SphereCast(const DirectX::XMFLOAT3& start, const float radius, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	if (model != nullptr) {
		return Collision::SphereCast(start, end, radius, model, hit);
	}

	return nullptr;
}


//行列更新処理
void StageGoal::UpdateTransform() {
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMMATRIX W = S * R * T;
	DirectX::XMStoreFloat4x4(&transform, W);


	if (model != nullptr) {
		model->UpdateTransform(transform);
	}

}

void StageGoal::CollisionPlayerVsGoal() {

	//衝突処理
	DirectX::XMFLOAT3 outPosition;

	if (Collision::IntersectCylinderVsCylinder(
		goalPosition,
		radius,
		height,
		Player::Instance().GetPosition(),
		Player::Instance().GetRadius(),
		Player::Instance().GetHeight(),
		outPosition)) {
		if (Player::Instance().haveKey) {
			Player::Instance().clear = true;
		}
	}

}

void StageGoal::DrawDebugGUI() {

}

void StageGoal::DrawDebugPrimitive() {
	DebugRenderer* debugRender = Graphics::Instance().GetDebugRenderer();
	debugRender->DrawCylinder(goalPosition, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1),{0,0,0});
}