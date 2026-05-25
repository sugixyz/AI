#include "Enemy.h"
#include"Player.h"
#include "time.h"

namespace
{
	const int ENEMY_SIZE = 48; //敵のサイズ 32*32
	const Point ENEMY_START_POS = { 20 * ENEMY_SIZE, 10 * ENEMY_SIZE }; //敵の初期位置
	const DIR INIT_ENEMY_DIR = { LEFT };
	const int ENEMY_DRAW_SIZE = 32; //敵の描画サイズ
	const int animFrame[4]{ 0, 1, 2, 1 };
	const float ANIM_INTERVAL = 0.2f;

	const int DIST = 15;
}


Enemy::Enemy()
	: GameObject() 
{
	hImage_ = LoadGraph("Assets/panda_R.png");
	pos_ = ENEMY_START_POS; //32はブロックの位置pos_
	dir_ = INIT_ENEMY_DIR;
}

Enemy::~Enemy()
{
}

void Enemy::Update()
{
	static float prog_timer = 0.5f;
	float dt = Time::DeltaTime();
	prog_timer = prog_timer - dt;

	if (prog_timer < 0.0f)
	{
		Move();
		prog_timer = 0.5f + prog_timer;
	}

}

void Enemy::Draw()
{
	static float animTimer = ANIM_INTERVAL;
	static int frame = 0;
	int nowFrame = animFrame[frame];

	Rect iRect[4] = {
		{  nowFrame * ENEMY_SIZE, 3 * ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE},
		{  nowFrame * ENEMY_SIZE, 0 * ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE},
		{  nowFrame * ENEMY_SIZE, 1 * ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE},
		{  nowFrame * ENEMY_SIZE, 2 * ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE}
	};
	DrawBox(pos_.x, pos_.y, pos_.x + ENEMY_DRAW_SIZE, pos_.y + ENEMY_DRAW_SIZE,
		GetColor(255, 255, 0), FALSE,2);
	DrawRectExtendGraph(pos_.x, pos_.y,pos_.x + ENEMY_DRAW_SIZE, pos_.y + ENEMY_DRAW_SIZE,
		               iRect[dir_].x, iRect[dir_].y, iRect[dir_].w, iRect[dir_].h, hImage_, TRUE);
	if (animTimer < 0) {
		frame = (++frame) % 4;
		animTimer = ANIM_INTERVAL + animTimer;
	}
	animTimer = animTimer - Time::DeltaTime();
}

bool Enemy::CheckLenght()
{
	Player* p = FindGameObject<Player>();
	Point pPos = p->GetPlayerPos();

	//マンハッタン距離で測定
	dist = { (pPos.x - pos_.x) / CHA_SIZE,(pPos.y - pos_.y) / CHA_SIZE };
	int distBlock = abs(dist.x) + abs(dist.y);
	if (distBlock <= DIST)
	{
		return true;
	}
	return false;
}

void Enemy::Chase()
{
	if (abs(dist.x) > abs(dist.y))
	{
		if (dist.x < 0)dir_ = LEFT;
		else if (dist.x > 0)dir_ = RIGHT;
	}
	else if (abs(dist.x) < abs(dist.y))
	{
		if (dist.y < 0)dir_ = UP;
		else if (dist.y > 0)dir_ = DOWN;
	}
}

void Enemy::Move()
{
	Point newPos = pos_;

	isChase = CheckLenght();
	if (isChase)Chase();
	switch (dir_)
	{
	case UP:
		newPos.y -= ENEMY_DRAW_SIZE;
		break;
	case DOWN:
		newPos.y += ENEMY_DRAW_SIZE;
		break;
	case LEFT:
		newPos.x -= ENEMY_DRAW_SIZE;
		break;
	case RIGHT:
		newPos.x += ENEMY_DRAW_SIZE;
		break;
	default:
		break;
	}
	//移動先がステージの外に出ないようにする
	if (stageData[newPos.y / CHA_SIZE][newPos.x / CHA_SIZE] != 1)
	{
		pos_ = newPos;
		//CSVに登録
		stageData[newPos.y / CHA_SIZE][newPos.x / CHA_SIZE] = 3;
	}
	else
	{
		switch (dir_)
		{
		case UP:
			dir_ = RIGHT;
			break;
		case DOWN:
			dir_ = LEFT;
			break;
		case LEFT:
			dir_ = UP;
			break;
		case RIGHT:
			dir_ = DOWN;
			break;
		default:
			break;
		}
	}
}
