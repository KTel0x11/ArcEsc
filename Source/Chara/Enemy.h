#pragma once

#include"Graphics/Shader.h"
#include"Character.h"

//エネミー
class Enemy : public Character
{
public:
	Enemy(){}
	~Enemy(){}

	//更新処理
	virtual void Update(float elapsedTime) = 0;

	//描画処理
	virtual void Render(ID3D11DeviceContext* dc, Shader* shader)=0;

	void Destroy();

	virtual void DrawDebugPrimitive();


};