#include<imgui.h>
#include "StageFall.h"
#include "Stage/StageManager.h"
#include "Stage//StageMoveFloor.h"
#include "Stage/SeesawFloor.h"
#define debug_new new(_NORMAL_BLOCK,__FILE__,__LINE__)

struct Gimmick
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 angle;
	DirectX::XMFLOAT3 scale;
};



static Gimmick moveFloor[] =
{
	{ DirectX::XMFLOAT3(1.0, 0, 0), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(-1.0, 0, 0), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
};



StageFall::StageFall() {

	scale = { 50.0f,50.0f,50.0f };

	model = new Model("Data/Model/StageModel/Room/DungeonFall.mdl");
}

StageFall::~StageFall() {

	if (model != nullptr) {
		delete model;
	}

}

void StageFall::Update(float elapsedTime) {
	UpdateTransform();

	if(!OnFloor)
	{

		if(angle.y==0||angle.y/180==0)
		{
			StageMoveFloor* stageMF = new StageMoveFloor();

			//test[0]のローカル座標をワールド座標に変換し移動床スタート位置とする
			{
				DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&transform);
				DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(moveFloor[START].position.x, moveFloor[START].position.y, moveFloor[START].position.z);
				DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(moveFloor[START].angle.x, moveFloor[START].angle.y, moveFloor[START].angle.z);
				DirectX::XMMATRIX P = R * T * W;
				DirectX::XMStoreFloat4x4(&gimmickTransform[START], P);
			}
			stageMF->SetStartPoint({ gimmickTransform[START]._41,gimmickTransform[START]._42,gimmickTransform[START]._43 });
			{
				DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&transform);
				DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(moveFloor[GOAL].position.x, moveFloor[GOAL].position.y, moveFloor[GOAL].position.z);
				DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(moveFloor[GOAL].angle.x, moveFloor[GOAL].angle.y, moveFloor[GOAL].angle.z);
				DirectX::XMMATRIX P = R * T * W;
				DirectX::XMStoreFloat4x4(&gimmickTransform[GOAL], P);
			}
			stageMF->SetGoalPoint({ gimmickTransform[GOAL]._41,gimmickTransform[GOAL]._42,gimmickTransform[GOAL]._43 });

			StageManager::Instance().Register(stageMF);
		}
		else
		{
			SeesawFloor* stageSF = new SeesawFloor();
				{
					DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&transform);
					DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(moveFloor[2].position.x, moveFloor[2].position.y, moveFloor[2].position.z);
					DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(moveFloor[2].angle.x, moveFloor[2].angle.y, moveFloor[2].angle.z);
					DirectX::XMMATRIX P = R * T * W;
					DirectX::XMStoreFloat4x4(&gimmickTransform[2], P);
				}
				stageSF->SetPosition({ gimmickTransform[2]._41,gimmickTransform[2]._42,gimmickTransform[2]._43 });
			
				StageManager::Instance().Register(stageSF);
		}
		OnFloor = true;
	}
}

void StageFall::Render(ID3D11DeviceContext* dc, Shader* shader) {

	if (model != nullptr) {
		shader->Draw(dc, model);
	}


}

bool StageFall::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) {
	if (model != nullptr) {
		return Collision::IntersectRayVsModel(start, end, model, hit);
	}

	/*return nullptr;*/
}

//スフィアキャスト
bool StageFall::SphereCast(const DirectX::XMFLOAT3& start, const float radius, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	if (model != nullptr) {
		return Collision::SphereCast(start, end, radius, model, hit);
	}

	return nullptr;
}


//行列更新処理
void StageFall::UpdateTransform() {
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMMATRIX W = S * R * T;
	DirectX::XMStoreFloat4x4(&transform, W);


	if (model != nullptr) {
		model->UpdateTransform(transform);
	}

}

void StageFall::DrawDebugGUI() {

	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Room", nullptr, ImGuiWindowFlags_None)) {

		if (ImGui::CollapsingHeader("Fall", ImGuiTreeNodeFlags_DefaultOpen)) {
			if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {


				ImGui::InputFloat3("Position", &position.x);



			}
		}
	}

	ImGui::End();

}

void StageFall::DrawDebugPrimitive() {

}