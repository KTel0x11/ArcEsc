#include "Gimmick.h"
#include"Graphics/Graphics.h"
#include"GimmickManager.h"

//行動更新処理
void Gimmick::UpdateTransform()
{
	//	スケール行列を作成
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

	//	回転行列を作成
	DirectX::XMMATRIX X = DirectX::XMMatrixRotationX(angle.x);
	DirectX::XMMATRIX Y = DirectX::XMMatrixRotationY(angle.y);
	DirectX::XMMATRIX Z = DirectX::XMMatrixRotationZ(angle.z);
	DirectX::XMMATRIX R = Y * X * Z;

	//	位置行列を作成
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

	//	3つの行列を組み合わせ、ワールド行列を作成
	DirectX::XMMATRIX W = S * R * T;

	//	計算したワールド行列を取り出す
	DirectX::XMStoreFloat4x4(&transform, W);


}

void Gimmick::DrawDebugPrimitive() {
	DebugRenderer* debugRender = Graphics::Instance().GetDebugRenderer();
}

void Gimmick::Destroy() {
	GimmickManager::Instance().Remove(this);
}