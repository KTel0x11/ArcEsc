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
	//�B��̃C���X�^���X�擾
	static EnemyManager& Instance()
	{
		static EnemyManager instance;
		return instance;
	}

	//�X�V����
	void Update(float elapsedTime);

	//�`�揈��
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	//UI�`��
	void UIRender(ID3D11DeviceContext* dc);

	void DrawDebugPrimitive();

	//�G�l�~�[���擾
	int GetEnemyCount() const { return static_cast<int>(enemies.size()); }

	//�G�l�~�[�擾
	Enemy* GetEnemy(int index) { return enemies.at(index); }

	//�G�l�~�[�����݂��Ă��邩
	bool GetEnemyExistence() { if (static_cast<int>(enemies.size()) > 0) { return true; } else { return false; } }



	//�G�l�~�[�o�^
	void Register(Enemy* enemy);

	//�G�l�~�[�폜
	void Remove(Enemy* enemy);

	//�G�l�~�[�S�폜
	void Clear();

	//�G�l�~�[���m�̏Փˏ���
	void CollisionEnemyVsEnemies();



private:
	std::vector<Enemy*> enemies;
	std::set<Enemy*> removes;
};