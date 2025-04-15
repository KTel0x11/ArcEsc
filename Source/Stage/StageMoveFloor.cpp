#include<imgui.h>
#include "StageMoveFloor.h"
#include"GameState.h"
#define debug_new new(_NORMAL_BLOCK,__FILE__,__LINE__)




StageMoveFloor::StageMoveFloor() {
	scale.x = scale.z = 40.0f;
	scale.y = 5.0f;
	//ステージモデルを読み込み
	model = new Model("Data/Model/Cube/SandCube.mdl");
}




StageMoveFloor::~StageMoveFloor() {
	//ステージモデルを破棄
	
	delete model;
	
}


//更新処理
void StageMoveFloor::Update(float elapsedTime) {

	//前回の情報を保存
	oldTransform = transform;
	oldAngle = angle;

	//スタートからゴールまでの距離を算出する
	DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start);
	DirectX::XMVECTOR Goal = DirectX::XMLoadFloat3(&goal);
	DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(Goal, Start);
	DirectX::XMVECTOR Length = DirectX::XMVector3Length(Vec);

	float length;
	DirectX::XMStoreFloat(&length, Length);

	if (GameState::Instance().currentState == GameState::State::Game) {

		//スタートからゴールまでの間を一秒間で進む割合(0.0~1.0)を算出する
		float speed = moveSpeed * elapsedTime;
		float speedRate = speed / length;
		moveRate += speedRate;

		//ゴールに到達、またはスタートに戻った場合、移動方向を反転させる
		if (moveRate <= 0.0f || moveRate >= 1.0f) {
			moveSpeed = -moveSpeed;
		}



		//線形補完で位置を算出する
		DirectX::XMVECTOR Position = DirectX::XMVectorLerp(Start, Goal, moveRate);
		DirectX::XMStoreFloat3(&position, Position);

		//回転
		angle.x += torque.x * elapsedTime;
		angle.y += torque.y * elapsedTime;
		angle.z += torque.z * elapsedTime;

	}
	//行列更新
	UpdateTransform();

	//レイキャスト用にモデル空間行列にするため単位行列を渡す
	const DirectX::XMFLOAT4X4 transformIdentity = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
	model->UpdateTransform(transformIdentity);

}

//描画処理
void StageMoveFloor::Render(ID3D11DeviceContext* dc, Shader* shader) {

	//表示用のためワールド行列に更新する
	model->UpdateTransform(transform);

	shader->Draw(dc, model);

}

//レイキャスト
bool StageMoveFloor::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) {

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

		DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(WorldPosition, WorldStart);
		DirectX::XMVECTOR Dist = DirectX::XMVector3Length(Vec);
		DirectX::XMStoreFloat3(&hit.position, WorldPosition);
		DirectX::XMStoreFloat3(&hit.normal, DirectX::XMVector3Normalize(WorldNormal));
		DirectX::XMStoreFloat(&hit.distance, Dist);
		hit.materialIndex = localHit.materialIndex;
		HitPosition = localHit.position;

		//回転差分を算出
		hit.rotation.x = angle.x - oldAngle.x;
		hit.rotation.y = angle.y - oldAngle.y;
		hit.rotation.z = angle.z - oldAngle.z;
		
		hitFlg = true;
		return true;
	}

	hitFlg = false;
	return false;
}

//スフィアキャスト
bool StageMoveFloor::SphereCast(const DirectX::XMFLOAT3& start, const float radius, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	if (model != nullptr) {
		return Collision::SphereCast(start, end, radius, model, hit);
	}

	return nullptr;
}




//行列更新処理
void StageMoveFloor::UpdateTransform() {
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMMATRIX W = S * R * T;
	DirectX::XMStoreFloat4x4(&transform, W);
}



void StageMoveFloor::DrawDebugGUI()
{
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Gimmick", nullptr, ImGuiWindowFlags_None)) {
		if (ImGui::CollapsingHeader("MoveFloor", ImGuiTreeNodeFlags_DefaultOpen)) {
			if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {


				ImGui::InputFloat3("position", &position.x);


			}
			if (ImGui::CollapsingHeader("RayCast Result")) {
				ImGui::Text("Hit: %s", hitFlg ? "True" : "False");
				ImGui::Text("Hit Position: (%f, %f, %f)", HitPosition.x, HitPosition.y, HitPosition.z);
			}
		}
	}
	

	ImGui::End();
}

void StageMoveFloor::DrawDebugPrimitive() {

}