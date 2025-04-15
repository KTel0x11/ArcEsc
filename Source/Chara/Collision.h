#pragma once
#include <DirectXMath.h>
#include "Graphics/Model.h"

//�q�b�g����
struct HitResult
{
	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMFLOAT3 normal = { 0,0,0 };
	DirectX::XMFLOAT3 rotation = { 0,0,0 };
	float distance = 0.0f;
	int materialIndex = -1;

};


//�R���W����
class Collision
{
public:
	//���Ƌ��̌�������
	static bool IntersectSphereVsSphere(
		const DirectX::XMFLOAT3& positionA,
		float radiusA,
		const DirectX::XMFLOAT3& positionB,
		float radiusB,
		DirectX::XMFLOAT3& outPositionB
	);

	//�~���Ɖ~���̌�������
	static bool IntersectCylinderVsCylinder(
		const DirectX::XMFLOAT3& positionA,
		float radiusA,
		float heightA,
		const DirectX::XMFLOAT3& positionB,
		float radiusB,
		float heightB,
		DirectX::XMFLOAT3& outPositionB
	);

	//���Ɖ~���̌�������
	static bool IntersectSphereVsCylinder(
		const DirectX::XMFLOAT3& spherePosition,
		float sphereRadius,
		const DirectX::XMFLOAT3& cylinderPosition,
		float cylinderRadius,
		float cylinderHeight,
		DirectX::XMFLOAT3& outCylinderPosition);

	//���C�ƃ��f���̌�������
	static bool IntersectRayVsModel(
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end,
		const Model* model,
		HitResult& result
	);

	static bool SphereCast(
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end,
		float radius,
		const Model* model,
		HitResult& result
	);

	static bool IntersectSphereVsTriangle(
		const DirectX::XMVECTOR& sphereStart,
		const DirectX::XMVECTOR& direction,
		float radius,
		const DirectX::XMVECTOR& A,
		const DirectX::XMVECTOR& B,
		const DirectX::XMVECTOR& C,
		float& t,
		DirectX::XMVECTOR& P,
		DirectX::XMVECTOR& N
	);

	static bool IsPointInsideTriangle(
		const DirectX::XMVECTOR& point,
		const DirectX::XMVECTOR& A,
		const DirectX::XMVECTOR& B,
		const DirectX::XMVECTOR& C);
private:

	float distToPlane  = 0;

};