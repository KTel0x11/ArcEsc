#include<time.h>
#include <stdlib.h>
#include <stdio.h>
#include "StageCross.h"
#include"GimmickObj/GimmickManager.h"
#include"GimmickObj/Gimmick/Spike.h"
#include"GimmickObj/Gimmick/Poison.h"
#include"GimmickObj/Gimmick/Coin.h"
#include"Stage/StageTree.h"
#include"Stage/StageManager.h"
#include"Chara/EnemyManager.h"
#include"Chara/EnemySlime.h"
#include"Chara/Player.h"
#define debug_new new(_NORMAL_BLOCK,__FILE__,__LINE__)


struct Spawn
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 angle;
	DirectX::XMFLOAT3 scale;
};


static Spawn CoinSpawn[] =
{
	{ DirectX::XMFLOAT3(1, 0,1), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(-1, 0, 1), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(-1, 0, -1), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(1, 0, -1), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
};

static Spawn PoisonSpawn[] =
{
	{ DirectX::XMFLOAT3(0, 0.01f ,0), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(30, 2, 30) },
	{ DirectX::XMFLOAT3(0, 0.01f, 2), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(15, 1, 15) },
	{ DirectX::XMFLOAT3(-2,0.01f, 0), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(15, 1, 15) },
	{ DirectX::XMFLOAT3(0, 0.01f, -2), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(15, 1, 15) },
	{ DirectX::XMFLOAT3(2,0.01f, 0), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(15, 1, 15) },

};

static Spawn EnemySpawn[] =
{
	{ DirectX::XMFLOAT3(1, 0.25,1), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(-1,0.25, 1), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(-1, 0.25, -1), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(1, 0.25, -1), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(0, 0.25, 0), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(0, 0.25 ,0), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(0, 0.25, 2), DirectX::XMFLOAT3(0, 0, 0),DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(-2,0.25, 0), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(0, 0.25, -2), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0) },
	{ DirectX::XMFLOAT3(2,0.25, 0), DirectX::XMFLOAT3(0, 0, 0),DirectX::XMFLOAT3(0, 0, 0) },
};


StageCross::StageCross() {

	scale = { 50.0f,50.0f,50.0f };


	model = new Model("Data/Model/StageModel/Room/DungeonCross.mdl");

	//DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&transform);
	//DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(tests[0].position.x, tests[0].position.y, tests[0].position.z);
	//DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(tests[0].angle.x, tests[0].angle.y, tests[0].angle.z);
	//DirectX::XMMATRIX P = R * T * W;

}

StageCross::~StageCross() {

	delete model;

}

void StageCross::Update(float elapsedTime) {
	srand((unsigned int)time(NULL));
	UpdateTransform();

	if (!OnSpawn)
	{



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
						DirectX::XMStoreFloat4x4(&coinTransform, P);
					}
					Coin* coin = new Coin();
					coin->SetPosition({ coinTransform._41,coinTransform._42,coinTransform._43 });
					coin->SetAngle({ 0,0,0 });
					GimmickManager::Instance().Register(coin);
				}
			}
			

		
			for (int i = 0; i < 5; i++) {

				int rnd = rand() % 5;

				if (rnd == 0 || rnd == 2) {
					{
						DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&transform);
						DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(EnemySpawn[i].position.x, EnemySpawn[i].position.y, EnemySpawn[i].position.z);
						DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(EnemySpawn[i].angle.x, EnemySpawn[i].angle.y, EnemySpawn[i].angle.z);
						DirectX::XMMATRIX P = R * T * W;
						DirectX::XMStoreFloat4x4(&enemyTransform, P);
					}
					EnemySlime* slime = new EnemySlime();
					slime->SetPosition({ enemyTransform._41,  enemyTransform._42,   enemyTransform._43 });
					slime->SetTerritory({ enemyTransform._41,  enemyTransform._42,   enemyTransform._43 }, 15.0f);
					EnemyManager::Instance().Register(slime);
				}
			}



			for (int i = 0; i < 5; i++) {

				int rnd = rand() % 5;

				if (rnd == 0 || rnd ==2) {

					{
						DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&transform);
						DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(PoisonSpawn[i].position.x, PoisonSpawn[i].position.y, PoisonSpawn[i].position.z);
						DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(PoisonSpawn[i].angle.x, PoisonSpawn[i].angle.y, PoisonSpawn[2].angle.z);
						DirectX::XMMATRIX S = DirectX::XMMatrixScaling(PoisonSpawn[i].scale.x, PoisonSpawn[i].scale.y, PoisonSpawn[i].scale.z);
						DirectX::XMMATRIX P = S * R * T * W;
						DirectX::XMStoreFloat4x4(&poisonTransform, P);
					}
					Poison* poison = new Poison();
					poison->SetPosition({ poisonTransform._41,poisonTransform._42,poisonTransform._43 });
					poison->SetScale({ PoisonSpawn[i].scale.x, PoisonSpawn[i].scale.y, PoisonSpawn[i].scale.z });
					//poison->SetRadius(PoisonSpawn[i].scale.x);
					poison->SetAngle({ 0,0,0 });
					GimmickManager::Instance().Register(poison);

				}
			}
			}
		OnSpawn = true;
	}




}

void StageCross::Render(ID3D11DeviceContext* dc, Shader* shader) {

	if (model != nullptr) {
		shader->Draw(dc, model);
	}


}

bool StageCross::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) {
	if (model != nullptr) {
		if (Collision::IntersectRayVsModel(start, end, model, hit)) {  
			Player::Instance().SetAxis(axis);
			
			return true;
		}
		else {
			return false;
		}
	}
	else {
		return nullptr;
	}
}

//スフィアキャスト
bool StageCross::SphereCast(const DirectX::XMFLOAT3& start, const float radius, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	if (model != nullptr) {
		return Collision::SphereCast(start, end, radius, model, hit);
	}

	return nullptr;
}


//行列更新処理
void StageCross::UpdateTransform() {
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMMATRIX W = S * R * T;
	DirectX::XMStoreFloat4x4(&transform, W);


	if (model != nullptr) {
		model->UpdateTransform(transform);
	}

}

void StageCross::DrawDebugGUI() {

}

void StageCross::DrawDebugPrimitive() {

}