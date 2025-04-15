#include<stdlib.h>
#include<time.h>
#include <stdio.h>
#include"Mathf.h"



float Mathf::Lerp(float a, float b, float t) {
	return a * (1.0f - t) + (b * t);
}

//指定範囲のランダム値を計算する
float Mathf::RandomRange(float min, float max) {
	srand((unsigned int)time(NULL));
	return static_cast<float>(rand() % RAND_MAX / RAND_MAX * (max - min) + min);
}