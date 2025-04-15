#include "Collision.h"

//���Ƌ��̌�������
bool Collision::IntersectSphereVsSphere(
	const DirectX::XMFLOAT3& positionA,
	float radiusA,
	const DirectX::XMFLOAT3& positionB,
	float radiusB,
	DirectX::XMFLOAT3& outPositionB
) {
	//A��B�̒P�ʃx�N�g�����Z�o
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

//�~���Ɖ~���̌�������
bool Collision::IntersectCylinderVsCylinder(
	const DirectX::XMFLOAT3& positionA,
	float radiusA,
	float heightA,
	const DirectX::XMFLOAT3& positionB,
	float radiusB,
	float heightB,
	DirectX::XMFLOAT3& outPositionB) 
{
	//A�̑�����B�̓�����Ȃ瓖�����Ă��Ȃ�
	if (positionA.y > positionB.y + heightB) {
		return false;
	}
	//A�̓���B�̑�������Ȃ瓖�����Ă��Ȃ�
	if (positionA.y + heightA < positionB.y) {
		return false;
	}

	//XZ���ʂł͈̔̓`�F�b�N
	float vx = positionB.x - positionA.x;
	float vz = positionB.z - positionA.z;
	float range = radiusA + radiusB;
	float distXZ = sqrtf(vx * vx + vz * vz);
	if (distXZ > range) {
		return false;
	}

	//A��B�������o��
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
	//�����`�F�b�N
	if (spherePosition.y + sphereRadius < cylinderPosition.y) return false;
	if (spherePosition.y - sphereRadius > cylinderPosition.y + cylinderHeight) return false;

	//XZ���ʂł͈̔̓`�F�b�N
	float vx = cylinderPosition.x - spherePosition.x;
	float vz = cylinderPosition.z - spherePosition.z;
	float range = sphereRadius + cylinderRadius;
	float distXZ = sqrtf(vx * vx + vz * vz);
	if (distXZ > range)return false;

	//�����~���������o��
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

				// �O�p�`�̒��_���擾
				const ModelResource::Vertex& a = vertices.at(indices.at(index));
				const ModelResource::Vertex& b = vertices.at(indices.at(index + 1));
				const ModelResource::Vertex& c = vertices.at(indices.at(index + 2));

				DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&a.position);
				DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&b.position);
				DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&c.position);

				// �O�p�`�ƃX�t�B�A�̏Փ˔���
				float t; // �Փˈʒu�i�J�n�_����̋����j
				DirectX::XMVECTOR P; // �Փ˓_
				DirectX::XMVECTOR N; // �@��
				

				// �X�P�[���s����擾
				DirectX::XMVECTOR scaleVec = DirectX::XMVector3Length(WorldTransform.r[0]); // X���X�P�[�����擾
				float scaleFactor;
				DirectX::XMStoreFloat(&scaleFactor, scaleVec);

				// �X�P�[�����l���������a
				float adjustedRadius = radius / scaleFactor;


				if (IntersectSphereVsTriangle(S, V, adjustedRadius, A, B, C, t, P, N)) {
					if (t >= 0.0f && t < neart) {
						neart = t;
						HitPosition = P;
						// �����o������
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
	// ���ʂ̖@�����v�Z
	DirectX::XMVECTOR AB = DirectX::XMVectorSubtract(B, A);
	DirectX::XMVECTOR AC = DirectX::XMVectorSubtract(C, A);
	N = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(AB, AC));

	// �X�t�B�A�̒��S�ƕ��ʂ̋���
	float d;
	DirectX::XMStoreFloat(&d, DirectX::XMVector3Dot(N, A));

	//float distToPlane;
	//DirectX::XMStoreFloat(&distToPlane,DirectX::XMVectorSubtract( DirectX::XMVector3Dot(N, sphereStart) , DirectX::XMVectorSet(d, d, d, d)));

	//// ���ʂɑ΂���X�t�B�A�̏Փ˃`�F�b�N
	//if (fabs(distToPlane) > radius) return false;

	//// �Փ˓_���v�Z
	//float projectionLength = distToPlane - radius;
	//P = DirectX::XMVectorAdd(sphereStart, DirectX::XMVectorScale(N, projectionLength));

	//// �Փ˓_���O�p�`���ɂ��邩�m�F
	//if (IsPointInsideTriangle(P, A, B, C)) {
	//	t = projectionLength;
	//	return true;
	//}

	//return false;

	float distToPlane;
	DirectX::XMStoreFloat(&distToPlane,DirectX::XMVectorSubtract(DirectX::XMVector3Dot(N, sphereStart) , DirectX::XMVectorReplicate(d)));

	// ���ʂɑ΂���X�t�B�A�̏Փ˃`�F�b�N
	if (fabs(distToPlane) > radius) {
		return false;
	}

	// �Փ˓_���v�Z
	float projectionLength = -distToPlane; // ���ʏ�ւ̓��e
	P = DirectX::XMVectorAdd(sphereStart, DirectX::XMVectorScale(N, projectionLength));

	// �Փ˓_���O�p�`���ɂ��邩�m�F
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
	// �ӂ̃x�N�g�����v�Z
	DirectX::XMVECTOR AB = DirectX::XMVectorSubtract(B, A);
	DirectX::XMVECTOR BC = DirectX::XMVectorSubtract(C, B);
	DirectX::XMVECTOR CA = DirectX::XMVectorSubtract(A, C);

	// �_�ƎO�p�`�̒��_�����ԃx�N�g�����v�Z
	DirectX::XMVECTOR AP = DirectX::XMVectorSubtract(point, A);
	DirectX::XMVECTOR BP = DirectX::XMVectorSubtract(point, B);
	DirectX::XMVECTOR CP = DirectX::XMVectorSubtract(point, C);

	// �O�ς��v�Z
	DirectX::XMVECTOR Cross1 = DirectX::XMVector3Cross(AB, AP);
	DirectX::XMVECTOR Cross2 = DirectX::XMVector3Cross(BC, BP);
	DirectX::XMVECTOR Cross3 = DirectX::XMVector3Cross(CA, CP);

	// �O�ς̕������m�F
	DirectX::XMVECTOR Dot1 = DirectX::XMVector3Dot(Cross1, Cross2);
	DirectX::XMVECTOR Dot2 = DirectX::XMVector3Dot(Cross2, Cross3);

	// ��������v���邩����
	float dot1, dot2;
	DirectX::XMStoreFloat(&dot1, Dot1);
	DirectX::XMStoreFloat(&dot2, Dot2);

	return (dot1 >= 0.0f) && (dot2 >= 0.0f);
}

