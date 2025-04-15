#include<imgui.h>
#include "Player.h"
#include "Input/Input.h"
#include"Camera/Camera.h"
#include"Graphics/Graphics.h"
#include"EnemyManager.h"
#include"Collision.h"
#include"ProjectileStraight.h"
#include"ProjectileHoming.h"
#include"Mathf.h"
#include"Audio/AudioSource.h"
#include"Audio/Audio.h"
#include"GameState.h"
#include"UI/NumFont.h"

static Player* instance = nullptr;

Player& Player::Instance() {
//インスタンス取得
	return *instance;
}

//コンストラクタ
Player::Player()
{
	//インスタンスポインタ設定
	instance = this;

	//モデル
	model = new Model("Data/Model/UnityChan/Unity.mdl");
	//攻撃エフェクト
	hitEffect = new Effect("Data/Effect/hit.efk");
	poisonEffect = new Effect("Data/Effect/poison.efk");
	swordEffect = new Effect("Data/Effect/sword.efk");

	//モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.07f;

	//UIスプライト
	spritePlayerHP = new Sprite("Data/Sprite/PlayerHPFrame.png");
	spriteHPBar = new Sprite("Data/Sprite/PlayerHPBar.png");
	spriteHPBack = new Sprite("Data/Sprite/PlayerHPBack.png");
	spriteItemFrame = new Sprite("Data/Sprite/ItemFrame.png");
	spriteItemLife = new Sprite("Data/Sprite/LifeItem.png");
	spriteItemPower = new Sprite("Data/Sprite/PowerItem.png");
	spriteCoin = new Sprite("Data/Sprite/coin.png");
	spriteKey = new Sprite("Data/Sprite/Key.png");
	spriteUp = new Sprite("Data/Sprite/Up.png");
	//SE
	RunSE = Audio::Instance().LoadAudioSource("Data/Audio/RunSE.wav");
	RunSE->SetVolume(0.1f);
	WalkSE = Audio::Instance().LoadAudioSource("Data/Audio/WalkSE.wav");
	WalkSE->SetVolume(0.1f);
	DodgeSE = Audio::Instance().LoadAudioSource("Data/Audio/DodgeSE.wav");
	DodgeSE->SetVolume(0.1f);
	HitSE = Audio::Instance().LoadAudioSource("Data/Audio/HitSE.wav");
	HitSE->SetVolume(0.1f);

	//パラメータ初期化
	position = startPosition;
	weight = 50;
	radius = 2;
	height = 6;
	maxHealth = 100;
	health = maxHealth;
	friction = 0.9f;
	gravity = -2.0f;
	angle = { 0.0f,0.0f,0.0f };
	isDead = false;
	coin = 10;
	itemNum = 0;
	lifeItem = 0;
	powerItem = 0;

	//待機ステートへ遷移
	TransitionIdleState();
}

//デストラクタ
Player::~Player() {

	//スプライト終了化
	if (spriteHPBack != nullptr) {
		delete spriteHPBack;
		spriteHPBack = nullptr;
	}

	if (spriteHPBar != nullptr) {
		delete spriteHPBar;
		spriteHPBar = nullptr;
	}

	if (spritePlayerHP != nullptr) {
		delete spritePlayerHP;
		spritePlayerHP = nullptr;
	}

	if (spriteItemFrame != nullptr) {
		delete spriteItemFrame;
		spriteItemFrame = nullptr;
	}

	if (spriteItemLife != nullptr) {
		delete spriteItemLife;
		spriteItemLife = nullptr;
	}

	if (spriteItemPower != nullptr) {
		delete spriteItemPower;
		spriteItemPower = nullptr;
	}
	if (spriteCoin != nullptr) {
		delete spriteCoin;
		spriteCoin = nullptr;
	}
	if (spriteKey != nullptr) {
		delete spriteKey;
		spriteKey = nullptr;
	}
	if (spriteUp != nullptr) {
		delete spriteUp;
		spriteUp = nullptr;
	}

	//弾丸終了化
	projectileManager.Clear();

	//エフェクト終了化
	delete hitEffect;
	delete poisonEffect;
	delete swordEffect;

	//SE終了化
	delete RunSE;
	delete WalkSE;
	delete DodgeSE;
	delete HitSE;
	//モデル終了化
	delete model;
}

//更新処理
void Player::Update(float elapsedTime)

{

	//ステート枚の処理

	if (GameState::Instance().currentState == GameState::State::Game) {

		switch (state)
		{
		case State::Idle:
			UpdateIdleState(elapsedTime);
			break;
		case State::MoveWalk:
			UpdateMoveWalkState(elapsedTime);
			break;
		case State::MoveRun:
			UpdateMoveRunState(elapsedTime);
			break;
		case State::Jump:
			UpdateJumpState(elapsedTime);
			break;

		case State::LowLand:
			UpdateLowLandState(elapsedTime);
			break;
		case State::HighLand:
			UpdateHighLandState(elapsedTime);
			break;

		case State::Attack:
			
			UpdateAttackState(elapsedTime);
			break;

		case State::Damage:
			UpdateDamageState(elapsedTime);
			break;

		case State::Death:
			UpdateDeathState(elapsedTime);
			break;

		case State::Revive:
			UpdateReviveState(elapsedTime);
			break;

		case State::Guard:
			UpdateGuardState(elapsedTime);
			break;

		case State::Dodge:
			UpdateDodgeState(elapsedTime);
			break;
		}
	}

	//近くの敵を探す
	SearchNearEnemy();

	//速力処理更新
	UpdateVelocity(elapsedTime);

	//無敵時間更新
	UpdateInvincibleTimer(elapsedTime);

	//弾丸更新処理
	//projectileManager.Update(elapsedTime);

	//プレイヤーと敵との衝突処理
	CollisionPlayerVsEnemies();

	//弾丸と敵の当たり判定
	CollisionProjectilesVsEnemies();

	//オブジェクト行列を更新
	UpdateTransform();

	//モデルアニメーション更新処理
	model->UpdateAnimation(elapsedTime);

	//モデル行列を更新
	model->UpdateTransform(transform);

	//プレイヤーのステータス処理
	PlayerStates();
	
	//アイテム使用の処理
	UseItem();

	//パワーアップ時のタイマー
	UpdatePowerUpTimer(elapsedTime);

	//ある一定の重力加速度で落ちたらもとの位置に戻る
	ResetPosition();


}


//描画処理
void Player::Render(ID3D11DeviceContext* dc, Shader* shader) {

	//プレイヤー描画
	shader->Draw(dc, model);
	//弾丸更新処理
	//projectileManager.Render(dc, shader);

}



//UI描画
void Player::UIRender(ID3D11DeviceContext* dc) {

	//Player_HP_UI関連
	{
		DirectX::XMFLOAT2 HPPosition = {80.0f,30.0f};
		

		//プレイヤーHP背景UI
		{
			float PHPtextureBackWidth = static_cast<float>(spriteHPBack->GetTextureWidth());
			float PHPtextureBackHeight = static_cast<float>(spriteHPBack->GetTextureHeight());
			const float BackWidth = 400.0f;
			const float BackHeight = 100.0f;
			//プレイヤーHP背景UI描画
			spriteHPBack->Render(dc, HPPosition.x, HPPosition.y, BackWidth, BackHeight, 0, 0, PHPtextureBackWidth, PHPtextureBackHeight, 0, 1, 1, 1, 1);
		}

		//プレイヤーHPバーUI
		{

			DirectX::XMFLOAT2 HPBarPosition = { 156.0f,94.0f };

			float HPRate = health / maxHealth;

			float PHPtextureBarWidth = static_cast<float>(spriteHPBar->GetTextureWidth());
			float PHPtextureBarHeight = static_cast<float>(spriteHPBar->GetTextureHeight());


			float BarWidth = 287.0f;
			float BarHeight = 24.0f;
						
			BarWidth *= HPRate;

			if (health <= maxHealth * 0.1f) {
				HPColor.x = 1.0f;
				HPColor.y = 0.0f;
			}
			else if (health <= maxHealth * 0.3f) {
				HPColor.x = 1.0f;
				HPColor.y = 0.5f;
			}
			else if (health <= maxHealth * 0.5f) {
				HPColor.x = 1.0f;
				HPColor.y = 1.0f;
			}
			else {
				HPColor.x = 0.0f;
				HPColor.y = 1.0f;
			}


			//プレイヤーHPバーUI描画
			//spriteHPBar->Render(dc, HPBarPosition.x, HPBarPosition.y, backbarWidth , BarHeight, 0, 0, PHPtextureBarWidth, PHPtextureBarHeight, 0, 1.0f, 1.0f, 1.0f, 1);
			spriteHPBar->Render(dc, HPBarPosition.x, HPBarPosition.y, BarWidth, BarHeight, 0, 0, PHPtextureBarWidth*HPRate, PHPtextureBarHeight, 0, HPColor.x,HPColor.y,HPColor.z,HPColor.w);
		}

		//プレイヤーHPフレーム
		{
			float PHPtextureFrameWidth = static_cast<float>(spritePlayerHP->GetTextureWidth());
			float PHPtextureFrameHeight = static_cast<float>(spritePlayerHP->GetTextureHeight());

			const float FrameWidth = 400.0f;
			const float FrameHeight = 100.0f;

			//プレイヤーHPフレームUI描画
			spritePlayerHP->Render(dc, HPPosition.x, HPPosition.y, FrameWidth, FrameHeight, 0, 0, PHPtextureFrameWidth, PHPtextureFrameHeight, 0, 1, 1, 1, 1);
		}

		if (powerUpTime > 0) {



			spriteUp->Render(dc, HPPosition.x-10, HPPosition.y-10, 50, 50, 0, 0, 128, 128, 0, 1, 1, 1, 1);
		}


		if (haveKey) {
			keyColor = { 1,1,1,1 };
		}
		else
		{
			keyColor = { 0,0,0,1 };
		}

		//コイン所持数描画
		spriteKey->Render(dc, 190, 45, 40, 40, 0, 0, 256, 256, 0, keyColor.x, keyColor.y, keyColor.z, keyColor.w );
		//コイン所持数描画
		spriteCoin->Render(dc, 250, 45, 40, 40, 0, 0, 256, 256, 0, 1, 1, 1, 1);
		NumFont::Instance().NumRender(dc, coin, { 340,45 }, { 40,40 }, 0, { 1,1,0,1 });


		float ItemWidth = static_cast<float>(spriteItemLife->GetTextureWidth());
		float ItemHeight = static_cast<float>(spriteItemLife->GetTextureHeight());

		switch (static_cast<Item>(itemNum))
		{

		case Player::Life:
			if (lifeItem > 0) {
				spriteItemLife->Render(dc,
					140, 515,
					120, 120,
					0, 0,
					ItemWidth, ItemHeight,
					0,
					1, 1, 1, 1);

				NumFont::Instance().NumRender(dc, lifeItem, { 220, 595 }, { 40,40 }, 0, { 1,1,1,1 });
			}
			break;

		case Player::Power:
			if (powerItem > 0 ) {

				spriteItemPower->Render(dc,
					140, 520,
					120, 120,
					0, 0,
					ItemWidth, ItemHeight,
					-60,
					1, 1, 1, 1);

				NumFont::Instance().NumRender(dc, powerItem, { 220, 595 }, { 40,40 }, 0, { 1,1,1,1 });

			}
			break;
		default:
			break;
		}

	



		float ItemFrameWidth = static_cast<float>(spriteItemFrame->GetTextureWidth());
		float ItemFrameHeight = static_cast<float>(spriteItemFrame->GetTextureHeight());

		spriteItemFrame->Render(dc,
			100, 500,
			200, 157,
			0, 0,
			ItemFrameWidth, ItemFrameHeight,
			0,
			1, 1, 1, 1);


	}


}




DirectX::XMFLOAT3 Player::GetMoveVec() const {
	//入力情報を取得
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisLX();
	float ay = gamePad.GetAxisLY();

	//カメラ方向とスティックの入力値によって進行方向を計算する
	Camera& camera = Camera::Instance();
	const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
	const DirectX::XMFLOAT3& cameraFront = camera.GetFront();

	//移動ベクトルはXZ平面に水平なベクトルになるようにする

	//カメラ右方向ベクトルをXZ単位ベクトルに変換
	float cameraRightX = cameraRight.x;
	float cameraRightZ = cameraRight.z;
	float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);

	if (cameraRightLength > 0.0f) {
		//単位ベクトル化
		cameraRightX /= cameraRightLength;
		cameraRightZ /= cameraRightLength;
	}
	//カメラ前方向ベクトルをXZ単位ベクトルに変換
	float cameraFrontX = cameraFront.x;
	float cameraFrontZ = cameraFront.z;
	float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);
	if (cameraFrontLength > 0.0f) {
		//単位ベクトル化
		cameraFrontX /= cameraFrontLength;
		cameraFrontZ /= cameraFrontLength;

		}

	//スティックの水平入力値をカメラ右方向に反映し、
	//スティックの垂直入力値をカメラの前方向に反映し、
	//進行ベクトルを計算する
	DirectX::XMFLOAT3 vec;
	vec.x = (cameraRightX * ax) + (cameraFrontX * ay);
	vec.z = (cameraRightZ * ax) + (cameraFrontZ * ay);
	//Y軸方向には移動しない
	vec.y = 0.0f;
	return vec;

}

void Player::PlayerStates() {

	if (OnPoison&&isGround) {
	
		if (ApplyDamage(5, 2)) {
			poisonEffect->Play(position, 10.0f);
			HitSE->Play(false);
		}		
	}

}

bool Player::SearchNearEnemy() {

	EnemyManager& enemyManager = EnemyManager::Instance();
	int enemyCount = enemyManager.GetEnemyCount();

	neardist = FLT_MAX;
	nearEnemy = nullptr;
	targetPosition = {};
	int no = -1;

	for (int i = 0; i < enemyCount; i++) {
		DirectX::XMFLOAT3 enemyPosition = enemyManager.GetEnemy(i)->GetPosition();
		float vx = enemyPosition.x - position.x;
		float vy = enemyPosition.y - position.y;
		float vz = enemyPosition.z - position.z;

		float dist = (vx * vx + vy * vy + vz * vz);

		if (dist < neardist) {
			neardist = dist;
			no = i;
		}
	}

	if (no == -1)	return false;

	neardist = sqrtf(neardist);

	if (neardist > 20) return false;

	nearEnemy = enemyManager.GetEnemy(no);
	targetPosition = nearEnemy->GetPosition();

	return true;

}


//目標地点へ攻撃
void Player::AttackToTarget(float elapsedTime, float speedRate)
{

	//ターゲット方向への進行ベクトルを算出
	float vx = targetPosition.x - position.x;
	float vy = targetPosition.y - position.y;
	float vz = targetPosition.z - position.z;
	float dist = sqrtf(vx * vx + vy * vy + vz * vz);
	vx /= dist;
	vy /= dist;
	vz /= dist;

	DirectX::XMFLOAT3 impulse;

	float power= 20.0f;

	impulse.x = vx * power;
	impulse.y = vy * power;
	impulse.z = vz * power;

	//移動処理
	if (dist < 20.0f) { 
		
		AddImpulse(impulse); 
	}

}



//移動入力処理
bool Player::InputMove(float elapsedTime) {
	DirectX::XMFLOAT3 moveVec = GetMoveVec();

	Move(moveVec.x, moveVec.z, moveSpeed);

	Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);

	return moveVec.x != 0 || moveVec.y != 0 || moveVec.z != 0;

}

//攻撃入力処理
bool Player::InputAttack()
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	if( gamePad.GetButtonDown() & GamePad::BTN_B)
	{
		return true;
	}
	return false;
}

//ガード入力処理
bool Player::InputGuard() {
	GamePad& gamePad = Input::Instance().GetGamePad();

	if (gamePad.GetButtonDown() & GamePad::BTN_RIGHT_SHOULDER)
	{
		return true;
	}
	return false;
}

bool Player::InputDodge() {
	//GamePad& gamePad = Input::Instance().GetGamePad();
	//if (gamePad.GetButtonDown() & GamePad::BTN_X) {
	//	return true;
	//}
	return false;
}

//ジャンプ入力処理
bool Player::InputJump() {
	GamePad& gamePad = Input::Instance().GetGamePad();
	if (gamePad.GetButtonDown() & GamePad::BTN_A)
	{
		//ジャンプ回数制限
		if (jumpCount < jumpLimit) {
			//ジャンプ
			jumpCount++;
			Jump(jumpSpeed);
			//ジャンプ入力した
			return true;
		}
	}

	return false;
}


//弾丸入力処理
void Player::InputProjectile()
{
	GamePad& gamepad = Input::Instance().GetGamePad();

	//直進弾丸発射
	//if (gamepad.GetButtonDown() & GamePad::BTN_Y) {
	//	//前方向
	//	DirectX::XMFLOAT3 dir;
	//	dir.x = sinf(angle.y);
	//	dir.y = 0.0f;
	//	dir.z = cosf(angle.y);
	//	//発射位置(プレイヤーの腰あたり)
	//	DirectX::XMFLOAT3 pos;
	//	pos.x = position.x;
	//	pos.y = position.y + height * 0.5f;
	//	pos.z = position.z;
	//	//発射
	//	ProjectileStraight* projectile = new ProjectileStraight(&projectileManager);
	//	projectile->Launch(dir, pos);



	//}


	////追尾弾丸発射
	//if (gamepad.GetButtonDown() & GamePad::BTN_Y)
	//{
	//	//前方向
	//	DirectX::XMFLOAT3 dir;
	//	dir.x = sinf(angle.y);
	//	dir.y = 0.0f;
	//	dir.z = cosf(angle.y);

	//	//発射位置
	//	DirectX::XMFLOAT3 pos;
	//	pos.x = position.x;
	//	pos.y = position.y + height * 0.5f;
	//	pos.z = position.z;

	//	//ターゲット
	//	DirectX::XMFLOAT3 target;
	//	target.x = pos.x + dir.x * 1000.0f;
	//	target.y = pos.y + dir.y * 1000.0f;
	//	target.z = pos.z + dir.z * 1000.0f;

	//	//一番近くの敵をターゲットにする
	//	float dist = FLT_MAX;
	//	EnemyManager& enemyManeger = EnemyManager::Instance();
	//	int enemyCount = enemyManeger.GetEnemyCount();
	//	for (int i = 0; i < enemyCount; ++i)
	//	{
	//		//敵との距離判定
	//		Enemy* enemy = EnemyManager::Instance().GetEnemy(i);
	//		DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);
	//		DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());
	//		DirectX::XMVECTOR V = DirectX::XMVectorSubtract(E, P);
	//		DirectX::XMVECTOR D = DirectX::XMVector3LengthSq(V);
	//		float d;
	//		DirectX::XMStoreFloat(&d, D);
	//		if (d < dist)
	//		{
	//			dist = d;
	//			target = enemy->GetPosition();
	//			target.y += enemy->GetHeight() * 0.5f;
	//		}

	//	}

	//	//発射
	//	ProjectileHoming* projectile = new ProjectileHoming(&projectileManager);
	//	projectile->Launch(dir, pos, target);



	//}



}

//アイテム使用処理
void Player::UseItem() {

	GamePad& gamepad = Input::Instance().GetGamePad();
	//アイテム選択
	if (GameState::Instance().currentState == GameState::State::Game) {
		if (gamepad.GetButtonDown() & GamePad::BTN_LEFT_SHOULDER || GetAsyncKeyState('Q') & 1) {
			itemNum--;
			if (itemNum < 0) {
				itemNum = MAX - 1;
			}
		}
		if (gamepad.GetButtonDown() & GamePad::BTN_RIGHT_SHOULDER || GetAsyncKeyState('E') & 1) {
			itemNum++;
			if (itemNum > MAX - 1) {
				itemNum = Life;
			}
		}


		//アイテム使用処理
		if (gamepad.GetButtonDown() & GamePad::BTN_Y) {
			switch (static_cast<Item>(itemNum))
			{

			case Player::Life:
				if (lifeItem > 0 && health < maxHealth ) {
					health += 20.0f;
					if (health > maxHealth) {
						health = maxHealth;
					}
					lifeItem--;
				}
				break;

			case Player::Power:
				if (powerItem > 0 && !(powerUpTime > 0.0f)) {
					powerUpTime = 10.0f;
					powerItem--;
				}
				break;
			default:
				break;
			}
		}
	}

}
void Player::UpdatePowerUpTimer(float elapsedTime)
{
	if (powerUpTime > 0.0f)
	{
		powerUpTime -= elapsedTime;
		DamageMax = 20.0f;
		DamageMin = 10.0f;
	}
	else {
		DamageMax = 10.0f;
		DamageMin = 10.0f;
	}
	
}

//プレイヤーと敵の当たり判定
void Player::CollisionPlayerVsEnemies() {

	EnemyManager& enemyManager = EnemyManager::Instance();

	//全ての敵と総当たりで衝突処理
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < enemyCount; i++) {
		Enemy* enemy = enemyManager.GetEnemy(i);
		
			//衝突処理
			DirectX::XMFLOAT3 outPosition;

			if (weight > enemy->GetWeight()) {
				if (Collision::IntersectCylinderVsCylinder(
					position,
					radius,
					height,
					enemy->GetPosition(),
					enemy->GetRadius(),
					enemy->GetHeight(),
					outPosition)) {
					DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);
					DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());
					DirectX::XMVECTOR V = DirectX::XMVectorSubtract(P, E);
					DirectX::XMVECTOR N = DirectX::XMVector3Normalize(V);
					DirectX::XMFLOAT3 normal;
					DirectX::XMStoreFloat3(&normal, N);
					if (normal.y > 0.8f) {
						//小ジャンプする
						Jump(jumpSpeed * 0.5f);
					}
					else
					{
						//押し出し後の位置設定
						enemy->SetPosition(outPosition);
						
					}

				}
			}
			else {
				if (Collision::IntersectCylinderVsCylinder(
					enemy->GetPosition(),
					enemy->GetRadius(),
					enemy->GetHeight(),
					position,
					radius,
					height,
					outPosition)) {
					DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);
					DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());
					DirectX::XMVECTOR V = DirectX::XMVectorSubtract(E, P);
					DirectX::XMVECTOR N = DirectX::XMVector3Normalize(V);
					DirectX::XMFLOAT3 normal;
					DirectX::XMStoreFloat3(&normal, N);
					if (normal.y > 0.8f) {
						//小ジャンプする
						Jump(jumpSpeed * 0.5f);
					}
					else
					{
						//押し出し後の位置設定
						SetPosition(outPosition);
						
					}
				}
			}
		
	}

}



//弾丸と敵の衝突処理
void Player::CollisionProjectilesVsEnemies() {
	EnemyManager& enemyManager = EnemyManager::Instance();

//全ての弾丸と全ての敵総当たりで衝突処理
	int projectileCount = projectileManager.GetProjectileCount();
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < projectileCount; ++i)
	{
		Projectile* projectile = projectileManager.GetProjectile(i);

		for (int j = 0; j < enemyCount; ++j) 
		{
			Enemy* enemy = enemyManager.GetEnemy(j);

			//衝突処理
			DirectX::XMFLOAT3 outPosition;

			if (Collision::IntersectSphereVsCylinder(
				projectile->GetPosition(),
				projectile->GetRadius(),
				enemy->GetPosition(),
				enemy->GetRadius(),
				enemy->GetHeight(),
				outPosition))
			{
				if (enemy->ApplyDamage(Mathf::RandomRange(DamageMin, DamageMax), 0.5f)) {
					{
						DirectX::XMFLOAT3 impulse;

						const float power = 10.0f;
						const DirectX::XMFLOAT3& e = enemy->GetPosition();
						const DirectX::XMFLOAT3& p = projectile->GetPosition();
						float vx = e.x - p.x;
						float vz = e.z - p.z;
						float lengthXZ = sqrtf(vx * vx + vz * vz);
						vx /= lengthXZ;
						vz /= lengthXZ;

						impulse.x = vx * power;
						impulse.y = power * 0.5f;
						impulse.z = vz * power;

						enemy->AddImpulse(impulse);

					}
					{
						DirectX::XMFLOAT3 e = enemy->GetPosition();
						e.y += enemy->GetHeight() * 0.5f;
						hitEffect->Play(e,5.0f);
					}
					//弾丸破棄
					projectile->Destroy();
				}
			}
		}
	}
}


//ノードと敵の衝突処理
void Player::CollisionNodeVsEnemies(const char* nodeName, float nodeRadius,float elapsedTime)
{
	//ノードの取得
	Model::Node* node = model->FindNode(nodeName);

	

	//ノードの位置取得
	DirectX::XMFLOAT3 nodePosition;
	nodePosition.x = node->worldTransform._41;
	nodePosition.y = node->worldTransform._42;
	nodePosition.z = node->worldTransform._43;


	//指定のノードと全ての敵を総当たりで衝突処理
	EnemyManager& enemyManager = EnemyManager::Instance();
	int enemyCount = enemyManager.GetEnemyCount();

	

	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);

		//衝突処理
		DirectX::XMFLOAT3 outPosition;

		if (Collision::IntersectSphereVsCylinder(
			nodePosition,
			nodeRadius,
			enemy->GetPosition(),
			enemy->GetRadius(),
			enemy->GetHeight(),
			outPosition))
		{

			//Damage = Mathf::RandomRange(DamageMin, DamageMax);
			Damage = DamageMax;

			if (enemy->ApplyDamage(Damage, 0.1f)) {
				{
					
					DirectX::XMFLOAT3 impulse;

					const float power = 500.0f/enemy->GetWeight();
					const DirectX::XMFLOAT3& e = enemy->GetPosition();
					
					float vx = e.x - nodePosition.x;
					float vz = e.z - nodePosition.z;
					float lengthXZ = sqrtf(vx * vx + vz * vz);
					vx /= lengthXZ;
					vz /= lengthXZ;

					impulse.x = vx * power;
					impulse.y = power * 0.5f;
					impulse.z = vz * power;

					//enemy->AddImpulse(impulse);
				}
				{
					DirectX::XMFLOAT3 e = enemy->GetPosition();
					e.y += enemy->GetHeight() * 0.5f;
					hitEffect->Play(nodePosition,5.0f);
					HitSE->Play(false);
				}


			}

		}
	}
}
		
	


//着地したときに呼ばれる
void Player::OnLanding()
{

	jumpCount = 0;

	//下方向の速力が一定以上なら着地ステートへ
	if (velocity.y < gravity * 20.0f ) {

		if (state != State::Damage && state != State::Death) {
			if (velocity.y > -50) {
				//着地ステートへ遷移
				
				TransitionLowLandState();
			}
			else {
				TransitionHighLandState();
			}

		}
	}
	else {
		TransitionIdleState();
	}
}

//ダメージを受けた時に呼ばれる
void Player::OnDamaged() {

	//ダメージステートへ遷移
	TransitionDamageState();
	
}

//死亡した時に呼ばれる
void Player::OnDead() {
	//死亡ステートへ遷移
	TransitionDeathState();
}

//回避の移動処理
void Player::DodgeImpulse() {

	//自身の回転値から前方向を求める
	float frontX = sinf(angle.y);
	float frontZ = cosf(angle.y);
	float power = 20.0f;
	DirectX::XMFLOAT3 vec = { frontX,0,frontZ };

	DirectX::XMStoreFloat3(&vec, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&vec)));


	velocity.x = vec.x * power;
	velocity.y = 0;
	velocity.z = vec.z * power;

	invincibleTimer = 1.0f;
	//velocity = { 0,0,0 };



}

//加速度をリセット
void Player::ResetVelocity() {
	velocity = { 0,velocity.y,0 };
}

//コインを増やす
bool Player::AddCoin(int Coin) {


	if (coin + Coin < 0)return false;
	coin += Coin;
	if (coin >= MAX_COIN) {
		coin = MAX_COIN;
	}
	return true;
}


//待機ステートへ遷移
void Player::TransitionIdleState() {

	state = State::Idle;
	isGuard = false;
	CounterFlag = false;
	//待機アニメーション再生
	model->PlayAnimation(Anim_WTD_Stand, true,0.2f,2.4f,3.66f);
	WalkSE->Stop();
	RunSE->Stop();
}

//待機ステート更新処理
void Player::UpdateIdleState(float elapsedTime) {
	//移動入力処理

	if (InputMove(elapsedTime)) {
		TransitionMoveWalkState();
	}

	//ジャンプ入力処理
	if (InputJump()) {
		TransitionJumpState();
	}

	//ガード入力処理
	if (InputGuard()) {
		TransitionGuardState();
	}

	//弾丸入力処理
	InputProjectile();

	//攻撃入力処理
	if (InputAttack()) {
		TransitionAttackState();
	}
	//回避入力処理
	if (InputDodge()) {
		TransitionDodgeState();
	}

}

//移動ステートへ遷移
void Player::TransitionMoveWalkState() {
	state = State::MoveWalk;

	//歩きアニメーション再生
	model->PlayAnimation(Anim_WTD_Walk_C_F, true);
	WalkSE->Play(true);
}

void Player::UpdateMoveWalkState(float elapsedTime) {

	if (abs(velocity.x) >= moveSpeed / 100 || abs(velocity.z) >= moveSpeed / 100) {
		//走りアニメーション再生
		WalkSE->Stop();
		TransitionMoveRunState();
		
	}


	//移動入力処理
	if (!InputMove(elapsedTime)) {
		WalkSE->Stop();
		TransitionIdleState();
	
	}

	//ジャンプ入力処理
	if (InputJump()) {
		WalkSE->Stop();
		TransitionJumpState();
	}

	//弾丸入力処理
	InputProjectile();

	//攻撃入力処理
	if (InputAttack()) {
		WalkSE->Stop();
		TransitionAttackState();
	}
	//回避入力処理
	if (InputDodge()) {
		WalkSE->Stop();
		TransitionDodgeState();
	}

	//ガード入力処理
	if (InputGuard()) {
		WalkSE->Stop();
		TransitionGuardState();
	}
}


//移動ステートへ遷移
void Player::TransitionMoveRunState() {
	state = State::MoveRun;

	//走りアニメーション再生
	model->PlayAnimation(Anim_WTD_Run, true,0.2f,0.5f,1.4f);
	RunSE->Play(true);
}

void Player::UpdateMoveRunState(float elapsedTime) {



	//移動入力処理
	if (!InputMove(elapsedTime)) {
		RunSE->Stop();
		TransitionMoveWalkState();

	}

	//ジャンプ入力処理
	if (InputJump()) {
		RunSE->Stop();
		TransitionJumpState();
	}

	//弾丸入力処理
	InputProjectile();

	//攻撃入力処理
	if (InputAttack()) {
		RunSE->Stop();
		TransitionAttackState();
	}

	if (InputDodge()) {
		RunSE->Stop();
		TransitionDodgeState();
	}


	//ガード入力処理
	if (InputGuard()) {
		RunSE->Stop();
		TransitionGuardState();
	}
}

//ジャンプステートへ遷移
void Player::TransitionJumpState() {
	state = State::Jump;
	
	//ジャンプアニメーション再生
	model->PlayAnimation(Anim_jump, false,0.2f,0.15f,0.6f);
}

//ジャンプステート更新処理
void Player::UpdateJumpState(float elapsedTime) {
	//移動入力処理
	InputMove(elapsedTime);


	//ジャンプ入力処理
	
	if (InputJump()) {
		model->PlayAnimation(Anim_jump, false,0.2f,0.6f,1.0f);
	}

	//攻撃入力処理
	if (InputAttack()) {
		TransitionAttackState();
	}

	if (velocity.y >= jumpSpeed / 9)UpDownFlg = true;



	if(!model->IsPlayAnimation()) {
		if (!UpDownFlg) {
			//上昇アニメーション
			model->PlayAnimation(Anim_jump, true, 0.2f, 0.6f, 1.0f);
		}
		else {
			//下降アニメーション
			model->PlayAnimation(Anim_jump, true, 0.2f, 1.0f, 2.5f);
		}
	}
	

	//弾丸入力処理
	InputProjectile();


}

//着地ステートへ遷移
void Player::TransitionLowLandState() {
	state = State::LowLand;
	UpDownFlg = false;
	//着地アニメーション再生
	model->PlayAnimation(Anim_jump, false, 0.2f, 2.5f,3.0f);
}

//着地ステート更新処理
void Player::UpdateLowLandState(float elapsedTime) {
	float animationTime = model->GetCurrentAnimationSeconds();
    LandingFlag = animationTime >= 0.0f && animationTime <= 0.3f;
	//移動入力処理
	if (LandingFlag) {
		InputMove(elapsedTime);
	}
	//着地処理
	if (!model->IsPlayAnimation()) {
		TransitionIdleState();
	}
	
}

//着地ステートへ遷移
void Player::TransitionHighLandState() {
	state = State::HighLand;
	UpDownFlg = false;
	//着地アニメーション再生
	model->PlayAnimation(Anim_Roll_F, false, 0.2f, 2.2f, 3.1f);
	velocity = { 0,0,0 };
	DodgeSE->Play(false);
}

//着地ステート更新処理
void Player::UpdateHighLandState(float elapsedTime) {
	float animationTime = model->GetCurrentAnimationSeconds();
	LandingFlag = animationTime >= 0.0f && animationTime <= 0.3f;

	
	//移動入力処理
	if (animationTime >= 0.0f && animationTime <= 2.5f) {
		DodgeImpulse();
	}
	else if (animationTime >= 2.6f) {

		ResetVelocity();

		//移動入力処理

	

		//攻撃入力処理
		if (InputAttack()) {
			TransitionAttackState();
		}
	}
	//着地処理
	if (!model->IsPlayAnimation()) {

		if (InputMove(elapsedTime)) {
			TransitionMoveWalkState();
		}

		TransitionIdleState();
	}

}

//攻撃1ステートへ遷移
void Player::TransitionAttackState() {
	state = State::Attack;

	//攻撃アニメーション再生
	model->PlayAnimation(Anim_AttackA, false,0.2f,0.3f,0.9f);

}

//攻撃1ステート更新処理
void Player::UpdateAttackState(float elapsedTime) {
	
	//ノードの取得
	Model::Node* nodeR = model->FindNode("Prop_R");

	//ノードの位置取得
	swordPosition[0].x = nodeR->worldTransform._41;
	swordPosition[0].y = nodeR->worldTransform._42;
	swordPosition[0].z = nodeR->worldTransform._43;



	Model::Node* nodeL = model->FindNode("Prop_L");

	//ノードの位置取得
	swordPosition[1].x = nodeL->worldTransform._41;
	swordPosition[1].y = nodeL->worldTransform._42;
	swordPosition[1].z = nodeL->worldTransform._43;




	//LookToTarget(elapsedTime, 1.0f);
	float animationTime = model->GetCurrentAnimationSeconds();

	switch (AttackState) {

	case 0:
		swordEffect->Play(swordPosition[0], 1.5f);

		if (InputDodge()) {
			TransitionDodgeState();
		}

		attackCollisionFlag = animationTime >= 0.5f && animationTime <= 0.6f;
		if (attackCollisionFlag) {
			CollisionNodeVsEnemies("Prop_R", WeaponRadius, elapsedTime);
		}

		nextAttackFlag = animationTime > 0.6f && animationTime <= 0.9f;

		if (nextAttackFlag && InputAttack()) {
			//攻撃アニメーション再生
			model->PlayAnimation(Anim_AttackA, false, 0.2f, 0.9f, 1.2f);
			AttackState++;
		}
		//攻撃処理
		if (!model->IsPlayAnimation()) {
			AttackState = 0;
			TransitionIdleState();

		}
		break;

	case 1:	
		swordEffect->Play(swordPosition[1], 1.5f);

		if (InputDodge()) {
			TransitionDodgeState();
		}

		attackCollisionFlag = animationTime >= 0.95f && animationTime <= 1.0f;
		if (attackCollisionFlag) {
			CollisionNodeVsEnemies("Prop_L", WeaponRadius, elapsedTime);
		}

		nextAttackFlag = animationTime > 1.0f && animationTime <= 1.2f;

		if (nextAttackFlag && InputAttack()) {
			model->PlayAnimation(Anim_AttackA, false, 0.2f, 1.2f, 2.1f);
			AttackState++;
		}
		//攻撃処理
		if (!model->IsPlayAnimation()) {
			AttackState = 0;
			TransitionIdleState();

		}
		break;

	case 2:
		swordEffect->Play(swordPosition[0], 1.5f);

		if (InputDodge()) {
			TransitionDodgeState();
		}

		attackCollisionFlag = animationTime >= 1.3f && animationTime <= 1.4f;
		if (attackCollisionFlag) {
			CollisionNodeVsEnemies("Prop_R", WeaponRadius, elapsedTime);
		}

		nextAttackFlag = animationTime > 1.4f && animationTime <= 2.1f;
		//攻撃処理
		if (!model->IsPlayAnimation()) {
			AttackState = 0;
			TransitionIdleState();
		}
		break;
	default:
		AttackState = 0;
		TransitionIdleState();
		break;


	}

}

//ダメージステートへ遷移
void Player::TransitionDamageState() {
	state = State::Damage;

	//ダメージアニメーション再生
	model->PlayAnimation(Anim_Damaged, false,0.2f,0.2f,1.3f);
}

//ダメージステート更新処理
void Player::UpdateDamageState(float elapsedTime) {
	//ダメージアニメーションが終ったら待機ステートへ遷移
	
	if (!model->IsPlayAnimation()) {
		
		TransitionIdleState();
	}
}

//死亡ステートへ遷移
void Player::TransitionDeathState() {
	state = State::Death;
	isDead = true;
	//死亡アニメーション再生
	model->PlayAnimation(Anim_Die, false);
}

//死亡ステート更新処理
void Player::UpdateDeathState(float elapsedTime) {
	if (!model->IsPlayAnimation()) {
	//ボタンを押したら復活ステートへ遷移
		GamePad& gamePad = Input::Instance().GetGamePad();
		if (gamePad.GetButtonDown() & GamePad::BTN_A) {
			TransitionReviveState();
		}
	}
}

//復活ステートへ遷移
void Player::TransitionReviveState() {
	state = State::Revive;
	//isDead = false;
	//死亡アニメーション再生
	model->PlayAnimation(Anim_Down_F, false,0.2f,2.0f,4.3f);
	health = maxHealth;
}

//復活ステート更新処理
void Player::UpdateReviveState(float elapsedTime) {
	//復活アニメーションが終ったら待機ステートへ遷移
	if (!model->IsPlayAnimation()) {
		TransitionIdleState();
	}

}


//ガードステートへ遷移
void Player::TransitionGuardState() {
	state = State::Guard;
	model->PlayAnimation(Anim_Defend, false, 0.2f, 0.5f, 1.3f);
}

//ガードステート更新処理
void Player::UpdateGuardState(float elapsedTime) {
	invincibleTimer = 0.8f;

	if (InputAttack()) {
		invincibleTimer = 0.0f;
		TransitionAttackState();
	}

	//ガードアニメーションが終ったら待機ステートへ遷移
	if (!model->IsPlayAnimation()) {
	
		TransitionIdleState();
	}

}

//回避ステートへ遷移
void Player::TransitionDodgeState() {
	state = State::Dodge;
	model->PlayAnimation(Anim_Roll_F, false,0.2f,2.0f,3.1f);

}

//回避ステート更新処理
void Player::UpdateDodgeState(float elapsedTime) {
	float animationTime = model->GetCurrentAnimationSeconds();


		DodgeImpulse();

	if (animationTime >= 2.0f && animationTime <= 2.5f) {
	}

	if (animationTime >= 2.6f) {
		ResetVelocity();

	}
	

	if (!model->IsPlayAnimation()) {

		TransitionIdleState();
	
	}

}



//デバッグ用
void Player::DrawDebugPrimitive() {
	DebugRenderer* debugRender = Graphics::Instance().GetDebugRenderer();
	debugRender->DrawCylinder(position,  radius,height, DirectX::XMFLOAT4(0, 0, 0, 1), angle);
	projectileManager.DrawDebugPrimitive();

	debugRender->DrawSphere(rayEndPos,
		radius,
		DirectX::XMFLOAT4(1, 1, 1, 1)
	);

	if (EnemyManager::Instance().GetEnemyCount() > 0) {
		debugRender->DrawSphere(targetPosition, 2.0f, DirectX::XMFLOAT4(1, 1, 1, 1));
	}
	//攻撃衝突用の左手ノードのデバッグ球を描画
	if (attackCollisionFlag) {
		/*Model::Node* leftHandBone = model->FindNode("mixamorig:LeftHand");*/
		Model::Node* leftHandBone = model->FindNode("Prop_L");
		Model::Node* RightHandBone = model->FindNode("Prop_R");
		debugRender->DrawSphere(DirectX::XMFLOAT3(
			leftHandBone->worldTransform._41,
			leftHandBone->worldTransform._42,
			leftHandBone->worldTransform._43),
			WeaponRadius,
			DirectX::XMFLOAT4(1, 0, 0, 1)
		);
		debugRender->DrawSphere(DirectX::XMFLOAT3(
			RightHandBone->worldTransform._41,
			RightHandBone->worldTransform._42,
			RightHandBone->worldTransform._43),
			WeaponRadius,
			DirectX::XMFLOAT4(1, 0, 0, 1)
		);

	

	}
}

//デバッグ用
void Player::DrawDebugGUI()
{
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Character", nullptr, ImGuiWindowFlags_None)) {
		if (ImGui::CollapsingHeader("Player")) {
			if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::InputFloat3("Position", &position.x);

				DirectX::XMFLOAT3 a;
				a.x = DirectX::XMConvertToDegrees(angle.x);
				a.y = DirectX::XMConvertToDegrees(angle.y);
				a.z = DirectX::XMConvertToDegrees(angle.z);
				ImGui::InputFloat3("Angle", &a.x);
				angle.x = DirectX::XMConvertToRadians(a.x);
				angle.y = DirectX::XMConvertToRadians(a.y);
				angle.z = DirectX::XMConvertToRadians(a.z);

				ImGui::InputFloat3("Scale", &scale.x);
				ImGui::InputFloat3("Velocity", &velocity.x);
			}
			if (ImGui::CollapsingHeader("States", ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::InputFloat("HP", &health);
				ImGui::InputFloat("slopeRate", &slopeRate);

				ImGui::InputFloat("PTime", &powerUpTime);
				ImGui::InputFloat("MaxDamage", &DamageMax);
				ImGui::InputFloat("MinDamage", &DamageMin);
				ImGui::InputFloat("Damage", &Damage);

				ImGui::InputInt("Coin", &coin);


				ImGui::InputInt("ItemNum", &itemNum);
				ImGui::InputInt("ItemP", &powerItem);
				ImGui::InputInt("ItemL", &lifeItem);

				ImGui::Checkbox("OnPoison", &OnPoison);
				ImGui::Checkbox("Key", &haveKey);
				ImGui::Checkbox("goal", &clear);
				ImGui::Checkbox("WallHit", &wallHit);
			}
		}

	}

	ImGui::End();

}