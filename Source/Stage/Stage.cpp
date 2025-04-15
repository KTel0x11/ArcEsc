#include"Graphics/Graphics.h"
#include"Stage/Stage.h"
#include"Stage/StageManager.h"

void Stage::Destroy() {
	StageManager::Instance().Remove(this);
}