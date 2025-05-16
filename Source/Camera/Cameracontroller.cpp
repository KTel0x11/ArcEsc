#include"CameraController.h"
#include"Camera.h"
#include"Input/Input.h"
#include"Chara/Collision.h"
#include"Chara/Player.h"
#include"Stage/Stage.h"
#include"GameState.h"

//�X�V����
void CameraController::Update(float elapsedTime) {




	if (GameState::Instance().controllerState == GameState::ControllerState::MouseAndKeyboard) {
		//�}�E�X�̈ʒu���擾
		float mouseX = Input::Instance().GetMouse().GetPositionX();
		float mouseY = Input::Instance().GetMouse().GetPositionY();

		//�}�E�X�̈ړ��ʂ��擾
		float lastMouseX = Input::Instance().GetMouse().GetOldPositionX();
		float lastMouseY = Input::Instance().GetMouse().GetOldPositionY();

		//�}�E�X�̈ړ��ʂ��擾
		float dx = mouseX - lastMouseX;
		float dy = mouseY - lastMouseY;

		//�J�����̉�]���x
		float speed = rollSpeed * elapsedTime;

		//�J�����̉�]�ʂ��v�Z


		angle.x += dy * speed;
		angle.y += dx * speed;

	}
	else if (GameState::Instance().controllerState == GameState::ControllerState::Controller) {
		GamePad& gamePad = Input::Instance().GetGamePad();
		float ax = gamePad.GetAxisRY();
		float ay = gamePad.GetAxisRX();



		//�J�����̉�]���x
		float speed = rollSpeed * elapsedTime;

		angle.x += ax * speed;
		angle.y += ay * speed;
	}

		DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

		DirectX::XMVECTOR Front = Transform.r[2];
		DirectX::XMFLOAT3 front;
		DirectX::XMStoreFloat3(&front, Front);

		DirectX::XMVECTOR Back{};
		Back = DirectX::XMVectorSubtract(Back, Front);
		DirectX::XMFLOAT3 back;
		DirectX::XMStoreFloat3(&back, Back);





		if (GameState::Instance().currentState == GameState::State::Pause) {
			eye.x = target.x - front.x;
			eye.y = 150.0f;
			eye.z = target.z - front.z;
		}
		else {
			eye.x = target.x - front.x * range;
			eye.y = target.y - front.y * range;
			eye.z = target.z - front.z * range;
		}
		//eye.x = target.x - front.x;
		//eye.y = target.y - front.y;
		//eye.z = target.z - front.z;



		if (angle.x > maxAngleX) {
			angle.x = maxAngleX;
		}
		if (angle.x < minAngleX) {
			angle.x = minAngleX;
		}

		if (angle.y < -DirectX::XM_PI) {
			angle.y += DirectX::XM_2PI;
		}
		if (angle.y > DirectX::XM_PI) {
			angle.y -= DirectX::XM_2PI;
		}

		DirectX::XMFLOAT3 PPos = Player::Instance().GetPosition();

		if (PPos.y > -200.0f) {
			Camera::Instance().SetLookAt(eye, target, DirectX::XMFLOAT3(0, 1, 0));
		}

	
}

