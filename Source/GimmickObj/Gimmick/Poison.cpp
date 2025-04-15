#include<imgui.h>
#include"Poison.h"
#include"Graphics/Graphics.h"
#include"Chara/Collision.h"
#include"Chara/Player.h"

Poison::Poison() {
	//scale = { 10,10,10 };

	model = new Model("Data/Model/OBJ/Poison.mdl");
}


Poison::~Poison() {

	if (model != nullptr) {
		delete model;
		model = nullptr;
	}

}

void  Poison::Update(float elapsedTime) {

	UpdateTransform();

	//Õ“Ëˆ—
	DirectX::XMFLOAT3 outPosition;

	//Player::Instance().OnPoison = Collision::IntersectCylinderVsCylinder(
	//	position,
	//	scale.x,
	//	height,
	//	Player::Instance().GetPosition(),
	//	Player::Instance().GetRadius(),
	//	Player::Instance().GetHeight(),
	//	outPosition);



}


void Poison::Render(ID3D11DeviceContext* dc, Shader* shader) {
	//ƒ‚ƒfƒ‹•`‰æ
	shader->Draw(dc, model);
}

void Poison::DrawDebugGUI() {
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Gimmick", nullptr, ImGuiWindowFlags_None)) {

		if (ImGui::CollapsingHeader("Poison", ImGuiTreeNodeFlags_DefaultOpen)) {
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

bool Poison::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) {
	if (model != nullptr) {
		return Collision::IntersectRayVsModel(start, end, model, hit);
	}

	return nullptr;
}

void Poison::DrawDebugPrimitive() {
	Graphics::Instance().GetDebugRenderer()->DrawCylinder(position, scale.x, height, { 1,0,0,1 }, { 0,0,0 });
}

void Poison::UpdateTransform() {
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMMATRIX W = S * R * T;
	DirectX::XMStoreFloat4x4(&transform, W);


	if (model != nullptr) {
		model->UpdateTransform(transform);
	}

}