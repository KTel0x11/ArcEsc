#include "Graphics/Graphics.h"
#include "Input/Input.h"
#include "SceneGame.h"
#include "SceneTitle.h"
#include "SceneLoading.h"
#include "SceneManager.h"
#include "Camera/Camera.h"
#include"Chara/EnemyManager.h"
#include"Chara/EnemySlime.h"
#include"Chara/EffectManager.h"
#include"Stage/StageManager.h"
#include"Stage/StageMain.h"
#include"Stage/StageMoveFloor.h"
#include"UI/CreateMap.h"
#include"GimmickObj/GimmickManager.h"
#include"Mathf.h"
#include"Audio/AudioSource.h"
#include"Audio/Audio.h"
#include"GameState.h"


#define debug_new new(_NORMAL_BLOCK,__FILE__,__LINE__)


// ������
void SceneGame::Initialize()
{
	//�}�b�v�̐����A�}�b�v����̏�����
	CreateMap::Instance().Initialize();
	CreateMap& createMap = CreateMap::Instance();
	startPosition = createMap.GetStartPosition();
	startPosition.y = 50.0f;

	//�X�e�[�W�̏�����	
	StageManager& stageManager = StageManager::Instance();
	GimmickManager& gimmickManager = GimmickManager::Instance();

	//�G�l�~�[������
	EnemyManager& enemyManager = EnemyManager::Instance();

	//�v���C���[������

	player = debug_new Player();
	player->SetPosition(startPosition);
	player->SetStartPosition(startPosition);


	//�J�����̏�����
	Graphics& graphics = Graphics::Instance();
	Camera& camera = Camera::Instance();

	camera.SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),
		graphics.GetScreenWidth() / graphics.GetScreenHeight(),
		0.1f,
		1000.0f
	);

	//�J�����̃R���g���[���[������
	cameraController = new CameraController();
	


	//UI�֌W�̏�����
	//gauge = new Sprite("Data/Sprite/hp_boss_01.png");
	//spriteEnemyHP = new Sprite("Data/Sprite/hp_boss_frame.png");
	//spriteEHPBar = new Sprite("Data/Sprite/hp_boss_01.png");
	spritePauseButton = new Sprite("Data/Sprite/PauseGuide.png");
	spriteGuide = new Sprite("Data/Sprite/Guide.png");
	spriteClear = debug_new Sprite("Data/Sprite/Clear.png");
	spriteOver = debug_new Sprite("Data/Sprite/Over.png");

	TitleChangeTimer = 0;

	//�ŏ��̃X�e�[�g�Z�b�g
	GameState::Instance().SetState(GameState::State::Start);
	GameState::Instance().SetSceneState(GameState::SceneState::SceneGame);


	//bgm�̏�����
	BGM = Audio::Instance().LoadAudioSource("Data/Audio/BGM.wav");
	BGM->SetVolume(0.1f);

}

// �I����
void SceneGame::Finalize()
{

	///UI�֌W�̏I����
	{
		if (spriteGuide != nullptr) {
			delete spriteGuide;
			spriteGuide = nullptr;
		}

		if (spritePauseButton != nullptr)
		{
			delete spritePauseButton;
			spritePauseButton = nullptr;
		}

		if (spriteEHPBar != nullptr) {
			delete spriteEHPBar;
			spriteEHPBar = nullptr;
		}
		if (spriteEnemyHP != nullptr) {
			delete spriteEnemyHP;
			spriteEnemyHP = nullptr;
		}

		if (spriteClear != nullptr) {
			delete spriteClear;
			spriteClear = nullptr;
		}

		if (spriteOver != nullptr) {
			delete spriteOver;
			spriteOver = nullptr;
		}


		if (gauge != nullptr) {
			delete gauge;
			gauge = nullptr;
		}

	}

	//�J�����̏I����
	if (cameraController != nullptr) {
		delete cameraController;
		cameraController = nullptr;
	}

	//�v���C���[�̏I����
	if (player != nullptr) {
		delete player;
		player = nullptr;
	}

	//BGM�̏I����
	if (BGM != nullptr) {
		delete BGM;
		BGM = nullptr;
	}

	//�}�b�v����̏I����
	CreateMap::Instance().Finalize();
	//�X�e�[�W�̏I����



	//���̑��}�l�[�W���[�̏I����
	EnemyManager::Instance().Clear();
	GimmickManager::Instance().Clear();
	StageManager::Instance().Clear();
	
}

// �X�V����
void SceneGame::Update(float elapsedTime)
{
	BGM->Play(true);

	//�J�����X�V
	DirectX::XMFLOAT3 target = player->GetPosition();

	target.y += 10.0f;

	cameraController->SetTarget(target);

	//�X�e�[�W�X�V
	StageManager::Instance().Update(elapsedTime);

	//�v���C���[�X�V
	player->Update(elapsedTime);


	//�R���g���[�����擾
	GamePad& gamePad = Input::Instance().GetGamePad();

	UIFrame::Instance().Update(elapsedTime);

	switch (GameState::Instance().currentState)
	{
	case GameState::State::Start: 
	{
		GameState::Instance().SetState(GameState::State::Game);
		break;
	}
	/******************************************�Q�[���i�s��***************************************/
	case GameState::State::Game: 
	{

		//�X�e�[�W�X�V
		StageManager::Instance().Update(elapsedTime);

		//�M�~�b�N�X�V
		GimmickManager::Instance().Update(elapsedTime);
		//�G�l�~�[�X�V
		EnemyManager::Instance().Update(elapsedTime);
		//�G�t�F�N�g�X�V
		EffectManager::Instance().Update(elapsedTime);
		//�Q�[����ʂ̃t���[���X�V
		
		//�J�����X�V
		cameraController->Update(elapsedTime);

		//Pause���[�h�؂�ւ�
		if (gamePad.GetButtonDown() & GamePad::BTN_START || GetAsyncKeyState('P') & 1) 
		{
			GameState::Instance().SetState(GameState::State::Pause);
			break;
		}


		//�}�b�v���샂�[�h�ɐ؂�ւ�
		if (gamePad.GetButtonDown() & GamePad::BTN_X) {
			CreateMap::Instance().SetState(CreateMap::State::STARTMENU);
			GameState::Instance().SetState(GameState::State::MapCreate);
			break;
		}

		//�Q�[���N���A�����ɕύX
		if (Player::Instance().clear) {
			GameState::Instance().SetState(GameState::State::GameClear);
			break;

		}


		//�Q�[���I�[�o�[�����ɐ؂�ւ� 
		if (Player::Instance().GetDead()) {	//�v���C���[�����񂾂�
			GameState::Instance().SetState(GameState::State::GameOver);
			break;
		}


		break;
	}
	 /******************************************�o�g����***************************************/
	case  GameState::State::Battle: 
	{

		//�}�b�v���샂�[�h�ɐ؂�ւ�
		if (gamePad.GetButtonDown() & GamePad::BTN_X) {
			GameState::Instance().SetState(GameState::State::MapCreate);
			break;
		}


		break;
	}

	/******************************************�}�b�v���쒆***************************************/
	case  GameState::State::MapCreate: 
	{




		//�}�b�v����֐��X�V
		CreateMap::Instance().Update(elapsedTime);
		//�Q�[����ʂ̃t���[���X�V
		//frameUI->Update(elapsedTime);

		//�Q�[���ɖ߂�
	/*	if (gamePad.GetButtonDown() & GamePad::BTN_X|| gamePad.GetButtonDown() & GamePad::BTN_A) {
			GameState::Instance().SetState(GameState::State::Game);
			break;
		}*/


		break;
	}

	/********************************************�|�[�Y���j���[��*************************************************/
	case  GameState::State::Pause:
	{

		//�J�����X�V
		cameraController->Update(elapsedTime);

		//�Q�[����ʂ̃t���[���X�V
		//frameUI->Update(elapsedTime);

		//�Q�[���ɖ߂�
		if (gamePad.GetButtonDown() & GamePad::BTN_START || GetAsyncKeyState('P') & 1) {	//�R���g���[���[�FSTART�@�L�[�{�[�h:P

			GameState::Instance().SetState(GameState::State::Game);
			break;

		}

		break;

	}
	/******************************************�Q�[���I�[�o�[����****************************************************************/
	case GameState::State::GameOver:	
	{
		


		switch (animeState) // UI�̃A�j���[�V����
		{
		case 0:
			OverPos = { 260,800 };
			OverColor = { 1,1,1,0 };
			animeState++;
			break;
		case 1:
			OverPos.y -= 200 * elapsedTime;
			OverColor.w += 0.2f *elapsedTime;
			if (OverPos.y <= 120) {
				animeState++;
			}
			break;
		case 2:
			OverColor.w += 0.3f * elapsedTime;
			if (OverColor.w >= 1) {
				animeState++;
			}
			break;
		case 3:
			TitleChangeTimer += elapsedTime;
			//�ܕb��Ƀ^�C�g���ɖ߂�
			if (TitleChangeTimer >= 5.0f) {
				SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTitle));
				break;
			}
			break;
	

		}

			//TitleChangeTimer += elapsedTime;
			////�ܕb��Ƀ^�C�g���ɖ߂�
			//if (TitleChangeTimer >= 5.0f) {
			//	SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTitle));
			//	break;
			//}

	
		break;
	}

	case GameState::State::GameClear: {
		switch (animeState) // UI�̃A�j���[�V����
		{
		case 0:
			ClearPos = { 260,800 };
			ClearColor = { 1,1,1,0 };
			animeState++;
			break;
		case 1:
			ClearPos.y -= 1000 * elapsedTime;
			ClearColor.w += 0.2f * elapsedTime;
			if (ClearPos.y <= 120) {
				animeState++;
			}
			break;
		case 2:
			ClearColor.w += 0.2f * elapsedTime;
			if (ClearColor.w >= 1) {
				animeState++;
			}
			break;
		case 3:
			TitleChangeTimer += elapsedTime;
			//�ܕb��Ƀ^�C�g���ɖ߂�
			if (TitleChangeTimer >= 5.0f) {
				SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTitle));
				break;
			}
			break;


		}
		break;

	}

	
		break;

	}

	if (gamePad.GetButtonDown() & GamePad::BTN_BACK) {
		SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTitle));

	}


}

// �`�揈��
void SceneGame::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();



	// ��ʃN���A�������_�[�^�[�Q�b�g�ݒ�
	FLOAT color[] = { 0.09f, 0.1f, 0.2f, 1.0f };	// RGBA(0.0�`1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	// �`�揈��
	RenderContext rc;
	Camera& camera = Camera::Instance();

	DirectX::XMFLOAT3 cameraDirection = camera.GetFront();

	
	rc.lightDirection = { cameraDirection.x,cameraDirection.y,cameraDirection.z,10.0f};	// ���C�g�����i�������j
	
	//	�J�����p�����[�^�ݒ�
	rc.view = camera.GetView();
	rc.projection = camera.GetProjection();

	//�V�F�[�_��ݒ�
	Shader* shader = graphics.GetShader(ShaderId::Toon);
	shader->Begin(dc, rc);

	//�X�e�[�g�J��
	switch (GameState::Instance().currentState)
	{

	/***********************************�Q�[�����̕`��*************************/
	case GameState::State::Game:			
	{
		// 3D���f���`��
		{
			//�X�e�[�W�`��
			StageManager::Instance().Render(dc, shader);

			//�M�~�b�N�`��
			GimmickManager::Instance().Render(dc, shader);

			//�v���C���[�`��
			player->Render(dc, shader);


			//�G�l�~�[�`��
			EnemyManager::Instance().Render(dc, shader);

			//�G�t�F�N�g�`��
			EffectManager::Instance().Render(rc.view, rc.projection);
			
		}
		shader->End(dc);


		// 2D�X�v���C�g�`��
		{
			//�v���C���[UI�֘A
			player->UIRender(dc);

			//createMap->Render(dc);

			//�Q�[��UI�t���[���`��
			//frameUI->Render(graphics, dc);
			UIFrame::Instance().Render(graphics, dc);

			//�|�[�Y�̃{�^���K�C�h
			{
				float PauseTexWidth = static_cast<float>(spritePauseButton->GetTextureWidth());
				float PauseTexHeight = static_cast<float>(spritePauseButton->GetTextureHeight());
				spritePauseButton->Render(dc, 64, 660, 512, 32, 0, 0, PauseTexWidth, PauseTexHeight, 0, 1, 1, 1, 1);
			}

			//Enemy_HP_UI�֘A
#if 0
			{
				RenderEnemyGauge(dc, rc.view, rc.projection);
			}
#endif
		}


		break;
	}


	/****************************************�}�b�v����********************************************/
	case  GameState::State::MapCreate:
	{
		// 3D���f���`��
		{
			//�X�e�[�W�`��
			StageManager::Instance().Render(dc, shader);

			//�M�~�b�N�`��
			GimmickManager::Instance().Render(dc, shader);

			//�v���C���[�`��
			player->Render(dc, shader);


			//�G�l�~�[�`��
			EnemyManager::Instance().Render(dc, shader);

			{
				//�G�t�F�N�g�`��
				EffectManager::Instance().Render(rc.view, rc.projection);
			}
			shader->End(dc);
		}
		{
			//�}�b�v����̕`��
			CreateMap::Instance().Render(dc);
			

		}
		// 2D�X�v���C�g�`��
		{
		
			//�Q�[��UI�t���[���`��

			//frameUI->Render(graphics, dc);
			UIFrame::Instance().Render(graphics, dc);
			{
				float PauseTexWidth = static_cast<float>(spritePauseButton->GetTextureWidth());
				float PauseTexHeight = static_cast<float>(spritePauseButton->GetTextureHeight());
				spritePauseButton->Render(dc, 64, 660, 512, 32, 0, 0, PauseTexWidth, PauseTexHeight, 0, 1, 1, 1, 1);
			}

			

		}


		////�@�X�e�[�W����
		{

			//ClickEnemyCreate(dc, rc.view, rc.projection);
		}

		break;
	}




	/************************************* �|�[�Y�� *********************************************************/

	case  GameState::State::Pause:
	{
		// 3D���f���`��
		{
			//�X�e�[�W�`��
			StageManager::Instance().Render(dc, shader);

			//�M�~�b�N�`��
			GimmickManager::Instance().Render(dc, shader);

			//�v���C���[�`��
			player->Render(dc, shader);


			//�G�l�~�[�`��
			EnemyManager::Instance().Render(dc, shader);

			{
				//�G�t�F�N�g�`��
				EffectManager::Instance().Render(rc.view, rc.projection);
			}

		}

		shader->End(dc);

	
		// 2D�X�v���C�g�`��
		{
			float screenWidth = static_cast<float>(graphics.GetScreenWidth());
			float screenHeight = static_cast<float>(graphics.GetScreenHeight());
			float GuideTextureWidth = static_cast<float>(spriteGuide->GetTextureWidth());
			float GuideTextureHeight = static_cast<float>(spriteGuide->GetTextureHeight());

			//spriteGuide->Render(dc, 0, 0, screenWidth, screenHeight, 0, 0, GuideTextureWidth, GuideTextureHeight, 0, 1, 1, 1, 1);


			//�Q�[��UI�t���[���`��

			//frameUI->Render(graphics, dc);
			UIFrame::Instance().Render(graphics, dc);
		}
		break;
	}
	case GameState::State::GameClear: {
		// 3D���f���`��
		{
			//�X�e�[�W�`��
			StageManager::Instance().Render(dc, shader);

			//�M�~�b�N�`��
			GimmickManager::Instance().Render(dc, shader);

			//�v���C���[�`��
			player->Render(dc, shader);


			//�G�l�~�[�`��
			EnemyManager::Instance().Render(dc, shader);

			//�G�t�F�N�g�`��
			EffectManager::Instance().Render(rc.view, rc.projection);

		}
		shader->End(dc);


		// 2D�X�v���C�g�`��
		{
			//�v���C���[UI�֘A
			player->UIRender(dc);

			//createMap->Render(dc);

			{
				float ClearTexWidth = static_cast<float>(spriteClear->GetTextureWidth());
				float ClearTexHeight = static_cast<float>(spriteClear->GetTextureHeight());
			
				spriteClear->Render(dc,
					ClearPos.x, ClearPos.y, 
					ClearSize.x, ClearSize.y, 
					0, 0, 
					ClearTexWidth, ClearTexHeight, 
					0, 
					ClearColor.x, ClearColor.y, ClearColor.z, ClearColor.w);
				
			}


			//�Q�[��UI�t���[���`��
			//frameUI->Render(graphics, dc);
			UIFrame::Instance().Render(graphics, dc);



			//�|�[�Y�̃{�^���K�C�h
			{
				float PauseTexWidth = static_cast<float>(spritePauseButton->GetTextureWidth());
				float PauseTexHeight = static_cast<float>(spritePauseButton->GetTextureHeight());
				spritePauseButton->Render(dc, 64, 660, 512, 32, 0, 0, PauseTexWidth, PauseTexHeight, 0, 1, 1, 1, 1);
			}

			//Enemy_HP_UI�֘A
#if 0
			{
				RenderEnemyGauge(dc, rc.view, rc.projection);
			}
#endif
		}


		break;
	}
	case GameState::State::GameOver: {
		// 3D���f���`��
		{
			//�X�e�[�W�`��
			StageManager::Instance().Render(dc, shader);

			//�M�~�b�N�`��
			GimmickManager::Instance().Render(dc, shader);

			//�v���C���[�`��
			player->Render(dc, shader);


			//�G�l�~�[�`��
			EnemyManager::Instance().Render(dc, shader);

			//�G�t�F�N�g�`��
			EffectManager::Instance().Render(rc.view, rc.projection);

		}
		shader->End(dc);


		// 2D�X�v���C�g�`��
		{
			//�v���C���[UI�֘A
			player->UIRender(dc);

			//createMap->Render(dc);

			//�Q�[��UI�t���[���`��
			//frameUI->Render(graphics, dc);
			UIFrame::Instance().Render(graphics, dc);
			{
				float OverTexWidth = static_cast<float>(spriteOver->GetTextureWidth());
				float OverTexHeight = static_cast<float>(spriteOver->GetTextureHeight());

				spriteOver->Render(dc,
					OverPos.x, OverPos.y,
					OverSize.x, OverSize.y,
					0, 0,
					OverTexWidth, OverTexHeight,
					0,
					OverColor.x, OverColor.y, OverColor.z, OverColor.w);

			}


			//�|�[�Y�̃{�^���K�C�h
			{
				float PauseTexWidth = static_cast<float>(spritePauseButton->GetTextureWidth());
				float PauseTexHeight = static_cast<float>(spritePauseButton->GetTextureHeight());
				spritePauseButton->Render(dc, 64, 660, 512, 32, 0, 0, PauseTexWidth, PauseTexHeight, 0, 1, 1, 1, 1);
			}

			//Enemy_HP_UI�֘A
#if 0
			{
				RenderEnemyGauge(dc, rc.view, rc.projection);
			}
#endif
		}


		break;
	}

	case  GameState::State::MAX:
		break;
	
	}

	// 3D�f�o�b�O�`��
	{
		// ���C�������_���`����s
		graphics.GetLineRenderer()->Render(dc, rc.view, rc.projection);

		// �f�o�b�O�����_���`����s
		graphics.GetDebugRenderer()->Render(dc, rc.view, rc.projection);
	}

	//�f�o�b�N�v���~�e�B�u�`��
	{
		//�v���C���[�f�o�b�N�v���~�e�B�u�`��
		//player->DrawDebugPrimitive();

		//�G�l�~�[�f�o�b�O�v���~�e�B�u�`��
		//EnemyManager::Instance().DrawDebugPrimitive();

		//GimmickManager::Instance().DrawDebugPrimitive();
	}


	// 2D�f�o�b�OGUI�`��
	{
		//player->DrawDebugGUI();
		//StageManager::Instance().DrawDebugGUI();
		//GimmickManager::Instance().DrawDebugGUI();
	}

}

//�G�l�~�[HP UI
void SceneGame::RenderEnemyGauge(ID3D11DeviceContext* dc,
	const DirectX::XMFLOAT4X4& view,
	const DirectX::XMFLOAT4X4& projection) {

	////�r���[�|�[�g
	//D3D11_VIEWPORT viewport;
	//UINT numViewports = 1;
	//dc->RSGetViewports(&numViewports, &viewport);

	////�ϊ��s��
	//DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&view);
	//DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&projection);
	//DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

	////�S�Ă̓G�̓����HP�Q�[�W��\��
	//EnemyManager& enemyManager = EnemyManager::Instance();
	//int enemyCount = enemyManager.GetEnemyCount();

	//for (int i = 0; i < enemyCount; ++i)
	//{
	//	Enemy* enemy = enemyManager.GetEnemy(i);

	//	//�G�l�~�[�̓���̃��[���h���W
	//	DirectX::XMFLOAT3 worldPosition = enemy->GetPosition();
	//	worldPosition.y += enemy->GetHeight();

	//	DirectX::XMVECTOR WorldPosition = DirectX::XMLoadFloat3(&worldPosition);

	//	//���[���h���W����X�N���[�����W�֕ϊ�
	//	DirectX::XMVECTOR ScreenPosition = DirectX::XMVector3Project(
	//		WorldPosition,
	//		viewport.TopLeftX,
	//		viewport.TopLeftY,
	//		viewport.Width,
	//		viewport.Height,
	//		viewport.MinDepth,
	//		viewport.MaxDepth,
	//		Projection,
	//		View,
	//		World
	//	);

	//	//�X�N���[�����W
	//	DirectX::XMFLOAT2 screenPosition;
	//	DirectX::XMStoreFloat2(&screenPosition, ScreenPosition);

	//	//�Q�[�W�̒���
	//	const float gaugeWidth = 60.0f;
	//	const float gaugeHeight = 10.0f;

	//	float healthRate = enemy->GetHealth() / static_cast<float> (enemy->GetMaxHealth());

	//	

	//	//�Q�[�W�`��
	//	gauge->Render(dc,
	//		screenPosition.x - gaugeWidth * 0.5f,
	//		screenPosition.y - gaugeHeight,
	//		gaugeWidth * healthRate,
	//		gaugeHeight,
	//		0, 0,
	//		static_cast<float> (gauge->GetTextureWidth()),
	//		static_cast<float> (gauge->GetTextureHeight()),
	//		0.0f,
	//		1.0f, 0.0f, 0.0f, 1.0f
	//	);
	//}

	if (player->SearchNearEnemy()) {
		Enemy* nearEnemy = player->GetNearEnemy();
		

		float HPPositionX = 250;
		float HPPositionY = 665;

		float FrameWidth = 1280.0f - 500.0f;
		float FrameHeight = 50;

		float HPWidth = FrameWidth * nearEnemy->GetHealth() / nearEnemy->GetMaxHealth();
		float HPHeight = 49;

		float EFtextureWidth = static_cast<float>(spriteEnemyHP->GetTextureWidth());
		float EFtextureHeight = static_cast<float>(spriteEnemyHP->GetTextureHeight());

		float EBtextureWidth = static_cast<float>(spriteEHPBar->GetTextureWidth());
		float EBtextureHeight = static_cast<float>(spriteEHPBar->GetTextureHeight());
		if (EnemyManager::Instance().GetEnemyCount() > 0) {
			spriteEHPBar->Render(dc, HPPositionX, HPPositionY, HPWidth, HPHeight, 0, 0, EBtextureWidth, EBtextureHeight, 0, 1, 1, 1, 1);
			spriteEnemyHP->Render(dc, HPPositionX - 20, HPPositionY, FrameWidth + 40, FrameHeight, 0, 0, EFtextureWidth, EFtextureHeight, 0, 1, 1, 1, 1);
		}
	}




}

//�N���b�N�ŃX���C�������@���f�o�b�N�p
void SceneGame::ClickEnemyCreate(ID3D11DeviceContext* dc,
	const DirectX::XMFLOAT4X4& view,
	const DirectX::XMFLOAT4X4& projection) {

	Graphics& graphics = Graphics::Instance();


	// �`�揈��
	RenderContext rc;
	rc.lightDirection = { 1.0f, 1.0f, 1.0f, 1.0f };	// ���C�g�����i�������j

	//�r���[�|�[�g
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	dc->RSGetViewports(&numViewports, &viewport);

	//�ϊ��s��
	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&view);
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&projection);
	DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

	//�G�l�~�[�z�u����
	//Mouse& mouse = Input::Instance().GetMouse();



	GamePad& gamePad = Input::Instance().GetGamePad();
	if (gamePad.GetButtonDown() & GamePad::BTN_RIGHT_TRIGGER) {
		//�}�E�X�J�[�\�����W���擾
		DirectX::XMFLOAT3 screenPosition;

		screenPosition.x = static_cast<float>(graphics.GetScreenWidth() / 2);
		screenPosition.y = static_cast<float>(graphics.GetScreenHeight() / 2);


		DirectX::XMVECTOR ScreenPosition, WorldPosition;

		//���C�̎n�_���Z�o
		screenPosition.z = 0.0f;
		ScreenPosition = DirectX::XMLoadFloat3(&screenPosition);
		WorldPosition = DirectX::XMVector3Unproject(
			ScreenPosition,
			viewport.TopLeftX,
			viewport.TopLeftY,
			viewport.Width,
			viewport.Height,
			viewport.MinDepth,
			viewport.MaxDepth,
			Projection,
			View,
			World
		);

		DirectX::XMFLOAT3 rayStart;
		DirectX::XMStoreFloat3(&rayStart, WorldPosition);

		//���C�̏I�_���Z�o
		screenPosition.z = 1.0f;
		ScreenPosition = DirectX::XMLoadFloat3(&screenPosition);
		WorldPosition = DirectX::XMVector3Unproject(
			ScreenPosition,
			viewport.TopLeftX,
			viewport.TopLeftY,
			viewport.Width,
			viewport.Height,
			viewport.MinDepth,
			viewport.MaxDepth,
			Projection,
			View,
			World
		);

		DirectX::XMFLOAT3 rayEnd;
		DirectX::XMStoreFloat3(&rayEnd, WorldPosition);

		//���C�L���X�g
		HitResult hit;
		if (StageManager::Instance().RayCast(rayStart, rayEnd, hit))
		{
			StageMoveFloor* stageMF = new StageMoveFloor();
			stageMF->SetStartPoint(hit.position);
			stageMF->SetGoalPoint(DirectX::XMFLOAT3(hit.position.x, hit.position.y + 10.0f, hit.position.z));
			StageManager::Instance().Register(stageMF);
		}


	}


}
