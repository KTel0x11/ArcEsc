#pragma once

class GameState {
public:
	//唯一のインスタンス取得
	static GameState& Instance()
	{
		static GameState instance;
		return instance;
	}

	enum SceneState
	{
		SceneTitle,
		SceneLoading,
		SceneGame,
		SceneMAX
	};

	enum  State
	{
		Start,
		Game,
		Battle,
		MapCreate,
		Pause,
		GameClear,
		GameOver,
		Camera,
		MAX
	};

	enum ControllerState
	{
		Controller,
		MouseAndKeyboard,
	};


	enum MAKESTATE {
		BLUE,
		PUT,
		ROTATE,
	};


	GameState() {};
	~GameState() {};

	//初期化処理
	void Initialize() { 
	currentState = State::Start;
	currentSceneState = SceneState::SceneTitle;
	}
	//終了処理
	void Finalize() {
		currentState = State::Start;
		currentSceneState = SceneState::SceneTitle;
	}



	//ステートを設定
	void SetState(State state) { currentState = state; }
	//ステートを取得
	State GetState() { return currentState; }

	//シーンの状態を設定
	void SetSceneState(SceneState state) { currentSceneState = state; }
	//シーンの状態を取得
	SceneState GetSceneState() { return currentSceneState; }

	//コントローラーの状態を設定
	void SetControllerState(ControllerState state) { controllerState = state; }
	//コントローラーの状態を取得
	ControllerState GetControllerState() { return controllerState; }

	//部屋の状態を設定
	void SetMakeState(MAKESTATE state) { makeState = state; }
	//部屋の状態を取得
	MAKESTATE GetMakeState() { return makeState; }


private:
	State currentState;
	SceneState currentSceneState;
	ControllerState controllerState = Controller;
	MAKESTATE makeState = MAKESTATE::BLUE;





};