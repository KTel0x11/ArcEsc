#include<stdlib.h>
#include<time.h>
#include <stdio.h>
#include"Mathf.h"



float Mathf::Lerp(float a, float b, float t) {
	return a * (1.0f - t) + (b * t);
}

//�w��͈͂̃����_���l���v�Z����
float Mathf::RandomRange(float min, float max) {

float range = max - min;
float randomValue = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
return min + (randomValue * range);


}