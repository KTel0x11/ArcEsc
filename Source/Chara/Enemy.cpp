#include "Enemy.h"
#include"Graphics/Graphics.h"
#include"EnemyManager.h"

void Enemy::DrawDebugPrimitive() {
	DebugRenderer* debugRender = Graphics::Instance().GetDebugRenderer();
	debugRender->DrawCylinder(position, radius,height, DirectX::XMFLOAT4(0, 0, 0, 1),{0,0,0});
}

void Enemy::Destroy() {
	EnemyManager::Instance().Remove(this);
}