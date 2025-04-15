#pragma once
#pragma once

#include"Graphics/Model.h"
#include"Stage.h"




class StageTree :public Stage
{
public:
	StageTree(float MaxHeight);
	~StageTree()override;


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



	//位置設定
	virtual void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; };

	//大きさ設定
	virtual void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }

	//角度設定
	virtual void SetAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }


private:
	//行列更新処理
	void UpdateTransform();

	bool UpdateGroundTimer(float elapsedTime);

	float Lerp(float a, float b, float t) {
		return a + (b - a) * t;
	}


private:

	struct GimmickSwitch
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 angle;
		DirectX::XMFLOAT3 scale;
		float range;
		float height;
	};

	//Model* model = nullptr;


	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMFLOAT3 angle = { 0,0,0 };
	DirectX::XMFLOAT3 scale = { 1,1,1 };
	DirectX::XMFLOAT4X4 transform = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };

	DirectX::XMFLOAT3 HitPosition{ 0,0,0 };

	float maxHeight = 0;
	float minHeight = 0;

	float moveSpeed = 5.0f;
	float moveRate = 0.0f;

	DirectX::XMFLOAT3 currentNormal = { 0.0f, 1.0f, 0.0f }; // 初期法線

	DirectX::XMFLOAT4X4 oldTransform = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };

	bool isPlayerOnSeesaw = false;        // プレイヤーがシーソーに乗っているか
 

};
