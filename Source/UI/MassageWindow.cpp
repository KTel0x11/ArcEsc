#include <iostream>
#include "MessageWindow.h"
#include"Graphics/Graphics.h"
#include"Graphics/Sprite.h"
#include"Graphics/DebugRenderer.h"
#include"imgui.h"
#include"Mathf.h"


MessageWindow::MessageWindow() {

	spriteFrame = new Sprite("Data/Sprite/winFrame.png");


}

MessageWindow::~MessageWindow() {
	if (spriteFrame != nullptr) {
		
		delete spriteFrame;
		spriteFrame = nullptr;

	}
}
//�E�B���h�E�̕`��
void MessageWindow::MessageRender(ID3D11DeviceContext* dc, Sprite* message, DirectX::XMFLOAT2 position, DirectX::XMFLOAT2 size,DirectX::XMFLOAT4 backColor) {

//�t���[���̓����`��


	//�T�C�Y��x��y��64�ȏ�̏ꍇ�̃t���[����`��
	//if(size.x >= uvFrameSize.x && size.y >= uvFrameSize.y)
	{
		//���ォ��E���܂�
		winPos.x = position.x ;
		winPos.y = position.y ;

		//�w�i�`��
		spriteFrame->Render(dc,
			winPos.x, winPos.y, size.x + uvFrameSize.x * 2, size.y + uvFrameSize.y * 2,//position
			uvPos.x, uvPos.y, uvSize.x, uvSize.y,//uv
			0.0f,//angle
			backColor.x, backColor.y, backColor.z, backColor.w//color
		);

		message->Render(dc,
			position.x, position.y, size.x + uvFrameSize.x * 2, size.y + uvFrameSize.y * 2,//position
			0, 0, static_cast<float>(message->GetTextureWidth()), static_cast<float>(message->GetTextureHeight()),//uv
			0.0f,//angle
			1.0f, 1.0f, 1.0f, 1.0f//color
		);


		//�t���[���̎l���`��
		{
			//�t���[������`��
			spriteFrame->Render(dc,
				winPos.x, winPos.y, uvFrameSize.x, uvFrameSize.y,//position
				uvCornerPos.x, uvCornerPos.y, uvSize.x, uvSize.y,//uv
				0.0f,//angle
				1.0f, 1.0f, 1.0f, 1.0f//color
			);
			//�t���[���E��`��
			spriteFrame->Render(dc,
				winPos.x + size.x + uvFrameSize.x, winPos.y, uvFrameSize.x, uvFrameSize.y,//position
				uvCornerPos.x, uvCornerPos.y, uvSize.x, uvSize.y,//uv
				90.0f,//angle
				1.0f, 1.0f, 1.0f, 1.0f//color
			);
			//�t���[�������`��
			spriteFrame->Render(dc,
				winPos.x, winPos.y + size.y + uvFrameSize.y, uvFrameSize.x, uvFrameSize.y,//position
				uvCornerPos.x, uvCornerPos.y, uvSize.x, uvSize.y,//uv
				270.0f,//angle
				1.0f, 1.0f, 1.0f, 1.0f//color
			);
			//�t���[���E���`��
			spriteFrame->Render(dc,
				winPos.x + size.x + uvFrameSize.x, winPos.y + size.y + uvFrameSize.y, uvFrameSize.x, uvFrameSize.y,//position
				uvCornerPos.x, uvCornerPos.y, uvSize.x, uvSize.y,//uv
				180.0f,//angle
				1.0f, 1.0f, 1.0f, 1.0f//color
			);
		}
		//�t���[���̎l�ӕ`��
		{
			//�t���[���̉����̌v�Z
			int countWidth = static_cast<int>(size.x / uvFrameSize.x);
			float remainderWidth = size.x - (uvFrameSize.x * countWidth);//�]��̕�

			//�t���[���̏c���̌v�Z
			int countHeight = static_cast<int>(size.y / uvFrameSize.y);
			float remainderHeight = size.y - (uvFrameSize.y * countHeight);//�]��̕�


			//countWidth��countHeight��1�ȉ��̏ꍇ�͕`�悵�Ȃ�
			//�v�Z���ʂŃ��[�v�`��
	
			for (int i = 0; i < countWidth; i++) {
				//�t���[����ӕ`��
				spriteFrame->Render(dc,
					winPos.x + uvFrameSize.x + (uvFrameSize.x * i), winPos.y, uvFrameSize.x, uvFrameSize.y,//position
					uvFramePos.x, uvFramePos.y, uvSize.x, uvSize.y,//uv
					0.0f,//angle
					1.0f, 1.0f, 1.0f, 1.0f//color
				);

				//�t���[�����ӕ`��
				spriteFrame->Render(dc,
					winPos.x + uvFrameSize.x + (uvFrameSize.x * i), winPos.y + size.y + uvFrameSize.y, uvFrameSize.x, uvFrameSize.y,//position
					uvFramePos.x, uvFramePos.y, uvSize.x, uvSize.y,//uv
					180.0f,//angle
					1.0f, 1.0f, 1.0f, 1.0f//color
				);
			}

			//�v�Z���ʂŃ��[�v
			for (int i = 0; i < countHeight; i++) {
				//�t���[�����ӕ`��
				spriteFrame->Render(dc,
					winPos.x, winPos.y + uvFrameSize.y + (uvFrameSize.y * i), uvFrameSize.x, uvFrameSize.y,//position
					uvFramePos.x, uvFramePos.y, uvSize.x, uvSize.y,//uv
					270.0f,//angle
					1.0f, 1.0f, 1.0f, 1.0f//color
				);
				//�t���[���E�ӕ`��
				spriteFrame->Render(dc,
					winPos.x + size.x + uvFrameSize.x, winPos.y + uvFrameSize.y + (uvFrameSize.y * i), uvFrameSize.x, uvFrameSize.y,//position
					uvFramePos.x, uvFramePos.y, uvSize.x, uvSize.y,//uv
					90.0f,//angle
					1.0f, 1.0f, 1.0f, 1.0f//color
				);
			}


			//�t���[���̗]�蕝������Ε`��
			//�`�悷��ۏd�Ȃ���l�����āA�]�蕝��`�悷��

			if (remainderWidth > 0) {
				//�t���[����ӕ`��
				spriteFrame->Render(dc,
					winPos.x + uvFrameSize.x + (uvFrameSize.x * countWidth), winPos.y, remainderWidth, uvFrameSize.y,//position
					uvFramePos.x, uvFramePos.y, uvSize.x, uvSize.y,//uv
					0.0f,//angle
					1.0f, 1.0f, 1.0f, 1.0f//color
				);
				//�t���[�����ӕ`��
				spriteFrame->Render(dc,
					winPos.x + uvFrameSize.x + (uvFrameSize.x * countWidth), winPos.y + size.y + uvFrameSize.y, remainderWidth, uvFrameSize.y,//position
					uvFramePos.x, uvFramePos.y, uvSize.x, uvSize.y,//uv
					180.0f,//angle
					1.0f, 1.0f, 1.0f, 1.0f//color
				);
			}

			float correction = static_cast<int>(countHeight);//�X�v���C�g�̉�]���l�������␳�l

			if (remainderHeight > 0) {
				//�t���[�����ӕ`��
				spriteFrame->Render(dc,
					winPos.x , winPos.y + uvFrameSize.y + (uvFrameSize.y * countHeight), uvFrameSize.x, uvFrameSize.y,  //position
					uvFramePos.x, uvFramePos.y, uvSize.x, uvSize.y,//uv
					270.0f,//angle
					1.0f, 1.0f, 1.0f, 1.0f//color
				);
				//�t���[���E�ӕ`��
				spriteFrame->Render(dc,
					winPos.x + size.x + uvFrameSize.x, winPos.y + uvFrameSize.y + (uvFrameSize.y * countHeight), uvFrameSize.x, uvFrameSize.y, //position
					uvFramePos.x, uvFramePos.y, uvSize.x, uvSize.y,//uv
					90.0f,//angle
					1.0f, 1.0f, 1.0f, 1.0f//color
				);
			}


		}
	}
	//else {
		//�G���[�Əo��
	//	std::cout << "Error: MessageWindow::MessageRender() size.x or size.y is less than 64" << std::endl;
	//}


}