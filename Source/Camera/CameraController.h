#pragma once

#include<DirectXMath.h>
#include<vector>

class CameraController
{
public:
	CameraController() {}
	~CameraController(){}

	//更新処理
	void Update(float elapsedTime);

	//ターゲット壱設定
	void SetTarget(const DirectX::XMFLOAT3& target) { this->target = target; }


private:
	DirectX::XMFLOAT3 target = { 0,0,0 };
	DirectX::XMFLOAT3 angle = { 0,0,0 };
	DirectX::XMFLOAT3 eye = {0,0,0};
	float rollSpeed = DirectX::XMConvertToRadians(180);
	float range = 30.0f;
	float maxAngleX = DirectX::XMConvertToRadians(60);
	float minAngleX = DirectX::XMConvertToRadians(-10);

	
};