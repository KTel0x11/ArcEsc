#pragma once
#include "Graphics/Graphics.h"
#include"Graphics/Sprite.h"

#include "Graphics/Graphics.h"
#include"Graphics/Sprite.h"
#include"Chara/Player.h"
//�t���[��UI

class UIFrame 
{
public:

	UIFrame();
	~UIFrame();

	//�X�V����
	void Update(float elapsedTime);

	//�`�揈��
	void Render(Graphics& graphics,ID3D11DeviceContext* dc);

private:
	const int ADD_COIN_GEAR = 10;

	Sprite* spriteFrame = nullptr;
	Sprite* spriteGear = nullptr;
	Sprite* spriteGear1 = nullptr;

	float GearAng = 0;
	int gearNum = 0;

};
