#pragma once

#include "Graphics/Graphics.h"
#include"Graphics/Sprite.h"
#include "Input/Input.h"

class PointUI
{
public:

	PointUI() {};
	~PointUI() {};

	//����������
	void Initialize();

	//�I������
	void Finalize();

	//�C���X�^���X�擾
	static PointUI& Instance()
	{
		static PointUI instance;
		return instance;
	}


	//�X�V����
	void Update(float elapsedTime);

	//�`�揈��
	void Render(Graphics& graphics, ID3D11DeviceContext* dc);

	//�����蔻��J�[�\��
	bool ClickButton(float posX, float posY, float width, float height);

	DirectX::XMFLOAT3& GetPoint() { return PointPos; }


	//�R���g���[���擾
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

	//render�p�ϐ�
	DirectX::XMFLOAT3 PointPos{0,0,0};
	float PointTexWidth;
	float PointTexHeight;

};