#pragma once

#include "Graphics/Model.h"
#include "Stage/Stage.h"
#include "Stage/Room.h"
#include  "Chara/Player.h"

//ステージ
class StageGoal : public Stage
{
public:
	StageGoal();
	~StageGoal() override;


	//更新処理
	void Update(float elapsedTime) override;

	//描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader) override;

	//レイキャスト
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) override;

	//スフィアキャスト
	bool SphereCast(const DirectX::XMFLOAT3& start, const float radius, const DirectX::XMFLOAT3& end, HitResult& hit) override;

	void DrawDebugGUI() override;

	void DrawDebugPrimitive() override;

	//位置設定
	virtual void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; };

	//大きさ設定
	virtual void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }

	//角度設定
	virtual void SetAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }

private:
	//行列更新処理
	void UpdateTransform();

	//プレイヤーとゴールの当たり判定
	void CollisionPlayerVsGoal();

private:

	//Model* model = nullptr;

	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMFLOAT3 angle = { 0,0,0 };
	DirectX::XMFLOAT3 scale = { 1,1,1 };
	DirectX::XMFLOAT4X4 transform = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };

	DirectX::XMFLOAT4X4 GoalGimmickTransform = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	DirectX::XMFLOAT3 goalPosition = { 0,0,0 };
	float radius = 50;
	float height = 10;
	bool OnGoal = false;

	
};
