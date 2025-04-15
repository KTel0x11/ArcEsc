#pragma once

#include<thread>

#include "Graphics/Sprite.h"
#include "Scene.h"

class SceneLoading : public Scene
{
public :
	SceneLoading(Scene* nextScene) : nextScene(nextScene) {}
	~SceneLoading(){}

	//������
	void Initialize() override;

	//�I����
	void Finalize() override;

	//�X�V����
	void Update(float elapsedTime) override;

	//�`�揈��
	void Render() override;

private:
	static void LoadingThread(SceneLoading* scene);

private:
	Sprite* spriteLoading = nullptr;
	Sprite* spriteLogo = nullptr;
	Sprite* spriteDot = nullptr;
	Sprite* spriteBack = nullptr;


	float angle = 0.0f;
	float Time = 0.0f;
	float animeTime = 0.0f;
	int animation = 0;

	Scene* nextScene = nullptr;
	std::thread* thread = nullptr;

};