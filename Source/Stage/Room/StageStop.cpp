#include"StageStop.h"
#include"GimmickObj/GimmickManager.h"
#include"GimmickObj/Gimmick/Coin.h"


#define debug_new new(_NORMAL_BLOCK,__FILE__,__LINE__)

struct Spawn
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 angle;
	DirectX::XMFLOAT3 scale;
};
static Spawn CoinSpawn[] =
{
	{ DirectX::XMFLOAT3(1.5, 0, 1.5), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(1, 0, 1.5), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(0.5, 0, 1.5), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(0, 0, 1.5), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(-0.5, 0, 1.5), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(-1.0, 0, 1.5), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(-1.5, 0, 1.5), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },


	{ DirectX::XMFLOAT3(1.5, 0, 1), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(1, 0, 1), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(0.5, 0, 1), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(0, 0, 1), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(-0.5, 0, 1), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(-1.0, 0, 1), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(-1.5, 0, 1), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },

	{ DirectX::XMFLOAT3(1.5, 0, 0.5), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(1, 0, 0.5), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(0.5, 0, 0.5), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(0, 0, 0.5), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(-0.5, 0, 0.5), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(-1.0, 0, 0.5), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(-1.5, 0, 0.5), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },

	{ DirectX::XMFLOAT3(1.5, 0, 0), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(1, 0, 0), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(0.5, 0, 0), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(-0.5, 0, 0), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(-1.0, 0, 0), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(-1.5, 0, 0), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },

	{ DirectX::XMFLOAT3(1.5, 0, -0.5), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(1, 0, -0.5), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(0.5, 0, -0.5), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(0, 0, -0.5), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(-0.5, 0, -0.5), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(-1.0, 0, -0.5), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(-1.5, 0, -0.5), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },

	{ DirectX::XMFLOAT3(1.5, 0, -1), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(1, 0, -1), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(0.5, 0, -1), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(0, 0, -1), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(-0.5, 0, -1), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(-1.0, 0, -1), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(-1.5, 0, -1), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },

	{ DirectX::XMFLOAT3(1.5, 0, -1.5), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(1, 0, -1.5), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(0.5, 0, -1.5), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(0, 0, -1.5), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(-0.5, 0, -1.5), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(-1.0, 0, -1.5), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(-1.5, 0, -1.5), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
};

StageStop::StageStop() {

	scale = { 50.0f,50.0f,50.0f };


	model = new Model("Data/Model/StageModel/Room/Dungeon.mdl");


}

StageStop::~StageStop() {


	delete model;

}

void StageStop::Update(float elapsedTime) {
	UpdateTransform();

	if (!OnSpawn)
	{

		for (int i = 0; i<49;i++)
		{

			{
				DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&transform);
				DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(CoinSpawn[i].position.x, CoinSpawn[i].position.y, CoinSpawn[i].position.z);
				DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(CoinSpawn[i].angle.x, CoinSpawn[i].angle.y, CoinSpawn[i].angle.z);
				DirectX::XMMATRIX P = R * T * W;
				DirectX::XMStoreFloat4x4(&spawnTransform, P);
			}
			Coin* coin = new Coin();
			coin->SetPosition({ spawnTransform._41,spawnTransform._42,spawnTransform._43 });
			coin->SetAngle({ 0,0,0 });
			GimmickManager::Instance().Register(coin);
		}
		OnSpawn = true;
	}
}

void StageStop::Render(ID3D11DeviceContext* dc, Shader* shader) {

	if (model != nullptr) {
		shader->Draw(dc, model);
	}


}

bool StageStop::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) {
	if (model != nullptr) {
		return Collision::IntersectRayVsModel(start, end, model, hit);
	}

	/*return nullptr;*/
}

//スフィアキャスト
bool StageStop::SphereCast(const DirectX::XMFLOAT3& start, const float radius, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	if (model != nullptr) {
		return Collision::SphereCast(start, end, radius, model, hit);
	}

	return nullptr;
}


//行列更新処理
void StageStop::UpdateTransform() {
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMMATRIX W = S * R * T;
	DirectX::XMStoreFloat4x4(&transform, W);


	if (model != nullptr) {
		model->UpdateTransform(transform);
	}

}

void StageStop::DrawDebugGUI() {

}

void StageStop::DrawDebugPrimitive() {

}