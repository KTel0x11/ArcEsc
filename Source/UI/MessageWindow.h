#pragma once
#include "Graphics/Graphics.h"
#include"Graphics/Sprite.h"

class MessageWindow {
public:
	//唯一のインスタンス取得
	static MessageWindow& Instance()
	{
		static MessageWindow instance;
		return instance;
	}
	MessageWindow();
	~MessageWindow();
	void MessageRender(ID3D11DeviceContext* dc,Sprite* message, DirectX::XMFLOAT2 position, DirectX::XMFLOAT2 size,DirectX::XMFLOAT4 backColor);
	Sprite* spriteFrame = nullptr;//ウィンドウフレームのスプライト
	DirectX::XMFLOAT2 uvCornerPos = { 0,128 };//フレームの角のuv座標
	DirectX::XMFLOAT2 uvCornerMiniPos = { 0,0 };//フレームの角のuv座標
	DirectX::XMFLOAT2 uvFramePos = { 128,128 };//フレームのuv座標
	DirectX::XMFLOAT2 uvPos = { 256,128 };//フレームの内側のuv座標
	DirectX::XMFLOAT2 uvSize = { 128,128 };//フレームのスプライトuvサイズ
	DirectX::XMFLOAT2 uvMiniSize = { 24,24 };//フレームのスプライトuvサイズ
	DirectX::XMFLOAT2 uvFrameSize = { 64,64 };//フレームのスプライトuvサイズ
	DirectX::XMFLOAT2 uvFrameMiniSize = { 12,12 };//フレームのスプライトuvサイズ

	DirectX::XMFLOAT2 winPos = { 0,0 };//ウィンドウの位置


	
};
