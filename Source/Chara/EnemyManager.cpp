#include "EnemyManager.h"
#include"Collision.h"

//更新処理
void EnemyManager::Update(float elapsedTime) {

	for (Enemy* enemy : enemies) {
		enemy->Update(elapsedTime);
	}


	//破棄処理
	for (Enemy* enemy : removes) {
		std::vector<Enemy*>::iterator it = std::find(enemies.begin(), enemies.end(), enemy);
		if (it != enemies.end()) {
			enemies.erase(it);
		}
		delete enemy;
	}
	removes.clear();



	CollisionEnemyVsEnemies();

	}

void EnemyManager::Render(ID3D11DeviceContext* context, Shader* shader) {
	for (Enemy* enemy : enemies) {
		enemy->Render(context,shader);
	}

}



void EnemyManager::Register(Enemy* enemy) {
	enemies.emplace_back(enemy);
}

void EnemyManager::Clear() {



	for (Enemy* enemy : enemies) {

		delete enemy;
	}

	enemies.clear();

}

void EnemyManager::DrawDebugPrimitive() {
	for (Enemy* enemy : enemies) {
		enemy->DrawDebugPrimitive();
	}
}

void EnemyManager::CollisionEnemyVsEnemies() {
	size_t enemyCount = enemies.size();
	for (int i = 0; i < enemyCount; i++) {
		Enemy* enemyA = enemies.at(i);
		for (int j = i + 1; j < enemyCount; j++) {
			Enemy* enemyB = enemies.at(j);
				DirectX::XMFLOAT3 outPosition;
			if (enemyA->GetWeight() > enemyB->GetWeight()) {
				if (Collision::IntersectCylinderVsCylinder(
					enemyA->GetPosition(),
					enemyA->GetRadius(),
					enemyA->GetHeight(),
					enemyB->GetPosition(),
					enemyB->GetRadius(),
					enemyB->GetHeight(),
					outPosition))
				{
					enemyB->SetPosition(outPosition);
				}
			}
			else {
				if (Collision::IntersectCylinderVsCylinder(
					enemyB->GetPosition(),
					enemyB->GetRadius(),
					enemyB->GetHeight(),
					enemyA->GetPosition(),
					enemyA->GetRadius(),
					enemyA->GetHeight(),
					outPosition))
				{
					enemyA->SetPosition(outPosition);
				}
			}
		}
	}
}


//エネミー削除
void EnemyManager::Remove(Enemy* enemy)
{
	//破棄リストに追加
	removes.insert(enemy);
}