#include "Collision.h"

//球と球の交差判定
bool Collision::IntersectSphereVsSphere(
	const DirectX::XMFLOAT3& positionA,
	float radiusA,
	const DirectX::XMFLOAT3& positionB,
	float radiusB,
	DirectX::XMFLOAT3& outPositionB
) {
	//A→Bの単位ベクトルを算出
	DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
	DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
	DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionB,PositionA);
	DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);
	float lengthSq;
	DirectX::XMStoreFloat(&lengthSq, LengthSq);

	float range = radiusA + radiusB;
	if (lengthSq > range * range) {
		return false;
	}

	Vec = DirectX::XMVector3Normalize(Vec);
	Vec = DirectX::XMVectorScale(Vec, range);
	PositionB = DirectX::XMVectorAdd(PositionA, Vec);
	DirectX::XMStoreFloat3(&outPositionB, PositionB);

	return true;
}

//円柱と円柱の交差判定
bool Collision::IntersectCylinderVsCylinder(
	const DirectX::XMFLOAT3& positionA,
	float radiusA,
	float heightA,
	const DirectX::XMFLOAT3& positionB,
	float radiusB,
	float heightB,
	DirectX::XMFLOAT3& outPositionB) 
{
	//Aの足元がBの頭より上なら当たっていない
	if (positionA.y > positionB.y + heightB) {
		return false;
	}
	//Aの頭がBの足元より上なら当たっていない
	if (positionA.y + heightA < positionB.y) {
		return false;
	}

	//XZ平面での範囲チェック
	float vx = positionB.x - positionA.x;
	float vz = positionB.z - positionA.z;
	float range = radiusA + radiusB;
	float distXZ = sqrtf(vx * vx + vz * vz);
	if (distXZ > range) {
		return false;
	}

	//AがBを押し出す
	vx /= distXZ;
	vz /= distXZ;
	outPositionB.x = positionA.x + (vx * range);
	outPositionB.y = positionB.y;
	outPositionB.z = positionA.z + (vz * range);

	return true;
}

bool Collision::IntersectSphereVsCylinder(
	const DirectX::XMFLOAT3& spherePosition,
	float sphereRadius,
	const DirectX::XMFLOAT3& cylinderPosition,
	float cylinderRadius,
	float cylinderHeight,
	DirectX::XMFLOAT3& outCylinderPosition)
{
	//高さチェック
	if (spherePosition.y + sphereRadius < cylinderPosition.y) return false;
	if (spherePosition.y - sphereRadius > cylinderPosition.y + cylinderHeight) return false;

	//XZ平面での範囲チェック
	float vx = cylinderPosition.x - spherePosition.x;
	float vz = cylinderPosition.z - spherePosition.z;
	float range = sphereRadius + cylinderRadius;
	float distXZ = sqrtf(vx * vx + vz * vz);
	if (distXZ > range)return false;

	//球が円柱を押し出す
	vx /= distXZ;
	vz /= distXZ;

	outCylinderPosition.x = spherePosition.x + (vx * range);
	outCylinderPosition.y = cylinderPosition.y;
	outCylinderPosition.z = spherePosition.z + (vz * range);

}



bool Collision::IntersectRayVsModel(
	const DirectX::XMFLOAT3& start,
	const DirectX::XMFLOAT3& end,
	const Model* model,
	HitResult& result
) {
	DirectX::XMVECTOR WorldStart = DirectX::XMLoadFloat3(&start);
	DirectX::XMVECTOR WorldEnd = DirectX::XMLoadFloat3(&end);
	DirectX::XMVECTOR WorldRayVec = DirectX::XMVectorSubtract(WorldEnd, WorldStart);
	DirectX::XMVECTOR WorldRayLength = DirectX::XMVector3Length(WorldRayVec);

	DirectX::XMStoreFloat(&result.distance, WorldRayLength);

	bool hit = false;
	const ModelResource* resource = model->GetResource();
	for (const ModelResource::Mesh& mesh : resource->GetMeshes()) {
		const Model::Node& node = model->GetNodes().at(mesh.nodeIndex);

		DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&node.worldTransform);
		DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);

		DirectX::XMVECTOR S = DirectX::XMVector3TransformCoord(WorldStart, InverseWorldTransform);
		DirectX::XMVECTOR E = DirectX::XMVector3TransformCoord(WorldEnd, InverseWorldTransform);
		DirectX::XMVECTOR SE = DirectX::XMVectorSubtract(E, S);
		DirectX::XMVECTOR V = DirectX::XMVector3Normalize(SE);
		DirectX::XMVECTOR Length = DirectX::XMVector3Length(SE);

		float neart;
		DirectX::XMStoreFloat(&neart, Length);

		const std::vector<ModelResource::Vertex>& vertices = mesh.vertices;
		const std::vector<UINT> indices = mesh.indices;

		int materialIndex = -1;
		DirectX::XMVECTOR HitPosition;
		DirectX::XMVECTOR HitNormal;
		for (const ModelResource::Subset& subset : mesh.subsets) {
			for (UINT i = 0; i < subset.indexCount; i += 3) {
				UINT index = subset.startIndex + i;

				const ModelResource::Vertex& a = vertices.at(indices.at(index));
				const ModelResource::Vertex& b = vertices.at(indices.at(index + 1));
				const ModelResource::Vertex& c = vertices.at(indices.at(index + 2));

				DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&a.position);
				DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&b.position);
				DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&c.position);

				DirectX::XMVECTOR AB = DirectX::XMVectorSubtract(B, A);
				DirectX::XMVECTOR BC = DirectX::XMVectorSubtract(C, B);
				DirectX::XMVECTOR CA = DirectX::XMVectorSubtract(A, C);

				DirectX::XMVECTOR N = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(AB, BC));

				DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(V, N);
				float d;
				DirectX::XMStoreFloat(&d, Dot);

				if (d >= 0) continue;

				DirectX::XMVECTOR SA = DirectX::XMVectorSubtract(A, S);
				DirectX::XMVECTOR X = DirectX::XMVectorDivide(DirectX::XMVector3Dot(N, SA), Dot);
				float x;
				DirectX::XMStoreFloat(&x, X);
				if (x < -0.01f || x > neart + 0.01f) continue;

				DirectX::XMVECTOR P = DirectX::XMVectorAdd(DirectX::XMVectorMultiply(V, X), S);

				DirectX::XMVECTOR PA = DirectX::XMVectorSubtract(A, P);
				DirectX::XMVECTOR Cross1 = DirectX::XMVector3Cross(PA, AB);
				DirectX::XMVECTOR Dot1 = DirectX::XMVector3Dot(Cross1, N);
				float dot1;
				DirectX::XMStoreFloat(&dot1, Dot1);
				if (dot1 < -0.01f) continue;

				DirectX::XMVECTOR PB = DirectX::XMVectorSubtract(B, P);
				DirectX::XMVECTOR Cross2 = DirectX::XMVector3Cross(PB, BC);
				DirectX::XMVECTOR Dot2 = DirectX::XMVector3Dot(Cross2, N);
				float dot2;
				DirectX::XMStoreFloat(&dot2, Dot2);
				if (dot2 < -0.01f) continue;

				DirectX::XMVECTOR PC = DirectX::XMVectorSubtract(C, P);
				DirectX::XMVECTOR Cross3 = DirectX::XMVector3Cross(PC, CA);
				DirectX::XMVECTOR Dot3 = DirectX::XMVector3Dot(Cross3, N);
				float dot3;
				DirectX::XMStoreFloat(&dot3, Dot3);
				if (dot3 < -0.01f) continue;

				if (x < neart) {
					neart = x;
					HitPosition = P;
					HitNormal = N;
					materialIndex = subset.materialIndex;
				}
			}
		}

		if (materialIndex >= 0) {
			DirectX::XMVECTOR WorldPosition = DirectX::XMVector3TransformCoord(HitPosition, WorldTransform);
			DirectX::XMVECTOR WorldCrossVec = DirectX::XMVectorSubtract(WorldPosition, WorldStart);
			DirectX::XMVECTOR WorldCrossLength = DirectX::XMVector3Length(WorldCrossVec);
			float distance;
			DirectX::XMStoreFloat(&distance, WorldCrossLength);

			if (result.distance > distance) {
				DirectX::XMVECTOR WorldNormal = DirectX::XMVector3TransformNormal(HitNormal, WorldTransform);

				result.distance = distance;
				result.materialIndex = materialIndex;
				DirectX::XMStoreFloat3(&result.position, WorldPosition);
				DirectX::XMStoreFloat3(&result.normal, DirectX::XMVector3Normalize(WorldNormal));
				hit = true;
			}
		}
	}
	return hit;
	
}

bool Collision::SphereCast(
	const DirectX::XMFLOAT3& start,
	const DirectX::XMFLOAT3& end,
	float radius,
	const Model* model,
	HitResult& result
) {
	DirectX::XMVECTOR WorldStart = DirectX::XMLoadFloat3(&start);
	DirectX::XMVECTOR WorldEnd = DirectX::XMLoadFloat3(&end);
	DirectX::XMVECTOR WorldRayVec = DirectX::XMVectorSubtract(WorldEnd, WorldStart);
	DirectX::XMVECTOR WorldRayLength = DirectX::XMVector3Length(WorldRayVec);

	DirectX::XMStoreFloat(&result.distance, WorldRayLength);

	bool hit = false;
	const ModelResource* resource = model->GetResource();
	for (const ModelResource::Mesh& mesh : resource->GetMeshes()) {
		const Model::Node& node = model->GetNodes().at(mesh.nodeIndex);

		DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&node.worldTransform);
		DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);

		DirectX::XMVECTOR S = DirectX::XMVector3TransformCoord(WorldStart, InverseWorldTransform);
		DirectX::XMVECTOR E = DirectX::XMVector3TransformCoord(WorldEnd, InverseWorldTransform);
		DirectX::XMVECTOR SE = DirectX::XMVectorSubtract(E, S);
		DirectX::XMVECTOR V = DirectX::XMVector3Normalize(SE);
		DirectX::XMVECTOR Length = DirectX::XMVector3Length(SE);



		float neart;
		DirectX::XMStoreFloat(&neart, Length);

		const std::vector<ModelResource::Vertex>& vertices = mesh.vertices;
		const std::vector<UINT> indices = mesh.indices;

		int materialIndex = -1;
		DirectX::XMVECTOR HitPosition;
		DirectX::XMVECTOR HitNormal;
		for (const ModelResource::Subset& subset : mesh.subsets) {
			for (UINT i = 0; i < subset.indexCount; i += 3) {
				UINT index = subset.startIndex + i;

				// 三角形の頂点を取得
				const ModelResource::Vertex& a = vertices.at(indices.at(index));
				const ModelResource::Vertex& b = vertices.at(indices.at(index + 1));
				const ModelResource::Vertex& c = vertices.at(indices.at(index + 2));

				DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&a.position);
				DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&b.position);
				DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&c.position);

				// 三角形とスフィアの衝突判定
				float t; // 衝突位置（開始点からの距離）
				DirectX::XMVECTOR P; // 衝突点
				DirectX::XMVECTOR N; // 法線
				

				// スケール行列を取得
				DirectX::XMVECTOR scaleVec = DirectX::XMVector3Length(WorldTransform.r[0]); // X軸スケールを取得
				float scaleFactor;
				DirectX::XMStoreFloat(&scaleFactor, scaleVec);

				// スケールを考慮した半径
				float adjustedRadius = radius / scaleFactor;


				if (IntersectSphereVsTriangle(S, V, adjustedRadius, A, B, C, t, P, N)) {
					if (t >= 0.0f && t < neart) {
						neart = t;
						HitPosition = P;
						// 押し出し処理
						float distToPlane;
						float d;
						DirectX::XMStoreFloat(&d, DirectX::XMVector3Dot(N, A));
						DirectX::XMStoreFloat(&distToPlane, DirectX::XMVectorSubtract(DirectX::XMVector3Dot(N, S), DirectX::XMVectorReplicate(d)));
						float penetrationDepth = radius - fabs(distToPlane);
						if (penetrationDepth > 0.0f) {
							HitPosition = DirectX::XMVectorAdd(P, DirectX::XMVectorScale(N, penetrationDepth));
						}
						HitNormal = N;
						materialIndex = subset.materialIndex;
					}
				}
			}
		}

		if (materialIndex >= 0) {
			DirectX::XMVECTOR WorldPosition = DirectX::XMVector3TransformCoord(HitPosition, WorldTransform);
			DirectX::XMVECTOR WorldCrossVec = DirectX::XMVectorSubtract(WorldPosition, WorldStart);
			DirectX::XMVECTOR WorldCrossLength = DirectX::XMVector3Length(WorldCrossVec);
			float distance;
			DirectX::XMStoreFloat(&distance, WorldCrossLength);

			if (result.distance > distance) {
				DirectX::XMVECTOR WorldNormal = DirectX::XMVector3TransformNormal(HitNormal, WorldTransform);

				result.distance = distance;
				result.materialIndex = materialIndex;
				DirectX::XMStoreFloat3(&result.position, WorldPosition);
				DirectX::XMStoreFloat3(&result.normal, DirectX::XMVector3Normalize(WorldNormal));
				hit = true;
			}
		}
	}

	return hit;
}

bool Collision::IntersectSphereVsTriangle(
	const DirectX::XMVECTOR& sphereStart,
	const DirectX::XMVECTOR& direction,
	float radius,
	const DirectX::XMVECTOR& A,
	const DirectX::XMVECTOR& B,
	const DirectX::XMVECTOR& C,
	float& t,
	DirectX::XMVECTOR& P,
	DirectX::XMVECTOR& N
) {
	// 平面の法線を計算
	DirectX::XMVECTOR AB = DirectX::XMVectorSubtract(B, A);
	DirectX::XMVECTOR AC = DirectX::XMVectorSubtract(C, A);
	N = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(AB, AC));

	// スフィアの中心と平面の距離
	float d;
	DirectX::XMStoreFloat(&d, DirectX::XMVector3Dot(N, A));

	//float distToPlane;
	//DirectX::XMStoreFloat(&distToPlane,DirectX::XMVectorSubtract( DirectX::XMVector3Dot(N, sphereStart) , DirectX::XMVectorSet(d, d, d, d)));

	//// 平面に対するスフィアの衝突チェック
	//if (fabs(distToPlane) > radius) return false;

	//// 衝突点を計算
	//float projectionLength = distToPlane - radius;
	//P = DirectX::XMVectorAdd(sphereStart, DirectX::XMVectorScale(N, projectionLength));

	//// 衝突点が三角形内にあるか確認
	//if (IsPointInsideTriangle(P, A, B, C)) {
	//	t = projectionLength;
	//	return true;
	//}

	//return false;

	float distToPlane;
	DirectX::XMStoreFloat(&distToPlane,DirectX::XMVectorSubtract(DirectX::XMVector3Dot(N, sphereStart) , DirectX::XMVectorReplicate(d)));

	// 平面に対するスフィアの衝突チェック
	if (fabs(distToPlane) > radius) {
		return false;
	}

	// 衝突点を計算
	float projectionLength = -distToPlane; // 平面上への投影
	P = DirectX::XMVectorAdd(sphereStart, DirectX::XMVectorScale(N, projectionLength));

	// 衝突点が三角形内にあるか確認
	if (IsPointInsideTriangle(P, A, B, C)) {
		t = projectionLength;
		return true;
}
	}


bool Collision::IsPointInsideTriangle(
	const DirectX::XMVECTOR& point,
	const DirectX::XMVECTOR& A,
	const DirectX::XMVECTOR& B,
	const DirectX::XMVECTOR& C
) {
	// 辺のベクトルを計算
	DirectX::XMVECTOR AB = DirectX::XMVectorSubtract(B, A);
	DirectX::XMVECTOR BC = DirectX::XMVectorSubtract(C, B);
	DirectX::XMVECTOR CA = DirectX::XMVectorSubtract(A, C);

	// 点と三角形の頂点を結ぶベクトルを計算
	DirectX::XMVECTOR AP = DirectX::XMVectorSubtract(point, A);
	DirectX::XMVECTOR BP = DirectX::XMVectorSubtract(point, B);
	DirectX::XMVECTOR CP = DirectX::XMVectorSubtract(point, C);

	// 外積を計算
	DirectX::XMVECTOR Cross1 = DirectX::XMVector3Cross(AB, AP);
	DirectX::XMVECTOR Cross2 = DirectX::XMVector3Cross(BC, BP);
	DirectX::XMVECTOR Cross3 = DirectX::XMVector3Cross(CA, CP);

	// 外積の符号を確認
	DirectX::XMVECTOR Dot1 = DirectX::XMVector3Dot(Cross1, Cross2);
	DirectX::XMVECTOR Dot2 = DirectX::XMVector3Dot(Cross2, Cross3);

	// 符号が一致するか判定
	float dot1, dot2;
	DirectX::XMStoreFloat(&dot1, Dot1);
	DirectX::XMStoreFloat(&dot2, Dot2);

	return (dot1 >= 0.0f) && (dot2 >= 0.0f);
}

