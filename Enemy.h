#pragma once
#include ".\Library\GameObject.h"
#include "global.h"
#include"Library/BehaviorTree.h"

class Player;

enum class StateType
{
	PATROL,
	CHASE,
	ATTACK,
	SEARCH
};

class Enemy : public GameObject
{
public:
	Enemy();
	~Enemy();
	void Update() override;
	void Draw() override;
	Player* GetTarget() { return target; }
	Pointf VNormal(Pointf a);
	Pointf GetDir();
	void Move();
public:
	NodeResult Attack();
	bool CanAttack();
	NodeResult Chase();
	bool CanChase();;
	NodeResult Search();
	bool CanSearch();
	NodeResult Patrol();
private:
	int hImage_;//画像ID
	Point pos_;//位置
	DIR dir_;//移動方向
	Player* target;

	StateType currentStateType;

	Selector root;
	//索敵中かどうか
	bool isSearching;
	//索敵の経過時間
	float searchTimer;
	//目的地に到着しているかどうか
	bool isAtDestination;
	//目的地
	Point destination;
	//プレイヤーとの距離
	float distanceToPlayer;
private:
	void DrawStateType();
	float CalculateDistance(Pointf vec);
	Pointf CalculateToPlayerVec();
	void Initialize();
	void SetBehavior();
};