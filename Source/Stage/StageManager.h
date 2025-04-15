#pragma once
#include<vector>
#include<set>
#include"Stage.h"
#include"Graphics/Model.h"

class StageManager
{
private:
	StageManager() {};
	~StageManager() {};

public:
	static StageManager& Instance()
	{
		static StageManager instance;
		return instance;
	}

	void Update(float elapsedTime);
	void Render(ID3D11DeviceContext* dc, Shader* shader);
	void Register(Stage* stage);
	void Clear();
	void Remove(Stage* stage);
	void RoomClear();
	Model* GetStageModel();


	//デバッグ用GUI描画
	void DrawDebugGUI();

	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);
	bool SphereCast(const DirectX::XMFLOAT3& start,const float radius ,const DirectX::XMFLOAT3& end, HitResult& hit);
	

private :
	std::vector<Stage*> stages;
	std::set<Stage*> removes;
};