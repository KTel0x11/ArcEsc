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

	GameState() { }
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



	void SetState(State state) { currentState = state; }
	State GetState() { return currentState; }
	void SetSceneState(SceneState state) { currentSceneState = state; }
	SceneState GetSceneState() { return currentSceneState; }
	
	State currentState;
	SceneState currentSceneState;

	enum ControllerState
	{
		Controller,
		MouseAndKeyboard,
	};
	ControllerState controllerState = Controller;



};