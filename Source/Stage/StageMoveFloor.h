#pragma once

#include"Graphics/Model.h"
#include"Stage.h"




class StageMoveFloor :public Stage
{
public:
	StageMoveFloor();
	~StageMoveFloor()override;


	//更新処理
	void Update(float elapsedTime) override;

	//描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader) override;

	//レイキャスト
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) override;

	//スフィアキャスト
	bool SphereCast(const DirectX::XMFLOAT3& start, const float radius, const DirectX::XMFLOAT3& end, HitResult& hit) override;

	//ImGui描画
	void DrawDebugGUI() override;

	void DrawDebugPrimitive() override;

	//スタート位置設定
	void SetStartPoint(const DirectX::XMFLOAT3& start) { this->start = start; }

	//ゴール位置設定
	void SetGoalPoint(const DirectX::XMFLOAT3& goal) { this->goal = goal; }

	void SetAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }

	//トルク設定
	void SetTorque(const DirectX::XMFLOAT3& torque) { this->torque = torque; }

	void SetMoveSpeed(const float& speed) { this->moveSpeed = speed; }

private :
	//行列更新処理
	void UpdateTransform();


private:
	//Model* model = nullptr;


	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMFLOAT3 angle = { 0,0,0 };
	DirectX::XMFLOAT3 scale = { 1,1,1 };
	DirectX::XMFLOAT4X4 transform = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };


	DirectX::XMFLOAT3 start = { 0,0,0 };
	DirectX::XMFLOAT3 goal = { 0,0,0 };
	DirectX::XMFLOAT3 torque = { 0,0,0 };
	DirectX::XMFLOAT3 oldAngle = { 0,0,0 };

	DirectX::XMFLOAT3 HitPosition{ 0,0,0 };
	bool hitFlg = false;

	float moveSpeed = 10.0f;
	float moveRate = 0.0f;


	DirectX::XMFLOAT4X4 oldTransform = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };

};
