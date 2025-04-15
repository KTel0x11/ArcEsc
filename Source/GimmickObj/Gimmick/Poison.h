#pragma once
#include "GimmickObj/Gimmick.h"
#include "Graphics/Model.h"

class Poison : public Gimmick
{
public:
	Poison();
	~Poison() override;


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

	float height = 2;

};