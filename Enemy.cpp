#include "Enemy.h"
#include"Player.h"
#include "time.h"
#include<cmath>

namespace
{
	const int ENEMY_SIZE = 48; //敵のサイズ 32*32
	const Point ENEMY_START_POS = { 20 * ENEMY_SIZE, 10 * ENEMY_SIZE }; //敵の初期位置
	const DIR INIT_ENEMY_DIR = { LEFT };
	const int ENEMY_DRAW_SIZE = 32; //敵の描画サイズ
	const int animFrame[4]{ 0, 1, 2, 1 };
	const float ANIM_INTERVAL = 0.2f;
	const float DegToRad = DX_PI_F / 180;

	const float CHASE_LENGHT = 48 * 8;
	const float ATTACK_LENGHT = 48 * 1;
}


Enemy::Enemy()
	: GameObject() 
{
	hImage_ = LoadGraph("Assets/panda_R.png");
	pos_ = ENEMY_START_POS; //32はブロックの位置pos_
	dir_ = INIT_ENEMY_DIR;
	target = FindGameObject<Player>();

	currentState = new PatrolState(this);
	nextState = nullptr;
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
		currentState->Update();
		prog_timer = 0.5f + prog_timer;
	}

	if (nextState == nullptr)return;
	currentState = nextState;
	nextState = nullptr;
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

void Enemy::ChangeState(StateBase* state)
{
	delete nextState;
	nextState = state;
}

void Enemy::Move()
{
	Point newPos = pos_;

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

PatrolState::PatrolState(Enemy* enemy)
{
	en = enemy;
}

PatrolState::~PatrolState()
{
}

void PatrolState::Update()
{

	en->Move();

	Player* p = en->GetTarget();
	Point pPos = p->GetPlayerPos();
	Pointf toPlayer = { pPos.x - en->pos_.x,pPos.y - en->pos_.y };
	float dist = toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y;
	if (dist > CHASE_LENGHT * CHASE_LENGHT)return;

	float dot = pPos.x * en->pos_.x + pPos.y * en->pos_.y;
	if (dot >= cos(60 * DegToRad))
	{
		en->ChangeState(new ChaseState(en));
	}
}

ChaseState::ChaseState(Enemy* enemy)
{
	en = enemy;
}

ChaseState::~ChaseState()
{
}

void ChaseState::Update()
{
	Player* p = en->GetTarget();
	Point pPos = p->GetPlayerPos();
	Pointf toPlayer = { pPos.x - en->pos_.x,pPos.y - en->pos_.y };
	float dist = toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y;
	if (dist > CHASE_LENGHT * CHASE_LENGHT)
	{
		en->ChangeState(new SearchState(en,pPos));
		return;
	}
	if (dist <= ATTACK_LENGHT * ATTACK_LENGHT)
	{
		en->ChangeState(new AttackState(en));
		return;
	}
	if (abs(toPlayer.x) > abs(toPlayer.y))
	{
		if (toPlayer.x < 0)en->dir_ = LEFT;
		else if (toPlayer.x > 0)en->dir_ = RIGHT;
	}
	else if (abs(toPlayer.x) < abs(toPlayer.y))
	{
		if (toPlayer.y < 0)en->dir_ = UP;
		else if (toPlayer.y > 0)en->dir_ = DOWN;
	}

	en->Move();
}

AttackState::AttackState(Enemy* enemy)
{
	en = enemy;
}

AttackState::~AttackState()
{
}

void AttackState::Update()
{
}

SearchState::SearchState(Enemy* enemy,Point plPos)
{
	en = enemy;
	targetPos = plPos;
	state = State::SEARCH;
}

SearchState::~SearchState()
{
}

void SearchState::Update()
{
	if (state == State::SEARCH)
	{
		Pointf toTarget = { targetPos.x - en->pos_.x,targetPos.y - en->pos_.y };
		float dist = toTarget.x * toTarget.x + toTarget.y * toTarget.y;
		if (abs(toTarget.x) > abs(toTarget.y))
		{
			if (toTarget.x < 0)en->dir_ = LEFT;
			else if (toTarget.x > 0)en->dir_ = RIGHT;
		}
		else if (abs(toTarget.x) < abs(toTarget.y))
		{
			if (toTarget.y < 0)en->dir_ = UP;
			else if (toTarget.y > 0)en->dir_ = DOWN;
		}

		en->Move();

		if (en->pos_.x == targetPos.x && en->pos_.y == targetPos.y)
		{
			state = State::GO;
		}
	}
	else if (state == State::GO)
	{
		static float timer = 0.0f;
		timer += Time::DeltaTime();
		if (timer >= 1.0f)
		{
			en->ChangeState(new PatrolState(en));
			timer = 0.0f;
		}
	}
}