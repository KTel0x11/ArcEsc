#pragma once

#include<DirectXMath.h>
#include<vector>

class CameraController
{
public:
	CameraController() {}
	~CameraController(){}

	//�X�V����
	void Update(float elapsedTime);

	//�^�[�Q�b�g��ݒ�
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