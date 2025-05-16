#include<imgui.h>
#include<time.h>
#include <stdlib.h>
#include <stdio.h>
#include <random>
#include"CreateMap.h"


#include"Stage/StageManager.h"
#include"Stage/StageMain.h"
#include"Stage/Room/StageStart.h"
#include"Stage/Room/StageGoal.h"
#include"Stage/Room/StageStop.h"
#include"Stage/Room/StageCurve.h"
#include"Stage/Room/StageCross.h"
#include"Stage/Room/StageFall.h"
#include"Stage/Room/StageKey.h"
#include"UI//NumFont.h"
#include"UI/PointUI.h"
#include"GameState.h"




#define debug_new new(_NORMAL_BLOCK,__FILE__,__LINE__)

void CreateMap::Initialize() {
	srand((unsigned int)time(NULL));


	//部屋マップ生成
	for (int i = 0; i < Axis_MAX; i++) {
		for (int j = 0; j < Axis_MAX; j++) {
			MapRoom[i][j] = new Room({ i,j,0 }, Room::RoomType::EMPTY, 0, false, false);
		}
	}


	//スタート部屋作成
	{

		for (;;) {
			startMap_Axis = { rand() % Axis_MAX,rand() % Axis_MAX,0 };
			goalMap_Axis = { rand() % Axis_MAX,rand() % Axis_MAX,0 };
			keyMap_Axis = { rand() % Axis_MAX,rand() % Axis_MAX,0 };
			//スタート部屋と隣り合わせにならないようにする処理
			if (!isAdjacent(startMap_Axis, goalMap_Axis) && !isAdjacent(keyMap_Axis, startMap_Axis) && !isAdjacent(keyMap_Axis, goalMap_Axis)) {
				break;
			}
		}

		for (int i = 0; i < 5; i++) {
			TmpRoom_Axis= { rand() % Axis_MAX,rand() % Axis_MAX,0 };
			if (!isAdjacent(TmpRoom_Axis, goalMap_Axis) && !isAdjacent(TmpRoom_Axis, startMap_Axis) && !isAdjacent(TmpRoom_Axis,keyMap_Axis)) {
				registerRoom(MapRoom[TmpRoom_Axis.x][TmpRoom_Axis.y], TmpRoom_Axis, Room::RoomType::EMPTY, keyRoomAng, true, true);
			}
		}


		//スタート部屋が端の場合向きが外側にさせない処理
		startRoomAng = determineRoomAngle(startMap_Axis);
		//プレイヤーのスタートポジション設定
		startPosition = { 250.0f * startMap_Axis.x, 10.0f ,-250.0f * startMap_Axis.y };

		//端になった時に向きが外側にさせない処理
		goalRoomAng = determineRoomAngle(goalMap_Axis);
		//ゴールのポジション設定
		goalPosition = { 250.0f * goalMap_Axis.x,0.0f,-250.0f * goalMap_Axis.y };

		//端になった時に向きが外側にさせない処理
		keyRoomAng = determineRoomAngle(keyMap_Axis);

		//地図にスタート部屋登録
		registerRoom(MapRoom[startMap_Axis.x][startMap_Axis.y], startMap_Axis, Room::RoomType::START, startRoomAng,true,true);
		CreateRoom(MapRoom[startMap_Axis.x][startMap_Axis.y]);//部屋作成
		//地図にゴール部屋登録
		registerRoom(MapRoom[goalMap_Axis.x][goalMap_Axis.y], goalMap_Axis, Room::RoomType::GOAL, goalRoomAng,true,true);
		CreateRoom(MapRoom[goalMap_Axis.x][goalMap_Axis.y]);//部屋作成
	
		//地図にゴール部屋登録
		registerRoom(MapRoom[keyMap_Axis.x][keyMap_Axis.y], keyMap_Axis, Room::RoomType::KEYROOM, keyRoomAng,true,true);
		CreateRoom(MapRoom[keyMap_Axis.x][keyMap_Axis.y]);//部屋作成


	}



	//所持している設計図情報
	for (int i = 0; i < 4; i++) {
		haveRoom[i] = new Room(Room::RoomType::EMPTY,0);
	}

	//積まないように十字路を持っておく
	haveRoom[0]->roomType = Room::CROSS;
	
	//選択している設計図の情報
	selectRoom = new Room(Room::RoomType::EMPTY, 0);


	//スプライト情報
	{
		//地図UI
		spriteDrawer = debug_new Sprite("Data/Sprite/Drawer.png");

		spriteMapB = debug_new Sprite("Data/Sprite/MapButton.png");

		spriteSmoke = debug_new Sprite("Data/Sprite/Smoke.png");

		spriteMap = debug_new Sprite("Data/Sprite/Map.png");

		spriteMake = debug_new Sprite("Data/Sprite/Make.png");

		spriteShop = debug_new Sprite("Data/Sprite/Break.png");

		spriteBack = debug_new Sprite("Data/Sprite/Back.png");

		spriteFrame = debug_new Sprite("Data/Sprite/SelectFrame.png");

		spriteSekkei = debug_new Sprite("Data/Sprite/Sekkei.png");

		spriteSetsumei = debug_new Sprite("Data/Sprite/MapSetsumei.png");

		spritePlayer = debug_new Sprite("Data/Sprite/UnityLogo.png");

		spriteItemShop = debug_new Sprite("Data/Sprite/ItemShop.png");

		spritePay = debug_new Sprite("Data/Sprite/Pay.png");

		spriteDelete = debug_new Sprite("Data/Sprite/RoomBreak.png");

		spriteCoinF = debug_new Sprite("Data/Sprite/CoinFrame.png");
	}


}


void CreateMap::Finalize() {


	//スプライト情報終了化
	{
		if (spriteCoinF != nullptr) {
			delete spriteCoinF;
			spriteCoinF = nullptr;
		}

		if (spriteDelete != nullptr) {
			delete spriteDelete;
			spriteDelete = nullptr;

		}

		if (spritePay != nullptr) {
			delete spritePay;
			spritePay = nullptr;

		}

		if (spriteShop != nullptr) {
			delete spriteShop;
			spriteShop = nullptr;
		}

		if (spritePlayer != nullptr) {
			delete spritePlayer;
			spritePlayer = nullptr;
		}

		if (spriteSekkei != nullptr) {
			delete spriteSekkei;
			spriteSekkei = nullptr;
		}
		if (spriteBack != nullptr) {
			delete spriteBack;
			spriteBack = nullptr;
		}
		if (spriteItemShop != nullptr) {
			delete spriteItemShop;
			spriteItemShop = nullptr;
		}

		if (spriteMake != nullptr) {
			delete spriteMake;
			spriteMake = nullptr;
		}

		if (spriteFrame != nullptr) {
			delete spriteFrame;
			spriteFrame = nullptr;
		}


		if (spriteMap != nullptr) {
			delete spriteMap;
			spriteMap = nullptr;
		}

		if (spriteSetsumei != nullptr) {
			delete spriteSetsumei;
			spriteSetsumei = nullptr;
		}
		
		if (spriteSmoke != nullptr) {
			delete spriteSmoke;
			spriteSmoke = nullptr;
		}

		if (spriteMapB != nullptr) {
			delete spriteMapB;
			spriteMapB = nullptr;
		}

		if (spriteDrawer != nullptr) {
			delete spriteDrawer;
			spriteDrawer = nullptr;
		}

		if (selectRoom != nullptr) {
			delete selectRoom;
			selectRoom = nullptr;
		}
	}


	//所持している部屋の終了化
	for (int i = 0; i < 4; i++) {
		if (haveRoom[i] != nullptr) {
			delete haveRoom[i];
			haveRoom[i] = nullptr;
		}
	}

	//地図上の部屋の終了化
	for (int i = 0; i < Axis_MAX; i++) {
		for (int j = 0; j < Axis_MAX; j++) {
			if (MapRoom[i][j] != nullptr) {

				delete MapRoom[i][j];
				MapRoom[i][j] = nullptr;

			}
		}
	}



}

//更新処理
void CreateMap::Update(float elapsedTime) {
	srand((unsigned int)time(NULL));

	//ゲームパッド取得
	GamePad& gamePad = Input::Instance().GetGamePad();
#if 0
	switch (state)
	{
	case State::STARTMENU:
	{
		switch (animeState) // UIのアニメーション
		{
		case 0:
			spriteMakePos = { 1800,50 };
			spriteShopPos = { 1800,200};
			MakeColor = { 1,1,1,1 };
			ShopColor = { 1,1,1,1 };
			isAnimation = true;
			animeState++;
			break;
		case 1:
			spriteMakePos.x -= MOVE_SPEED_FAST * elapsedTime;
			if (spriteMakePos.x <= TARGET_POSITION) {
				animeState++;
			}
			break;
		case 2:
			spriteMakePos.x += 1000.0f * elapsedTime;
		
			if (spriteMakePos.x >= 1000.0f ) {
				animeState++;
			}
			break;
		case 3:

			spriteShopPos.x -= 3000.0f * elapsedTime;
			if ( spriteShopPos.x <= 800.0f) {
				animeState++;
			}
			break;
		case 4:
			spriteShopPos.x += 1000.0f * elapsedTime;
			if (spriteShopPos.x >= 1000.0f) {
				animeState++;
			}
			break;
		case 5:
			isAnimation = false;
			animeState = 0;
			state = State::MENU;
			break;


		}

		
		break;
	}
	case State::MENU://メニュー画面へ遷移
	{
		SelectMenu();

	break;
	}
	case State::MAKE://部屋づくりモード（パズルモード）へ遷移
	{
		MakeColor = { 1,1,1,1 };
		
		MakeRoom();
		MoveCursorFrame();

	}break;
	case State::SHOP://部屋破壊のモードへ遷移
	{
		ShopColor = { 1,1,1,1 };
		ShopMenu(elapsedTime);
		break;
	}

	}
#endif

	switch (state)
	{
	case State::STARTMENU:
	{
		switch (animeState) // UIのアニメーション
		{
		case 0://初期化
			spriteDrawerPos = { 200,-450.0f };
			spriteMapBPos = { 550,-300.0f };
			animeState++;
			break;
		case 1:
			spriteDrawerPos.y += MOVE_SPEED_FAST * elapsedTime;
			spriteMapBPos.y += MOVE_SPEED_FAST * elapsedTime;
			//一定を超えたら止まる
			if (spriteDrawerPos.y >= 0.0f)spriteDrawerPos.y = 0.0f;
			if (spriteMapBPos.y >= 200.0f)spriteMapBPos.y = 250.0f;

			if (spriteDrawerPos.y >= 0.0f&&spriteMapBPos.y >= 250.0f) {
				animeState++;
			}
			break;
		case 2:
			spriteDrawerPos = { 200,0.0f };
			spriteMapBPos = { 550,250.0f };											
			isAnimation = false;
			animeState = 0;
			state = State::MENU;
			break;

		}


		break;
	}
	case State::MENU://メニュー画面へ遷移
	{
		SelectMenu(elapsedTime);
		
		//ゲームに戻る
		if (gamePad.GetButtonDown() & GamePad::BTN_X || gamePad.GetButtonDown() & GamePad::BTN_A) {
		state = State::END;
		}

		break;
	}

	case State::STARTMAKE://部屋づくりモード（パズルモード）へ遷移
	{
		//部屋作成の初期化
		spriteDrawerPos = { 200,0.0f };
		spriteMapBPos = { 550,150.0f };
		MakeColor = { 1,1,1,1 };
		isAnimation = false;
		animeState = 0;
		state = State::MAKE;
		break;
	}

	case State::MAKE://部屋づくりモード（パズルモード）へ遷移
	{
		MakeColor = { 1,1,1,1 };

		MakeRoom();
		MoveCursorFrame();
		break;
	}
	case State::STARTSHOP://ショップの初期化
	{
		spriteDrawerPos = { 200,0.0f };
		spriteMapBPos = { 550,150.0f };
		ShopColor = { 1,1,1,1 };
		isAnimation = false;
		animeState = 0;
		state = State::SHOP;
		break;
	}

	case State::SHOP://部屋破壊のモードへ遷移
	{
		ShopColor = { 1,1,1,1 };
		ShopMenu(elapsedTime);
		break;
	}

	case State::END:

		switch (animeState) // UIのアニメーション
		{
		case 0:
			spriteDrawerPos = { 200,0.0f };
			spriteMapBPos = { 550,150.0f };
			isAnimation = true;
			animeState++;
			break;
		case 1:
			spriteDrawerPos.y -= MOVE_SPEED_FAST * elapsedTime;
			spriteMapBPos.y -= MOVE_SPEED_FAST * elapsedTime;
			//一定を超えたら止まる
			if (spriteDrawerPos.y <= -450.0f)spriteDrawerPos.y = -450.0f;
			if (spriteMapBPos.y <= -300.0f)spriteMapBPos.y = -300.0f;

			if (spriteDrawerPos.y <= -450.0f && spriteMapBPos.y <= -300.0f) {
				animeState++;
			}
		
			break;
		case 2:
			spriteDrawerPos = { 200,-450.0f };
			spriteMapBPos = { 550,-300.0f };
			isAnimation = false;
			animeState = 0;
			GameState::Instance().SetState(GameState::State::Game);
			state = State::STARTMENU;
			break;

		}

		
		break;
	}




}



//描画処理
void CreateMap::Render(ID3D11DeviceContext* dc) {

	Graphics& graphics = Graphics::Instance();

	DrawDebugGUI();

	

	//PlayerRender(dc);

	switch (state)
	{
	case CreateMap::State::STARTMENU: //メニューの描画
		MenuRender(dc);
		//MapRender(dc);
		//PlayerRender(dc);
		break;
	case CreateMap::State::MENU://メニューの描画
		MenuRender(dc);
		//MapRender(dc);
		//PlayerRender(dc);
		break;
	case CreateMap::State::STARTMAKE://マップ作成の描画
		MapRender(dc);
		PlayerRender(dc);
		break;
	case CreateMap::State::MAKE://マップ作成の描画
		MapRender(dc);
		MakeRender(dc);
		PlayerRender(dc);
		//FrameRender(dc);
		break;
	case CreateMap::State::STARTSHOP://ショップの描画
		break;
	case CreateMap::State::SHOP://ショップの描画
		ShopRender(dc);
		break;
	case CreateMap::State::END://メニューの描画
		MenuRender(dc);
		//MapRender(dc);
		//PlayerRender(dc);
		break;
	default:
		break;
	}

	PointUI::Instance().Render(graphics, dc);

}

void CreateMap::MapRender(ID3D11DeviceContext* dc) {
	//地図のUI描画
	{
		float mapTexHeight = static_cast<float>(spriteMap->GetTextureHeight());
		float mapTexWidth = static_cast<float>(spriteMap->GetTextureWidth());
		spriteMap->Render(dc, spriteMapPos.x, spriteMapPos.y, spriteMapSize, spriteMapSize, 0, 0, mapTexWidth, mapTexHeight, 0, 1, 1, 1, 1);
	}


	for (int i = 0; i < Axis_MAX; i++) {
		for (int j = 0; j < Axis_MAX; j++) {
			MapRoom[j][i]->MapRender(dc);
		}
	}
}

void CreateMap::SelectMenu(float elapsedTime) {

	//もし地図ボタン（スプライトで表示）を押されたら（カーソルが範囲内かつが押されたら)stateをMAKEに変更
	if (PointUI::Instance().ClickButton(spriteMapBPos.x, spriteMapBPos.y, spriteMapBSize.x, spriteMapBSize.y)) {
		//カーソルが範囲内の時
		ShakeAngle(mapBangle, elapsedTime, 100.0f, -10.0f, 10.0f);


		if ((gamepad.GetButton() & GamePad::BTN_B && GameState::Instance().controllerState == GameState::Controller) ||
			(mouse.GetButton() & Mouse::BTN_LEFT && GameState::Instance().controllerState == GameState::ControllerState::MouseAndKeyboard))
		{
			MapBColor = { 0.5f,0.5f,0.5f,1 };
		}
		else if ((gamepad.GetButtonUp() & GamePad::BTN_B && GameState::Instance().controllerState == GameState::Controller) ||
			(mouse.GetButtonUp() & Mouse::BTN_LEFT && GameState::Instance().controllerState == GameState::ControllerState::MouseAndKeyboard))
		{
			state = State::MAKE;
		}
		else
		{
			//カーソルが範囲内の時
			//色を変える
			MapBColor = { 1,1,1,1 };
			//サイズを変える
			spriteMapBSize = { 160 * 1.2f,160 * 1.2f };
		}
	}
	else
	{
		//カーソルが範囲外の時
		//サイズを戻す
		spriteMapBSize = { 160,160 };
		//色を戻す
		MapBColor = { 1,1,1,1 };

		mapBangle = 0;

	}
	

	//もしショップボタン（スプライトで表示）を押されたら（カーソルが範囲内かつが押されたら)stateをBREAKに変更
	if (PointUI::Instance().ClickButton(spriteShopPos.x, spriteShopPos.y, spriteShopSize.x, spriteShopSize.y)) {
		if ((gamepad.GetButton() & GamePad::BTN_B &&GameState::Instance().controllerState == GameState::ControllerState::Controller) ||
			(mouse.GetButton() & Mouse::BTN_LEFT &&GameState::Instance().controllerState == GameState::ControllerState::MouseAndKeyboard)) 
		{
			ShopColor = { 0.5f,0.5f,0.5f,1 };	
		}
		else if ((gamepad.GetButtonUp() & GamePad::BTN_B &&GameState::Instance().controllerState == GameState::ControllerState::Controller)||
			(mouse.GetButtonUp() & Mouse::BTN_LEFT &&GameState::Instance().controllerState == GameState::ControllerState::MouseAndKeyboard))
		{
			state = State::SHOP;
		}
		else 
		{
			ShopColor = { 1,1,1,1 };
		}
	}
	else 
	{
		ShopColor = { 1,1,1,1 };
	}


}

//メニュー画面描画処理
void CreateMap::MenuRender(ID3D11DeviceContext* dc) {

	Graphics& graphics = Graphics::Instance();

	//引き出しの描画
	{
		float drawerTexHeight = static_cast<float>(spriteDrawer->GetTextureHeight());
		float drawerTexWidth = static_cast<float>(spriteDrawer->GetTextureWidth());
		spriteDrawer->Render(dc, spriteDrawerPos.x, spriteDrawerPos.y, spriteDrawerSize.x, spriteDrawerSize.y, 0, 0, drawerTexWidth, drawerTexHeight, 0, 1, 1, 1, 1);
	}

	//地図のボタン描画
	{
		float mapBTexHeight = static_cast<float>(spriteMapB->GetTextureHeight());
		float mapBTexWidth = static_cast<float>(spriteMapB->GetTextureWidth());
		//ボタンの影描画
		spriteMapB->Render(dc, spriteMapBPos.x + 10, spriteMapBPos.y + 10, spriteMapBSize.x, spriteMapBSize.y, 0, 0, mapBTexWidth, mapBTexHeight, mapBangle * 1.5f, 0.0f, 0.0f, 0.0f, 0.5f);
		//ボタンの描画
		spriteMapB->Render(dc, spriteMapBPos.x, spriteMapBPos.y, spriteMapBSize.x, spriteMapBSize.y, 0, 0, mapBTexWidth, mapBTexHeight, mapBangle, MapBColor.x, MapBColor.y, MapBColor.z, MapBColor.w);

	}


	//作るボタンを描画
	//float makeTexHeight = static_cast<float>(spriteMake->GetTextureHeight());
	//float makeTexWidth = static_cast<float>(spriteMake->GetTextureWidth());
	//spriteMake->Render(dc, spriteMakePos.x, spriteMakePos.y, spriteMakeSize.x, spriteMakeSize.y, 0, 0, makeTexWidth, makeTexHeight, 0, MakeColor.x, MakeColor.y, MakeColor.z, MakeColor.w);

	////壊すボタンを描画
	//float breakTexHeight = static_cast<float>(spriteShop->GetTextureHeight());
	//float breakTexWidth = static_cast<float>(spriteShop->GetTextureWidth());
	//spriteShop->Render(dc, spriteShopPos.x, spriteShopPos.y, spriteShopSize.x, spriteShopSize.y, 0, 0, breakTexWidth, breakTexHeight, 0, ShopColor.x, ShopColor.y, ShopColor.z, ShopColor.w);

}


//部屋作成モード処理
void CreateMap::MakeRoom(){

	spriteMakePos = { 70,550 };

	//部屋の回転処理
	
	if ((gamepad.GetButtonDown() & GamePad::BTN_LEFT_SHOULDER && GameState::Instance().controllerState == GameState::Controller) ||
		(GetAsyncKeyState('Q') & 1 && GameState::Instance().controllerState == GameState::MouseAndKeyboard)) {
		//部屋の回転
		roomAng--;
		selectRoom->angleType = roomAng;

	}
	else if ((gamepad.GetButtonDown() & GamePad::BTN_RIGHT_SHOULDER &&GameState::Instance().controllerState == GameState::Controller) 
		||(GetAsyncKeyState('E') & 1 &&GameState::Instance().controllerState == GameState::MouseAndKeyboard)) 
	{
		//部屋の回転
		roomAng++;
		selectRoom->angleType = roomAng;
	}
	


	//設計図内の処理
	for (int i = 0; i < 4; i++)
	{
		if (PointUI::Instance().ClickButton(bulePrint.spritePos.x, bulePrint.spritePos.y + bulePrint.blockInterval * i,
			bulePrint.spriteSize.x, bulePrint.spriteSize.y)) //もしカーソルが設計図に触れていたら
		{
			if ((gamepad.GetButtonDown() & GamePad::BTN_B && GameState::Instance().controllerState == GameState::Controller)|| 
			(mouse.GetButtonDown() & Mouse::BTN_LEFT&& GameState::Instance().controllerState == GameState::MouseAndKeyboard)) //もしBボタンor左クリックが押されたら
			{
				if (selectRoom->roomType == Room::EMPTY) //もし何も選択されていなければ
				{
					//選択欄の設計図を所持する処理
					selectRoom->roomType = haveRoom[i]->roomType;
					haveRoom[i]->color = { 0.8f,0.8f,0.8f,1.0f };
					selectNo = i;
				}
				else {

					haveRoom[selectNo]->roomType = selectRoom->roomType;
					haveRoom[selectNo]->color = { 1,1,1,1 };
					selectRoom->roomType = Room::EMPTY;
					selectRoom->angleType = 0;
				}

			}

		}

	}

	//ステージマネジャーインスタンス取得	
	StageManager& stageManager = StageManager::Instance();

	//ステージ生成
	if (PointUI::Instance().ClickButton(spriteMapPos.x, spriteMapPos.y, spriteMapSize, spriteMapSize)) {
		if ((gamepad.GetButtonDown() & GamePad::BTN_B && GameState::Instance().controllerState == GameState::Controller) || 
			(mouse.GetButtonDown() & Mouse::BTN_LEFT && GameState::Instance().controllerState == GameState::MouseAndKeyboard))
		{
			//部屋が制作可能だったら
			if (!MapRoom[Map_Axis.x][Map_Axis.y]->canCreate) {

				if (selectRoom->roomType!=Room::EMPTY) {
					//部屋の登録処理
					registerRoom(MapRoom[Map_Axis.x][Map_Axis.y], Map_Axis, selectRoom->roomType, selectRoom->angleType,false,true);

					clearRoom = true;//部屋削除を可能にする

					CreateRoom(MapRoom[Map_Axis.x][Map_Axis.y]);//部屋の生成

					//所持している設計図をクリアする
					{
						selectRoom->roomType = Room::EMPTY;
						selectRoom->angleType = 0;
						haveRoom[selectNo]->roomType = Room::EMPTY;
						haveRoom[selectNo]->color = { 1,1,1,1 };
						roomAng = 0;
					}

				}
			}
		}
	}

	////破壊ボタンの処理
	if (PointUI::Instance().ClickButton(spriteDeletePos.x, spriteDeletePos.y, spriteDeleteSize.x, spriteDeleteSize.y)) {
		if ((gamepad.GetButton() & GamePad::BTN_B && GameState::Instance().controllerState == GameState::Controller) || 
			(mouse.GetButton() & Mouse::BTN_LEFT && GameState::Instance().controllerState == GameState::MouseAndKeyboard))
		{
			DeleteColor = { 0.5f,0.5f,0.5f,1 };	
		}
		else if ( selectRoom->roomType!=Room::EMPTY &&
			((gamepad.GetButtonUp() & GamePad::BTN_B&&GameState::Instance().controllerState == GameState::Controller) || 
			(mouse.GetButtonUp() & Mouse::BTN_LEFT && GameState::Instance().controllerState == GameState::MouseAndKeyboard))) 
		{
			selectRoom->roomType = Room::EMPTY;
			selectRoom->angleType = 0;
			haveRoom[selectNo]->roomType = Room::EMPTY;
			haveRoom[selectNo]->color = { 1,1,1,1 };
			roomAng = 0;
		}
		else
		{
			DeleteColor = { 1,1,1,1 };
		}
	}
	else
	{
		DeleteColor = { 1,1,1,1 };
	}


	//ゲーム画面に戻った時の処理
	if (selectRoom->roomType != Room::EMPTY) {
		if ((gamepad.GetButtonDown() & GamePad::BTN_X && GameState::Instance().controllerState == GameState::Controller)||
			(GetAsyncKeyState('M') & 1 && GameState::Instance().controllerState == GameState::MouseAndKeyboard))
		{
		haveRoom[selectNo]->roomType = selectRoom->roomType;
		haveRoom[selectNo]->color = { 1,1,1,1 };
		selectRoom->roomType = Room::EMPTY;
		selectRoom->angleType = 0;
		}
	}

	

	//戻るボタン処理
	if (PointUI::Instance().ClickButton(spriteBackPos.x, spriteBackPos.y, spriteBackSize.x, spriteBackSize.y)) {



		if ((gamepad.GetButton() & GamePad::BTN_B && GameState::Instance().controllerState == GameState::Controller) || 
			(mouse.GetButton() & Mouse::BTN_LEFT && GameState::Instance().controllerState == GameState::MouseAndKeyboard))
		{
			BackColor = { 0.5f,0.5f,0.5f,1 };
		}
		else if ((gamepad.GetButtonUp() & GamePad::BTN_B && GameState::Instance().controllerState == GameState::Controller) || 
			(mouse.GetButtonUp() & Mouse::BTN_LEFT&& GameState::Instance().controllerState == GameState::MouseAndKeyboard))
		{
			//設計図を持っていたらリセットする処理
			if (selectRoom->roomType != Room::EMPTY)
			{
				haveRoom[selectNo]->roomType = selectRoom->roomType;
				haveRoom[selectNo]->color = { 1,1,1,1 };
				selectRoom->roomType = Room::EMPTY;
				selectRoom->angleType = 0;
			}
			state = State::STARTMENU;
		}
		else
		{
			BackColor = { 1,1,1,1 };
		}
	}
	else
	{
		BackColor = { 1,1,1,1 };
	}


}

//部屋作成モード描画処理
void CreateMap::MakeRender(ID3D11DeviceContext* dc) {



	//作るボタンを描画
	float makeTexHeight = static_cast<float>(spriteMake->GetTextureHeight());
	float makeTexWidth = static_cast<float>(spriteMake->GetTextureWidth());
	spriteMake->Render(dc, spriteMakePos.x, spriteMakePos.y, spriteMakeSize.x, spriteMakeSize.y, 0, 0, makeTexWidth, makeTexHeight, 0, MakeColor.x, MakeColor.y, MakeColor.z, MakeColor.w);

	////ゴミ箱ボタンを描画
	float deleteTexHeight = static_cast<float>(spriteDelete->GetTextureHeight());
	float deleteTexWidth = static_cast<float>(spriteDelete->GetTextureWidth());
	

	spriteDelete->Render(dc,
		spriteDeletePos.x, spriteDeletePos.y,
		spriteDeleteSize.x, spriteDeleteSize.y,
		0, 0,
		deleteTexWidth, deleteTexHeight,
		0,
		DeleteColor.x, DeleteColor.y, DeleteColor.z, DeleteColor.w
	);

	//戻るボタンを描画
	float backTexHeight = static_cast<float>(spriteBack->GetTextureHeight());
	float backTexWidth = static_cast<float>(spriteBack->GetTextureWidth());
	spriteBack->Render(dc, spriteBackPos.x, spriteBackPos.y, spriteBackSize.x, spriteBackSize.y, 0, 0, backTexWidth, backTexHeight, 0, BackColor.x, BackColor.y, BackColor.z, BackColor.w);

	//設計図枠を描画
	float SekkeiTexHeight = static_cast<float>(spriteSekkei->GetTextureHeight());
	float SekkeiTexWidth = static_cast<float>(spriteSekkei->GetTextureWidth());
	spriteSekkei->Render(dc, spriteSekkeiPos.x, spriteSekkeiPos.y, spriteSekkeiSize.x, spriteSekkeiSize.y, 0, 0, SekkeiTexWidth, SekkeiTexHeight, 0, 1, 1, 1, 1);

	//設計図枠を描画
	float SetsumeiTexHeight = static_cast<float>(spriteSetsumei->GetTextureHeight());
	float SetsumeiTexWidth = static_cast<float>(spriteSetsumei->GetTextureWidth());
	spriteSetsumei->Render(dc, spriteSetsumeiPos.x, spriteSetsumeiPos.y, spriteSetsumeiSize.x, spriteSetsumeiSize.y, 0, 0, SetsumeiTexWidth, SetsumeiTexHeight, 0, 1, 1, 1, 1);

	//選択しているマスのフレームを描画
	FrameRender(dc);

	//設計図（パズルピース)の描画
	for (int i = 0; i < 4; i++)
	{
		if (haveRoom[i]->roomType != Room::RoomType::EMPTY) 
		{
			haveRoom[i]->BulePrintRender(dc, i);
		}
	}

	//選択している設計図の描画
	if (selectRoom->roomType != Room::EMPTY) {
		selectRoom->SelectRender(dc, PointUI::Instance().GetPoint());
	}
}



//ショップモード処理
void CreateMap::ShopMenu(float elapsedTime) {
	srand((unsigned int)time(NULL));
	spriteShopPos = { 1000, 50 };


	//戻る時の処理
	if (PointUI::Instance().ClickButton(spriteBackPos.x, spriteBackPos.y, spriteBackSize.x, spriteBackSize.y)) 
	{
		//設計図を持っていたらリセットする処理
		if ((gamepad.GetButton() & GamePad::BTN_B && GameState::Instance().controllerState == GameState::Controller) || 
			(mouse.GetButton() & Mouse::BTN_LEFT && GameState::Instance().controllerState == GameState::MouseAndKeyboard))
		{
			BackColor = {0.5f,0.5f,0.5f,1 };	
		}
		else if ((gamepad.GetButtonUp() & GamePad::BTN_B && GameState::Instance().controllerState == GameState::Controller) || 
			(mouse.GetButtonUp() & Mouse::BTN_LEFT&& GameState::Instance().controllerState == GameState::MouseAndKeyboard))
		{
			state = State::STARTMENU;
		}
		else 
		{
			BackColor = { 1,1,1,1 };
		}
	}
	else 
	{
		BackColor = { 1,1,1,1 };
	}

	//購入処理
	for (int i = 0; i < 3; i++) {


		switch (i)
		{
		case 0://回復アイテム

			//回復アイテムの購入処理
			if (PointUI::Instance().ClickButton(spritePayPos[i].x, spritePayPos[i].y, spritePaySize.x, spritePaySize.y)) 
			{
				if ((gamepad.GetButton() & GamePad::BTN_B && GameState::Instance().controllerState == GameState::Controller) ||
					(mouse.GetButton() & Mouse::BTN_LEFT&& GameState::Instance().controllerState == GameState::MouseAndKeyboard))
				{
					payColor[0] = { 0.5,0.5,0.5,1 };//ボタンを押している時の色
				}
				else if (((gamepad.GetButtonUp() & GamePad::BTN_B && GameState::Instance().controllerState == GameState::Controller) || 
					(mouse.GetButtonUp() & Mouse::BTN_LEFT && GameState::Instance().controllerState == GameState::MouseAndKeyboard)) &&
					payTimer <= 0.0f ) //ボタンを上げた時アイテムを購入
				{	
					if (Player::Instance().lifeItem <= 5 && Player::Instance().AddCoin(-2)) 
					{
						Player::Instance().lifeItem++;
						payTimer = 2.0f;
					}
				}
				else 
				{
					payColor[0] = { 1,1,1,1 };
				}
			}
			else 
			{ 
					payColor[0] = { 1,1,1,1 };
			}


			//購入不可の時
			if (Player::Instance().lifeItem >= 5 || Player::Instance().coin < 2)
			{	
				payColor[0] = { 0.4,0.4,0.4,1 };	
			}


			break;

		case 1://パワーアップアイテム
			//パワーアップアイテムの購入処理
			if (PointUI::Instance().ClickButton(spritePayPos[i].x, spritePayPos[i].y, spritePaySize.x, spritePaySize.y)) {


				if ((gamepad.GetButton() & GamePad::BTN_B && GameState::Instance().controllerState == GameState::Controller) || 
					(mouse.GetButton() & Mouse::BTN_LEFT && GameState::Instance().controllerState == GameState::MouseAndKeyboard))
				{
					payColor[1] = { 0.7,0.7,0.7,1 };
				}
				else if ((gamepad.GetButtonUp() & GamePad::BTN_B && GameState::Instance().controllerState == GameState::Controller) || 
					(mouse.GetButtonUp() & Mouse::BTN_LEFT && GameState::Instance().controllerState == GameState::MouseAndKeyboard) && 
					payTimer <= 0.0f) 
				{


					if (Player::Instance().AddCoin(-3) && Player::Instance().powerItem <= 2) 
					{
						payTimer = 2.0f;
						Player::Instance().powerItem++;
					}


				}
				else 
				{
					payColor[1] = { 1,1,1,1 };
				}


			}
			else 
			{
				payColor[1] = { 1,1,1,1 };
			}

			//購入不可の時
			if (Player::Instance().powerItem >= 2 || Player::Instance().coin < 3) 
			{	
				payColor[1] = { 0.4,0.4,0.4,1 };
			}
			break;


		case 2://設計図

			//設計図の購入処理
			if (PointUI::Instance().ClickButton(spritePayPos[i].x, spritePayPos[i].y, spritePaySize.x, spritePaySize.y)) {

				if ((gamepad.GetButton() & GamePad::BTN_B && GameState::Instance().controllerState == GameState::Controller) || 
					(mouse.GetButton() & Mouse::BTN_LEFT && GameState::Instance().controllerState == GameState::MouseAndKeyboard)) {
					payColor[2] = { 0.7,0.7,0.7,1 };
				}
				else if (gamepad.GetButtonUp() & GamePad::BTN_B || mouse.GetButtonUp() & Mouse::BTN_LEFT && payTimer <= 0.0f) {
					
					std::random_device rd;
					std::mt19937 gen(rd());
					std::uniform_int_distribution<int> dis(1, 100);
					payTimer = 2.0f;
					rnd = dis(gen);
					GetRoom(rnd);

				}
				else {
					payColor[2] = { 1,1,1,1 };
				}
			}
			else {


				payColor[2] = { 1,1,1,1 };

			}

			//購入不可の時
			if (HaveRoomMax() || Player::Instance().coin < 5) {
				payColor[2] = { 0.4,0.4,0.4,1 };
			}


			break;
		default:
			break;
		}

		//タイマー
		if (payTimer > 0) {
			payTimer -= elapsedTime;
		}


	}
}
//ショップモード描画処理
void CreateMap::ShopRender(ID3D11DeviceContext* dc) {


	//スプライト情報取得
	float shopTexHeight = static_cast<float>(spriteItemShop->GetTextureHeight());
	float shopTexWidth = static_cast<float>(spriteItemShop->GetTextureWidth());

	float payTexHeight = static_cast<float>(spritePay->GetTextureHeight());
	float payTexWidth = static_cast<float>(spritePay->GetTextureWidth());


	//スプライトの描画
	for (int i = 0; i < 3; i++) {
		spriteItemShop->Render(dc,
			spriteItemShopPos[i].x, spriteItemShopPos[i].y,
			spriteItemShopSize.x, spriteItemShopSize.y,
			(shopTexWidth / 3) * i, 0,
			shopTexWidth / 3, shopTexHeight,
			0,
			1, 1, 1, 1);

		spritePay->Render(dc,
			spritePayPos[i].x, spritePayPos[i].y,
			spritePaySize.x, spritePaySize.y,
			0, 0,
			payTexWidth, payTexHeight,
			0,
			payColor[i].x, payColor[i].y, payColor[i].z, payColor[i].w);

	}

	//壊すボタンを描画
	float breakTexHeight = static_cast<float>(spriteShop->GetTextureHeight());
	float breakTexWidth = static_cast<float>(spriteShop->GetTextureWidth());
	spriteShop->Render(dc, spriteShopPos.x, spriteShopPos.y, spriteShopSize.x, spriteShopSize.y, 0, 0, breakTexWidth, breakTexHeight, 0, 1, 1, 1, 1);

	float coinFTexHeight = static_cast<float>(spriteCoinF->GetTextureHeight());
	float coinFTexWidth = static_cast<float>(spriteCoinF->GetTextureWidth());
	spriteCoinF->Render(dc, spriteCoinFPos.x, spriteCoinFPos.y, spriteCoinFSize.x, spriteCoinFSize.y, 0, 0, coinFTexWidth, coinFTexHeight, 0, 1, 1, 1, 1);
	NumFont::Instance().NumRender(dc, Player::Instance().coin, { 1170, 195 }, {70, 70}, 0, {1,1,0,1});

	//戻るボタンを描画
	float backTexHeight = static_cast<float>(spriteBack->GetTextureHeight());
	float backTexWidth = static_cast<float>(spriteBack->GetTextureWidth());
	spriteBack->Render(dc, spriteBackPos.x, spriteBackPos.y, spriteBackSize.x, spriteBackSize.y, 0, 0, backTexWidth, backTexHeight, 0, BackColor.x, BackColor.y, BackColor.z, BackColor.w);

}



//ゲームパッドorマウスで選択されている部屋を枠で囲む処理
void CreateMap::MoveCursorFrame() {

	//枠の移動入力処理
	if(GameState::Instance().controllerState==GameState::Controller)
	{
		if (gamepad.GetButtonDown() & GamePad::BTN_UP && Map_Axis.y > 0) {
			--Map_Axis.y;
		}
		if (gamepad.GetButtonDown() & GamePad::BTN_DOWN && Map_Axis.y < Axis_MAX - 1) {
			++Map_Axis.y;
		}
		if (gamepad.GetButtonDown() & GamePad::BTN_LEFT && Map_Axis.x > 0) {
			--Map_Axis.x;
		}
		if (gamepad.GetButtonDown() & GamePad::BTN_RIGHT && Map_Axis.x < Axis_MAX - 1) {
			++Map_Axis.x;
		}
	}

	for (int i = 0; i < Axis_MAX; i++) {
		for (int j = 0; j < Axis_MAX; j++) {
			if (PointUI::Instance().ClickButton(spriteBlockPos.x + blockInterval * j, spriteBlockPos.y + blockInterval * i, spriteBlockSize.x, spriteBlockSize.y))
			{
				
				Map_Axis.x = j;
				Map_Axis.y = i;
				
			}
		}
	}

}

//選択されている部屋の枠描画処理
void CreateMap::FrameRender(ID3D11DeviceContext* dc) 
{

	float frameTexHeight = static_cast<float>(spriteFrame->GetTextureHeight());
	float frameTexWidth = static_cast<float>(spriteFrame->GetTextureWidth());
	spriteFrame->Render(dc, spriteFramePos.x + blockInterval * Map_Axis.x, spriteFramePos.y + blockInterval * Map_Axis.y, spriteFrameSize.x, spriteFrameSize.y, 0, 0, frameTexWidth, frameTexHeight, 0, 1, 1, 1, 1);

}

//プレイヤーの描画処理
void CreateMap::PlayerRender(ID3D11DeviceContext* dc) {
	//スプライト情報取得
	float playerTexHeight = static_cast<float>(spritePlayer->GetTextureHeight());
	float playerTexWidth = static_cast<float>(spritePlayer->GetTextureWidth());

	DirectX::XMINT3 Axis = Player::Instance().GetAxis();
	DirectX::XMFLOAT3 angle = Player::Instance().GetAngle();
	angle.y = DirectX::XMConvertToDegrees(angle.y);

	//プレイヤーの描画処理
	spritePlayer->Render(dc, (spriteBlockPos.x +15.0f) + (blockInterval) * Axis.x, (spriteBlockPos.y + 15.0f) + blockInterval * Axis.y, spriteBlockSize.x -30, spriteBlockSize.y -30, 0, 0, playerTexWidth, playerTexHeight, 0, 0, 0, 0, 1);

}


void CreateMap::DrawDebugGUI()
{
	srand((unsigned int)time(NULL));

	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Room", nullptr, ImGuiWindowFlags_None)) {



		if (ImGui::CollapsingHeader("RoomType", ImGuiTreeNodeFlags_DefaultOpen)) {

			ImGui::Text("0:UNKNOWN");
			ImGui::Text("1:STARTROOM");
			ImGui::Text("2:GOALROOM");
			ImGui::Text("3:STOPROOM");
			ImGui::Text("4:CURVEROOM");
			ImGui::Text("5:CROSSROOM");
			ImGui::Text("6:FALLROOM");


		}

		if (ImGui::CollapsingHeader("RoomSetting", ImGuiTreeNodeFlags_DefaultOpen)) {


			ImGui::InputInt("RoomType", &type);

			if (ImGui::Button("Get Room")) {

				GetRoom(type);
			}

			if (ImGui::Button("Get Coin")) {

				Player::Instance().AddCoin(10);
			}


			if (ImGui::Button("Get RandomRoom")||gamepad.GetButtonDown()&GamePad::BTN_BACK) {
				
				GetRoom(rnd);
			}

			if (ImGui::Button("Room Clear")) {
				if (clearRoom) {
					ResetRoom(NewRoom_Axis);
				}
				clearRoom = false;
				
			}


		}


	}

	ImGui::End();

}

//設計図を入手する処理
void CreateMap::GetRoom(int Rnd) {
	//部屋の種類を決定する処理
	for (int i = 0; i < 4; i++) {
		//もし部屋が空いていたら
		if (haveRoom[i]->roomType == Room::RoomType::EMPTY&&Player::Instance().AddCoin(-5)) {

			if (Rnd <= 100 && Rnd > 95)
			{
				haveRoom[i]->roomType = Room::STOPROOM;
			}
			else if (Rnd <= 95 && Rnd > 75) {
				haveRoom[i]->roomType = Room::CROSS;
			}
			else if (Rnd <= 75 && Rnd > 45) {
				haveRoom[i]->roomType = Room::CURVE;
			}
			else if (Rnd <= 45) {
				haveRoom[i]->roomType = Room::FALL;
			}
			else {
				//Rndが0以下だったらエラー処理

				Player::Instance().AddCoin(5);
				haveRoom[i]->roomType = Room::EMPTY;

			}
			



			break;
		}
	}

}

//部屋のモデルを生成する処理
void CreateMap::CreateRoom(Room* roomData) {
	StageManager& stageManager = StageManager::Instance();

	DirectX::XMFLOAT3 position = { RoomSize * roomData->axis.x, 0.0f, -RoomSize * roomData->axis.y };
	DirectX::XMFLOAT3 angle = { 0.0f,DirectX::XMConvertToRadians(90.0f * roomData->angleType),0.0f };
	Room::RoomType Type = roomData->roomType;

	//部屋の生成処理
	switch (Type)
	{
	case Room::RoomType::EMPTY:
		break;
	case Room::RoomType::START:
	{
		StageStart* stage = debug_new StageStart();
		stage->SetPosition(position);
		stage->SetAngle(angle);
		stage->SetAxis(roomData->axis);
		stageManager.Register(stage);
		break;
	}
	case Room::RoomType::GOAL:
	{
		StageGoal* stage = debug_new StageGoal();
		stage->SetPosition(position);
		stage->SetAngle(angle);
		stage->SetAxis(roomData->axis);
		stageManager.Register(stage);
		break;
	}

	case Room::RoomType::KEYROOM:
	{
		StageKey* stage = debug_new StageKey();
		stage->SetPosition(position);
		stage->SetAngle(angle);
		stage->SetAxis(roomData->axis);
		stageManager.Register(stage);
		break;
	}

	case Room::RoomType::CURVE:
	{
		StageCurve* stage = new StageCurve();
		stage->SetPosition(position);
		stage->SetAngle(angle);
		stage->SetAxis(roomData->axis);
		stageManager.Register(stage);
		break;
	}
	case Room::RoomType::CROSS:
	{
		StageCross* stage = new StageCross();
		stage->SetPosition(position);
		stage->SetAngle(angle);
		stage->SetAxis(roomData->axis);
		stageManager.Register(stage);
		break;
	}
	case Room::RoomType::FALL:
	{
		StageFall* stage = new StageFall();
		stage->SetPosition(position);
		stage->SetAngle(angle);
		stage->SetAxis(roomData->axis);
		stageManager.Register(stage);
		break;
	}
	case Room::RoomType::STOPROOM:
	{
		StageStop* stage = new StageStop();
		stage->SetPosition(position);
		stage->SetAngle(angle);
		stage->SetAxis(roomData->axis);
		stageManager.Register(stage);
		break;
	}
	case Room::RoomType::MAX:
		break;

	}



}


//部屋の削除(初期化)処理
void CreateMap::ResetRoom(DirectX::XMINT3 Axis) {

	if (!MapRoom[Axis.x][Axis.y]->canBreak) {

		MapRoom[Axis.x][Axis.y]->roomType = Room::EMPTY;
		MapRoom[Axis.x][Axis.y]->canCreate = false;
		MapRoom[Axis.x][Axis.y]->angleType = 0;

		StageManager& stageManager = StageManager::Instance();
		stageManager.RoomClear();
	}
	
}

//持っている設計図の最大数を超えているかの判定
bool CreateMap::HaveRoomMax() {
	
	if (haveRoom[0]->roomType != Room::RoomType::EMPTY &&
		haveRoom[1]->roomType != Room::RoomType::EMPTY &&
		haveRoom[2]->roomType != Room::RoomType::EMPTY &&
		haveRoom[3]->roomType != Room::RoomType::EMPTY) {

		return true;

	}
	else {
		return false;
	}

}

void CreateMap::ShakeAngle(float& angle, float elapsedTime, float speed, float start, float end) {

	

	//角度がendを超えたら
	//スピードを反転させる
	if (angle >= end) {
		PM = -PM;
	}
	else if (angle <= start) {
		PM = -PM;
	}
	//角度を加算する
	angle += PM * speed * elapsedTime;


}

bool CreateMap::SmokeAnimation(ID3D11DeviceContext* dc, float elapsedTime, DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 size) {
	int animation = 0;
	float animeTime = 0.0f;

	animeTime += elapsedTime;
	if (animeTime >= 0.1f) {
		animeTime = 0.0f;
		animation++;
	}


	spriteSmoke->Render(dc, pos.x, pos.y,size.x, size.y, 128 * animation, 0, 128, 128, 0, 1, 1, 1, 1);


	return false;
}