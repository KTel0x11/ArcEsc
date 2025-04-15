#include"ProjectileManager.h"

//�R���X�g���N�^
ProjectileManager::ProjectileManager() {

}

//�f�X�g���N�^
ProjectileManager::~ProjectileManager() {
	Clear();
}

//�X�V����
void ProjectileManager::Update(float elapsedTime) {
	for (Projectile* projectile : projectiles) {
		projectile->Update(elapsedTime);
	}



	//�j������
	for (Projectile* projectile : removes) {
		std::vector<Projectile*>::iterator it = std::find(projectiles.begin(), projectiles.end(), projectile);
		if (it != projectiles.end()) {
			projectiles.erase(it);
		}
		delete projectile;
	}
	removes.clear();
}

//�`�揈��
void ProjectileManager::Render(ID3D11DeviceContext* dc, Shader* shader) {

	for (Projectile* projectile : projectiles) {
		projectile->Render(dc,shader);
	}

}

//�f�o�b�O�v���~�e�B�u�`��
void ProjectileManager::DrawDebugPrimitive() {
	for (Projectile* projectile : projectiles) {
		projectile->DrawDebugPrimitive();
	}
}

//�e�ۏ���
void ProjectileManager::Register(Projectile* projectile) {
	projectiles.emplace_back(projectile);
}

//�e�ۑS�폜
void ProjectileManager::Clear() {
	for (Projectile* projectile : projectiles) {
		delete projectile;
	}
	projectiles.clear();
}

//�e�ۍ폜
void ProjectileManager::Remove(Projectile* projectile) {
	//�j�����X�g�ɒǉ�
	removes.insert(projectile);
}


