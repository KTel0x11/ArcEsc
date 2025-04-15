#pragma once

#include <vector>
#include <set>
#include"Gimmick.h"

class GimmickManager
{
private:
	GimmickManager() {}
	~GimmickManager() {}

public:
	//唯一のインスタンス取得
	static GimmickManager& Instance()
	{
		static GimmickManager instance;
		return instance;
	}

	//更新処理
	void Update(float elapsedTime);

	//描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	//UI描画
	void UIRender(ID3D11DeviceContext* dc);

	void DrawDebugPrimitive();

	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);

	void DrawDebugGUI();

	//エネミー数取得
	int GetEnemyCount() const { return static_cast<int>(gimmicks.size()); }

	//エネミー取得
	Gimmick* GetGimmick(int index) { return gimmicks.at(index); }

	//エネミーが存在しているか
	bool GetGimmickExistence() { if (static_cast<int>(gimmicks.size()) > 0) { return true; } else { return false; } }



	//エネミー登録
	void Register(Gimmick* Gimmick);

	//エネミー削除
	void Remove(Gimmick* Gimmick);

	//エネミー全削除
	void Clear();

	//エネミー同士の衝突処理
	//void CollisionEnemyVsEnemies();



private:
	std::vector<Gimmick*> gimmicks;
	std::set<Gimmick*> removes;
};