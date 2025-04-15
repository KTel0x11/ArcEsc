#include<imgui.h>
#include<algorithm>
#include"Graphics/Graphics.h"
#include "SeesawFloor.h"
#include "Chara/Player.h"
#include"GameState.h"
#define debug_new new(_NORMAL_BLOCK,__FILE__,__LINE__)




SeesawFloor::SeesawFloor() {
	scale.x =  40.0f;
	scale.y = 5.0f;
	scale.z = 100.0f;
	//ステージモデルを読み込み
	model = new Model("Data/Model/Cube/SandCube.mdl");
}




SeesawFloor::~SeesawFloor() {
	//ステージモデルを破棄
	if (model != nullptr) {
		delete model;
		model = nullptr;
	}

}


//更新処理
void SeesawFloor::Update(float elapsedTime) {
	//前回の情報を保存
	oldTransform = transform;
	//行列更新
	UpdateTransform();

	
	if (GameState::Instance().currentState == GameState::State::Game) {

		// シーソーの傾き更新
		if (hitFlg) { // プレイヤーが乗っている場合
			isPlayerOnSeesaw = true;
			timeSincePlayerLeft = 0.0f; // 離れてからの時間をリセット

			// 傾き更新
			angle.x += DirectX::XMConvertToRadians(HitPosition.z * moveSpeed * elapsedTime);
			angle.x = std::clamp(angle.x, -5.0f, 5.0f); // 最大傾き制限
		}
		else {
			if (isPlayerOnSeesaw) {
				// プレイヤーが離れた場合、経過時間を加算
				timeSincePlayerLeft += elapsedTime;
				if (timeSincePlayerLeft >= resetDelay) {
					isPlayerOnSeesaw = false; // 一定時間後にリセットモードへ
				}
			}

			// プレイヤーが離れた後、徐々に元の角度に戻る
			if (!isPlayerOnSeesaw) {
				angle.x = Lerp(angle.x, initialAngle.x, resetSpeed * elapsedTime);
			}
		}

	}

	//レイキャスト用にモデル空間行列にするため単位行列を渡す
	const DirectX::XMFLOAT4X4 transformIdentity = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
	model->UpdateTransform(transformIdentity);

}

//描画処理
void SeesawFloor::Render(ID3D11DeviceContext* dc, Shader* shader) {

	//DrawDebugGUI();
	//表示用のためワールド行列に更新する
	model->UpdateTransform(transform);

	shader->Draw(dc, model);

}

//レイキャスト
bool SeesawFloor::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) {

	//前回のワールド行列と逆行列を求める
	DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&oldTransform);
	DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);

	//前回のローカル空間でのレイに変換
	DirectX::XMVECTOR WorldStart = DirectX::XMLoadFloat3(&start);
	DirectX::XMVECTOR WorldEnd = DirectX::XMLoadFloat3(&end);
	DirectX::XMVECTOR LocalStart = DirectX::XMVector3TransformCoord(WorldStart, InverseWorldTransform);
	DirectX::XMVECTOR LocalEnd = DirectX::XMVector3TransformCoord(WorldEnd, InverseWorldTransform);

	DirectX::XMFLOAT3 localStart, localEnd;
	DirectX::XMStoreFloat3(&localStart, LocalStart);
	DirectX::XMStoreFloat3(&localEnd, LocalEnd);

	HitResult localHit;
	if (Collision::IntersectRayVsModel(localStart, localEnd, model, localHit)) {
		
		WorldTransform = DirectX::XMLoadFloat4x4(&transform);
		DirectX::XMVECTOR LocalPosition = DirectX::XMLoadFloat3(&localHit.position);
		DirectX::XMVECTOR WorldPosition = DirectX::XMVector3TransformCoord(LocalPosition, WorldTransform);

		DirectX::XMVECTOR LocalNormal = DirectX::XMLoadFloat3(&localHit.normal);
		DirectX::XMVECTOR WorldNormal = DirectX::XMVector3TransformNormal(LocalNormal, WorldTransform);

		// currentNormal を適用
		DirectX::XMVECTOR AdjustedNormal = DirectX::XMLoadFloat3(&currentNormal);

		DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(WorldPosition, WorldStart);
		DirectX::XMVECTOR Dist = DirectX::XMVector3Length(Vec);
		DirectX::XMStoreFloat3(&hit.position, WorldPosition);
		DirectX::XMStoreFloat3(&hit.normal, DirectX::XMVector3Normalize(AdjustedNormal));
		DirectX::XMStoreFloat(&hit.distance, Dist);
		hit.materialIndex = localHit.materialIndex;
		HitPosition = localHit.position;

		hitFlg = true;
	}
	else {

	hitFlg = false;

	}
	return hitFlg;
}

//スフィアキャスト
bool SeesawFloor::SphereCast(const DirectX::XMFLOAT3& start, const float radius, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	if (model != nullptr) {
		return Collision::SphereCast(start, end, radius, model, hit);
	}

	return nullptr;
}




//行列更新処理
void SeesawFloor::UpdateTransform() {
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMMATRIX W = S * R * T;
	DirectX::XMStoreFloat4x4(&transform, W);

	// 法線の更新
	DirectX::XMVECTOR defaultNormal = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // デフォルト法線 (Y軸)
	DirectX::XMVECTOR rotatedNormal = DirectX::XMVector3TransformNormal(defaultNormal, R); // 回転適用
	DirectX::XMStoreFloat3(&currentNormal, rotatedNormal); // 更新された法線を保存
}



void SeesawFloor::DrawDebugGUI()
{
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Gimmick", nullptr, ImGuiWindowFlags_None)) {
		if (ImGui::CollapsingHeader("SeesawFloor", ImGuiTreeNodeFlags_DefaultOpen)) {
			if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {


				ImGui::InputFloat3("position", &position.x);
				ImGui::InputFloat3("Scale", &scale.x);
				ImGui::InputFloat3("Angle", &angle.x);

			}
			if (ImGui::CollapsingHeader("RayCast Result")) {
				ImGui::Text("Hit: %s", hitFlg ? "True" : "False");
				ImGui::Text("Hit Position: (%f, %f, %f)", HitPosition.x, HitPosition.y, HitPosition.z);
			}
		}
	}


	ImGui::End();
}

void SeesawFloor::DrawDebugPrimitive() {
	Graphics::Instance().GetDebugRenderer()->DrawBox(position, scale, { 0,0,0,1 }, angle);
}

