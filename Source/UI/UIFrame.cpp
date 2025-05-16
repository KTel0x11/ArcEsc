#include"UIFrame.h"
#include"Chara/Player.h"
#include"GameState.h"
#define debug_new new(_NORMAL_BLOCK,__FILE__,__LINE__)

 
void UIFrame::Initialize() {
	
	
	spriteFrame = new Sprite("Data/Sprite/GameFrame.png");
	spriteGear = new Sprite("Data/Sprite/Gear.png");
	spriteGear1 = new Sprite("Data/Sprite/Gear1.png");

	
}

void UIFrame::Finalize() {

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

	DamageAnimation(elapsedTime);

	if (GameState::Instance().currentSceneState == GameState::SceneState::SceneGame) {
		if(Player::Instance().OnPoison){
			velocity = 0.0f;
			GearColor = { 0.5f,0.0f,0.5f,1 };
		
		}
		else if (Player::Instance().isDamaged) {
			velocity = 0.0f;
			GearColor = { 1,0.5f,0.5f,1 };
		}
		else {

			GearColor = { 1,1,1,1 };

			if ((Player::Instance().GetHealth() / Player::Instance().GetMaxHealth()) * MAX_GEAR_SPEED  > velocity) {
				velocity = (Player::Instance().GetHealth() / Player::Instance().GetMaxHealth()) * MAX_GEAR_SPEED;
			}
			else {
				velocity += ToggleSpeed * elapsedTime;
			}

		}
		

		//回転
		GearAng += velocity * (Player::Instance().GetHealth() / Player::Instance().GetMaxHealth()) * elapsedTime;


		//歯車が一回転したら
		if (GearAng > 360.0f) {
			GearAng -= GearAng;//回転位置リセット
			gearNum++;//回転数を増やす

		}

	
	}
	else {
		ToggleSpeed = 50.0f;
		GearAng += ToggleSpeed * elapsedTime;
		if (GearAng > 360.0f) {
			GearAng -= GearAng;//回転位置リセット
		}
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



	spriteGear->Render(dc, GearPos.x, GearPos.y, GearSize.x, GearSize.y, 0, 0, GearTexWidth, GearTexHeight, -GearAng, GearColor.x, GearColor.y, GearColor.z, GearColor.w);

	spriteGear1->Render(dc, Gear1Pos.x, Gear1Pos.y, Gear1Size.x, Gear1Size.y, 0, 0, Gear1TexWidth, Gear1TexHeight, GearAng * 2.0f, GearColor.x, GearColor.y, GearColor.z, GearColor.w);



}

//ダメージアニメーション
void UIFrame::DamageAnimation(float elapsedTime) {

	switch (animeState)
	{
	case 0://初期位置設定
		GearPos = { 1120,550 };
		Gear1Pos = { -50,-40 };
		animeState++;
		break;
	case 1://ダメージアニメーション

		GearPos.x += 500.0f * elapsedTime;
		Gear1Pos.x += 500.0f * elapsedTime;

		if (GearPos.x > 1150.0f) {
			GearPos.x = 1150.0f;
		}
		if (Gear1Pos.x > -20.0f) {
			Gear1Pos.x = -20.0f;
		}


		if (GearPos.x >= 1150.0f||Gear1Pos.x >= -20.0f) {
			animeState++;
		}
		break;

	case 2://ダメージアニメーション
		GearPos.x -= 500.0f * elapsedTime;
		Gear1Pos.x -= 500.0f * elapsedTime;
		if (GearPos.x < 1120.0f) {
			GearPos.x = 1120.0f;
		}
		if (Gear1Pos.x < -50.0f) {
			Gear1Pos.x = -50.0f;
		}

		if (GearPos.x <= 1120.0f || Gear1Pos.x <= -50.0f) {
			animeState++;
		}
		break;
	case 3://初期位置設定
		GearPos = { 1120,550 };
		Gear1Pos = { -50,-40 };
		animeState = -1;
		break;
	default:
		break;


	}

}