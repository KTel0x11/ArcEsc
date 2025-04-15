#pragma once

#include <vector>
#include <set>
#include"Enemy.h"

class EnemyManager
{
private:
	EnemyManager(){}
	~EnemyManager(){}

public:
	//唯一のインスタンス取得
	static EnemyManager& Instance()
	{
		static EnemyManager instance;
		return instance;
	}

	//更新処理
	void Update(float elapsedTime);

	//描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	//UI描画
	void UIRender(ID3D11DeviceContext* dc);

	void DrawDebugPrimitive();

	//エネミー数取得
	int GetEnemyCount() const { return static_cast<int>(enemies.size()); }

	//エネミー取得
	Enemy* GetEnemy(int index) { return enemies.at(index); }

	//エネミーが存在しているか
	bool GetEnemyExistence() { if (static_cast<int>(enemies.size()) > 0) { return true; } else { return false; } }



	//エネミー登録
	void Register(Enemy* enemy);

	//エネミー削除
	void Remove(Enemy* enemy);

	//エネミー全削除
	void Clear();

	//エネミー同士の衝突処理
	void CollisionEnemyVsEnemies();



private:
	std::vector<Enemy*> enemies;
	std::set<Enemy*> removes;
};