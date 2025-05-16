#pragma once

class GameState {
public:
	//�B��̃C���X�^���X�擾
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

	//����������
	void Initialize() { 
	currentState = State::Start;
	currentSceneState = SceneState::SceneTitle;
	}
	//�I������
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