#include<imgui.h>
#include<time.h>
#include <stdlib.h>
#include <stdio.h>
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



#define debug_new new(_NORMAL_BLOCK,__FILE__,__LINE__)

CreateMap::CreateMap() {
	srand((unsigned int)time(NULL));

	


	//部屋マップ生成
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			MapRoom[i][j] = new Room({ i,j,0 }, Room::RoomType::EMPTY, 0, false, false);
		}
	}


	//スタート部屋作成
	{

		for (;;) {
			startMap_Axis = { rand() % 10,rand() % 10,0 };
			goalMap_Axis = { rand() % 10,rand() % 10,0 };
			keyMap_Axis = { rand() % 10,rand() % 10,0 };
			//スタート部屋と隣り合わせにならないようにする処理
			if (!isAdjacent(startMap_Axis, goalMap_Axis) && !isAdjacent(keyMap_Axis, startMap_Axis) && !isAdjacent(keyMap_Axis, goalMap_Axis)) {
				break;
			}
		}

		for (int i = 0; i < 20; i++) {
			TmpRoom_Axis= { rand() % 10,rand() % 10,0 };
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

	//カーソルの初期化
	pointUI = new PointUI();
	


}


CreateMap::~CreateMap() {

	//カーソル終了化
	if (pointUI != nullptr) {
		delete pointUI;
		pointUI = nullptr;
	}

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
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
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

	pointUI->Update(elapsedTime);

	{rnd = rand() % 4 + 4; }

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
		MapRender(dc);
		break;
	case CreateMap::State::MENU://メニューの描画
		MenuRender(dc);
		MapRender(dc);
		break;
	case CreateMap::State::STARTMAKE://マップ作成の描画
		MapRender(dc);
		break;
	case CreateMap::State::MAKE://マップ作成の描画
		MapRender(dc);
		MakeRender(dc);
		//FrameRender(dc);
		break;
	case CreateMap::State::STARTSHOP://ショップの描画
		break;
	case CreateMap::State::SHOP://ショップの描画
		ShopRender(dc);
		break;
	default:
		break;
	}

	pointUI->Render(graphics, dc);

}

void CreateMap::MapRender(ID3D11DeviceContext* dc) {
	//地図のUI描画
	{
		float mapTexHeight = static_cast<float>(spriteMap->GetTextureHeight());
		float mapTexWidth = static_cast<float>(spriteMap->GetTextureWidth());
		spriteMap->Render(dc, spriteMapPos.x, spriteMapPos.y, spriteMapSize, spriteMapSize, 0, 0, mapTexWidth, mapTexHeight, 0, 1, 1, 1, 1);
	}


	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			MapRoom[j][i]->MapRender(dc);
		}
	}
}

void CreateMap::SelectMenu() {

	//もし作るボタン（スプライトで表示）を押されたら（カーソルが範囲内かつが押されたら)stateをMAKEに変更
	if (pointUI->ClickButton(spriteMakePos.x, spriteMakePos.y, spriteMakeSize.x, spriteMakeSize.y)) {
		
		if (gamepad.GetButton() & GamePad::BTN_B || mouse.GetButton() & Mouse::BTN_LEFT) {
			MakeColor = {0.5f,0.5f,0.5f,1};
			
		}
		else if(gamepad.GetButtonUp() & GamePad::BTN_B || mouse.GetButtonUp() & Mouse::BTN_LEFT) {
			state = State::MAKE;

		}
		else {
				MakeColor = { 1,1,1,1 };
			}
	}
	else {
		MakeColor = { 1,1,1,1 };
	}

	//もしショップボタン（スプライトで表示）を押されたら（カーソルが範囲内かつが押されたら)stateをBREAKに変更
	if (pointUI->ClickButton(spriteShopPos.x, spriteShopPos.y, spriteShopSize.x, spriteShopSize.y)) {
		if (gamepad.GetButton() & GamePad::BTN_B || mouse.GetButton() & Mouse::BTN_LEFT) {
			ShopColor = { 0.5f,0.5f,0.5f,1 };
			
		}
		else if (gamepad.GetButtonUp() & GamePad::BTN_B || mouse.GetButtonUp() & Mouse::BTN_LEFT) {
			state = State::SHOP;

		}
		else {
			ShopColor = { 1,1,1,1 };
		}
	}
	else {
		ShopColor = { 1,1,1,1 };
	}


}

//メニュー画面描画処理
void CreateMap::MenuRender(ID3D11DeviceContext* dc) {

	Graphics& graphics = Graphics::Instance();


	//作るボタンを描画
	float makeTexHeight = static_cast<float>(spriteMake->GetTextureHeight());
	float makeTexWidth = static_cast<float>(spriteMake->GetTextureWidth());
	spriteMake->Render(dc, spriteMakePos.x, spriteMakePos.y, spriteMakeSize.x, spriteMakeSize.y, 0, 0, makeTexWidth, makeTexHeight, 0, MakeColor.x, MakeColor.y, MakeColor.z, MakeColor.w);

	//壊すボタンを描画
	float breakTexHeight = static_cast<float>(spriteShop->GetTextureHeight());
	float breakTexWidth = static_cast<float>(spriteShop->GetTextureWidth());
	spriteShop->Render(dc, spriteShopPos.x, spriteShopPos.y, spriteShopSize.x, spriteShopSize.y, 0, 0, breakTexWidth, breakTexHeight, 0, ShopColor.x, ShopColor.y, ShopColor.z, ShopColor.w);

}


//部屋作成モード処理
void CreateMap::MakeRoom(){
	spriteMakePos = { 70,550 };

	//部屋の回転処理
	{
		if (gamepad.GetButtonDown() & GamePad::BTN_LEFT_SHOULDER || GetAsyncKeyState('Q') & 1) {
			roomAng--;
			selectRoom->angleType = roomAng;
		}
		if (gamepad.GetButtonDown() & GamePad::BTN_RIGHT_SHOULDER || GetAsyncKeyState('E') & 1) {
			roomAng++;
			selectRoom->angleType = roomAng;
		}
	}


	//設計図内の処理
	for (int i = 0; i < 4; i++)
	{
		if (pointUI->ClickButton(bulePrint.spritePos.x, bulePrint.spritePos.y + bulePrint.blockInterval * i,
			bulePrint.spriteSize.x, bulePrint.spriteSize.y)) //もしカーソルが設計図に触れていたら
		{
			if (gamepad.GetButtonDown() & GamePad::BTN_B
				|| mouse.GetButtonDown() & Mouse::BTN_LEFT) //もしBボタンor左クリックが押されたら
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
	if (pointUI->ClickButton(spriteMapPos.x, spriteMapPos.y, spriteMapSize, spriteMapSize)) {
		if (gamepad.GetButtonDown() & GamePad::BTN_B || mouse.GetButtonDown() & Mouse::BTN_LEFT)
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
	if (pointUI->ClickButton(spriteDeletePos.x, spriteDeletePos.y, spriteDeleteSize.x, spriteDeleteSize.y)) {
		if (gamepad.GetButton() & GamePad::BTN_B || mouse.GetButton() & Mouse::BTN_LEFT) {
			DeleteColor = { 0.5f,0.5f,0.5f,1 };

	
		}
		else if (gamepad.GetButtonUp() & GamePad::BTN_B || mouse.GetButtonUp() & Mouse::BTN_LEFT) {
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
		if (gamepad.GetButtonDown() & GamePad::BTN_X){
		haveRoom[selectNo]->roomType = selectRoom->roomType;
		haveRoom[selectNo]->color = { 1,1,1,1 };
		selectRoom->roomType = Room::EMPTY;
		selectRoom->angleType = 0;
			}
	}

	

	//戻るボタン処理
	if (pointUI->ClickButton(spriteBackPos.x, spriteBackPos.y, spriteBackSize.x, spriteBackSize.y)) {
		if (gamepad.GetButton() & GamePad::BTN_B || mouse.GetButton() & Mouse::BTN_LEFT) {
			BackColor = { 0.5f,0.5f,0.5f,1 };

		}
		else if (gamepad.GetButtonUp() & GamePad::BTN_B || mouse.GetButtonUp() & Mouse::BTN_LEFT) {
			//設計図を持っていたらリセットする処理
			if (selectRoom->roomType != Room::EMPTY) {
				haveRoom[selectNo]->roomType = selectRoom->roomType;
				haveRoom[selectNo]->color = { 1,1,1,1 };
				selectRoom->roomType = Room::EMPTY;
				selectRoom->angleType = 0;
			}
			state = State::STARTMENU;
		}
		else {
			BackColor = { 1,1,1,1 };
		}
	}
	else {
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

	FrameRender(dc);

	//設計図（パズルピース)の描画
	for (int i = 0; i < 4; i++) {
		if (haveRoom[i]->roomType != Room::RoomType::EMPTY) {
			haveRoom[i]->BulePrintRender(dc, i);
		}
	}
	if (selectRoom->roomType != Room::EMPTY) {
		selectRoom->SelectRender(dc, pointUI->GetPoint());
	}
}



//ショップモード処理
void CreateMap::ShopMenu(float elapsedTime) {
	srand((unsigned int)time(NULL));
	spriteShopPos = { 1000, 50 };


	//戻る時の処理
	if (pointUI->ClickButton(spriteBackPos.x, spriteBackPos.y, spriteBackSize.x, spriteBackSize.y)) {
		if (gamepad.GetButton() & GamePad::BTN_B || mouse.GetButton() & Mouse::BTN_LEFT) {
			BackColor = {0.5f,0.5f,0.5f,1 };
			
		}
		else if (gamepad.GetButtonUp() & GamePad::BTN_B || mouse.GetButtonUp() & Mouse::BTN_LEFT) {
			state = State::STARTMENU;
		}
		else {
			BackColor = { 1,1,1,1 };
		}
	}
	else {
		BackColor = { 1,1,1,1 };
	}

	//購入処理
	for (int i = 0; i < 3; i++) {


		switch (i)
		{
		case 0://回復アイテム
			if (pointUI->ClickButton(spritePayPos[i].x, spritePayPos[i].y, spritePaySize.x, spritePaySize.y)) {//ボタンを押したとき時色を変化
				if (gamepad.GetButton() & GamePad::BTN_B || mouse.GetButton() & Mouse::BTN_LEFT) {
					payColor[0] = { 0.5,0.5,0.5,1 };


				}
				else if ((gamepad.GetButtonUp() & GamePad::BTN_B || mouse.GetButtonUp() & Mouse::BTN_LEFT)&&payTimer <= 0.0f ) {//ボタンを上げた時アイテムを購入
					if (Player::Instance().lifeItem <= 5 && Player::Instance().AddCoin(-2)) {

						Player::Instance().lifeItem++;
						payTimer = 2.0f;
					}
				}
				else {
					payColor[0] = { 1,1,1,1 };
				}
			}
			else {
	
					payColor[0] = { 1,1,1,1 };

			}
			if (Player::Instance().lifeItem >= 5 || Player::Instance().coin < 2) {	//購入不可の時
				payColor[0] = { 0.4,0.4,0.4,1 };	
			}


			break;

		case 1://パワーアップアイテム
			if (pointUI->ClickButton(spritePayPos[i].x, spritePayPos[i].y, spritePaySize.x, spritePaySize.y)) {
				if (gamepad.GetButton() & GamePad::BTN_B || mouse.GetButton() & Mouse::BTN_LEFT) {
					payColor[1] = { 0.7,0.7,0.7,1 };
				}
				else if (gamepad.GetButtonUp() & GamePad::BTN_B || mouse.GetButtonUp() & Mouse::BTN_LEFT && payTimer <= 0.0f) {
					if (Player::Instance().AddCoin(-3) && Player::Instance().powerItem <= 2) {
						payTimer = 2.0f;
						Player::Instance().powerItem++;

					}


				}
				else {
					payColor[1] = { 1,1,1,1 };
				}
			}
			else {

				payColor[1] = { 1,1,1,1 };

			}

			if (Player::Instance().powerItem >= 2 || Player::Instance().coin < 3) {		//購入不可の時
				payColor[1] = { 0.4,0.4,0.4,1 };
			}


			break;
		case 2://設計図
			if (pointUI->ClickButton(spritePayPos[i].x, spritePayPos[i].y, spritePaySize.x, spritePaySize.y)) {
				if (gamepad.GetButton() & GamePad::BTN_B || mouse.GetButton() & Mouse::BTN_LEFT) {
					payColor[2] = { 0.7,0.7,0.7,1 };
				}
				else if (gamepad.GetButtonUp() & GamePad::BTN_B || mouse.GetButtonUp() & Mouse::BTN_LEFT && payTimer <= 0.0f) {
					
					payTimer = 2.0f;
					GetRoom(rnd);

				}
				else {
					payColor[2] = { 1,1,1,1 };
				}
			}
			else {


				payColor[2] = { 1,1,1,1 };

			}

			if (HaveRoomMax() || Player::Instance().coin < 5) {
				payColor[2] = { 0.4,0.4,0.4,1 };
			}


			break;
		default:
			break;
		}

		if (payTimer > 0) {
			payTimer -= elapsedTime;
		}


	}
}
//ショップモード描画処理
void CreateMap::ShopRender(ID3D11DeviceContext* dc) {


	float shopTexHeight = static_cast<float>(spriteItemShop->GetTextureHeight());
	float shopTexWidth = static_cast<float>(spriteItemShop->GetTextureWidth());

	float payTexHeight = static_cast<float>(spritePay->GetTextureHeight());
	float payTexWidth = static_cast<float>(spritePay->GetTextureWidth());

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
	{
		if (gamepad.GetButtonDown() & GamePad::BTN_UP && Map_Axis.y > 0) {
			--Map_Axis.y;
		}
		if (gamepad.GetButtonDown() & GamePad::BTN_DOWN && Map_Axis.y < AxisY_MAX - 1) {
			++Map_Axis.y;
		}
		if (gamepad.GetButtonDown() & GamePad::BTN_LEFT && Map_Axis.x > 0) {
			--Map_Axis.x;
		}
		if (gamepad.GetButtonDown() & GamePad::BTN_RIGHT && Map_Axis.x < AxisX_MAX - 1) {
			++Map_Axis.x;
		}
	}

	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			if (pointUI->ClickButton(spriteBlockPos.x + blockInterval * j, spriteBlockPos.y + blockInterval * i, spriteBlockSize.x, spriteBlockSize.y)) {
				
				Map_Axis.x = j;
				Map_Axis.y = i;
				
			}
		}
	}

}

//選択されている部屋の枠描画処理
void CreateMap::FrameRender(ID3D11DeviceContext* dc) {

	float frameTexHeight = static_cast<float>(spriteFrame->GetTextureHeight());
	float frameTexWidth = static_cast<float>(spriteFrame->GetTextureWidth());
	spriteFrame->Render(dc, spriteFramePos.x + blockInterval * Map_Axis.x, spriteFramePos.y + blockInterval * Map_Axis.y, spriteFrameSize.x, spriteFrameSize.y, 0, 0, frameTexWidth, frameTexHeight, 0, 1, 1, 1, 1);

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
void CreateMap::GetRoom(int Type) {
	for (int i = 0; i < 4; i++) {
		if (haveRoom[i]->roomType == Room::RoomType::EMPTY&&Player::Instance().AddCoin(-5)) {
			haveRoom[i]->roomType = static_cast<Room::RoomType>(Type);
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

	switch (Type)
	{
	case Room::RoomType::EMPTY:
		break;
	case Room::RoomType::START:
	{
		StageStart* stage = debug_new StageStart();
		stage->SetPosition(position);
		stage->SetAngle(angle);
		stageManager.Register(stage);
		break;
	}
	case Room::RoomType::GOAL:
	{
		StageGoal* stage = debug_new StageGoal();
		stage->SetPosition(position);
		stage->SetAngle(angle);
		stageManager.Register(stage);
		break;
	}

	case Room::RoomType::KEYROOM:
	{
		StageKey* stage = debug_new StageKey();
		stage->SetPosition(position);
		stage->SetAngle(angle);
		stageManager.Register(stage);
		break;
	}

	case Room::RoomType::CURVE:
	{
		StageCurve* stage = new StageCurve();
		stage->SetPosition(position);
		stage->SetAngle(angle);
		stageManager.Register(stage);
		break;
	}
	case Room::RoomType::CROSS:
	{
		StageCross* stage = new StageCross();
		stage->SetPosition(position);
		stage->SetAngle(angle);
		stageManager.Register(stage);
		break;
	}
	case Room::RoomType::FALL:
	{
		StageFall* stage = new StageFall();
		stage->SetPosition(position);
		stage->SetAngle(angle);
		stageManager.Register(stage);
		break;
	}
	case Room::RoomType::STOPROOM:
	{
		StageStop* stage = new StageStop();
		stage->SetPosition(position);
		stage->SetAngle(angle);
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

