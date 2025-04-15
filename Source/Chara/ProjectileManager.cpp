#include"ProjectileManager.h"

//コンストラクタ
ProjectileManager::ProjectileManager() {

}

//デストラクタ
ProjectileManager::~ProjectileManager() {
	Clear();
}

//更新処理
void ProjectileManager::Update(float elapsedTime) {
	for (Projectile* projectile : projectiles) {
		projectile->Update(elapsedTime);
	}



	//破棄処理
	for (Projectile* projectile : removes) {
		std::vector<Projectile*>::iterator it = std::find(projectiles.begin(), projectiles.end(), projectile);
		if (it != projectiles.end()) {
			projectiles.erase(it);
		}
		delete projectile;
	}
	removes.clear();
}

//描画処理
void ProjectileManager::Render(ID3D11DeviceContext* dc, Shader* shader) {

	for (Projectile* projectile : projectiles) {
		projectile->Render(dc,shader);
	}

}

//デバッグプリミティブ描画
void ProjectileManager::DrawDebugPrimitive() {
	for (Projectile* projectile : projectiles) {
		projectile->DrawDebugPrimitive();
	}
}

//弾丸処理
void ProjectileManager::Register(Projectile* projectile) {
	projectiles.emplace_back(projectile);
}

//弾丸全削除
void ProjectileManager::Clear() {
	for (Projectile* projectile : projectiles) {
		delete projectile;
	}
	projectiles.clear();
}

//弾丸削除
void ProjectileManager::Remove(Projectile* projectile) {
	//破棄リストに追加
	removes.insert(projectile);
}


