#include"Room.h"


#define debug_new new(_NORMAL_BLOCK,__FILE__,__LINE__)

//RoomƒNƒ‰ƒX

Room::Room(DirectX::XMINT3 Axis, Room::RoomType Type, int Angtype, bool CanBreak, bool CanCreate) {
	axis = Axis;
	roomType = Type;
	angleType = Angtype;
	canBreak = CanBreak;
	canCreate = CanCreate;
	spriteRoom = new Sprite("Data/Sprite/RoomMap.png");
}

Room::Room(Room::RoomType Type, int Angtype) {

	axis = {0,0,0};
	roomType = Type;
	angleType = Angtype;
	canBreak = false;
	canCreate = false;
	spriteRoom = new Sprite("Data/Sprite/RoomMap.png");
}

Room::~Room() {

	if (spriteRoom != nullptr) {
		delete spriteRoom;
		spriteRoom = nullptr;
	}

};


void Room::MapRender(ID3D11DeviceContext* dc) {

	if (roomType == EMPTY && canCreate) {
		color = { 0,0,0,1 };
	}

	spriteRoom->Render(dc, map.spritePos.x + map.blockInterval * axis.x, map.spritePos.y + map.blockInterval * axis.y, map.spriteSize.x, map.spriteSize.y, 128 * static_cast<int>(roomType), 0, 128, 128, 90 * angleType, color.x, color.y, color.z, color.w);

}


void Room::BulePrintRender(ID3D11DeviceContext* dc,int Y_axis) {

	spriteRoom->Render(dc, bulePrint.spritePos.x, bulePrint.spritePos.y + bulePrint.blockInterval * Y_axis, bulePrint.spriteSize.x, bulePrint.spriteSize.y, 128 * static_cast<int>(roomType), 0, 128, 128, 0, color.x,color.y,color.z,color.w);

}

void Room::SelectRender(ID3D11DeviceContext* dc, DirectX::XMFLOAT3 Pos) {

	spriteRoom->Render(dc, Pos.x - 20,Pos.y - 20 , 40, 40, 128 * static_cast<int>(roomType), 0, 128, 128, 90 * angleType, color.x, color.y, color.z, color.w);

}