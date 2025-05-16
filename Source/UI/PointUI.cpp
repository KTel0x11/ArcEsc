#include"PointUI.h"
#include"GameState.h"



#define debug_new new(_NORMAL_BLOCK,__FILE__,__LINE__)

void PointUI::Initialize() {

	//�X�v���C�g�쐬
	spritePoint = new Sprite("Data/Sprite/Point.png");
	PointTexWidth = static_cast<float>(spritePoint->GetTextureWidth());
	PointTexHeight = static_cast<float>(spritePoint->GetTextureHeight());

}

void PointUI::Finalize() {
	//�X�v���C�g�폜
	if (spritePoint != nullptr) {
		delete spritePoint;
		spritePoint = nullptr;
	}

}

//�J�[�\���X�V
void PointUI::Update(float elapsedTime) {

	


	if (GameState::Instance().controllerState==GameState::ControllerState::Controller) {
		//�J�[�\���ړ�����
		PointPos.y -= gamePad.GetAxisLY() * elapsedTime * moveSpeed;
		PointPos.x += gamePad.GetAxisLX() * elapsedTime * moveSpeed;
	}

	if (GameState::Instance().controllerState == GameState::ControllerState::MouseAndKeyboard) {
		PointPos.x = mouse.GetPositionX();
		PointPos.y = mouse.GetPositionY();
	}

}

//�J�[�\���`��
void PointUI::Render(Graphics& graphics, ID3D11DeviceContext* dc) {
	float screenWidth = static_cast<float>(graphics.GetScreenWidth());
	float screenHeight = static_cast<float>(graphics.GetScreenHeight());
	

	//��ʊO�ɍs���Ȃ��悤�ɂ���
	if (PointPos.x < 0.0f) {
		PointPos.x = 0.0f;
	}
	if (PointPos.y < 0.0f) {
		PointPos.y = 0.0f;
	}
	if (PointPos.x > screenWidth ) {
		PointPos.x = screenWidth;
	}
	if (PointPos.y > screenHeight) {
		PointPos.y = screenHeight;
	}

	//�`�揈��
	spritePoint->Render(dc, PointPos.x, PointPos.y,pointSize,pointSize, 0, 0, PointTexWidth, PointTexHeight, 0, 1, 1, 1, 1);
	
}

bool PointUI::ClickButton(float posX, float posY, float width, float height) {

	// �_����`�̒��ɂ��邩�`�F�b�N
	if (PointPos.x >= posX && PointPos.x <= (posX+ width) &&
		PointPos.y >= posY && PointPos.y <= (posY + height))
	{
		return true;
	}
	else
	{
		return false;
	}
}