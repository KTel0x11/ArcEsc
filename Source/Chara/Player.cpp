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
//�C���X�^���X�擾
	return *instance;
}

//�R���X�g���N�^
Player::Player()
{
	//�C���X�^���X�|�C���^�ݒ�
	instance = this;

	//���f��
	model = new Model("Data/Model/UnityChan/Unity.mdl");
	//�U���G�t�F�N�g
	hitEffect = new Effect("Data/Effect/hit.efk");
	poisonEffect = new Effect("Data/Effect/poison.efk");
	swordEffect = new Effect("Data/Effect/sword.efk");

	//���f�����傫���̂ŃX�P�[�����O
	scale.x = scale.y = scale.z = 0.07f;

	//UI�X�v���C�g
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

	//�p�����[�^������
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

	//�ҋ@�X�e�[�g�֑J��
	TransitionIdleState();
}

//�f�X�g���N�^
Player::~Player() {

	//�X�v���C�g�I����
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

	//�e�ۏI����
	projectileManager.Clear();

	//�G�t�F�N�g�I����
	delete hitEffect;
	delete poisonEffect;
	delete swordEffect;

	//SE�I����
	delete RunSE;
	delete WalkSE;
	delete DodgeSE;
	delete HitSE;
	//���f���I����
	delete model;
}

//�X�V����
void Player::Update(float elapsedTime)

{

	//�X�e�[�g���̏���

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

	//�߂��̓G��T��
	SearchNearEnemy();

	//���͏����X�V
	UpdateVelocity(elapsedTime);

	//���G���ԍX�V
	UpdateInvincibleTimer(elapsedTime);

	//�e�ۍX�V����
	//projectileManager.Update(elapsedTime);

	//�v���C���[�ƓG�Ƃ̏Փˏ���
	CollisionPlayerVsEnemies();

	//�e�ۂƓG�̓����蔻��
	CollisionProjectilesVsEnemies();

	//�I�u�W�F�N�g�s����X�V
	UpdateTransform();

	//���f���A�j���[�V�����X�V����
	model->UpdateAnimation(elapsedTime);

	//���f���s����X�V
	model->UpdateTransform(transform);

	//�v���C���[�̃X�e�[�^�X����
	PlayerStates();
	
	//�A�C�e���g�p�̏���
	UseItem();

	//�p���[�A�b�v���̃^�C�}�[
	UpdatePowerUpTimer(elapsedTime);

	//������̏d�͉����x�ŗ���������Ƃ̈ʒu�ɖ߂�
	ResetPosition();


}


//�`�揈��
void Player::Render(ID3D11DeviceContext* dc, Shader* shader) {

	//�v���C���[�`��
	shader->Draw(dc, model);
	//�e�ۍX�V����
	//projectileManager.Render(dc, shader);

}



//UI�`��
void Player::UIRender(ID3D11DeviceContext* dc) {

	//Player_HP_UI�֘A
	{
		DirectX::XMFLOAT2 HPPosition = {80.0f,30.0f};
		

		//�v���C���[HP�w�iUI
		{
			float PHPtextureBackWidth = static_cast<float>(spriteHPBack->GetTextureWidth());
			float PHPtextureBackHeight = static_cast<float>(spriteHPBack->GetTextureHeight());
			const float BackWidth = 400.0f;
			const float BackHeight = 100.0f;
			//�v���C���[HP�w�iUI�`��
			spriteHPBack->Render(dc, HPPosition.x, HPPosition.y, BackWidth, BackHeight, 0, 0, PHPtextureBackWidth, PHPtextureBackHeight, 0, 1, 1, 1, 1);
		}

		//�v���C���[HP�o�[UI
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


			//�v���C���[HP�o�[UI�`��
			//spriteHPBar->Render(dc, HPBarPosition.x, HPBarPosition.y, backbarWidth , BarHeight, 0, 0, PHPtextureBarWidth, PHPtextureBarHeight, 0, 1.0f, 1.0f, 1.0f, 1);
			spriteHPBar->Render(dc, HPBarPosition.x, HPBarPosition.y, BarWidth, BarHeight, 0, 0, PHPtextureBarWidth*HPRate, PHPtextureBarHeight, 0, HPColor.x,HPColor.y,HPColor.z,HPColor.w);
		}

		//�v���C���[HP�t���[��
		{
			float PHPtextureFrameWidth = static_cast<float>(spritePlayerHP->GetTextureWidth());
			float PHPtextureFrameHeight = static_cast<float>(spritePlayerHP->GetTextureHeight());

			const float FrameWidth = 400.0f;
			const float FrameHeight = 100.0f;

			//�v���C���[HP�t���[��UI�`��
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

		//�R�C���������`��
		spriteKey->Render(dc, 190, 45, 40, 40, 0, 0, 256, 256, 0, keyColor.x, keyColor.y, keyColor.z, keyColor.w );
		//�R�C���������`��
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
	//���͏����擾
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisLX();
	float ay = gamePad.GetAxisLY();

	//�J���������ƃX�e�B�b�N�̓��͒l�ɂ���Đi�s�������v�Z����
	Camera& camera = Camera::Instance();
	const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
	const DirectX::XMFLOAT3& cameraFront = camera.GetFront();

	//�ړ��x�N�g����XZ���ʂɐ����ȃx�N�g���ɂȂ�悤�ɂ���

	//�J�����E�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
	float cameraRightX = cameraRight.x;
	float cameraRightZ = cameraRight.z;
	float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);

	if (cameraRightLength > 0.0f) {
		//�P�ʃx�N�g����
		cameraRightX /= cameraRightLength;
		cameraRightZ /= cameraRightLength;
	}
	//�J�����O�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
	float cameraFrontX = cameraFront.x;
	float cameraFrontZ = cameraFront.z;
	float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);
	if (cameraFrontLength > 0.0f) {
		//�P�ʃx�N�g����
		cameraFrontX /= cameraFrontLength;
		cameraFrontZ /= cameraFrontLength;

		}

	//�X�e�B�b�N�̐������͒l���J�����E�����ɔ��f���A
	//�X�e�B�b�N�̐������͒l���J�����̑O�����ɔ��f���A
	//�i�s�x�N�g�����v�Z����
	DirectX::XMFLOAT3 vec;
	vec.x = (cameraRightX * ax) + (cameraFrontX * ay);
	vec.z = (cameraRightZ * ax) + (cameraFrontZ * ay);
	//Y�������ɂ͈ړ����Ȃ�
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


//�ڕW�n�_�֍U��
void Player::AttackToTarget(float elapsedTime, float speedRate)
{

	//�^�[�Q�b�g�����ւ̐i�s�x�N�g�����Z�o
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

	//�ړ�����
	if (dist < 20.0f) { 
		
		AddImpulse(impulse); 
	}

}



//�ړ����͏���
bool Player::InputMove(float elapsedTime) {
	DirectX::XMFLOAT3 moveVec = GetMoveVec();

	Move(moveVec.x, moveVec.z, moveSpeed);

	Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);

	return moveVec.x != 0 || moveVec.y != 0 || moveVec.z != 0;

}

//�U�����͏���
bool Player::InputAttack()
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	if( gamePad.GetButtonDown() & GamePad::BTN_B)
	{
		return true;
	}
	return false;
}

//�K�[�h���͏���
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

//�W�����v���͏���
bool Player::InputJump() {
	GamePad& gamePad = Input::Instance().GetGamePad();
	if (gamePad.GetButtonDown() & GamePad::BTN_A)
	{
		//�W�����v�񐔐���
		if (jumpCount < jumpLimit) {
			//�W�����v
			jumpCount++;
			Jump(jumpSpeed);
			//�W�����v���͂���
			return true;
		}
	}

	return false;
}


//�e�ۓ��͏���
void Player::InputProjectile()
{
	GamePad& gamepad = Input::Instance().GetGamePad();

	//���i�e�۔���
	//if (gamepad.GetButtonDown() & GamePad::BTN_Y) {
	//	//�O����
	//	DirectX::XMFLOAT3 dir;
	//	dir.x = sinf(angle.y);
	//	dir.y = 0.0f;
	//	dir.z = cosf(angle.y);
	//	//���ˈʒu(�v���C���[�̍�������)
	//	DirectX::XMFLOAT3 pos;
	//	pos.x = position.x;
	//	pos.y = position.y + height * 0.5f;
	//	pos.z = position.z;
	//	//����
	//	ProjectileStraight* projectile = new ProjectileStraight(&projectileManager);
	//	projectile->Launch(dir, pos);



	//}


	////�ǔ��e�۔���
	//if (gamepad.GetButtonDown() & GamePad::BTN_Y)
	//{
	//	//�O����
	//	DirectX::XMFLOAT3 dir;
	//	dir.x = sinf(angle.y);
	//	dir.y = 0.0f;
	//	dir.z = cosf(angle.y);

	//	//���ˈʒu
	//	DirectX::XMFLOAT3 pos;
	//	pos.x = position.x;
	//	pos.y = position.y + height * 0.5f;
	//	pos.z = position.z;

	//	//�^�[�Q�b�g
	//	DirectX::XMFLOAT3 target;
	//	target.x = pos.x + dir.x * 1000.0f;
	//	target.y = pos.y + dir.y * 1000.0f;
	//	target.z = pos.z + dir.z * 1000.0f;

	//	//��ԋ߂��̓G���^�[�Q�b�g�ɂ���
	//	float dist = FLT_MAX;
	//	EnemyManager& enemyManeger = EnemyManager::Instance();
	//	int enemyCount = enemyManeger.GetEnemyCount();
	//	for (int i = 0; i < enemyCount; ++i)
	//	{
	//		//�G�Ƃ̋�������
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

	//	//����
	//	ProjectileHoming* projectile = new ProjectileHoming(&projectileManager);
	//	projectile->Launch(dir, pos, target);



	//}



}

//�A�C�e���g�p����
void Player::UseItem() {

	GamePad& gamepad = Input::Instance().GetGamePad();
	//�A�C�e���I��
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


		//�A�C�e���g�p����
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

//�v���C���[�ƓG�̓����蔻��
void Player::CollisionPlayerVsEnemies() {

	EnemyManager& enemyManager = EnemyManager::Instance();

	//�S�Ă̓G�Ƒ�������ŏՓˏ���
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < enemyCount; i++) {
		Enemy* enemy = enemyManager.GetEnemy(i);
		
			//�Փˏ���
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
						//���W�����v����
						Jump(jumpSpeed * 0.5f);
					}
					else
					{
						//�����o����̈ʒu�ݒ�
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
						//���W�����v����
						Jump(jumpSpeed * 0.5f);
					}
					else
					{
						//�����o����̈ʒu�ݒ�
						SetPosition(outPosition);
						
					}
				}
			}
		
	}

}



//�e�ۂƓG�̏Փˏ���
void Player::CollisionProjectilesVsEnemies() {
	EnemyManager& enemyManager = EnemyManager::Instance();

//�S�Ă̒e�ۂƑS�Ă̓G��������ŏՓˏ���
	int projectileCount = projectileManager.GetProjectileCount();
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < projectileCount; ++i)
	{
		Projectile* projectile = projectileManager.GetProjectile(i);

		for (int j = 0; j < enemyCount; ++j) 
		{
			Enemy* enemy = enemyManager.GetEnemy(j);

			//�Փˏ���
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
					//�e�۔j��
					projectile->Destroy();
				}
			}
		}
	}
}


//�m�[�h�ƓG�̏Փˏ���
void Player::CollisionNodeVsEnemies(const char* nodeName, float nodeRadius,float elapsedTime)
{
	//�m�[�h�̎擾
	Model::Node* node = model->FindNode(nodeName);

	

	//�m�[�h�̈ʒu�擾
	DirectX::XMFLOAT3 nodePosition;
	nodePosition.x = node->worldTransform._41;
	nodePosition.y = node->worldTransform._42;
	nodePosition.z = node->worldTransform._43;


	//�w��̃m�[�h�ƑS�Ă̓G�𑍓�����ŏՓˏ���
	EnemyManager& enemyManager = EnemyManager::Instance();
	int enemyCount = enemyManager.GetEnemyCount();

	

	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);

		//�Փˏ���
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
		
	


//���n�����Ƃ��ɌĂ΂��
void Player::OnLanding()
{

	jumpCount = 0;

	//�������̑��͂����ȏ�Ȃ璅�n�X�e�[�g��
	if (velocity.y < gravity * 20.0f ) {

		if (state != State::Damage && state != State::Death) {
			if (velocity.y > -50) {
				//���n�X�e�[�g�֑J��
				
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

//�_���[�W���󂯂����ɌĂ΂��
void Player::OnDamaged() {

	//�_���[�W�X�e�[�g�֑J��
	TransitionDamageState();
	
}

//���S�������ɌĂ΂��
void Player::OnDead() {
	//���S�X�e�[�g�֑J��
	TransitionDeathState();
}

//����̈ړ�����
void Player::DodgeImpulse() {

	//���g�̉�]�l����O���������߂�
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

//�����x�����Z�b�g
void Player::ResetVelocity() {
	velocity = { 0,velocity.y,0 };
}

//�R�C���𑝂₷
bool Player::AddCoin(int Coin) {


	if (coin + Coin < 0)return false;
	coin += Coin;
	if (coin >= MAX_COIN) {
		coin = MAX_COIN;
	}
	return true;
}


//�ҋ@�X�e�[�g�֑J��
void Player::TransitionIdleState() {

	state = State::Idle;
	isGuard = false;
	CounterFlag = false;
	//�ҋ@�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_WTD_Stand, true,0.2f,2.4f,3.66f);
	WalkSE->Stop();
	RunSE->Stop();
}

//�ҋ@�X�e�[�g�X�V����
void Player::UpdateIdleState(float elapsedTime) {
	//�ړ����͏���

	if (InputMove(elapsedTime)) {
		TransitionMoveWalkState();
	}

	//�W�����v���͏���
	if (InputJump()) {
		TransitionJumpState();
	}

	//�K�[�h���͏���
	if (InputGuard()) {
		TransitionGuardState();
	}

	//�e�ۓ��͏���
	InputProjectile();

	//�U�����͏���
	if (InputAttack()) {
		TransitionAttackState();
	}
	//�����͏���
	if (InputDodge()) {
		TransitionDodgeState();
	}

}

//�ړ��X�e�[�g�֑J��
void Player::TransitionMoveWalkState() {
	state = State::MoveWalk;

	//�����A�j���[�V�����Đ�
	model->PlayAnimation(Anim_WTD_Walk_C_F, true);
	WalkSE->Play(true);
}

void Player::UpdateMoveWalkState(float elapsedTime) {

	if (abs(velocity.x) >= moveSpeed / 100 || abs(velocity.z) >= moveSpeed / 100) {
		//����A�j���[�V�����Đ�
		WalkSE->Stop();
		TransitionMoveRunState();
		
	}


	//�ړ����͏���
	if (!InputMove(elapsedTime)) {
		WalkSE->Stop();
		TransitionIdleState();
	
	}

	//�W�����v���͏���
	if (InputJump()) {
		WalkSE->Stop();
		TransitionJumpState();
	}

	//�e�ۓ��͏���
	InputProjectile();

	//�U�����͏���
	if (InputAttack()) {
		WalkSE->Stop();
		TransitionAttackState();
	}
	//�����͏���
	if (InputDodge()) {
		WalkSE->Stop();
		TransitionDodgeState();
	}

	//�K�[�h���͏���
	if (InputGuard()) {
		WalkSE->Stop();
		TransitionGuardState();
	}
}


//�ړ��X�e�[�g�֑J��
void Player::TransitionMoveRunState() {
	state = State::MoveRun;

	//����A�j���[�V�����Đ�
	model->PlayAnimation(Anim_WTD_Run, true,0.2f,0.5f,1.4f);
	RunSE->Play(true);
}

void Player::UpdateMoveRunState(float elapsedTime) {



	//�ړ����͏���
	if (!InputMove(elapsedTime)) {
		RunSE->Stop();
		TransitionMoveWalkState();

	}

	//�W�����v���͏���
	if (InputJump()) {
		RunSE->Stop();
		TransitionJumpState();
	}

	//�e�ۓ��͏���
	InputProjectile();

	//�U�����͏���
	if (InputAttack()) {
		RunSE->Stop();
		TransitionAttackState();
	}

	if (InputDodge()) {
		RunSE->Stop();
		TransitionDodgeState();
	}


	//�K�[�h���͏���
	if (InputGuard()) {
		RunSE->Stop();
		TransitionGuardState();
	}
}

//�W�����v�X�e�[�g�֑J��
void Player::TransitionJumpState() {
	state = State::Jump;
	
	//�W�����v�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_jump, false,0.2f,0.15f,0.6f);
}

//�W�����v�X�e�[�g�X�V����
void Player::UpdateJumpState(float elapsedTime) {
	//�ړ����͏���
	InputMove(elapsedTime);


	//�W�����v���͏���
	
	if (InputJump()) {
		model->PlayAnimation(Anim_jump, false,0.2f,0.6f,1.0f);
	}

	//�U�����͏���
	if (InputAttack()) {
		TransitionAttackState();
	}

	if (velocity.y >= jumpSpeed / 9)UpDownFlg = true;



	if(!model->IsPlayAnimation()) {
		if (!UpDownFlg) {
			//�㏸�A�j���[�V����
			model->PlayAnimation(Anim_jump, true, 0.2f, 0.6f, 1.0f);
		}
		else {
			//���~�A�j���[�V����
			model->PlayAnimation(Anim_jump, true, 0.2f, 1.0f, 2.5f);
		}
	}
	

	//�e�ۓ��͏���
	InputProjectile();


}

//���n�X�e�[�g�֑J��
void Player::TransitionLowLandState() {
	state = State::LowLand;
	UpDownFlg = false;
	//���n�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_jump, false, 0.2f, 2.5f,3.0f);
}

//���n�X�e�[�g�X�V����
void Player::UpdateLowLandState(float elapsedTime) {
	float animationTime = model->GetCurrentAnimationSeconds();
    LandingFlag = animationTime >= 0.0f && animationTime <= 0.3f;
	//�ړ����͏���
	if (LandingFlag) {
		InputMove(elapsedTime);
	}
	//���n����
	if (!model->IsPlayAnimation()) {
		TransitionIdleState();
	}
	
}

//���n�X�e�[�g�֑J��
void Player::TransitionHighLandState() {
	state = State::HighLand;
	UpDownFlg = false;
	//���n�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Roll_F, false, 0.2f, 2.2f, 3.1f);
	velocity = { 0,0,0 };
	DodgeSE->Play(false);
}

//���n�X�e�[�g�X�V����
void Player::UpdateHighLandState(float elapsedTime) {
	float animationTime = model->GetCurrentAnimationSeconds();
	LandingFlag = animationTime >= 0.0f && animationTime <= 0.3f;

	
	//�ړ����͏���
	if (animationTime >= 0.0f && animationTime <= 2.5f) {
		DodgeImpulse();
	}
	else if (animationTime >= 2.6f) {

		ResetVelocity();

		//�ړ����͏���

	

		//�U�����͏���
		if (InputAttack()) {
			TransitionAttackState();
		}
	}
	//���n����
	if (!model->IsPlayAnimation()) {

		if (InputMove(elapsedTime)) {
			TransitionMoveWalkState();
		}

		TransitionIdleState();
	}

}

//�U��1�X�e�[�g�֑J��
void Player::TransitionAttackState() {
	state = State::Attack;

	//�U���A�j���[�V�����Đ�
	model->PlayAnimation(Anim_AttackA, false,0.2f,0.3f,0.9f);

}

//�U��1�X�e�[�g�X�V����
void Player::UpdateAttackState(float elapsedTime) {
	
	//�m�[�h�̎擾
	Model::Node* nodeR = model->FindNode("Prop_R");

	//�m�[�h�̈ʒu�擾
	swordPosition[0].x = nodeR->worldTransform._41;
	swordPosition[0].y = nodeR->worldTransform._42;
	swordPosition[0].z = nodeR->worldTransform._43;



	Model::Node* nodeL = model->FindNode("Prop_L");

	//�m�[�h�̈ʒu�擾
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
			//�U���A�j���[�V�����Đ�
			model->PlayAnimation(Anim_AttackA, false, 0.2f, 0.9f, 1.2f);
			AttackState++;
		}
		//�U������
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
		//�U������
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
		//�U������
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

//�_���[�W�X�e�[�g�֑J��
void Player::TransitionDamageState() {
	state = State::Damage;

	//�_���[�W�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Damaged, false,0.2f,0.2f,1.3f);
}

//�_���[�W�X�e�[�g�X�V����
void Player::UpdateDamageState(float elapsedTime) {
	//�_���[�W�A�j���[�V�������I������ҋ@�X�e�[�g�֑J��
	
	if (!model->IsPlayAnimation()) {
		
		TransitionIdleState();
	}
}

//���S�X�e�[�g�֑J��
void Player::TransitionDeathState() {
	state = State::Death;
	isDead = true;
	//���S�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Die, false);
}

//���S�X�e�[�g�X�V����
void Player::UpdateDeathState(float elapsedTime) {
	if (!model->IsPlayAnimation()) {
	//�{�^�����������畜���X�e�[�g�֑J��
		GamePad& gamePad = Input::Instance().GetGamePad();
		if (gamePad.GetButtonDown() & GamePad::BTN_A) {
			TransitionReviveState();
		}
	}
}

//�����X�e�[�g�֑J��
void Player::TransitionReviveState() {
	state = State::Revive;
	//isDead = false;
	//���S�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Down_F, false,0.2f,2.0f,4.3f);
	health = maxHealth;
}

//�����X�e�[�g�X�V����
void Player::UpdateReviveState(float elapsedTime) {
	//�����A�j���[�V�������I������ҋ@�X�e�[�g�֑J��
	if (!model->IsPlayAnimation()) {
		TransitionIdleState();
	}

}


//�K�[�h�X�e�[�g�֑J��
void Player::TransitionGuardState() {
	state = State::Guard;
	model->PlayAnimation(Anim_Defend, false, 0.2f, 0.5f, 1.3f);
}

//�K�[�h�X�e�[�g�X�V����
void Player::UpdateGuardState(float elapsedTime) {
	invincibleTimer = 0.8f;

	if (InputAttack()) {
		invincibleTimer = 0.0f;
		TransitionAttackState();
	}

	//�K�[�h�A�j���[�V�������I������ҋ@�X�e�[�g�֑J��
	if (!model->IsPlayAnimation()) {
	
		TransitionIdleState();
	}

}

//����X�e�[�g�֑J��
void Player::TransitionDodgeState() {
	state = State::Dodge;
	model->PlayAnimation(Anim_Roll_F, false,0.2f,2.0f,3.1f);

}

//����X�e�[�g�X�V����
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



//�f�o�b�O�p
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
	//�U���Փ˗p�̍���m�[�h�̃f�o�b�O����`��
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

//�f�o�b�O�p
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