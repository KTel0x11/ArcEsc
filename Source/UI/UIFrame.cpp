#include"UIFrame.h"
#include"Chara/Player.h"
#include"GameState.h"
#define debug_new new(_NORMAL_BLOCK,__FILE__,__LINE__)

 
void UIFrame::Initialize() {
	//spriteの初期化
	spriteFrame = new Sprite("Data/Sprite/GameFrame.png");
	spriteGear = new Sprite("Data/Sprite/Gear.png");
	spriteGear1 = new Sprite("Data/Sprite/Gear1.png");

	
}

void UIFrame::Finalize() {
	//spriteの終了化
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
	
	

	if (GameState::Instance().GetSceneState() == GameState::SceneState::SceneGame) {
		GameUpdate(elapsedTime);
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

void UIFrame::GameUpdate(float elapsedTime) {

	//damageアニメーションの更新
	DamageAnimation(elapsedTime);


	if (Player::Instance().OnPoison) {
		GearSpeed = 0.0f;
		GearColor = { 0.5f,0.0f,0.5f,1 };

	}

	else if (Player::Instance().isDamaged) {
		GearSpeed = 0.0f;
		GearColor = { 1,0.5f,0.5f,1 };
	}

	else {
		
		//色の初期化
		GearColor = { 1,1,1,1 };

		//歯車の回転速度を計算
		if (GearSpeed > MAX_GEAR_SPEED)
		{
			GearSpeed = MAX_GEAR_SPEED;
		}
		else
		{
			GearSpeed += 10.0f * elapsedTime;
		}

		//速度を計算
		ToggleSpeed = GearSpeed * (Player::Instance().GetHealth() / Player::Instance().GetMaxHealth());


		if (GameState::Instance().GetState() != GameState::GameOver) {
			//回転
			GearAng += ToggleSpeed * elapsedTime;
		}
	}
	//歯車が一回転したら
	if (GearAng > 360.0f) {
		GearAng -= GearAng;//回転位置リセット
		gearNum++;//回転数を増やす

	}


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

		//左上に移動
		GearPos.x += DMAGE_GEAR_SPEED * elapsedTime;
		GearPos.y += DMAGE_GEAR_SPEED * elapsedTime;
		//右下に移動
		Gear1Pos.x -= DMAGE_GEAR_SPEED * elapsedTime;
		Gear1Pos.y -= DMAGE_GEAR_SPEED * elapsedTime;

		//一定を超えたら止まる
		if (GearPos.x > 1150.0f) {
			GearPos.x = 1150.0f;
		}
		if (GearPos.y > 580.0f) {
			GearPos.y = 580.0f;
		}
		if (Gear1Pos.x < -80.0f) {
			Gear1Pos.x = -80.0f;
		}
		if (Gear1Pos.y < -70.0f) {
			Gear1Pos.y = -70.0f;
		}

		//条件を満たしたら次のアニメーションへ
		if (GearPos.x >= 1150.0f||Gear1Pos.x <= -80.0f) {
			animeState++;
		}
		break;

	case 2://ダメージアニメーション
		//左上に移動
		GearPos.x -= DMAGE_GEAR_SPEED * elapsedTime;
		GearPos.y -= DMAGE_GEAR_SPEED * elapsedTime;

		//右下に移動
		Gear1Pos.x += DMAGE_GEAR_SPEED * elapsedTime;
		Gear1Pos.y += DMAGE_GEAR_SPEED * elapsedTime;

		//一定を超えたら止まる
		if (GearPos.x < 1120.0f) {
			GearPos.x = 1120.0f;
		}
		if (GearPos.y < 550.0f) {
			GearPos.y = 550.0f;
		}
		if (Gear1Pos.x > -50.0f) {
			Gear1Pos.x = -50.0f;
		}
		if (Gear1Pos.y > -40.0f) {
			Gear1Pos.y = -40.0f;
		}

		//条件を満たしたら次のアニメーションへ
		if (GearPos.x <= 1120.0f && Gear1Pos.x >= -50.0f) {
			GearPos.x = 1120.0f;
			Gear1Pos.x = -50.0f;
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