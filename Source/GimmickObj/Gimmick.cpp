#include "Gimmick.h"
#include"Graphics/Graphics.h"
#include"GimmickManager.h"

//�s���X�V����
void Gimmick::UpdateTransform()
{
	//	�X�P�[���s����쐬
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

	//	��]�s����쐬
	DirectX::XMMATRIX X = DirectX::XMMatrixRotationX(angle.x);
	DirectX::XMMATRIX Y = DirectX::XMMatrixRotationY(angle.y);
	DirectX::XMMATRIX Z = DirectX::XMMatrixRotationZ(angle.z);
	DirectX::XMMATRIX R = Y * X * Z;

	//	�ʒu�s����쐬
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

	//	3�̍s���g�ݍ��킹�A���[���h�s����쐬
	DirectX::XMMATRIX W = S * R * T;

	//	�v�Z�������[���h�s������o��
	DirectX::XMStoreFloat4x4(&transform, W);


}

void Gimmick::DrawDebugPrimitive() {
	DebugRenderer* debugRender = Graphics::Instance().GetDebugRenderer();
}

void Gimmick::Destroy() {
	GimmickManager::Instance().Remove(this);
}