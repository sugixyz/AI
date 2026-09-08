#include "Enemy.h"
#include"Player.h"
#include "time.h"
#include<cmath>

namespace
{
	const float EXECUTION_INTERVAL = 0.5f;

	const int ENEMY_SIZE = 48; //敵のサイズ
	const Point ENEMY_START_POS = { 20 * ENEMY_SIZE, 10 * ENEMY_SIZE }; //敵の初期位置
	const DIR INIT_ENEMY_DIR = { LEFT };
	const int ENEMY_DRAW_SIZE = 32; //敵の描画サイズ
	const int animFrame[4]{ 0, 1, 2, 1 };
	const float ANIM_INTERVAL = 0.2f;

	const float DegToRad = DX_PI_F / 180;

	const float CHASE_LENGHT = ENEMY_SIZE * 6;
	const float ATTACK_LENGHT = ENEMY_SIZE * 1;

	const float MAX_SEARCH_TIME = 1.0f;
}


Enemy::Enemy()
	: GameObject() 
{
	Initialize();
	SetBehavior();
}

Enemy::~Enemy()
{
}

void Enemy::Update()
{
	static float prog_timer = EXECUTION_INTERVAL;
	float dt = Time::DeltaTime();
	prog_timer = prog_timer - dt;

	if (prog_timer < 0.0f)
	{
		distanceToPlayer = CalculateDistance(CalculateToPlayerVec());
		root.Tick();
		prog_timer = EXECUTION_INTERVAL + prog_timer;
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

	DrawStateType();
}

Pointf Enemy::VNormal(Pointf a)
{
	float l = sqrtf(a.x * a.x + a.y * a.y);
	if (l < 0.0001f)return{ 0.0f,0.0f };
	Pointf ret = { a.x / l,a.y / l };
	return ret;
}

Pointf Enemy::GetDir()
{
	switch (dir_)
	{
	case UP:
		return { 0.0f,-1.0f };
		break;
	case DOWN:
		return{ 0.0f,1.0f };
		break;
	case RIGHT:
		return { 1.0f,0.0f };
		break;
	case LEFT:
		return { -1.0f,0.0f };
		break;
	}
	return Pointf();
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
		//stageData[newPos.y / CHA_SIZE][newPos.x / CHA_SIZE] = 3;
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

NodeResult Enemy::Attack()
{
	currentStateType = StateType::ATTACK;

	return NodeResult::SUCCESS;
}

bool Enemy::CanAttack()
{
	if (distanceToPlayer <= ATTACK_LENGHT * ATTACK_LENGHT)
	{
		return true;
	}
	return false;
}

NodeResult Enemy::Chase()
{
	currentStateType = StateType::CHASE;

	Pointf toPlayer = CalculateToPlayerVec();
	if (abs(toPlayer.x) > abs(toPlayer.y))
	{
		if (toPlayer.x < 0)dir_ = LEFT;
		else if (toPlayer.x > 0)dir_ = RIGHT;
	}
	else if (abs(toPlayer.x) < abs(toPlayer.y))
	{
		if (toPlayer.y < 0)dir_ = UP;
		else if (toPlayer.y > 0)dir_ = DOWN;
	}

	Move();
	searchTimer = 0.0f;
	isSearching = true;
	isAtDestination = false;
	destination = target->GetPlayerPos();
	return NodeResult::SUCCESS;
}

bool Enemy::CanChase()
{
	if (distanceToPlayer > CHASE_LENGHT * CHASE_LENGHT)return false;

	Pointf toPlayer = CalculateToPlayerVec();
	Pointf pNormal = VNormal(toPlayer);
	Pointf eNormal = VNormal(GetDir());
	float dot = pNormal.x * eNormal.x + pNormal.y * eNormal.y;
	if (dot >= cos(60 * DegToRad))
	{
		return true;
	}
	return false;
}

NodeResult Enemy::Search()
{
	currentStateType = StateType::SEARCH;

	if (!isAtDestination)
	{
		Pointf toDestination = { destination.x - pos_.x,destination.y - pos_.y };
		if (abs(toDestination.x) > abs(toDestination.y))
		{
			if (toDestination.x < 0)dir_ = LEFT;
			else if (toDestination.x > 0)dir_ = RIGHT;
		}
		else if (abs(toDestination.x) < abs(toDestination.y))
		{
			if (toDestination.y < 0)dir_ = UP;
			else if (toDestination.y > 0)dir_ = DOWN;
		}

		Move();
		if (destination.x == pos_.x && destination.y == pos_.y)isAtDestination = true;
		return NodeResult::SUCCESS;
	}

	searchTimer += EXECUTION_INTERVAL;
	if (searchTimer >= MAX_SEARCH_TIME)isSearching = false;
	return NodeResult::SUCCESS;

}

bool Enemy::CanSearch()
{
	if (isSearching)
	{
		return true;
	}
	return false;
}

NodeResult Enemy::Patrol()
{
	Move();
	currentStateType = StateType::PATROL;
	return NodeResult::SUCCESS;
}

void Enemy::DrawStateType()
{
	switch (currentStateType)
	{
	case StateType::PATROL:
		DrawString(0, 0, "PATROL", GetColor(255, 255, 255));
		break;
	case StateType::CHASE:
		DrawString(0, 0, "CHASE", GetColor(255, 255, 255));
		break;
	case StateType::ATTACK:
		DrawString(0, 0, "ATTACK", GetColor(255, 255, 255));
		break;
	case StateType::SEARCH:
		DrawString(0, 0, "SEARCH", GetColor(255, 255, 255));
		break;
	}
}

float Enemy::CalculateDistance(Pointf vec)
{
	float dist = vec.x * vec.x + vec.y * vec.y;
	return dist;
}

Pointf Enemy::CalculateToPlayerVec()
{
	Point pPos = target->GetPlayerPos();
	Pointf toPlayer = { pPos.x - pos_.x,pPos.y - pos_.y };
	return toPlayer;
}

void Enemy::Initialize()
{
	hImage_ = LoadGraph("Assets/panda_R.png");
	pos_ = ENEMY_START_POS; //32はブロックの位置pos_
	dir_ = INIT_ENEMY_DIR;
	target = FindGameObject<Player>();
	currentStateType = StateType::PATROL;

	isSearching = false;
	searchTimer = 0.0f;
	isAtDestination = false;
	destination = { 0,0 };
	distanceToPlayer = 0.0f;
}

void Enemy::SetBehavior()
{
	auto attackNode = new ActionNode<Enemy>(this, &Enemy::Attack);
	auto attackCheckNode = new ConditionNode<Enemy>(this, &Enemy::CanAttack);
	Sequence* attackSequence = new Sequence();
	attackSequence->AddChildren(attackCheckNode);
	attackSequence->AddChildren(attackNode);

	auto chaseNode = new ActionNode<Enemy>(this, &Enemy::Chase);
	auto chaseCheckNode = new ConditionNode<Enemy>(this, &Enemy::CanChase);
	Sequence* chaseSequence = new Sequence();
	chaseSequence->AddChildren(chaseCheckNode);
	chaseSequence->AddChildren(chaseNode);

	auto searchNode = new ActionNode<Enemy>(this, &Enemy::Search);
	auto searchCheckNode = new ConditionNode<Enemy>(this, &Enemy::CanSearch);
	Sequence* searchSequence = new Sequence();
	searchSequence->AddChildren(searchCheckNode);
	searchSequence->AddChildren(searchNode);

	auto patrolNode = new ActionNode<Enemy>(this, &Enemy::Patrol);

	root.AddChildren(attackSequence);
	root.AddChildren(chaseSequence);
	root.AddChildren(searchSequence);
	root.AddChildren(patrolNode);
}