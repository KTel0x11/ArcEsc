#include<imgui.h>
#include<time.h>
#include <stdlib.h>
#include <stdio.h>

#include"Key.h"
#include"Graphics/Graphics.h"
#include"Chara/Collision.h"
#include"Chara/Player.h"
#include"Audio/Audio.h"
#include"Audio/AudioSource.h"

Key::Key() {
	//scale = { 5,5,5 };
	scale = { 100,100,100 };
	angle = { 0,0,0 };
	radius = 4;
	model = new Model("Data/Model/OBJ/Key.mdl");
	Player::Instance().spawnKey = true;
	CoinSE = Audio::Instance().LoadAudioSource("Data/Audio/CoinSE.wav");

}


Key::~Key() {

	if (model != nullptr) {
		delete model;
		model = nullptr;
	}

	delete CoinSE;

}

void  Key::Update(float elapsedTime) {
	UpdateTransform();


	angle.y += moveSpeed * elapsedTime;//モデルを回転させる


	if (destroyFlg) {//もしフラグがtrueだったら
		destroyTime += elapsedTime;//タイマー作動

		position.y = 5.0f + -10.0f * (1.0f - destroyTime * moveSpeed) * (1.0f - destroyTime * moveSpeed);

	}
	if (destroyTime >= 2.0f) {//タイマーが10秒過ぎたら
		Destroy();//削除
	}

	//衝突処理
	DirectX::XMFLOAT3 outPosition;

	if (Collision::IntersectCylinderVsCylinder(//もしプレイヤーとコインが当たっていてかつフラグがfalseだったら
		position,
		radius,
		height,
		Player::Instance().GetPosition(),
		Player::Instance().GetRadius(),
		Player::Instance().GetHeight(),
		outPosition) && !destroyFlg) {
		CoinSE->Play(false);//SEを再生
		destroyFlg = true;//フラグをtrueに変更
		fastPositionY = position.y;
		Player::Instance().haveKey = true;
	}


}


void Key::Render(ID3D11DeviceContext* dc, Shader* shader) {
	//モデル描画
	if (destroyTime <= 2.0f) {//もし10秒過ぎたらモデルを消す。
		shader->Draw(dc, model);
	}
}


bool Key::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) {
	return nullptr;
}


void Key::DrawDebugGUI() {
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Gimmick", nullptr, ImGuiWindowFlags_None)) {

		if (ImGui::CollapsingHeader("Key", ImGuiTreeNodeFlags_DefaultOpen)) {
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

		}

	}

	ImGui::End();
}

void Key::DrawDebugPrimitive() {
	Graphics::Instance().GetDebugRenderer()->DrawCylinder(position, radius, height, { 1,0,0,1 }, { 0,0,0 });
}

void Key::UpdateTransform() {
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMMATRIX W = S * R * T;
	DirectX::XMStoreFloat4x4(&transform, W);


	if (model != nullptr) {
		model->UpdateTransform(transform);
	}

}