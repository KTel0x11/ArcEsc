#include"StageManager.h"


void StageManager::Update(float elapsedTime) {
	for (Stage* stage : stages) {
		stage->Update(elapsedTime);
		
	}

	//�j������
	for (Stage* stage : removes) {
		std::vector<Stage*>::iterator it = std::find(stages.begin(), stages.end(), stage);
		if (it != stages.end()) {
			stages.erase(it);
		}
		delete stage;
	}
	removes.clear();
}


void StageManager::Render(ID3D11DeviceContext* dc, Shader* shader) {
	for (Stage* stage : stages) {
		stage->Render(dc,shader);
	}

}

void StageManager::Clear() {
	for (Stage* stage : stages) {
		delete stage;
	}

	stages.clear();
}

void StageManager::Register(Stage* stage) {
	stages.emplace_back(stage);
}

bool StageManager::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) {

	bool result = false;
	hit.distance = FLT_MAX;

	for (Stage* stage : stages) {
		
		HitResult tmp;
		if (stage->RayCast(start, end, tmp)) {
			if (hit.distance > tmp.distance) {
				hit = tmp;
				result = true;
			}
		}
	}

	return result;

}

bool StageManager::SphereCast(const DirectX::XMFLOAT3& start,const float radius ,const DirectX::XMFLOAT3& end, HitResult& hit) {

	bool result = false;
	hit.distance = FLT_MAX;

	for (Stage* stage : stages) {

		HitResult tmp;
		if (stage->SphereCast(start,radius,end, tmp)) {
			if (hit.distance > tmp.distance) {
				hit = tmp;
				result = true;
			}
		}
	}

	return result;

}



Model* StageManager::GetStageModel() {
	for (Stage* stage : stages) {
		return stage->GetStageModel();
	}
}


//�X�e�[�W�폜
void StageManager::Remove(Stage* stage) {
	
	//�j�����X�g�ɒǉ�
	removes.insert(stage);
	
}

void StageManager:: RoomClear() {
	Remove(stages.back());
}

//�f�o�b�O�pGUI�`��
void  StageManager::DrawDebugGUI() {
	for (Stage* stage : stages) {
		stage->DrawDebugGUI();
		stage->DrawDebugPrimitive();
	}
}