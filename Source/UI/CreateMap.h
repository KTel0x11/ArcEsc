#pragma once
#include "Graphics/Model.h"
#include "Graphics/Sprite.h"
#include "Stage/Stage.h"
#include"Stage/Room.h"
#include "UI/PointUI.h"
#include"Chara/Player.h"

#include<vector>

#define debug_new new(_NORMAL_BLOCK,__FILE__,__LINE__)

const int Axis_MAX = 5;


 constexpr float MOVE_SPEED_FAST = 3000.0f;
 constexpr float MOVE_SPEED_SLOW = 1000.0f;
 constexpr float TARGET_POSITION = 800.0f;


class CreateMap {

public:
	CreateMap() {};
	~CreateMap() {};

	//初期化処理
	void Initialize();
	//終了処理
	void Finalize();
	//インスタンス取得
	static CreateMap& Instance()
	{
		static CreateMap instance;
		return instance;
	}

	void Update(float elapsedTime);
	void Render(ID3D11DeviceContext* dc);



	enum class State {
		STARTMENU,
		MENU,
		STARTMAKE,
		MAKE,
		MAKESETSUMEI,
		STARTSHOP,
		SHOP,
		END,
	};



public:



	void SetState(State stt) { state = stt; };

	DirectX::XMFLOAT3& GetStartPosition() { return startPosition; };
	DirectX::XMFLOAT3& GetGoalPosition() { return goalPosition; };
	int GetStartAngle() { return startRoomAng; };
	int GetGoalAngle() { return goalRoomAng; };
private :

	//メニュー処理
	void SelectMenu(float elapsedTime);
	void MenuRender(ID3D11DeviceContext* dc);

	//部屋制作の処理
	void MakeRoom();
	void MakeRender(ID3D11DeviceContext* dc);

	//ショップの処理
	void ShopMenu(float elapsedTime);
	void ShopRender(ID3D11DeviceContext* dc);

	//枠の処理
	void MoveCursorFrame();
	//マップ描画処理
	void MapRender(ID3D11DeviceContext* dc);

	//選択している部屋を囲むフレーム
	void FrameRender(ID3D11DeviceContext* dc);

	//プレイヤーの位置描画
	void PlayerRender(ID3D11DeviceContext* dc);

	//部屋を生成
	void CreateRoom(Room* roomData);

	//部屋の削除
	void ResetRoom(DirectX::XMINT3 Axis);


	void DrawDebugGUI();
	//部屋入手処理
	void GetRoom(int Rnd);

	//設計図の所持がMAXか
	bool HaveRoomMax();

	bool SmokeAnimation(ID3D11DeviceContext* dc,float elapsedTime, DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 size);


	//スプライトを揺らす処理
	void ShakeAngle(float& angle, float elapsedTime, float speed,float start, float end );


	//スタート、ゴール、鍵の部屋が重ならないようにする処理
	bool isAdjacent(const 	DirectX::XMINT3& a, const	DirectX::XMINT3& b) {
		return (abs(a.x - b.x) <= 1 && abs(a.y - b.y) <= 1);
	}


	//部屋が壁際の時出口が外側に行かないようにする処理
	int determineRoomAngle(const DirectX::XMINT3& axis) {
		if (axis.x == 0) return 2;
		if (axis.x == Axis_MAX - 1) return 0;
		if (axis.y == 0) return 3;
		if (axis.y == Axis_MAX -1) return 1;
		return rand() % 4;
	}

	//部屋登録関数
	void registerRoom(Room* room, const DirectX::XMINT3& axis, Room::RoomType type, int angle,bool CanBreak,bool CanCreate) {
		room->axis = axis;
		NewRoom_Axis = axis;
		room->roomType = type;
		room->angleType = angle;
		room->canBreak = CanBreak;
		room->canCreate = CanCreate;
	}




private:

	

	GamePad& gamepad = Input::Instance().GetGamePad();
	Mouse& mouse = Input::Instance().GetMouse();

	DirectX::XMINT3 Map_Axis{0,0,0};			//選択している部屋の位置
	DirectX::XMINT3 NewRoom_Axis{ 0,0,0 };		//最新に設置した部屋の位置
	DirectX::XMINT3 TmpRoom_Axis{ 0,0,0 };		//最新に設置した部屋の位置
	DirectX::XMINT3 startMap_Axis{ 0,0,0 };		//スタート部屋の位置
	DirectX::XMINT3 goalMap_Axis{ 0,0,0 };		//ゴール部屋の位置
	DirectX::XMINT3 keyMap_Axis{ 0,0,0 };		//鍵部屋の位置

	//引き出しのスプライト関係
	Sprite* spriteDrawer = nullptr;
	DirectX::XMFLOAT2 spriteDrawerPos = { 0,0 };
	DirectX::XMFLOAT2 spriteDrawerSize = { 900,550 };

	//地図(ボタン)のスプライト関係
	Sprite* spriteMapB = nullptr;
	DirectX::XMFLOAT2 spriteMapBPos = { 550,200 };
	DirectX::XMFLOAT2 spriteMapBSize = { 160,160 };
	DirectX::XMFLOAT4 MapBColor = { 1,1,1,1 };
	float mapBangle = 0;



	//地図のスプライト関係
		Sprite* spriteMap = nullptr;
		DirectX::XMFLOAT2 spriteMapPos = { 270,15 };
		float spriteMapSize = 680.0f;

	//作成スプライト関係
		Sprite* spriteMake = nullptr;
		DirectX::XMFLOAT2 spriteMakePos = { 1800,50 };
		DirectX::XMFLOAT2 spriteMakeSize = { 200,100 };
		DirectX::XMFLOAT4 MakeColor = { 1,1,1,1 };

		Sprite* spriteMakeText = nullptr;
		DirectX::XMFLOAT2 spriteMakeTextPos = { 50,200 };
		DirectX::XMFLOAT2 spriteMakeTextSize = { 300,50 };

	//ショップスプライト関係
		Sprite* spriteShop = nullptr;
		DirectX::XMFLOAT2 spriteShopPos = { 1900, 200 };
		DirectX::XMFLOAT2 spriteShopSize = { 200,100 };
		DirectX::XMFLOAT4 ShopColor = { 1,1,1,1 };
	//戻るスプライト関係
		Sprite* spriteBack = nullptr;
		DirectX::XMFLOAT2 spriteBackPos = { 1000, 500 };
		DirectX::XMFLOAT2 spriteBackSize = { 150,75 };
		DirectX::XMFLOAT4 BackColor = { 1,1,1,1 };
	//枠スプライト関係
		Sprite* spriteFrame = nullptr;
		DirectX::XMFLOAT2 spriteFramePos = { 347, 87 };
		DirectX::XMFLOAT2 spriteFrameSize = { 106,106 };
	//設計図の木枠スプライト関係
		Sprite* spriteSekkei = nullptr;
		DirectX::XMFLOAT2 spriteSekkeiPos = { 60, 75 };
		DirectX::XMFLOAT2 spriteSekkeiSize = { 235,439 };

	//操作説明スプライト関係
		Sprite* spriteSetsumei = nullptr;
		DirectX::XMFLOAT2 spriteSetsumeiPos = { 970, 50};
		DirectX::XMFLOAT2 spriteSetsumeiSize = { 256,256 };

	//ショップスプライト関係
		Sprite* spriteItemShop = nullptr;
		DirectX::XMFLOAT2 spriteItemShopPos[3] = { {50, 50},{360,50},{670,50}};
		DirectX::XMFLOAT2 spriteItemShopSize = { 300,512 };

		Sprite* spritePlayer = nullptr;
		DirectX::XMFLOAT2 spritePPos = { 100.0f, 90.0f };
		DirectX::XMFLOAT2 spritePSize = { 50,100 };
	
		//購入ボタンスプライト関係
		Sprite* spritePay = nullptr;
		DirectX::XMFLOAT2 spritePayPos[3] = { {135, 570},{440,570},{750,570} };
		DirectX::XMFLOAT2 spritePaySize = { 150,75 };
		DirectX::XMFLOAT4 payColor[3] = { {1,1,1,1},{1,1,1,1},{1,1,1,1} };
		float payTimer = 0;
		//破壊ボタンスプライト関係
		Sprite* spriteDelete = nullptr;
		DirectX::XMFLOAT2 spriteDeletePos = { 975, 350 };
		DirectX::XMFLOAT2 spriteDeleteSize = { 200,95 };
		DirectX::XMFLOAT4 DeleteColor = { 1,1,1,1 };
		//コイン枠スプライト関係
		Sprite* spriteCoinF = nullptr;
		DirectX::XMFLOAT2 spriteCoinFPos = { 965, 150 };
		DirectX::XMFLOAT2 spriteCoinFSize = { 295,160 };
		DirectX::XMFLOAT4 CoinFColor = { 1,1,1,1 };

		//煙のスプライト関係
		Sprite* spriteSmoke = nullptr;
		DirectX::XMFLOAT2 spriteSmokePos[6] = {};
		DirectX::XMFLOAT2 spriteSmokeSize = {256,256};
		DirectX::XMFLOAT4 SmokeColor = { 1,1,1,1 };


		Room* MapRoom[Axis_MAX][Axis_MAX];//地図の部屋情報

		float PM = 1.0f;

		int rnd = 0;//乱数用


		DirectX::XMFLOAT2 spriteBlockPos = { 350, 90 };
		DirectX::XMFLOAT2 spriteBlockSize = { 100,100 };
		float color[10][10]{};

		float blockInterval = 106.0f;
	
		int roomAng = 0;
		int startRoomAng = 0;
		int goalRoomAng = 0;
		int keyRoomAng = 0;
		Room::RoomType roomType = Room::RoomType::EMPTY;
		int type = 0;

		bool clearRoom = false;


	float animationTimer = 0;
	int animeState = 0;
	bool isAnimation = false;

	State state = State::STARTMENU;
	//PointUI* pointUI = nullptr;
	Player* player = nullptr;

	//スタート部屋位置
	DirectX::XMFLOAT3 startPosition = {0,0,0};
	//ゴール部屋位置
	DirectX::XMFLOAT3 goalPosition = {0,0,0};

	const float RoomSize = 250.0f;

	Room* selectRoom;	//選んだ設計図の情報
	int selectNo = 0;


	Room* haveRoom[4];	//持っている設計図の情報
	
	Room::SpriteInfo bulePrint{
	{140.0f, 140.0f },
	{70.0f,70.0f},
	92.5f
	};

};


