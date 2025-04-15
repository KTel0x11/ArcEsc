#pragma once


#include"Chara/Player.h"
#include"Camera/CameraController.h"
#include"Stage/CreateMap.h"
#include"Scene.h"
#include"Graphics/Sprite.h"
#include"Audio/AudioSource.h"

#include"UI/UIFrame.h"

// �Q�[���V�[��
class SceneGame : public Scene
{
public:
	//�B��̃C���X�^���X�擾
	static SceneGame& Instance()
	{
		static SceneGame instance;
		return instance;
	}



	SceneGame() {}
	~SceneGame() override {}

	// ������
	void Initialize() override;

	// �I����
	void Finalize()override;

	// �X�V����
	void Update(float elapsedTime)override;

	// �`�揈��
	void Render()override;

	Player* GetPlayer() { return player; };


private:

	//�G�l�~�[HP�Q�[�W�`��
	void RenderEnemyGauge(
		ID3D11DeviceContext* dc,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection
	);

	//�N���b�N�ŃG�l�~�[����
	void ClickEnemyCreate(ID3D11DeviceContext* dc,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection);

	

private:
	
	Player* player = nullptr;
	DirectX::XMFLOAT3 startPosition = { 0,0,0 };

	CameraController* cameraController= nullptr;
	Sprite* gauge = nullptr;
	Sprite* spriteEnemyHP = nullptr;
	Sprite* spriteEHPBar = nullptr;
	Sprite* spritePauseButton = nullptr;
	Sprite* spriteGuide = nullptr;

	Sprite* spriteClear= nullptr;
	DirectX::XMFLOAT2 ClearPos = { 0,0 };
	DirectX::XMFLOAT2 ClearSize = { 730,256 };
	DirectX::XMFLOAT4 ClearColor = { 1,1,1,1 };

	Sprite* spriteOver = nullptr;
	DirectX::XMFLOAT2 OverPos = { 250,800 };
	DirectX::XMFLOAT2 OverSize = { 730,256 };
	DirectX::XMFLOAT4 OverColor = { 1,1,1,1 };

	CreateMap* createMap = nullptr;
	UIFrame* frameUI = nullptr;


	
	AudioSource* BGM = nullptr;

	float TitleChangeTimer = 0;
	int animeState = 0;


};
