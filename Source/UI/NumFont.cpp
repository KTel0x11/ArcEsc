#include "UI/NumFont.h"

NumFont::NumFont() {

	spriteNum = new Sprite("Data/Sprite/NumFont.png");

}

NumFont::~NumFont() {
	if (spriteNum != nullptr) {
		delete spriteNum;
		spriteNum = nullptr;
	}
}

void NumFont::NumRender(ID3D11DeviceContext* dc,int num, DirectX::XMFLOAT2 position, DirectX::XMFLOAT2 size, float angle, DirectX::XMFLOAT4 color) {
	float fontTexWidth = 256;
	float fontTexHeight = static_cast<float>(spriteNum->GetTextureHeight());
	tmp = num;
	if (num >= 1000) {
		digit = 3;
		
	}
	else if (num >= 100) {
		digit = 2;
	}
	else if (num >= 10)
	{
		digit = 1;
	}
	else
	{
		digit = 0;
	}

	switch (digit)
	{
	case 0: 
		digitNum[0] = tmp;
		break;
	case 1:
		digitNum[1] = tmp / 10;
		digitNum[0] = tmp % 10;
		break;
	case 2:
		digitNum[2] = tmp / 100;
		tmp = tmp - digitNum[0] * 100;
		digitNum[1] = tmp / 10;
		tmp = tmp - digitNum[1] * 10;
		digitNum[0] = tmp;
		break;
	case 3:
		digitNum[3] = tmp / 1000;
		tmp = tmp - digitNum[0] * 1000;
		digitNum[2] = tmp / 100;
		tmp = tmp - digitNum[1] * 100;
		digitNum[1] = tmp / 10;
		tmp = tmp - digitNum[2] * 10;
		digitNum[0] = tmp;
		break;

	default:
		break;
	}
	





	for (int i = 0; i < digit+1; ++i) {
		spriteNum->Render(dc, (position.x - size.x *0.8 * i), position.y, size.x, size.y, fontTexWidth * digitNum[i], 0, fontTexWidth, fontTexHeight, angle, color.x, color.y, color.z, color.w);
	}









}