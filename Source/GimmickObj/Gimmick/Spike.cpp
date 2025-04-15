#include<imgui.h>
#include"Spike.h"
#include"Graphics/Graphics.h"
#include"Chara/Collision.h"
#include"Chara/Player.h"

Spike::Spike() {
	scale = { 10,10,10 };
	radius = 2;
	model = new Model("Data/Model/OBJ/Spike.mdl");
}


Spike::~Spike() {

	if (model != nullptr) {
		delete model;
		model = nullptr;
	}

}

void  Spike::Update(float elapsedTime) {

	UpdateTransform();
	
	//Õ“Ëˆ—
	DirectX::XMFLOAT3 outPosition;

	if (Collision::IntersectCylinderVsCylinder(
		position,
		radius,
		height,
		Player::Instance().GetPosition(),
		Player::Instance().GetRadius(),
		Player::Instance().GetHeight(),
		outPosition)) {
		DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&Player::Instance().GetPosition());
		DirectX::XMVECTOR O = DirectX::XMLoadFloat3(&position);
		DirectX::XMVECTOR V = DirectX::XMVectorSubtract(P,O);
		DirectX::XMVECTOR N = DirectX::XMVector3Normalize(V);
		DirectX::XMFLOAT3 normal;
		DirectX::XMStoreFloat3(&normal, N);
		if (normal.y > 0.3f) {
			Player::Instance().ApplyDamage(10, 1);
		}
		else
		{
			//‰Ÿ‚µo‚µŒã‚ÌˆÊ’uÝ’è
			Player::Instance().SetPosition(outPosition);

		}
	}


}


void Spike::Render(ID3D11DeviceContext* dc, Shader* shader) {
	//ƒ‚ƒfƒ‹•`‰æ
	shader->Draw(dc, model);
}

bool Spike::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) {
	return nullptr;
}


void Spike::DrawDebugGUI() {
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Gimmick", nullptr, ImGuiWindowFlags_None)) {

		if (ImGui::CollapsingHeader("Spike", ImGuiTreeNodeFlags_DefaultOpen)) {
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

void Spike::DrawDebugPrimitive() {
	Graphics::Instance().GetDebugRenderer()->DrawCylinder(position, radius, height, { 1,0,0,1 },{0,0,0});
}

void Spike::UpdateTransform() {
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMMATRIX W = S * R * T;
	DirectX::XMStoreFloat4x4(&transform, W);


	if (model != nullptr) {
		model->UpdateTransform(transform);
	}

}