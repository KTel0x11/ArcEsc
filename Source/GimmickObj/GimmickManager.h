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
	//�B��̃C���X�^���X�擾
	static GimmickManager& Instance()
	{
		static GimmickManager instance;
		return instance;
	}

	//�X�V����
	void Update(float elapsedTime);

	//�`�揈��
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	//UI�`��
	void UIRender(ID3D11DeviceContext* dc);

	void DrawDebugPrimitive();

	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);

	void DrawDebugGUI();

	//�G�l�~�[���擾
	int GetEnemyCount() const { return static_cast<int>(gimmicks.size()); }

	//�G�l�~�[�擾
	Gimmick* GetGimmick(int index) { return gimmicks.at(index); }

	//�G�l�~�[�����݂��Ă��邩
	bool GetGimmickExistence() { if (static_cast<int>(gimmicks.size()) > 0) { return true; } else { return false; } }



	//�G�l�~�[�o�^
	void Register(Gimmick* Gimmick);

	//�G�l�~�[�폜
	void Remove(Gimmick* Gimmick);

	//�G�l�~�[�S�폜
	void Clear();

	//�G�l�~�[���m�̏Փˏ���
	//void CollisionEnemyVsEnemies();



private:
	std::vector<Gimmick*> gimmicks;
	std::set<Gimmick*> removes;
};