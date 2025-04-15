#pragma once
#include "GimmickObj/Gimmick.h"
#include "Graphics/Model.h"
#include "Audio/AudioSource.h"

class Key : public Gimmick
{
public:
	Key();
	~Key() override;


	//更新処理
	void Update(float elapsedTime) override;

	//描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader) override;

	//レイキャスト
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) override;

	void DrawDebugGUI() override;
	void DrawDebugPrimitive() override;

	void UpdateTransform();
private:
	Model* model = nullptr;

	float height = 5;
	float moveSpeed = 5.0f;

	float fastPositionY;

	float destroyTime = 0;
	bool destroyFlg = false;

	AudioSource* CoinSE = nullptr;

};