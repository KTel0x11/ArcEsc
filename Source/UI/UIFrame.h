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

	UIFrame() {};
	~UIFrame() {};
	//����������
	void Initialize();
	//�I������
	void Finalize();
	//�C���X�^���X�擾
	static UIFrame& Instance()
	{
		static UIFrame instance;
		return instance;
	}

	//�X�V����
	void Update(float elapsedTime);

	//�`�揈��
	void Render(Graphics& graphics,ID3D11DeviceContext* dc);

	void DamageAnimation(float elapsedTime);

	int animeState = -1;

private:
	const float MAX_GEAR_SPEED = 10.0f;

	Sprite* spriteFrame = nullptr;
	Sprite* spriteGear = nullptr;
	Sprite* spriteGear1 = nullptr;

	DirectX::XMFLOAT2 GearPos = { 1120, 550 };
	DirectX::XMFLOAT2 GearSize = { 256,256 };

	DirectX::XMFLOAT2 Gear1Pos = { -50, -40 };
	DirectX::XMFLOAT2 Gear1Size = { 128,128 };

	DirectX::XMFLOAT4 GearColor = { 1,1,1,1 };

	float GearAng = 0;

	

	int gearNum = 0;
	float ToggleSpeed = 5.0f;
	float velocity = 0.0f; //
};
