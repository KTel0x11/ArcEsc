#include "UI/UIButton.h"
#include "UI/PointUI.h"
#include "Input/Input.h"
#include "GameState.h"

void UIButton::Initialize()
{
	// Initialize the button
}

void UIButton::Finalize()
{
	// Finalize the button
}


bool UIButton::ButtonUp(DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2& size, DirectX::XMFLOAT4& color, bool colorChange, bool sizeChange, const DirectX::XMFLOAT2 changeSize, const DirectX::XMFLOAT2 oldSize)
{
	GamePad& gamepad = Input::Instance().GetGamePad();
	Mouse& mouse = Input::Instance().GetMouse();

	//カーソルポインターがボタンの範囲内に入ったら
	if (PointUI::Instance().ClickButton(pos.x, pos.y, size.x, size.y))
	{
		//カーソルが範囲内の時

		
		if ((gamepad.GetButton() & GamePad::BTN_B && GameState::Instance().GetControllerState() == GameState::Controller) ||
			(mouse.GetButton() & Mouse::BTN_LEFT && GameState::Instance().GetControllerState() == GameState::MouseAndKeyboard))//ボタンが押されたら
		{
			//サイズを変える
			if (sizeChange)
			{
				size = oldSize;
			}
	
			//色を変える
			if (colorChange)
			{
				color = { 0.5f,0.5f,0.5f,1 };
			}
		}
		else if ((gamepad.GetButtonUp() & GamePad::BTN_B && GameState::Instance().GetControllerState() == GameState::Controller) ||
			(mouse.GetButtonUp() & Mouse::BTN_LEFT && GameState::Instance().GetControllerState() == GameState::MouseAndKeyboard))//ボタンが離されたら
		{
			return true;//ボタンが押された
		}
		else
		{
			//サイズを変える
			if (sizeChange)
			{
				size = changeSize;
			}


			//色を変える
			if (colorChange)
			{
				color = { 1.5f,1.5f,1.5f,1 };
			}

		}
	}
	else
	{
		//カーソルが範囲外の時
		//サイズを戻す
		if (sizeChange)
		{
			size = oldSize;
		}

		//色を戻す
		if (colorChange)
		{
			color = { 1,1,1,1 };
		}
	
	}

	return false;
}

bool UIButton::ButtonDown(DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 size)
{
	GamePad& gamepad = Input::Instance().GetGamePad();
	Mouse& mouse = Input::Instance().GetMouse();
	//カーソルポインターがボタンの範囲内に入ったら
	if (PointUI::Instance().ClickButton(pos.x, pos.y, size.x, size.y))
	{
		//カーソルが範囲内の時
		if ((gamepad.GetButtonDown() & GamePad::BTN_B && GameState::Instance().GetControllerState() == GameState::Controller) ||
			(mouse.GetButtonDown() & Mouse::BTN_LEFT && GameState::Instance().GetControllerState() == GameState::MouseAndKeyboard))//ボタンが押されたら
		{
			return true;//ボタンが押された
		}
		
	}
	else
	{
		//カーソルが範囲外の時
		
	}

	return false;
}