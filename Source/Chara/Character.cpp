#include"Character.h"
#include"Stage/StageManager.h"
#include"Mathf.h"
#include"Graphics/DebugRenderer.h"
#include"Graphics/Graphics.h"
#include"GimmickObj/GimmickManager.h"

//�s���X�V����
void Character::UpdateTransform()
{
	//	�X�P�[���s����쐬
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

	//	��]�s����쐬
	DirectX::XMMATRIX X = DirectX::XMMatrixRotationX(angle.x);
	DirectX::XMMATRIX Y = DirectX::XMMatrixRotationY(angle.y);
	DirectX::XMMATRIX Z = DirectX::XMMatrixRotationZ(angle.z);
	DirectX::XMMATRIX R = Y * X * Z;

	//	�ʒu�s����쐬
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x,position.y,position.z);

	//	3�̍s���g�ݍ��킹�A���[���h�s����쐬
	DirectX::XMMATRIX W = S * R * T;

	//	�v�Z�������[���h�s������o��
	DirectX::XMStoreFloat4x4(&transform, W);


}

void Character::Move(float vx, float vz, float speed) {

	moveVecX = vx;
	moveVecZ = vz;
	maxMoveSpeed = speed;

}

//���񏈗�
void Character::Turn(float elapsedTime, float vx, float vz, float speed) {
	speed *= elapsedTime;
	//�i�s�x�N�g�����[���x�N�g���̏ꍇ�͏�������K�v�Ȃ�
	float length = sqrt(vx * vx + vz * vz);
	if (length < 0.001f)return;

	//�i�s�x�N�g����P�ʃx�N�g����
	vx /= length;
	vz /= length;



	//���g�̉�]�l����O���������߂�
	float frontX = sinf(angle.y);
	float frontZ = cosf(angle.y);

	//��]�p�����߂邽�߁A2�̒P�ʃx�N�g���̓��ς��v�Z����
	float dot = (frontX * vx) + (frontZ * vz);

	//���ϒl��-1.0�`1.0�ŕ\������Ă���A�Q�̒P�ʃx�N�g���̊p�x��
	//�������ق�1.0�ɋ߂Â��Ƃ��������𗘗p���ĉ�]���x�𒲐�����
	float rot = 1.0 - dot;
	if (rot > speed)rot = speed;

	//���E������s�����߂�2�̒P�ʃx�N�g���̊O�ς��v�Z����
	float cross = (frontZ * vx) - (frontX * vz);

	//2D�̊O�ϒl�����̏ꍇ�����̏ꍇ�ɂ���č��E���肪�s����
	//���E������s�����Ƃɂ���č��E��]��I������
	if (cross < 0.0f) {
		angle.y -= rot;
	}
	else {
		angle.y += rot;
	}

}

//�W�����v����
void Character::Jump(float speed) {
	//������̗͂�ݒ�
	velocity.y = speed;
}

//�����x�ǉ�����
void Character::AddImpulse(const DirectX::XMFLOAT3& impulse){

	velocity.x += impulse.x;
	velocity.y += impulse.y;
	velocity.z += impulse.z;

}

//���͏����X�V
void Character::UpdateVelocity(float elapsedTime) {

	//�o�߃t���[��
	float elapsedFrame = 60.0f * elapsedTime;

	DirectX::XMFLOAT3 oldAngle = angle;

	//�������͍X�V����
	UpdateVerticalVelocity(elapsedFrame);

	//�������͍X�V����
	UpdateHorizontalVelocity(elapsedFrame);

	//�����ړ��X�V����
	UpdateVerticalMove(elapsedTime);

	//�����ړ��X�V����
	UpdateHorizontalMove(elapsedTime);
	



}

//�_���[�W����
bool Character::ApplyDamage(float damage,float invincibleTime) {



	if (damage == 0.0f)return false;

	if (health <= 0.0f)
	{
		health = 0.0f;
		return false;
	}

	if (invincibleTimer > 0.0f)	return false;


	if(damage < health){

		health -= damage;
	}
	else {
		health = 0.0f;
	}

	invincibleTimer = invincibleTime; //���G����


	if (health <= 0.0f) { //����HP��0��菭�Ȃ�������
		OnDead();//���S����
	}
	else {
		OnDamaged();//�_���[�W����
	}



// ���N��Ԃ��ύX�����ꍇ��true��Ԃ�
		return true;
}

//���G���ԏ���
void Character::UpdateInvincibleTimer(float elapsedTime)
{
	if (invincibleTimer > 0.0f)
	{
		invincibleTimer -= elapsedTime;
	}
}

//���������x����
void Character::UpdateVerticalVelocity(float elapsedFrame) {
	//�d�͏���
	velocity.y += gravity * elapsedFrame;
}

//���������x����
void Character::UpdateHorizontalVelocity(float elapsedFrame)
{
	float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);

	if (length > 0.0f)
	{
		float friction = this->friction * elapsedFrame;
		

		if (length > friction)
		{


		if (!this->IsGround()) {
			friction -= airControl * elapsedFrame;
		}

			//�P�ʃx�N�g����
			float vx = velocity.x / length;
			float vz = velocity.z / length;

			velocity.x -= vx * friction;
			velocity.z -= vz * friction;
		}
		//�������̑��͂����C�͈ȉ��ɂȂ����̂ő��͂𖳌���
		else
		{
			velocity.x = 0.0f;
			velocity.z = 0.0f;
		}

	}
	if (length <= maxMoveSpeed)
	{
		float moveVecLength = sqrtf(moveVecX * moveVecX + moveVecZ * moveVecZ);

		if (moveVecLength > 0.0f) {

			float acceleration = this->acceleration * elapsedFrame;

			velocity.x += moveVecX * acceleration;
			velocity.z += moveVecZ * acceleration;

			float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
			if(length>maxMoveSpeed)
			{
				if (!this->IsGround()){
					maxMoveSpeed -= airControl*elapsedFrame;
				}

				float vx = velocity.x / length;
				float vz = velocity.z / length;

				velocity.x = vx * maxMoveSpeed;
				velocity.z = vz * maxMoveSpeed;
			
			}

			if (isGround && slopeRate > 0.0f) {
				velocity.y -= length * slopeRate * elapsedFrame;
			}

		}
		
	}
	
	moveVecX = 0.0f;
	moveVecZ = 0.0f;
	
}

//�����ړ�����


void Character::UpdateVerticalMove(float elapsedTime) {
	//float my = -0.01f + velocity.y* elapsedTime;
	float my = velocity.y * elapsedTime;
	// �L�����N�^�[��Y�������ƂȂ�@���x�N�g��
	DirectX::XMFLOAT3 normal = { 0, 1, 0 };

	// �������܂��͐ڒn���Ă���ꍇ
	if (my < 0.0f) {
		DirectX::XMFLOAT3 start = { position.x, position.y + stepOffset, position.z };
		DirectX::XMFLOAT3 end = { position.x + velocity.x * elapsedTime,
								  position.y + my,
								  position.z + velocity.z * elapsedTime };;

		HitResult hit;
		if (StageManager::Instance().RayCast(start, end, hit)) {
			normal = hit.normal;
			position = hit.position;

			if (GimmickManager::Instance().RayCast(start, end, hit)) {
				OnPoison = true;
			}
			else {
				OnPoison = false;
			}


			// �ǂƏ��̔���
			if (fabs(hit.normal.y) < 0.7f) { // �ǂ̏ꍇ
				// �ǂƂ̔���F���菈�������n�������s��Ȃ�
				velocity.y = 0.0f;
				return;
			}

			float normalLengthXZ = sqrtf(hit.normal.x * hit.normal.x + hit.normal.z * hit.normal.z);
			slopeRate = 1.0f - (hit.normal.y / (normalLengthXZ + hit.normal.y));

			if (!isGround) {
				OnLanding(); // ���n����
			}

			velocity.y = 0.0f;
			isGround = true;

			// �X�Δ���F�X�Ίp���������l�𒴂����ꍇ�Ɋ���
			float slopeAngle = acosf(hit.normal.y) * (180.0f / 3.14159265f); // �X�Ίp�x�i�x���@�j

			if (slopeAngle > 20.0f&&fabs(hit.normal.y) > 0.7f) { // 30�x�ȏ�̌X�΂Ŋ���
				float slideSpeed = 0;
				slideSpeed += 10 * slopeRate * 9.8f; // �X�΂Ɋ�Â����葬�x
				velocity.x += hit.normal.x * slideSpeed;
				velocity.z += hit.normal.z * slideSpeed;
			}
		}
		else {
			position.y += my;
			isGround = false;
		}
	}
	else if (my > 0.0f) {
		position.y += my;
		isGround = false;
	}

	DirectX::XMFLOAT3 start = { position.x, position.y + stepOffset, position.z };
	DirectX::XMFLOAT3 end = { position.x + velocity.x * elapsedTime,
							  position.y + velocity.y * elapsedTime - 1.0f,
							  position.z + velocity.z * elapsedTime };

	HitResult hit;

	//int numSteps = 5; // �����ړ����̕�ԃX�e�b�v��
	//bool hitDetected = false;
	//for (int i = 0; i <= numSteps; ++i) {
	//	float t = static_cast<float>(i) / numSteps;
	//	DirectX::XMFLOAT3 interpolatedStart = {
	//		start.x + t * (end.x - start.x),
	//		start.y + t * (end.y - start.y),
	//		start.z + t * (end.z - start.z),
	//	};

	//	if (StageManager::Instance().RayCast(interpolatedStart, end, hit)) {
	//		hitDetected = true;
	//		break;
	//	}
	//}

	//if (hitDetected) {
	//	// �Փˏ���
	//}
	//else {
	//	position.x += velocity.x * elapsedTime;
	//	position.z += velocity.z * elapsedTime;
	//}



	// �n�ʂ̌����ɉ����悤��XZ����]
	{
	/*	float ax = atan2f(normal.z, normal.y);
		float az = -atan2f(normal.x, normal.y);
		angle.x = Mathf::Lerp(angle.x, ax, 0.2f);
		angle.z = Mathf::Lerp(angle.z, az, 0.2f);*/
	}
}



void Character::UpdateHorizontalMove(float elapsedTime) {
	// �������͗ʌv�Z
	float velocityLengthXZ = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
	if (velocityLengthXZ > 0.0f) {
		float mx = velocity.x * speed * elapsedTime;
		float mz = velocity.z * speed * elapsedTime;

		// �L�����N�^�[�̒��S����̈ړ���
		DirectX::XMFLOAT3 start = { position.x, position.y + stepOffset, position.z };
		DirectX::XMFLOAT3 end = { position.x + mx, position.y + stepOffset, position.z + mz };

		// �ǔ���
		HitResult hit;
		if (StageManager::Instance().RayCast(start, end, hit)) {
			// ���C���ǂɓ��������ꍇ

			// �ǂ܂ł̃x�N�g�����v�Z
			DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start);
			DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&end);
			DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);

			// �ǂ̖@��
			DirectX::XMVECTOR Normal = DirectX::XMLoadFloat3(&hit.normal);

			// �L�����N�^�[�̔��a���l�����������o������
			float penetrationDepth = radius - hit.distance; // ���a - �ǂ܂ł̋���
			if (penetrationDepth > 0.0f) {
				DirectX::XMVECTOR PushOut = DirectX::XMVectorScale(Normal, penetrationDepth);
				Start = DirectX::XMVectorAdd(Start, PushOut); // �L�����N�^�[��ǂ̊O���ɉ����o��
			}

			// �ǂɉ������ړ�����
			DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Vec, Normal);
			DirectX::XMVECTOR SlideVec = DirectX::XMVectorSubtract(Vec, DirectX::XMVectorMultiply(Normal, Dot));
			DirectX::XMVECTOR FinalPosition = DirectX::XMVectorAdd(Start, SlideVec);

			// ���ʂ�K�p
			position.x = FinalPosition.m128_f32[0];
			position.y = FinalPosition.m128_f32[1];
			position.z = FinalPosition.m128_f32[2];

			// �ǂɏՓ˂��Ă���t���O��ݒ�
			wallHit = true;
		}
		else {
			// �ǂɏՓ˂��Ȃ��ꍇ�͂��̂܂܈ړ�
			position.x += mx;
			position.z += mz;

			wallHit = false;
		}
	}
}

//���̈ʒu�ɖ߂鏈��
void Character::ResetPosition() {
	if (velocity.y <= -150.0f ) {

		isDead = true;
		

	}
}