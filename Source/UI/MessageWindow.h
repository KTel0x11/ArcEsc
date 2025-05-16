#pragma once
#include "Graphics/Graphics.h"
#include"Graphics/Sprite.h"

class MessageWindow {
public:
	//�B��̃C���X�^���X�擾
	static MessageWindow& Instance()
	{
		static MessageWindow instance;
		return instance;
	}
	MessageWindow();
	~MessageWindow();
	void MessageRender(ID3D11DeviceContext* dc,Sprite* message, DirectX::XMFLOAT2 position, DirectX::XMFLOAT2 size,DirectX::XMFLOAT4 backColor);
	Sprite* spriteFrame = nullptr;//�E�B���h�E�t���[���̃X�v���C�g
	DirectX::XMFLOAT2 uvCornerPos = { 0,128 };//�t���[���̊p��uv���W
	DirectX::XMFLOAT2 uvCornerMiniPos = { 0,0 };//�t���[���̊p��uv���W
	DirectX::XMFLOAT2 uvFramePos = { 128,128 };//�t���[����uv���W
	DirectX::XMFLOAT2 uvPos = { 256,128 };//�t���[���̓�����uv���W
	DirectX::XMFLOAT2 uvSize = { 128,128 };//�t���[���̃X�v���C�guv�T�C�Y
	DirectX::XMFLOAT2 uvMiniSize = { 24,24 };//�t���[���̃X�v���C�guv�T�C�Y
	DirectX::XMFLOAT2 uvFrameSize = { 64,64 };//�t���[���̃X�v���C�guv�T�C�Y
	DirectX::XMFLOAT2 uvFrameMiniSize = { 12,12 };//�t���[���̃X�v���C�guv�T�C�Y

	DirectX::XMFLOAT2 winPos = { 0,0 };//�E�B���h�E�̈ʒu


	
};
