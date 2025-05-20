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

	//����������
	void Initialize();
	//�I������
	void Finalize();
	//�C���X�^���X�擾
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

	//���j���[����
	void SelectMenu(float elapsedTime);
	void MenuRender(ID3D11DeviceContext* dc);

	//��������̏���
	void MakeRoom();
	void MakeRender(ID3D11DeviceContext* dc);

	//�V���b�v�̏���
	void ShopMenu(float elapsedTime);
	void ShopRender(ID3D11DeviceContext* dc);

	//�g�̏���
	void MoveCursorFrame();
	//�}�b�v�`�揈��
	void MapRender(ID3D11DeviceContext* dc);

	//�I�����Ă��镔�����͂ރt���[��
	void FrameRender(ID3D11DeviceContext* dc);

	//�v���C���[�̈ʒu�`��
	void PlayerRender(ID3D11DeviceContext* dc);

	//�����𐶐�
	void CreateRoom(Room* roomData);

	//�����̍폜
	void ResetRoom(DirectX::XMINT3 Axis);


	void DrawDebugGUI();
	//�������菈��
	void GetRoom(int Rnd);

	//�݌v�}�̏�����MAX��
	bool HaveRoomMax();

	bool SmokeAnimation(ID3D11DeviceContext* dc,float elapsedTime, DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 size);


	//�X�v���C�g��h�炷����
	void ShakeAngle(float& angle, float elapsedTime, float speed,float start, float end );


	//�X�^�[�g�A�S�[���A���̕������d�Ȃ�Ȃ��悤�ɂ��鏈��
	bool isAdjacent(const 	DirectX::XMINT3& a, const	DirectX::XMINT3& b) {
		return (abs(a.x - b.x) <= 1 && abs(a.y - b.y) <= 1);
	}


	//�������Ǎۂ̎��o�����O���ɍs���Ȃ��悤�ɂ��鏈��
	int determineRoomAngle(const DirectX::XMINT3& axis) {
		if (axis.x == 0) return 2;
		if (axis.x == Axis_MAX - 1) return 0;
		if (axis.y == 0) return 3;
		if (axis.y == Axis_MAX -1) return 1;
		return rand() % 4;
	}

	//�����o�^�֐�
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

	DirectX::XMINT3 Map_Axis{0,0,0};			//�I�����Ă��镔���̈ʒu
	DirectX::XMINT3 NewRoom_Axis{ 0,0,0 };		//�ŐV�ɐݒu���������̈ʒu
	DirectX::XMINT3 TmpRoom_Axis{ 0,0,0 };		//�ŐV�ɐݒu���������̈ʒu
	DirectX::XMINT3 startMap_Axis{ 0,0,0 };		//�X�^�[�g�����̈ʒu
	DirectX::XMINT3 goalMap_Axis{ 0,0,0 };		//�S�[�������̈ʒu
	DirectX::XMINT3 keyMap_Axis{ 0,0,0 };		//�������̈ʒu

	//�����o���̃X�v���C�g�֌W
	Sprite* spriteDrawer = nullptr;
	DirectX::XMFLOAT2 spriteDrawerPos = { 0,0 };
	DirectX::XMFLOAT2 spriteDrawerSize = { 900,550 };

	//�n�}(�{�^��)�̃X�v���C�g�֌W
	Sprite* spriteMapB = nullptr;
	DirectX::XMFLOAT2 spriteMapBPos = { 550,200 };
	DirectX::XMFLOAT2 spriteMapBSize = { 160,160 };
	DirectX::XMFLOAT4 MapBColor = { 1,1,1,1 };
	float mapBangle = 0;



	//�n�}�̃X�v���C�g�֌W
		Sprite* spriteMap = nullptr;
		DirectX::XMFLOAT2 spriteMapPos = { 270,15 };
		float spriteMapSize = 680.0f;

	//�쐬�X�v���C�g�֌W
		Sprite* spriteMake = nullptr;
		DirectX::XMFLOAT2 spriteMakePos = { 1800,50 };
		DirectX::XMFLOAT2 spriteMakeSize = { 200,100 };
		DirectX::XMFLOAT4 MakeColor = { 1,1,1,1 };

		Sprite* spriteMakeText = nullptr;
		DirectX::XMFLOAT2 spriteMakeTextPos = { 50,200 };
		DirectX::XMFLOAT2 spriteMakeTextSize = { 300,50 };

	//�V���b�v�X�v���C�g�֌W
		Sprite* spriteShop = nullptr;
		DirectX::XMFLOAT2 spriteShopPos = { 1900, 200 };
		DirectX::XMFLOAT2 spriteShopSize = { 200,100 };
		DirectX::XMFLOAT4 ShopColor = { 1,1,1,1 };
	//�߂�X�v���C�g�֌W
		Sprite* spriteBack = nullptr;
		DirectX::XMFLOAT2 spriteBackPos = { 1000, 500 };
		DirectX::XMFLOAT2 spriteBackSize = { 150,75 };
		DirectX::XMFLOAT4 BackColor = { 1,1,1,1 };
	//�g�X�v���C�g�֌W
		Sprite* spriteFrame = nullptr;
		DirectX::XMFLOAT2 spriteFramePos = { 347, 87 };
		DirectX::XMFLOAT2 spriteFrameSize = { 106,106 };
	//�݌v�}�ؘ̖g�X�v���C�g�֌W
		Sprite* spriteSekkei = nullptr;
		DirectX::XMFLOAT2 spriteSekkeiPos = { 60, 75 };
		DirectX::XMFLOAT2 spriteSekkeiSize = { 235,439 };

	//��������X�v���C�g�֌W
		Sprite* spriteSetsumei = nullptr;
		DirectX::XMFLOAT2 spriteSetsumeiPos = { 970, 50};
		DirectX::XMFLOAT2 spriteSetsumeiSize = { 256,256 };

	//�V���b�v�X�v���C�g�֌W
		Sprite* spriteItemShop = nullptr;
		DirectX::XMFLOAT2 spriteItemShopPos[3] = { {50, 50},{360,50},{670,50}};
		DirectX::XMFLOAT2 spriteItemShopSize = { 300,512 };

		Sprite* spritePlayer = nullptr;
		DirectX::XMFLOAT2 spritePPos = { 100.0f, 90.0f };
		DirectX::XMFLOAT2 spritePSize = { 50,100 };
	
		//�w���{�^���X�v���C�g�֌W
		Sprite* spritePay = nullptr;
		DirectX::XMFLOAT2 spritePayPos[3] = { {135, 570},{440,570},{750,570} };
		DirectX::XMFLOAT2 spritePaySize = { 150,75 };
		DirectX::XMFLOAT4 payColor[3] = { {1,1,1,1},{1,1,1,1},{1,1,1,1} };
		float payTimer = 0;
		//�j��{�^���X�v���C�g�֌W
		Sprite* spriteDelete = nullptr;
		DirectX::XMFLOAT2 spriteDeletePos = { 975, 350 };
		DirectX::XMFLOAT2 spriteDeleteSize = { 200,95 };
		DirectX::XMFLOAT4 DeleteColor = { 1,1,1,1 };
		//�R�C���g�X�v���C�g�֌W
		Sprite* spriteCoinF = nullptr;
		DirectX::XMFLOAT2 spriteCoinFPos = { 965, 150 };
		DirectX::XMFLOAT2 spriteCoinFSize = { 295,160 };
		DirectX::XMFLOAT4 CoinFColor = { 1,1,1,1 };

		//���̃X�v���C�g�֌W
		Sprite* spriteSmoke = nullptr;
		DirectX::XMFLOAT2 spriteSmokePos[6] = {};
		DirectX::XMFLOAT2 spriteSmokeSize = {256,256};
		DirectX::XMFLOAT4 SmokeColor = { 1,1,1,1 };


		Room* MapRoom[Axis_MAX][Axis_MAX];//�n�}�̕������

		float PM = 1.0f;

		int rnd = 0;//�����p


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

	//�X�^�[�g�����ʒu
	DirectX::XMFLOAT3 startPosition = {0,0,0};
	//�S�[�������ʒu
	DirectX::XMFLOAT3 goalPosition = {0,0,0};

	const float RoomSize = 250.0f;

	Room* selectRoom;	//�I�񂾐݌v�}�̏��
	int selectNo = 0;


	Room* haveRoom[4];	//�����Ă���݌v�}�̏��
	
	Room::SpriteInfo bulePrint{
	{140.0f, 140.0f },
	{70.0f,70.0f},
	92.5f
	};

};


