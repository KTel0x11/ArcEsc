#include"UIFrame.h"
#include"Chara/Player.h"
#include"GameState.h"
#define debug_new new(_NORMAL_BLOCK,__FILE__,__LINE__)


UIFrame::UIFrame() {
	
	
	spriteFrame = new Sprite("Data/Sprite/GameFrame.png");
	spriteGear = new Sprite("Data/Sprite/Gear.png");
	spriteGear1 = new Sprite("Data/Sprite/Gear1.png");

	
}

UIFrame::~UIFrame() {

	if (spriteGear1 != nullptr) {
		delete spriteGear1;
		spriteGear1 = nullptr;
	}

	if (spriteGear != nullptr) {
		delete spriteGear;
		spriteGear = nullptr;
	}

	if (spriteFrame != nullptr) {
		delete spriteFrame;
		spriteFrame = nullptr;
	}



}

//フレーム更新
void UIFrame::Update(float elapsedTime) {
	//歯車回転処理
	GearAng += 5.0f * Player::Instance().GetHealth() * elapsedTime;

	//歯車が一回転したら
	if (GearAng > 360.0f) {
		GearAng = 0;//回転位置リセット
		gearNum++;//回転数を増やす
	
	}

	if (gearNum == ADD_COIN_GEAR) {	//歯車が一定数回転したら
		if (GameState::Instance().currentState == GameState::State::Game) {
			Player::Instance().AddCoin(1);//コインが１増える
		}

		gearNum = 0;
	}


}


//フレーム描画処理
void UIFrame::Render(Graphics& graphics,ID3D11DeviceContext* dc) {

	float screenWidth = static_cast<float>(graphics.GetScreenWidth());
	float screenHeight = static_cast<float>(graphics.GetScreenHeight());
	float textureWidth = static_cast<float>(spriteFrame->GetTextureWidth());
	float textureHeight = static_cast<float>(spriteFrame->GetTextureHeight());

	spriteFrame->Render(dc, 0, 0, screenWidth, screenHeight, 0, 0, textureWidth, textureHeight, 0, 1, 1, 1, 1);

	float GearTexWidth = static_cast<float>(spriteGear->GetTextureWidth());
	float GearTexHeight = static_cast<float>(spriteGear->GetTextureHeight());
	float Gear1TexWidth = static_cast<float>(spriteGear1->GetTextureWidth());
	float Gear1TexHeight = static_cast<float>(spriteGear1->GetTextureHeight());


	spriteGear->Render(dc, 1120, 550, 256, 256, 0, 0, GearTexWidth, GearTexHeight, -GearAng, 1, 1, 1, 1);
	spriteGear1->Render(dc, -50, -40, 128, 128, 0, 0, Gear1TexWidth, Gear1TexHeight, GearAng, 1, 1, 1, 1);

}