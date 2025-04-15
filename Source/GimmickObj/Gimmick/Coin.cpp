#include<imgui.h>
#include<time.h>
#include <stdlib.h>
#include <stdio.h>

#include"Coin.h"
#include"Graphics/Graphics.h"
#include"Chara/Collision.h"
#include"Chara/Player.h"
#include"Audio/Audio.h"
#include"Audio/AudioSource.h"

Coin::Coin() {
	scale = { 5,5,5 };
	//scale = { 100,100,100 };
	angle = { 90,0,0 };
	radius = 4;
	model = new Model("Data/Model/OBJ/Coin.mdl");

	CoinSE = Audio::Instance().LoadAudioSource("Data/Audio/CoinSE.wav");

}


Coin::~Coin() {

	if (model != nullptr) {
		delete model;
		model = nullptr;
	}

	delete CoinSE;

}

void  Coin::Update(float elapsedTime) {
	srand((unsigned int)time(NULL));
	UpdateTransform();


		angle.y += ternSpeed * elapsedTime;//���f������]������
	

	if (destroyFlg) {//�����t���O��true��������
		destroyTime += elapsedTime;//�^�C�}�[�쓮
	
		position.y = 5.0f + -10.0f * (1.0f - destroyTime * moveSpeed) * (1.0f - destroyTime * moveSpeed);

	}
	if (destroyTime >= 2.0f) {//�^�C�}�[��10�b�߂�����
		Destroy();//�폜
	}
	
	//�Փˏ���
	DirectX::XMFLOAT3 outPosition;

	if (Collision::IntersectCylinderVsCylinder(//�����v���C���[�ƃR�C�����������Ă��Ă��t���O��false��������
		position,
		radius,
		height,
		Player::Instance().GetPosition(),
		Player::Instance().GetRadius(),
		Player::Instance().GetHeight(),
		outPosition)&& !destroyFlg) {
		CoinSE->Play(false);//SE���Đ�
		destroyFlg = true;//�t���O��true�ɕύX
		ternSpeed = 100.0f;
		fastPositionY = position.y;
		int rnd = rand() % 5 + 1;
		Player::Instance().AddCoin(rnd);
	}


}


void Coin::Render(ID3D11DeviceContext* dc, Shader* shader) {
	//���f���`��
	if (destroyTime<=2.0f) {//����10�b�߂����烂�f���������B
		shader->Draw(dc, model);
	}
}

bool Coin::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) {
	return nullptr;
}

void Coin::DrawDebugGUI() {
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Gimmick", nullptr, ImGuiWindowFlags_None)) {

		if (ImGui::CollapsingHeader("Coin", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::InputFloat3("Position", &position.x);

			DirectX::XMFLOAT3 a;
			a.x = DirectX::XMConvertToDegrees(angle.x);
			a.y = DirectX::XMConvertToDegrees(angle.y);
			a.z = DirectX::XMConvertToDegrees(angle.z);
			ImGui::InputFloat3("Angle", &a.x);
			angle.x = DirectX::XMConvertToRadians(a.x);
			angle.y = DirectX::XMConvertToRadians(a.y);
			angle.z = DirectX::XMConvertToRadians(a.z);

			ImGui::InputFloat3("Scale", &scale.x);

		}

	}

	ImGui::End();
}

void Coin::DrawDebugPrimitive() {
	Graphics::Instance().GetDebugRenderer()->DrawCylinder(position, radius, height, { 1,0,0,1 }, { 0,0,0 });
}

void Coin::UpdateTransform() {
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMMATRIX W = S * R * T;
	DirectX::XMStoreFloat4x4(&transform, W);


	if (model != nullptr) {
		model->UpdateTransform(transform);
	}

}