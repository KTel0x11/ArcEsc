#include<imgui.h>
#include "StageMoveFloor.h"
#include"GameState.h"
#define debug_new new(_NORMAL_BLOCK,__FILE__,__LINE__)




StageMoveFloor::StageMoveFloor() {
	scale.x = scale.z = 40.0f;
	scale.y = 5.0f;
	//�X�e�[�W���f����ǂݍ���
	model = new Model("Data/Model/Cube/SandCube.mdl");
}




StageMoveFloor::~StageMoveFloor() {
	//�X�e�[�W���f����j��
	
	delete model;
	
}


//�X�V����
void StageMoveFloor::Update(float elapsedTime) {

	//�O��̏���ۑ�
	oldTransform = transform;
	oldAngle = angle;

	//�X�^�[�g����S�[���܂ł̋������Z�o����
	DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start);
	DirectX::XMVECTOR Goal = DirectX::XMLoadFloat3(&goal);
	DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(Goal, Start);
	DirectX::XMVECTOR Length = DirectX::XMVector3Length(Vec);

	float length;
	DirectX::XMStoreFloat(&length, Length);

	if (GameState::Instance().currentState == GameState::State::Game) {

		//�X�^�[�g����S�[���܂ł̊Ԃ���b�ԂŐi�ފ���(0.0~1.0)���Z�o����
		float speed = moveSpeed * elapsedTime;
		float speedRate = speed / length;
		moveRate += speedRate;

		//�S�[���ɓ��B�A�܂��̓X�^�[�g�ɖ߂����ꍇ�A�ړ������𔽓]������
		if (moveRate <= 0.0f || moveRate >= 1.0f) {
			moveSpeed = -moveSpeed;
		}



		//���`�⊮�ňʒu���Z�o����
		DirectX::XMVECTOR Position = DirectX::XMVectorLerp(Start, Goal, moveRate);
		DirectX::XMStoreFloat3(&position, Position);

		//��]
		angle.x += torque.x * elapsedTime;
		angle.y += torque.y * elapsedTime;
		angle.z += torque.z * elapsedTime;

	}
	//�s��X�V
	UpdateTransform();

	//���C�L���X�g�p�Ƀ��f����ԍs��ɂ��邽�ߒP�ʍs���n��
	const DirectX::XMFLOAT4X4 transformIdentity = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
	model->UpdateTransform(transformIdentity);

}

//�`�揈��
void StageMoveFloor::Render(ID3D11DeviceContext* dc, Shader* shader) {

	//�\���p�̂��߃��[���h�s��ɍX�V����
	model->UpdateTransform(transform);

	shader->Draw(dc, model);

}

//���C�L���X�g
bool StageMoveFloor::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) {

	//�O��̃��[���h�s��Ƌt�s������߂�
	DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&oldTransform);
	DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);

	//�O��̃��[�J����Ԃł̃��C�ɕϊ�
	DirectX::XMVECTOR WorldStart = DirectX::XMLoadFloat3(&start);
	DirectX::XMVECTOR WorldEnd = DirectX::XMLoadFloat3(&end);
	DirectX::XMVECTOR LocalStart = DirectX::XMVector3TransformCoord(WorldStart, InverseWorldTransform);
	DirectX::XMVECTOR LocalEnd = DirectX::XMVector3TransformCoord(WorldEnd, InverseWorldTransform);

	DirectX::XMFLOAT3 localStart, localEnd;
	DirectX::XMStoreFloat3(&localStart, LocalStart);
	DirectX::XMStoreFloat3(&localEnd, LocalEnd);

	HitResult localHit;
	if (Collision::IntersectRayVsModel(localStart, localEnd, model, localHit)) {
		WorldTransform = DirectX::XMLoadFloat4x4(&transform);
		DirectX::XMVECTOR LocalPosition = DirectX::XMLoadFloat3(&localHit.position);
		DirectX::XMVECTOR WorldPosition = DirectX::XMVector3TransformCoord(LocalPosition, WorldTransform);

		DirectX::XMVECTOR LocalNormal = DirectX::XMLoadFloat3(&localHit.normal);
		DirectX::XMVECTOR WorldNormal = DirectX::XMVector3TransformNormal(LocalNormal, WorldTransform);

		DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(WorldPosition, WorldStart);
		DirectX::XMVECTOR Dist = DirectX::XMVector3Length(Vec);
		DirectX::XMStoreFloat3(&hit.position, WorldPosition);
		DirectX::XMStoreFloat3(&hit.normal, DirectX::XMVector3Normalize(WorldNormal));
		DirectX::XMStoreFloat(&hit.distance, Dist);
		hit.materialIndex = localHit.materialIndex;
		HitPosition = localHit.position;

		//��]�������Z�o
		hit.rotation.x = angle.x - oldAngle.x;
		hit.rotation.y = angle.y - oldAngle.y;
		hit.rotation.z = angle.z - oldAngle.z;
		
		hitFlg = true;
		return true;
	}

	hitFlg = false;
	return false;
}

//�X�t�B�A�L���X�g
bool StageMoveFloor::SphereCast(const DirectX::XMFLOAT3& start, const float radius, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	if (model != nullptr) {
		return Collision::SphereCast(start, end, radius, model, hit);
	}

	return nullptr;
}




//�s��X�V����
void StageMoveFloor::UpdateTransform() {
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMMATRIX W = S * R * T;
	DirectX::XMStoreFloat4x4(&transform, W);
}



void StageMoveFloor::DrawDebugGUI()
{
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Gimmick", nullptr, ImGuiWindowFlags_None)) {
		if (ImGui::CollapsingHeader("MoveFloor", ImGuiTreeNodeFlags_DefaultOpen)) {
			if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {


				ImGui::InputFloat3("position", &position.x);


			}
			if (ImGui::CollapsingHeader("RayCast Result")) {
				ImGui::Text("Hit: %s", hitFlg ? "True" : "False");
				ImGui::Text("Hit Position: (%f, %f, %f)", HitPosition.x, HitPosition.y, HitPosition.z);
			}
		}
	}
	

	ImGui::End();
}

void StageMoveFloor::DrawDebugPrimitive() {

}