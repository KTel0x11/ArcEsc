#include <memory>
#include <sstream>

#include "Graphics/Graphics.h"
#include "Input/Input.h"
#include "Framework.h"
#include "Chara/EffectManager.h"
#include "UI/PointUI.h"
#include "UI/UIFrame.h"
#include "UI/NumFont.h"
#include"GameState.h"

//#include "Audio/Audio.h"

#include"Scene/SceneTitle.h"
#include"Scene/SceneManager.h"

#define debug_new new(_NORMAL_BLOCK,__FILE__,__LINE__)

// 垂直同期間隔設定
static const int syncInterval = 0;

// コンストラクタ
Framework::Framework(HWND hWnd)
	: hWnd(hWnd)
	, input(hWnd)
	, graphics(hWnd)
{
	//初期化処理
	Audio::Instance().Initialize();
	UIFrame::Instance().Initialize();
	PointUI::Instance().Initialize();
	EffectManager::Instance().Initialize();
	SceneManager::Instance().ChangeScene(new  SceneTitle);
	GameState::Instance().Initialize();
	GameState::Instance().controllerState = GameState::ControllerState::MouseAndKeyboard;
	//GameState::Instance().controllerState = GameState::ControllerState::Controller;
}

// デストラクタ
Framework::~Framework()
{
	//終了処理
	UIFrame::Instance().Finalize();
	PointUI::Instance().Finalize();

	SceneManager::Instance().Clear();
	
	EffectManager::Instance().Finalize();

	Audio::Instance().Finalize();

	GameState::Instance().Finalize();


}

// 更新処理
void Framework::Update(float elapsedTime/*Elapsed seconds from last frame*/)
{
	// 入力更新処理
	input.Update();

	PointUI::Instance().Update(elapsedTime);

	// シーン更新処理
	SceneManager::Instance().Update(elapsedTime);

	
}

// 描画処理
void Framework::Render(float elapsedTime/*Elapsed seconds from last frame*/)
{
	std::lock_guard<std::mutex> lock(graphics.GetMutex());


	ID3D11DeviceContext* dc = graphics.GetDeviceContext();

	// IMGUIフレーム開始処理
	graphics.GetImGuiRenderer()->NewFrame();

	// シーン描画処理
	SceneManager::Instance().Render();

	// IMGUIデモウインドウ描画（IMGUI機能テスト用）
	//ImGui::ShowDemoWindow();

	// IMGUI描画
	graphics.GetImGuiRenderer()->Render(dc);

	// バックバッファに描画した画を画面に表示する。
	graphics.GetSwapChain()->Present(syncInterval, 0);
}

// フレームレート計算
void Framework::CalculateFrameStats()
{
	// Code computes the average frames per second, and also the 
	// average time it takes to render one frame.  These stats 
	// are appended to the window caption bar.
	static int frames = 0;
	static float time_tlapsed = 0.0f;

	frames++;

	// Compute averages over one second period.
	if ((timer.TimeStamp() - time_tlapsed) >= 1.0f)
	{
		float fps = static_cast<float>(frames); // fps = frameCnt / 1
		float mspf = 1000.0f / fps;
		std::ostringstream outs;
		outs.precision(6);
		outs << "FPS : " << fps << " / " << "Frame Time : " << mspf << " (ms)";
		SetWindowTextA(hWnd, outs.str().c_str());

		// Reset for next average.
		frames = 0;
		time_tlapsed += 1.0f;
	}
}

// アプリケーションループ
int Framework::Run()
{
	MSG msg = {};

	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			timer.Tick();
			CalculateFrameStats();

			float elapsedTime = syncInterval == 0
				? timer.TimeInterval()
				: syncInterval / 60.0f
				;
			Update(elapsedTime);
			Render(elapsedTime);
		}
	}
	return static_cast<int>(msg.wParam);
}

// メッセージハンドラ
LRESULT CALLBACK Framework::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (Graphics::Instance().GetImGuiRenderer()->HandleMessage(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc;
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CREATE:
		break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) PostMessage(hWnd, WM_CLOSE, 0, 0);
		break;
	case WM_ENTERSIZEMOVE:
		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
		timer.Stop();
		break;
	case WM_EXITSIZEMOVE:
		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
		timer.Start();
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}
