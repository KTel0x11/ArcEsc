#pragma once

#include "Graphics/Graphics.h"
#include"Graphics/Sprite.h"
#include "Input/Input.h"

class PointUI
{
public:

	PointUI() {};
	~PointUI() {};

	//初期化処理
	void Initialize();

	//終了処理
	void Finalize();

	//インスタンス取得
	static PointUI& Instance()
	{
		static PointUI instance;
		return instance;
	}


	//更新処理
	void Update(float elapsedTime);

	//描画処理
	void Render(Graphics& graphics, ID3D11DeviceContext* dc);

	//当たり判定カーソル
	bool ClickButton(float posX, float posY, float width, float height);

	DirectX::XMFLOAT3& GetPoint() { return PointPos; }


	//コントローラ取得
	GamePad& gamePad = Input::Instance().GetGamePad();
	Mouse& mouse = Input::Instance().GetMouse();

private:

	Sprite* spritePoint = nullptr;

	float moveSpeed = 700.0f;
	float pointSize = 30.0f;



	bool mouseFlg =true;
	bool padFlg = false;

	const GamePadButton anyPadButton =
		GamePad::BTN_A
		| GamePad::BTN_B
		| GamePad::BTN_X
		| GamePad::BTN_Y
		;

	const MouseButton anyMouseButton =
		Mouse::BTN_RIGHT
		| Mouse::BTN_LEFT
		;

	//render用変数
	DirectX::XMFLOAT3 PointPos{0,0,0};
	float PointTexWidth;
	float PointTexHeight;

};