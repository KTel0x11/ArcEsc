#pragma once
#include"Chara/Player.h"


class BattleSystem {
public:
	BattleSystem();
	~BattleSystem();

	void Update(float elapsedTime);
	void render();


private:
	enum class STATE{
		START,
		PLAYERDAMAGE,
		ENEMYDAMAGE,
		PLAYERCOMAND,
		ENEMYCOMAND,
		PLAYERDEAD,
		ENEMYDEAD,
		PENALTY,
		CLEAR,
		END
	};


};
