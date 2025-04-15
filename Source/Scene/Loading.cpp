#include "Graphics/Graphics.h"
#include "Input/Input.h"
#include"SceneLoading.h"
#include"SceneManager.h"

#define debug_new new(_NORMAL_BLOCK,__FILE__,__LINE__)

void SceneLoading::Initialize() {
	
	

	thread = new std::thread(LoadingThread, this);
	spriteDot = new Sprite("Data/Sprite/LoadingDot.png");
	spriteLogo = new Sprite("Data/Sprite/UnityLogo.png");
	spriteLoading = new Sprite("Data/Sprite/LoadingLogo.png");
	spriteBack = new Sprite("Data/Sprite/LoadingBack.png");
}


void SceneLoading::Finalize() {
	//スプライト終了化
	if (spriteLoading != nullptr)
	{
		delete spriteLoading;
		spriteLoading = nullptr;
	}
	if (spriteDot != nullptr)
	{
		delete spriteDot;
		spriteDot = nullptr;
	}

	if (spriteLogo != nullptr)
	{
		delete spriteLogo;
		spriteLogo = nullptr;
	}


	if (spriteBack != nullptr)
	{
		delete spriteBack;
		spriteBack = nullptr;
	}

	if (thread != nullptr)
	{
		thread->join();
		delete thread;
		thread = nullptr;
	}

}

void  SceneLoading::Update(float elapsedTime) {
	constexpr float speed = 90;
	angle -= speed * elapsedTime;

	Time += elapsedTime;

	animeTime += 0.01f;
	if (animeTime >= 5.0f) {
		animation++;
		if (animation > 3) {
			animation = 0;
		}
		animeTime = 0;
	}

	if (Time > 5.0f) {
		if (SceneLoading::nextScene) {
			SceneManager::Instance().ChangeScene(nextScene);
		}
	}
}

void SceneLoading::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	FLOAT color[] = { 0.0f,0.0f,0.0f,1.0f };
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	
	{
		float screenWidth = 512;
		float screenHeight = 128;
		float textureWidth = static_cast<float>(spriteLoading->GetTextureWidth());
		float textureHeight = static_cast<float>(spriteLoading->GetTextureHeight());
		float positionX = 700;
		float positionY = 570;



		//タイトルスプライト描画
		float BtextureWidth = static_cast<float>(spriteBack->GetTextureWidth());
		float BtextureHeight = static_cast<float>(spriteBack->GetTextureHeight());
		spriteBack->Render(dc,
			0, 0, 
			BtextureWidth,BtextureHeight,
			0, 0, BtextureWidth, BtextureHeight,
			0,
			0.4, 0.4, 0.8, 1);


		spriteLoading->Render(dc,
			positionX, positionY, screenWidth, screenHeight,
			0, 0, textureWidth, textureHeight,
			0,
			1, 1, 1, 1);

	
		float textureLogoWidth = static_cast<float>(spriteLogo->GetTextureWidth());
		float textureLogoHeight = static_cast<float>(spriteLogo->GetTextureHeight());
		float LogopositionX = 700-textureLogoWidth;
		float LogopositionY = 570;

		spriteLogo->Render(dc,
			LogopositionX, LogopositionY, textureLogoWidth, textureLogoHeight,
			0, 0, textureLogoWidth, textureLogoHeight,
			angle,
			1, 1, 1, 1);

	

		float textureDotWidth = 512.0f;
		float textureDotHeight = static_cast<float>(spriteDot->GetTextureHeight());
		float DotpositionX = 640+screenWidth;
		float DotpositionY = 570;

		spriteDot->Render(dc,
			DotpositionX, DotpositionY, textureLogoWidth, textureLogoHeight,
			textureDotWidth * animation, 0, textureDotWidth, textureDotHeight,
			0,
			1, 1, 1, 1);



	}
	



}


void SceneLoading::LoadingThread(SceneLoading* scene)
{
	//CoInitialize(nullptr);

	scene->nextScene->Initialize();

	CoUninitialize();

	scene->nextScene->SetReady();


}