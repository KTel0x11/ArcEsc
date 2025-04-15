#pragma once

class GameState {
public:
	//唯一のインスタンス取得
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