#pragma once
#include "Graphics/Model.h"
#include "Graphics/Sprite.h"

//部屋クラス
class Room {
public:
	enum RoomType
	{
		EMPTY,
		START,
		GOAL,
		KEYROOM,
		CURVE,
		CROSS,
		FALL,
		STOPROOM,
		MAX,
	};

	struct SpriteInfo {
		DirectX::XMFLOAT2 spritePos;
		DirectX::XMFLOAT2 spriteSize;
		float blockInterval = 56.0f;
		DirectX::XMFLOAT4 color;
	};

	Room(DirectX::XMINT3 Axis,Room::RoomType Type, int Angtype,bool CanBreak,bool CanCreate);
	Room(Room::RoomType Type, int Angtype);
	~Room();

	//部屋描画
	void MapRender(ID3D11DeviceContext* dc);

	//部屋描画
	void BulePrintRender(ID3D11DeviceContext* dc,int Y_axis);

	//部屋描画
	void SelectRender(ID3D11DeviceContext* dc, DirectX::XMFLOAT3 Pos);

	void SetRoomType(Room::RoomType Type) { roomType = Type; }
	void SetRoomAngle(int AngType) { angle = 90.0f * AngType; }

public:
	SpriteInfo* GetSpriteInfo() { return &bulePrint; }

private:

	//部屋のスプライト情報
public:


	RoomType roomType=RoomType::EMPTY;
	int angleType = 0;
	float angle = 0;
	DirectX::XMINT3 axis{};
	bool canBreak = false;
	bool canCreate = false;
	Sprite* spriteRoom = nullptr;
	DirectX::XMFLOAT4 color = {1,1,1,1};

private:


	SpriteInfo map{
		{350.0f, 90.0f },
		{100.0f,100.0f},
		106.0f,

	};

	SpriteInfo bulePrint{
	{140.0f, 140.0f },
	{70.0f,70.0f},
	92.5f,
	
	};


};