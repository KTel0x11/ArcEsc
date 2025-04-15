#include "StageStart.h"
#define debug_new new(_NORMAL_BLOCK,__FILE__,__LINE__)

struct Test
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 angle;
	DirectX::XMFLOAT3 scale;
};
static Test tests[] =
{
	{ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
};

StageStart::StageStart() {

	scale = { 50.0f,50.0f,50.0f };

	model = new Model("Data/Model/StageModel/Room/Dungeon.mdl");


	DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&transform);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(tests[0].position.x, tests[0].position.y, tests[0].position.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(tests[0].angle.x, tests[0].angle.y, tests[0].angle.z);
	DirectX::XMMATRIX P = R * T * W;

}

StageStart::~StageStart() {


	delete model;

}

void StageStart::Update(float elapsedTime) {
	UpdateTransform();
}

void StageStart::Render(ID3D11DeviceContext* dc, Shader* shader) {

	if (model != nullptr) {
		shader->Draw(dc, model);
	}


}

bool StageStart::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) {
	if (model != nullptr) {
		return Collision::IntersectRayVsModel(start, end, model, hit);
	}

	/*return nullptr;*/
}

//スフィアキャスト
bool StageStart::SphereCast(const DirectX::XMFLOAT3& start, const float radius, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	if (model != nullptr) {
		return Collision::SphereCast(start, end, radius, model, hit);
	}

	return nullptr;
}


//行列更新処理
void StageStart::UpdateTransform() {
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMMATRIX W = S * R * T;
	DirectX::XMStoreFloat4x4(&transform, W);


	if (model != nullptr) {
		model->UpdateTransform(transform);
	}

}

void StageStart::DrawDebugGUI() {

}

void StageStart::DrawDebugPrimitive() {

}