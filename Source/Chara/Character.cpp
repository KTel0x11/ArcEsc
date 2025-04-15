#include"Character.h"
#include"Stage/StageManager.h"
#include"Mathf.h"
#include"Graphics/DebugRenderer.h"
#include"Graphics/Graphics.h"
#include"GimmickObj/GimmickManager.h"

//行動更新処理
void Character::UpdateTransform()
{
	//	スケール行列を作成
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

	//	回転行列を作成
	DirectX::XMMATRIX X = DirectX::XMMatrixRotationX(angle.x);
	DirectX::XMMATRIX Y = DirectX::XMMatrixRotationY(angle.y);
	DirectX::XMMATRIX Z = DirectX::XMMatrixRotationZ(angle.z);
	DirectX::XMMATRIX R = Y * X * Z;

	//	位置行列を作成
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x,position.y,position.z);

	//	3つの行列を組み合わせ、ワールド行列を作成
	DirectX::XMMATRIX W = S * R * T;

	//	計算したワールド行列を取り出す
	DirectX::XMStoreFloat4x4(&transform, W);


}

void Character::Move(float vx, float vz, float speed) {

	moveVecX = vx;
	moveVecZ = vz;
	maxMoveSpeed = speed;

}

//旋回処理
void Character::Turn(float elapsedTime, float vx, float vz, float speed) {
	speed *= elapsedTime;
	//進行ベクトルがゼロベクトルの場合は処理する必要なし
	float length = sqrt(vx * vx + vz * vz);
	if (length < 0.001f)return;

	//進行ベクトルを単位ベクトル化
	vx /= length;
	vz /= length;



	//自身の回転値から前方向を求める
	float frontX = sinf(angle.y);
	float frontZ = cosf(angle.y);

	//回転角を求めるため、2つの単位ベクトルの内積を計算する
	float dot = (frontX * vx) + (frontZ * vz);

	//内積値は-1.0～1.0で表現されており、２つの単位ベクトルの角度が
	//小さいほど1.0に近づくという性質を利用して回転速度を調整する
	float rot = 1.0 - dot;
	if (rot > speed)rot = speed;

	//左右判定を行うために2つの単位ベクトルの外積を計算する
	float cross = (frontZ * vx) - (frontX * vz);

	//2Dの外積値が正の場合か負の場合によって左右判定が行える
	//左右判定を行うことによって左右回転を選択する
	if (cross < 0.0f) {
		angle.y -= rot;
	}
	else {
		angle.y += rot;
	}

}

//ジャンプ処理
void Character::Jump(float speed) {
	//上方向の力を設定
	velocity.y = speed;
}

//加速度追加処理
void Character::AddImpulse(const DirectX::XMFLOAT3& impulse){

	velocity.x += impulse.x;
	velocity.y += impulse.y;
	velocity.z += impulse.z;

}

//速力処理更新
void Character::UpdateVelocity(float elapsedTime) {

	//経過フレーム
	float elapsedFrame = 60.0f * elapsedTime;

	DirectX::XMFLOAT3 oldAngle = angle;

	//垂直速力更新処理
	UpdateVerticalVelocity(elapsedFrame);

	//水平速力更新処理
	UpdateHorizontalVelocity(elapsedFrame);

	//垂直移動更新処理
	UpdateVerticalMove(elapsedTime);

	//水平移動更新処理
	UpdateHorizontalMove(elapsedTime);
	



}

//ダメージ処理
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

	invincibleTimer = invincibleTime; //無敵時間


	if (health <= 0.0f) { //もしHPが0より少なかったら
		OnDead();//死亡処理
	}
	else {
		OnDamaged();//ダメージ処理
	}



// 健康状態が変更した場合はtrueを返す
		return true;
}

//無敵時間処理
void Character::UpdateInvincibleTimer(float elapsedTime)
{
	if (invincibleTimer > 0.0f)
	{
		invincibleTimer -= elapsedTime;
	}
}

//垂直加速度処理
void Character::UpdateVerticalVelocity(float elapsedFrame) {
	//重力処理
	velocity.y += gravity * elapsedFrame;
}

//水平加速度処理
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

			//単位ベクトル化
			float vx = velocity.x / length;
			float vz = velocity.z / length;

			velocity.x -= vx * friction;
			velocity.z -= vz * friction;
		}
		//横方向の速力が摩擦力以下になったので速力を無効化
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

//垂直移動処理


void Character::UpdateVerticalMove(float elapsedTime) {
	//float my = -0.01f + velocity.y* elapsedTime;
	float my = velocity.y * elapsedTime;
	// キャラクターのY軸方向となる法線ベクトル
	DirectX::XMFLOAT3 normal = { 0, 1, 0 };

	// 落下中または接地している場合
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


			// 壁と床の判定
			if (fabs(hit.normal.y) < 0.7f) { // 壁の場合
				// 壁との判定：滑り処理も着地処理も行わない
				velocity.y = 0.0f;
				return;
			}

			float normalLengthXZ = sqrtf(hit.normal.x * hit.normal.x + hit.normal.z * hit.normal.z);
			slopeRate = 1.0f - (hit.normal.y / (normalLengthXZ + hit.normal.y));

			if (!isGround) {
				OnLanding(); // 着地処理
			}

			velocity.y = 0.0f;
			isGround = true;

			// 傾斜判定：傾斜角がしきい値を超えた場合に滑る
			float slopeAngle = acosf(hit.normal.y) * (180.0f / 3.14159265f); // 傾斜角度（度数法）

			if (slopeAngle > 20.0f&&fabs(hit.normal.y) > 0.7f) { // 30度以上の傾斜で滑る
				float slideSpeed = 0;
				slideSpeed += 10 * slopeRate * 9.8f; // 傾斜に基づく滑り速度
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

	//int numSteps = 5; // 高速移動中の補間ステップ数
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
	//	// 衝突処理
	//}
	//else {
	//	position.x += velocity.x * elapsedTime;
	//	position.z += velocity.z * elapsedTime;
	//}



	// 地面の向きに沿うようにXZ軸回転
	{
	/*	float ax = atan2f(normal.z, normal.y);
		float az = -atan2f(normal.x, normal.y);
		angle.x = Mathf::Lerp(angle.x, ax, 0.2f);
		angle.z = Mathf::Lerp(angle.z, az, 0.2f);*/
	}
}



void Character::UpdateHorizontalMove(float elapsedTime) {
	// 水平速力量計算
	float velocityLengthXZ = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
	if (velocityLengthXZ > 0.0f) {
		float mx = velocity.x * speed * elapsedTime;
		float mz = velocity.z * speed * elapsedTime;

		// キャラクターの中心からの移動先
		DirectX::XMFLOAT3 start = { position.x, position.y + stepOffset, position.z };
		DirectX::XMFLOAT3 end = { position.x + mx, position.y + stepOffset, position.z + mz };

		// 壁判定
		HitResult hit;
		if (StageManager::Instance().RayCast(start, end, hit)) {
			// レイが壁に当たった場合

			// 壁までのベクトルを計算
			DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start);
			DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&end);
			DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);

			// 壁の法線
			DirectX::XMVECTOR Normal = DirectX::XMLoadFloat3(&hit.normal);

			// キャラクターの半径を考慮した押し出し処理
			float penetrationDepth = radius - hit.distance; // 半径 - 壁までの距離
			if (penetrationDepth > 0.0f) {
				DirectX::XMVECTOR PushOut = DirectX::XMVectorScale(Normal, penetrationDepth);
				Start = DirectX::XMVectorAdd(Start, PushOut); // キャラクターを壁の外側に押し出す
			}

			// 壁に沿った移動処理
			DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Vec, Normal);
			DirectX::XMVECTOR SlideVec = DirectX::XMVectorSubtract(Vec, DirectX::XMVectorMultiply(Normal, Dot));
			DirectX::XMVECTOR FinalPosition = DirectX::XMVectorAdd(Start, SlideVec);

			// 結果を適用
			position.x = FinalPosition.m128_f32[0];
			position.y = FinalPosition.m128_f32[1];
			position.z = FinalPosition.m128_f32[2];

			// 壁に衝突しているフラグを設定
			wallHit = true;
		}
		else {
			// 壁に衝突しない場合はそのまま移動
			position.x += mx;
			position.z += mz;

			wallHit = false;
		}
	}
}

//元の位置に戻る処理
void Character::ResetPosition() {
	if (velocity.y <= -150.0f ) {

		isDead = true;
		

	}
}