#include<time.h>
#include <stdlib.h>
#include <stdio.h>
#include<imgui.h>
#include "StageCurve.h"
#include"Chara/EnemyManager.h"
#include"Chara/EnemySlime.h"
#define debug_new new(_NORMAL_BLOCK,__FILE__,__LINE__)



struct Spawn
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 angle;
	DirectX::XMFLOAT3 scale;
};
static Spawn enemy[] =
{
	{ DirectX::XMFLOAT3(-1.45, 0.25, 1.45), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(1.35, 0.25, 1.45), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
};

StageCurve::StageCurve() {

	scale = { 50.0f,50.0f,50.0f };


	model = new Model("Data/Model/StageModel/Room/DungeonCurve.mdl");


}

StageCurve::~StageCurve() {


	delete model;

}

void StageCurve::Update(float elapsedTime) {
	UpdateTransform();
	srand((unsigned int)time(NULL));
	if (!OnEnemy)
	{


		{
		
			rnd = rand() % 2;
			//test[0]のローカル座標をワールド座標に変換し移動床スタート位置とする

			{
				DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&transform);
				DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(enemy[rnd].position.x, enemy[rnd].position.y, enemy[rnd].position.z);
				DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(enemy[rnd].angle.x, enemy[rnd].angle.y, enemy[rnd].angle.z);
				DirectX::XMMATRIX P = R * T * W;
				DirectX::XMStoreFloat4x4(&spawnTransform, P);
			}
			EnemySlime* slime = new EnemySlime();
			slime->SetPosition({ spawnTransform._41, spawnTransform._42, spawnTransform._43 });
			slime->SetTerritory(slime->GetPosition(), 15.0f);
			EnemyManager::Instance().Register(slime);
		}
		OnEnemy = true;
	}
}

void StageCurve::Render(ID3D11DeviceContext* dc, Shader* shader) {

	if (model != nullptr) {
		shader->Draw(dc, model);
	}

	//DrawDebugGUI();
}

bool StageCurve::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) {
	if (model != nullptr) {
		return Collision::IntersectRayVsModel(start, end, model, hit);
	}

	/*return nullptr;*/
}

//スフィアキャスト
bool StageCurve::SphereCast(const DirectX::XMFLOAT3& start, const float radius, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	if (model != nullptr) {
		return Collision::SphereCast(start, end, radius, model, hit);
	}

	return nullptr;
}


//行列更新処理
void StageCurve::UpdateTransform() {
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMMATRIX W = S * R * T;
	DirectX::XMStoreFloat4x4(&transform, W);


	if (model != nullptr) {
		model->UpdateTransform(transform);
	}

}

void StageCurve::DrawDebugGUI() {
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None)) {

		if (ImGui::CollapsingHeader("Menu", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::InputInt("RND", &rnd);


		}

	}

	ImGui::End();
}

void StageCurve::DrawDebugPrimitive() {

}