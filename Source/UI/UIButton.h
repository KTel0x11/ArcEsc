#pragma once
#include "Graphics/Graphics.h"
#include"Graphics/Sprite.h"
#include"GameState.h"

class UIButton {
public:
	UIButton() {};
	~UIButton() {};

	void Initialize();
	void Finalize();


	static UIButton& Instance() {
		static UIButton instance;
		return instance;
	}


	bool ButtonUp(DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 size, DirectX::XMFLOAT4& color, bool colorChange);
	bool ButtonDown(DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 size);
};