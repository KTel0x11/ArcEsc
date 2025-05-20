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


// 初期化
void SceneGame::Initialize()
{
	//マップの生成、マップ制作の初期化
	CreateMap::Instance().Initialize();
	CreateMap& createMap = CreateMap::Instance();
	startPosition = createMap.GetStartPosition();
	startPosition.y = 50.0f;

	//ステージの初期化	
	StageManager& stageManager = StageManager::Instance();
	GimmickManager& gimmickManager = GimmickManager::Instance();

	//エネミー初期化
	EnemyManager& enemyManager = EnemyManager::Instance();

	//プレイヤー初期化

	player = debug_new Player();
	player->SetPosition(startPosition);
	player->SetStartPosition(startPosition);


	//カメラの初期化
	Graphics& graphics = Graphics::Instance();
	Camera& camera = Camera::Instance();

	camera.SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),
		graphics.GetScreenWidth() / graphics.GetScreenHeight(),
		0.1f,
		1000.0f
	);

	//カメラのコントローラー初期化
	cameraController = new CameraController();
	


	//UI関係の初期化
	//gauge = new Sprite("Data/Sprite/hp_boss_01.png");
	//spriteEnemyHP = new Sprite("Data/Sprite/hp_boss_frame.png");
	//spriteEHPBar = new Sprite("Data/Sprite/hp_boss_01.png");
	spritePauseButton = new Sprite("Data/Sprite/PauseGuide.png");
	spriteGuide = new Sprite("Data/Sprite/Guide.png");
	spriteClear = debug_new Sprite("Data/Sprite/Clear.png");
	spriteOver = debug_new Sprite("Data/Sprite/Over.png");

	TitleChangeTimer = 0;

	//最初のステートセット
	GameState::Instance().SetState(GameState::State::Start);
	GameState::Instance().SetSceneState(GameState::SceneState::SceneGame);


	//bgmの初期化
	BGM = Audio::Instance().LoadAudioSource("Data/Audio/BGM.wav");
	BGM->SetVolume(0.1f);

}

// 終了化
void SceneGame::Finalize()
{

	///UI関係の終了化
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

	//カメラの終了化
	if (cameraController != nullptr) {
		delete cameraController;
		cameraController = nullptr;
	}

	//プレイヤーの終了化
	if (player != nullptr) {
		delete player;
		player = nullptr;
	}

	//BGMの終了化
	if (BGM != nullptr) {
		delete BGM;
		BGM = nullptr;
	}

	//マップ制作の終了化
	CreateMap::Instance().Finalize();
	//ステージの終了化



	//その他マネージャーの終了化
	EnemyManager::Instance().Clear();
	GimmickManager::Instance().Clear();
	StageManager::Instance().Clear();
	
}

// 更新処理
void SceneGame::Update(float elapsedTime)
{
	BGM->Play(true);

	//カメラ更新
	DirectX::XMFLOAT3 target = player->GetPosition();

	target.y += 10.0f;

	cameraController->SetTarget(target);

	//ステージ更新
	StageManager::Instance().Update(elapsedTime);

	//プレイヤー更新
	player->Update(elapsedTime);


	//コントローラを取得
	GamePad& gamePad = Input::Instance().GetGamePad();

	UIFrame::Instance().Update(elapsedTime);

	switch (GameState::Instance().currentState)
	{
	case GameState::State::Start: 
	{
		GameState::Instance().SetState(GameState::State::Game);
		break;
	}
	/******************************************ゲーム進行中***************************************/
	case GameState::State::Game: 
	{

		//ステージ更新
		StageManager::Instance().Update(elapsedTime);

		//ギミック更新
		GimmickManager::Instance().Update(elapsedTime);
		//エネミー更新
		EnemyManager::Instance().Update(elapsedTime);
		//エフェクト更新
		EffectManager::Instance().Update(elapsedTime);
		//ゲーム画面のフレーム更新
		
		//カメラ更新
		cameraController->Update(elapsedTime);

		//Pauseモード切り替え
		if (gamePad.GetButtonDown() & GamePad::BTN_START || GetAsyncKeyState('P') & 1) 
		{
			GameState::Instance().SetState(GameState::State::Pause);
			break;
		}


		//マップ制作モードに切り替え
		if (gamePad.GetButtonDown() & GamePad::BTN_X) {
			CreateMap::Instance().SetState(CreateMap::State::STARTMENU);
			GameState::Instance().SetState(GameState::State::MapCreate);
			break;
		}

		//ゲームクリア処理に変更
		if (Player::Instance().clear) {
			GameState::Instance().SetState(GameState::State::GameClear);
			break;

		}


		//ゲームオーバー処理に切り替え 
		if (Player::Instance().GetDead()) {	//プレイヤーが死んだら
			GameState::Instance().SetState(GameState::State::GameOver);
			break;
		}


		break;
	}
	 /******************************************バトル中***************************************/
	case  GameState::State::Battle: 
	{

		//マップ制作モードに切り替え
		if (gamePad.GetButtonDown() & GamePad::BTN_X) {
			GameState::Instance().SetState(GameState::State::MapCreate);
			break;
		}


		break;
	}

	/******************************************マップ制作中***************************************/
	case  GameState::State::MapCreate: 
	{




		//マップ制作関数更新
		CreateMap::Instance().Update(elapsedTime);
		//ゲーム画面のフレーム更新
		//frameUI->Update(elapsedTime);

		//ゲームに戻る
	/*	if (gamePad.GetButtonDown() & GamePad::BTN_X|| gamePad.GetButtonDown() & GamePad::BTN_A) {
			GameState::Instance().SetState(GameState::State::Game);
			break;
		}*/


		break;
	}

	/********************************************ポーズメニュー中*************************************************/
	case  GameState::State::Pause:
	{

		//カメラ更新
		cameraController->Update(elapsedTime);

		//ゲーム画面のフレーム更新
		//frameUI->Update(elapsedTime);

		//ゲームに戻る
		if (gamePad.GetButtonDown() & GamePad::BTN_START || GetAsyncKeyState('P') & 1) {	//コントローラー：START　キーボード:P

			GameState::Instance().SetState(GameState::State::Game);
			break;

		}

		break;

	}
	/******************************************ゲームオーバー処理****************************************************************/
	case GameState::State::GameOver:	
	{
		


		switch (animeState) // UIのアニメーション
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
			//五秒後にタイトルに戻る
			if (TitleChangeTimer >= 5.0f) {
				SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTitle));
				break;
			}
			break;
	

		}

			//TitleChangeTimer += elapsedTime;
			////五秒後にタイトルに戻る
			//if (TitleChangeTimer >= 5.0f) {
			//	SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTitle));
			//	break;
			//}

	
		break;
	}

	case GameState::State::GameClear: {
		switch (animeState) // UIのアニメーション
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
			//五秒後にタイトルに戻る
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

// 描画処理
void SceneGame::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();



	// 画面クリア＆レンダーターゲット設定
	FLOAT color[] = { 0.09f, 0.1f, 0.2f, 1.0f };	// RGBA(0.0～1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	// 描画処理
	RenderContext rc;
	Camera& camera = Camera::Instance();

	DirectX::XMFLOAT3 cameraDirection = camera.GetFront();

	
	rc.lightDirection = { cameraDirection.x,cameraDirection.y,cameraDirection.z,10.0f};	// ライト方向（下方向）
	
	//	カメラパラメータ設定
	rc.view = camera.GetView();
	rc.projection = camera.GetProjection();

	//シェーダを設定
	Shader* shader = graphics.GetShader(ShaderId::Toon);
	shader->Begin(dc, rc);

	//ステート遷移
	switch (GameState::Instance().currentState)
	{

	/***********************************ゲーム中の描画*************************/
	case GameState::State::Game:			
	{
		// 3Dモデル描画
		{
			//ステージ描画
			StageManager::Instance().Render(dc, shader);

			//ギミック描画
			GimmickManager::Instance().Render(dc, shader);

			//プレイヤー描画
			player->Render(dc, shader);


			//エネミー描画
			EnemyManager::Instance().Render(dc, shader);

			//エフェクト描画
			EffectManager::Instance().Render(rc.view, rc.projection);
			
		}
		shader->End(dc);


		// 2Dスプライト描画
		{
			//プレイヤーUI関連
			player->UIRender(dc);

			//createMap->Render(dc);

			//ゲームUIフレーム描画
			//frameUI->Render(graphics, dc);
			UIFrame::Instance().Render(graphics, dc);

			//ポーズのボタンガイド
			{
				float PauseTexWidth = static_cast<float>(spritePauseButton->GetTextureWidth());
				float PauseTexHeight = static_cast<float>(spritePauseButton->GetTextureHeight());
				spritePauseButton->Render(dc, 64, 660, 512, 32, 0, 0, PauseTexWidth, PauseTexHeight, 0, 1, 1, 1, 1);
			}

			//Enemy_HP_UI関連
#if 0
			{
				RenderEnemyGauge(dc, rc.view, rc.projection);
			}
#endif
		}


		break;
	}


	/****************************************マップ制作********************************************/
	case  GameState::State::MapCreate:
	{
		// 3Dモデル描画
		{
			//ステージ描画
			StageManager::Instance().Render(dc, shader);

			//ギミック描画
			GimmickManager::Instance().Render(dc, shader);

			//プレイヤー描画
			player->Render(dc, shader);


			//エネミー描画
			EnemyManager::Instance().Render(dc, shader);

			{
				//エフェクト描画
				EffectManager::Instance().Render(rc.view, rc.projection);
			}
			shader->End(dc);
		}
		{
			//マップ制作の描画
			CreateMap::Instance().Render(dc);
			

		}
		// 2Dスプライト描画
		{
		
			//ゲームUIフレーム描画

			//frameUI->Render(graphics, dc);
			UIFrame::Instance().Render(graphics, dc);
			{
				float PauseTexWidth = static_cast<float>(spritePauseButton->GetTextureWidth());
				float PauseTexHeight = static_cast<float>(spritePauseButton->GetTextureHeight());
				spritePauseButton->Render(dc, 64, 660, 512, 32, 0, 0, PauseTexWidth, PauseTexHeight, 0, 1, 1, 1, 1);
			}

			

		}


		////　ステージ生成
		{

			//ClickEnemyCreate(dc, rc.view, rc.projection);
		}

		break;
	}




	/************************************* ポーズ中 *********************************************************/

	case  GameState::State::Pause:
	{
		// 3Dモデル描画
		{
			//ステージ描画
			StageManager::Instance().Render(dc, shader);

			//ギミック描画
			GimmickManager::Instance().Render(dc, shader);

			//プレイヤー描画
			player->Render(dc, shader);


			//エネミー描画
			EnemyManager::Instance().Render(dc, shader);

			{
				//エフェクト描画
				EffectManager::Instance().Render(rc.view, rc.projection);
			}

		}

		shader->End(dc);

	
		// 2Dスプライト描画
		{
			float screenWidth = static_cast<float>(graphics.GetScreenWidth());
			float screenHeight = static_cast<float>(graphics.GetScreenHeight());
			float GuideTextureWidth = static_cast<float>(spriteGuide->GetTextureWidth());
			float GuideTextureHeight = static_cast<float>(spriteGuide->GetTextureHeight());

			//spriteGuide->Render(dc, 0, 0, screenWidth, screenHeight, 0, 0, GuideTextureWidth, GuideTextureHeight, 0, 1, 1, 1, 1);


			//ゲームUIフレーム描画

			//frameUI->Render(graphics, dc);
			UIFrame::Instance().Render(graphics, dc);
		}
		break;
	}
	case GameState::State::GameClear: {
		// 3Dモデル描画
		{
			//ステージ描画
			StageManager::Instance().Render(dc, shader);

			//ギミック描画
			GimmickManager::Instance().Render(dc, shader);

			//プレイヤー描画
			player->Render(dc, shader);


			//エネミー描画
			EnemyManager::Instance().Render(dc, shader);

			//エフェクト描画
			EffectManager::Instance().Render(rc.view, rc.projection);

		}
		shader->End(dc);


		// 2Dスプライト描画
		{
			//プレイヤーUI関連
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


			//ゲームUIフレーム描画
			//frameUI->Render(graphics, dc);
			UIFrame::Instance().Render(graphics, dc);



			//ポーズのボタンガイド
			{
				float PauseTexWidth = static_cast<float>(spritePauseButton->GetTextureWidth());
				float PauseTexHeight = static_cast<float>(spritePauseButton->GetTextureHeight());
				spritePauseButton->Render(dc, 64, 660, 512, 32, 0, 0, PauseTexWidth, PauseTexHeight, 0, 1, 1, 1, 1);
			}

			//Enemy_HP_UI関連
#if 0
			{
				RenderEnemyGauge(dc, rc.view, rc.projection);
			}
#endif
		}


		break;
	}
	case GameState::State::GameOver: {
		// 3Dモデル描画
		{
			//ステージ描画
			StageManager::Instance().Render(dc, shader);

			//ギミック描画
			GimmickManager::Instance().Render(dc, shader);

			//プレイヤー描画
			player->Render(dc, shader);


			//エネミー描画
			EnemyManager::Instance().Render(dc, shader);

			//エフェクト描画
			EffectManager::Instance().Render(rc.view, rc.projection);

		}
		shader->End(dc);


		// 2Dスプライト描画
		{
			//プレイヤーUI関連
			player->UIRender(dc);

			//createMap->Render(dc);

			//ゲームUIフレーム描画
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


			//ポーズのボタンガイド
			{
				float PauseTexWidth = static_cast<float>(spritePauseButton->GetTextureWidth());
				float PauseTexHeight = static_cast<float>(spritePauseButton->GetTextureHeight());
				spritePauseButton->Render(dc, 64, 660, 512, 32, 0, 0, PauseTexWidth, PauseTexHeight, 0, 1, 1, 1, 1);
			}

			//Enemy_HP_UI関連
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

	// 3Dデバッグ描画
	{
		// ラインレンダラ描画実行
		graphics.GetLineRenderer()->Render(dc, rc.view, rc.projection);

		// デバッグレンダラ描画実行
		graphics.GetDebugRenderer()->Render(dc, rc.view, rc.projection);
	}

	//デバックプリミティブ描画
	{
		//プレイヤーデバックプリミティブ描画
		//player->DrawDebugPrimitive();

		//エネミーデバッグプリミティブ描画
		//EnemyManager::Instance().DrawDebugPrimitive();

		//GimmickManager::Instance().DrawDebugPrimitive();
	}


	// 2DデバッグGUI描画
	{
		//player->DrawDebugGUI();
		//StageManager::Instance().DrawDebugGUI();
		//GimmickManager::Instance().DrawDebugGUI();
	}

}

//エネミーHP UI
void SceneGame::RenderEnemyGauge(ID3D11DeviceContext* dc,
	const DirectX::XMFLOAT4X4& view,
	const DirectX::XMFLOAT4X4& projection) {

	////ビューポート
	//D3D11_VIEWPORT viewport;
	//UINT numViewports = 1;
	//dc->RSGetViewports(&numViewports, &viewport);

	////変換行列
	//DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&view);
	//DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&projection);
	//DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

	////全ての敵の頭上二HPゲージを表示
	//EnemyManager& enemyManager = EnemyManager::Instance();
	//int enemyCount = enemyManager.GetEnemyCount();

	//for (int i = 0; i < enemyCount; ++i)
	//{
	//	Enemy* enemy = enemyManager.GetEnemy(i);

	//	//エネミーの頭上のワールド座標
	//	DirectX::XMFLOAT3 worldPosition = enemy->GetPosition();
	//	worldPosition.y += enemy->GetHeight();

	//	DirectX::XMVECTOR WorldPosition = DirectX::XMLoadFloat3(&worldPosition);

	//	//ワールド座標からスクリーン座標へ変換
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

	//	//スクリーン座標
	//	DirectX::XMFLOAT2 screenPosition;
	//	DirectX::XMStoreFloat2(&screenPosition, ScreenPosition);

	//	//ゲージの長さ
	//	const float gaugeWidth = 60.0f;
	//	const float gaugeHeight = 10.0f;

	//	float healthRate = enemy->GetHealth() / static_cast<float> (enemy->GetMaxHealth());

	//	

	//	//ゲージ描画
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

//クリックでスライム生成　※デバック用
void SceneGame::ClickEnemyCreate(ID3D11DeviceContext* dc,
	const DirectX::XMFLOAT4X4& view,
	const DirectX::XMFLOAT4X4& projection) {

	Graphics& graphics = Graphics::Instance();


	// 描画処理
	RenderContext rc;
	rc.lightDirection = { 1.0f, 1.0f, 1.0f, 1.0f };	// ライト方向（下方向）

	//ビューポート
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	dc->RSGetViewports(&numViewports, &viewport);

	//変換行列
	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&view);
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&projection);
	DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

	//エネミー配置処理
	//Mouse& mouse = Input::Instance().GetMouse();



	GamePad& gamePad = Input::Instance().GetGamePad();
	if (gamePad.GetButtonDown() & GamePad::BTN_RIGHT_TRIGGER) {
		//マウスカーソル座標を取得
		DirectX::XMFLOAT3 screenPosition;

		screenPosition.x = static_cast<float>(graphics.GetScreenWidth() / 2);
		screenPosition.y = static_cast<float>(graphics.GetScreenHeight() / 2);


		DirectX::XMVECTOR ScreenPosition, WorldPosition;

		//レイの始点を算出
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

		//レイの終点を算出
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

		//レイキャスト
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
