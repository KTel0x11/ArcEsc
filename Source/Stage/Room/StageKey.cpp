#include<time.h>
#include <stdlib.h>
#include <stdio.h>
#include "StageKey.h"
#include"GimmickObj/GimmickManager.h"
#include"GimmickObj/Gimmick/Spike.h"
#include"GimmickObj/Gimmick/Poison.h"
#include"GimmickObj/Gimmick/Coin.h"
#include"GimmickObj/Gimmick/Key.h"
#include"Stage/StageTree.h"
#include"Stage/StageManager.h"
#include"Chara/EnemyManager.h"
#include"Chara/EnemySlime.h"
#define debug_new new(_NORMAL_BLOCK,__FILE__,__LINE__)


struct Spawn
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 angle;
	DirectX::XMFLOAT3 scale;
};

static Spawn TreeSpawn[] =
{
	{DirectX::XMFLOAT3(0,0,0), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0,0,0)},
	{DirectX::XMFLOAT3(0.5,0,0.5), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0,0,0)},
	{DirectX::XMFLOAT3(-0.5,0,0.5), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0,0,0)},
	{DirectX::XMFLOAT3(-0.5,0,-0.5), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0,0,0)},
	{DirectX::XMFLOAT3(0.5,0,-0.5), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0,0,0)},
	{DirectX::XMFLOAT3(1.5,0,0), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0,0,0)},
	{DirectX::XMFLOAT3(0,0,1.5), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0,0,0)},
	{DirectX::XMFLOAT3(-1.5,0,0), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0,0,0)},
	{DirectX::XMFLOAT3(0,0,-1.5), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0,0,0)},
};

float TreeHeight[] = {
	30,
	20,
	20,
	20,
	20,
	10,
	10,
	10,
	10,
};


static Spawn CoinSpawn[] =
{
	{ DirectX::XMFLOAT3(1, 0,1), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(-1, 0, 1), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(-1, 0, -1), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(1, 0, -1), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
};

static Spawn SpikeSpawn[] =
{
	{ DirectX::XMFLOAT3(0, 0.02f ,0), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(30, 2, 30) },
	{ DirectX::XMFLOAT3(0, 0.01f, 2), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(15, 1, 15) },
	{ DirectX::XMFLOAT3(-2,0.01f, 0), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(15, 1, 15) },
	{ DirectX::XMFLOAT3(0, 0.01f, -2), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(15, 1, 15) },
	{ DirectX::XMFLOAT3(2,0.01f, 0), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(15, 1, 15) },

};

static Spawn KeySpawn = { DirectX::XMFLOAT3(0, 5, 0), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) };


StageKey::StageKey() {

	scale = { 50.0f,50.0f,50.0f };

	model = new Model("Data/Model/StageModel/Room/DungeonCross.mdl");

}

StageKey::~StageKey() {


	delete model;

}

void StageKey::Update(float elapsedTime) {
	srand((unsigned int)time(NULL));
	UpdateTransform();

	if (!OnSpawn)
	{
		for(int i = 0; i < 9; i++)
		{
			{
				DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&transform);
				DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(TreeSpawn[i].position.x, TreeSpawn[i].position.y, TreeSpawn[i].position.z);
				DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(TreeSpawn[i].angle.x, TreeSpawn[i].angle.y, TreeSpawn[i].angle.z);
				DirectX::XMMATRIX P = R * T * W;
				DirectX::XMStoreFloat4x4(&spawnTransform, P);
			}
			StageTree* tree = new StageTree(TreeHeight[i]);
			tree->SetPosition({ spawnTransform._41,spawnTransform._42,spawnTransform._43 });
			StageManager::Instance().Register(tree);

			if (i == 0) {
				Key* key = new Key();
				key->SetPosition({ spawnTransform._41, 60.0f ,spawnTransform._43});
				GimmickManager::Instance().Register(key);
			}


		}



		{

			//コイン生成処理
			for (int i = 0; i < 5; i++) {

				int rnd = rand() % 5;

				if (rnd == 0 || rnd == 2) {
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
			}


			{
				DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&transform);
				DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(CoinSpawn[1].position.x, CoinSpawn[1].position.y, CoinSpawn[1].position.z);
				DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(CoinSpawn[1].angle.x, CoinSpawn[1].angle.y, CoinSpawn[1].angle.z);
				DirectX::XMMATRIX P = R * T * W;
				DirectX::XMStoreFloat4x4(&spawnTransform, P);
			}
			Spike* spike = new Spike();
			spike->SetPosition({ spawnTransform._41,spawnTransform._42,spawnTransform._43 });
			spike->SetAngle({ 0,0,0 });
			GimmickManager::Instance().Register(spike);


	
			
		}
		OnSpawn = true;
	}




}

void StageKey::Render(ID3D11DeviceContext* dc, Shader* shader) {

	if (model != nullptr) {
		shader->Draw(dc, model);
	}


}

bool StageKey::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) {
	if (model != nullptr) {
		return Collision::IntersectRayVsModel(start, end, model, hit);
	}

	return nullptr;
}

//スフィアキャスト
bool StageKey::SphereCast(const DirectX::XMFLOAT3& start, const float radius, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	if (model != nullptr) {
		return Collision::SphereCast(start, end, radius, model, hit);
	}

	return nullptr;
}


//行列更新処理
void StageKey::UpdateTransform() {
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMMATRIX W = S * R * T;
	DirectX::XMStoreFloat4x4(&transform, W);


	if (model != nullptr) {
		model->UpdateTransform(transform);
	}

}

void StageKey::DrawDebugGUI() {

}

void StageKey::DrawDebugPrimitive() {

}