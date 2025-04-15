#include "GimmickManager.h"
#include"Chara/Collision.h"

//更新処理
void GimmickManager::Update(float elapsedTime) {

	for (Gimmick* gimmick :gimmicks) {
		gimmick->Update(elapsedTime);
	}


	//破棄処理
	for (Gimmick* gimmick : removes) {
		std::vector<Gimmick*>::iterator it = std::find(gimmicks.begin(), gimmicks.end(), gimmick);
		if (it != gimmicks.end()) {
			gimmicks.erase(it);
		}
		delete gimmick;
	}
	removes.clear();



	//CollisionEnemyVsEnemies();

}

void GimmickManager::Render(ID3D11DeviceContext* context, Shader* shader) {
	for (Gimmick* gimmick : gimmicks) {
		gimmick->Render(context, shader);
	}

}



void GimmickManager::Register(Gimmick* gimmick) {
	gimmicks.emplace_back(gimmick);
}

void GimmickManager::Clear() {



	for (Gimmick* gimmick : gimmicks) {

		delete gimmick;
	}

	gimmicks.clear();

}

void GimmickManager::DrawDebugPrimitive() {
	for (Gimmick* gimmick : gimmicks) {
		gimmick->DrawDebugPrimitive();
	}
}

void GimmickManager::DrawDebugGUI() {
	for (Gimmick* gimmick : gimmicks) {
		gimmick->DrawDebugGUI();
	}
}

bool GimmickManager::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) {

	bool result = false;
	hit.distance = FLT_MAX;

	for (Gimmick* gimmick : gimmicks) {

		HitResult tmp;
		if (gimmick->RayCast(start, end, tmp)) {
			if (hit.distance > tmp.distance) {
				hit = tmp;
				result = true;
			}
		}
	}

	return result;

}

//エネミー削除
void GimmickManager::Remove(Gimmick* gimmick)
{
	//破棄リストに追加
	removes.insert(gimmick);
}