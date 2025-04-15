#include "StageMain.h"
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

StageMain::StageMain(Room::RoomType Type) {

	scale = { 50.0f,50.0f,50.0f };

	switch (Type)
	{
	case Room::RoomType::EMPTY:
		model = new Model("Data/Model/StageModel/Room/Dungeon.mdl");
		break;
	case Room::RoomType::START:
		model = new Model("Data/Model/StageModel/Room/Dungeon.mdl");
		break;
	case Room::RoomType::GOAL:
		model = new Model("Data/Model/StageModel/Room/Dungeon.mdl");
		break;
	case Room::RoomType::STOPROOM:
		break;
	case Room::RoomType::CURVE:
		model = new Model("Data/Model/StageModel/Room/Dungeon1.mdl");
		break;
	case Room::RoomType::CROSS:
		scale = { 50.0f,50.0f,50.0f };
		model = new Model("Data/Model/StageModel/Room/DungeonCross.mdl");
		break;
	case Room::RoomType::MAX:
		break;
	default:
		break;
	}

	DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&transform);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(tests[0].position.x, tests[0].position.y, tests[0].position.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(tests[0].angle.x, tests[0].angle.y, tests[0].angle.z);
	DirectX::XMMATRIX P = R * T * W;

}

StageMain::	~StageMain() {

	 
	delete model;
	
}

void StageMain:: Update(float elapsedTime) {
	UpdateTransform();
}

void StageMain::Render(ID3D11DeviceContext* dc, Shader* shader) {

	if (model != nullptr) {
		shader->Draw(dc, model);
	}


}

bool StageMain::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) {
	if (model != nullptr) {
		return Collision::IntersectRayVsModel(start, end, model, hit);
	}

	/*return nullptr;*/
}

//s—ñXVˆ—
void StageMain::UpdateTransform() {
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMMATRIX W = S * R * T;
	DirectX::XMStoreFloat4x4(&transform, W);


	if (model != nullptr) {
		model->UpdateTransform(transform);
	}

}

void StageMain::DrawDebugGUI() {

}