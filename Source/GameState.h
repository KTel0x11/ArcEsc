#pragma once

class GameState {
public:
	//�B��̃C���X�^���X�擾
	static GameState& Instance()
	{
		static GameState instance;
		return instance;
	}

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

	GameState() { currentState = Start; }
	~GameState() {};

	void SetState(State state) { currentState = state; }
	
	State currentState;


};