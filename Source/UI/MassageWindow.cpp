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
//ウィンドウの描画
void MessageWindow::MessageRender(ID3D11DeviceContext* dc, Sprite* message, DirectX::XMFLOAT2 position, DirectX::XMFLOAT2 size,DirectX::XMFLOAT4 backColor) {

//フレームの内側描画


	//サイズがxとyが64以上の場合のフレームを描画
	//if(size.x >= uvFrameSize.x && size.y >= uvFrameSize.y)
	{
		//左上から右下まで
		winPos.x = position.x ;
		winPos.y = position.y ;

		//背景描画
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


		//フレームの四隅描画
		{
			//フレーム左上描画
			spriteFrame->Render(dc,
				winPos.x, winPos.y, uvFrameSize.x, uvFrameSize.y,//position
				uvCornerPos.x, uvCornerPos.y, uvSize.x, uvSize.y,//uv
				0.0f,//angle
				1.0f, 1.0f, 1.0f, 1.0f//color
			);
			//フレーム右上描画
			spriteFrame->Render(dc,
				winPos.x + size.x + uvFrameSize.x, winPos.y, uvFrameSize.x, uvFrameSize.y,//position
				uvCornerPos.x, uvCornerPos.y, uvSize.x, uvSize.y,//uv
				90.0f,//angle
				1.0f, 1.0f, 1.0f, 1.0f//color
			);
			//フレーム左下描画
			spriteFrame->Render(dc,
				winPos.x, winPos.y + size.y + uvFrameSize.y, uvFrameSize.x, uvFrameSize.y,//position
				uvCornerPos.x, uvCornerPos.y, uvSize.x, uvSize.y,//uv
				270.0f,//angle
				1.0f, 1.0f, 1.0f, 1.0f//color
			);
			//フレーム右下描画
			spriteFrame->Render(dc,
				winPos.x + size.x + uvFrameSize.x, winPos.y + size.y + uvFrameSize.y, uvFrameSize.x, uvFrameSize.y,//position
				uvCornerPos.x, uvCornerPos.y, uvSize.x, uvSize.y,//uv
				180.0f,//angle
				1.0f, 1.0f, 1.0f, 1.0f//color
			);
		}
		//フレームの四辺描画
		{
			//フレームの横幅の計算
			int countWidth = static_cast<int>(size.x / uvFrameSize.x);
			float remainderWidth = size.x - (uvFrameSize.x * countWidth);//余りの幅

			//フレームの縦幅の計算
			int countHeight = static_cast<int>(size.y / uvFrameSize.y);
			float remainderHeight = size.y - (uvFrameSize.y * countHeight);//余りの幅


			//countWidthとcountHeightが1以下の場合は描画しない
			//計算結果でループ描画
	
			for (int i = 0; i < countWidth; i++) {
				//フレーム上辺描画
				spriteFrame->Render(dc,
					winPos.x + uvFrameSize.x + (uvFrameSize.x * i), winPos.y, uvFrameSize.x, uvFrameSize.y,//position
					uvFramePos.x, uvFramePos.y, uvSize.x, uvSize.y,//uv
					0.0f,//angle
					1.0f, 1.0f, 1.0f, 1.0f//color
				);

				//フレーム下辺描画
				spriteFrame->Render(dc,
					winPos.x + uvFrameSize.x + (uvFrameSize.x * i), winPos.y + size.y + uvFrameSize.y, uvFrameSize.x, uvFrameSize.y,//position
					uvFramePos.x, uvFramePos.y, uvSize.x, uvSize.y,//uv
					180.0f,//angle
					1.0f, 1.0f, 1.0f, 1.0f//color
				);
			}

			//計算結果でループ
			for (int i = 0; i < countHeight; i++) {
				//フレーム左辺描画
				spriteFrame->Render(dc,
					winPos.x, winPos.y + uvFrameSize.y + (uvFrameSize.y * i), uvFrameSize.x, uvFrameSize.y,//position
					uvFramePos.x, uvFramePos.y, uvSize.x, uvSize.y,//uv
					270.0f,//angle
					1.0f, 1.0f, 1.0f, 1.0f//color
				);
				//フレーム右辺描画
				spriteFrame->Render(dc,
					winPos.x + size.x + uvFrameSize.x, winPos.y + uvFrameSize.y + (uvFrameSize.y * i), uvFrameSize.x, uvFrameSize.y,//position
					uvFramePos.x, uvFramePos.y, uvSize.x, uvSize.y,//uv
					90.0f,//angle
					1.0f, 1.0f, 1.0f, 1.0f//color
				);
			}


			//フレームの余り幅があれば描画
			//描画する際重なりを考慮して、余り幅を描画する

			if (remainderWidth > 0) {
				//フレーム上辺描画
				spriteFrame->Render(dc,
					winPos.x + uvFrameSize.x + (uvFrameSize.x * countWidth), winPos.y, remainderWidth, uvFrameSize.y,//position
					uvFramePos.x, uvFramePos.y, uvSize.x, uvSize.y,//uv
					0.0f,//angle
					1.0f, 1.0f, 1.0f, 1.0f//color
				);
				//フレーム下辺描画
				spriteFrame->Render(dc,
					winPos.x + uvFrameSize.x + (uvFrameSize.x * countWidth), winPos.y + size.y + uvFrameSize.y, remainderWidth, uvFrameSize.y,//position
					uvFramePos.x, uvFramePos.y, uvSize.x, uvSize.y,//uv
					180.0f,//angle
					1.0f, 1.0f, 1.0f, 1.0f//color
				);
			}

			float correction = static_cast<int>(countHeight);//スプライトの回転を考慮した補正値

			if (remainderHeight > 0) {
				//フレーム左辺描画
				spriteFrame->Render(dc,
					winPos.x , winPos.y + uvFrameSize.y + (uvFrameSize.y * countHeight), uvFrameSize.x, uvFrameSize.y,  //position
					uvFramePos.x, uvFramePos.y, uvSize.x, uvSize.y,//uv
					270.0f,//angle
					1.0f, 1.0f, 1.0f, 1.0f//color
				);
				//フレーム右辺描画
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
		//エラーと出力
	//	std::cout << "Error: MessageWindow::MessageRender() size.x or size.y is less than 64" << std::endl;
	//}


}