#pragma once
#include "Graphics/Graphics.h"
#include"Graphics/Sprite.h"

class NumFont {
public:
	//唯一のインスタンス取得
	static NumFont& Instance()
	{
		static NumFont instance;
		return instance;
	}

	NumFont();
	~NumFont();

	void NumRender(ID3D11DeviceContext* dc,int num, DirectX::XMFLOAT2 position, DirectX::XMFLOAT2 size, float angle, DirectX::XMFLOAT4 color);


	Sprite* spriteNum = nullptr;
	const int DIGIT_MAX = 4;

	int digit = 0;
	int tmp;
	int digitNum[4];


};

