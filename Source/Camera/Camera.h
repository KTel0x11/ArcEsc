#pragma once

#include <DirectXMath.h>
#include"Chara/Collision.h"
#include"Stage/Stage.h"
//カメラ

class Camera
{
private:
	Camera() {}
	~Camera(){}
public:
	//唯一のインスタンスを取得
	static Camera& Instance()
	{
		static Camera camera;
		return camera;
	}

	//指定方向を向く
	void SetLookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up);

	//パースペクティブ設定
	void SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ);

	//ビュー行列取得
	const DirectX::XMFLOAT4X4& GetView() const { return view; }

	//プロジェクション行列取得
	const DirectX::XMFLOAT4X4& GetProjection() const { return projection; }

	//視点取得
	const DirectX::XMFLOAT3& GetEye() const { return eye; }

	//注視点取得
	const DirectX::XMFLOAT3& GetFocus()const { return focus; }


	const DirectX::XMFLOAT3& GetUp()const { return up; }


	const DirectX::XMFLOAT3& GetFront()const { return front; }


	const DirectX::XMFLOAT3& GetRight()const { return right; }

	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);

private:
	DirectX::XMFLOAT4X4 view{};
	DirectX::XMFLOAT4X4 projection{};

	DirectX::XMFLOAT3 eye{};
	DirectX::XMFLOAT3 focus{};

	DirectX::XMFLOAT3 up{};
	DirectX::XMFLOAT3 front{};
	DirectX::XMFLOAT3 right{};

	std::vector<Stage*> stages;
};