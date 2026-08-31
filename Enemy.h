#pragma once
#include ".\Library\GameObject.h"
#include "global.h"

class Enemy;
class Player;

enum class StateType
{
	PATROL,
	CHASE,
	ATTACK,
	SEARCH
};

class StateBase
{
public:
	virtual ~StateBase() = default;
	virtual void Update() = 0;
protected:
	Enemy* en;
};

class PatrolState : public StateBase
{
public:
	PatrolState(Enemy* enemy);
	~PatrolState() override;
	void Update() override;
};

class ChaseState : public StateBase
{
public:
	ChaseState(Enemy* enemy);
	~ChaseState() override;
	void Update() override;
};

class AttackState : public StateBase
{
public:
	AttackState(Enemy* enemy);
	~AttackState() override;
	void Update() override;
};

class SearchState : public StateBase
{
public:
	SearchState(Enemy* enemy, Point plPos);
	~SearchState() override;
	void Update() override;
private:
	Point targetPos;
	enum class State
	{
		SEARCH,
		GO
	};
	State state;
};


class Enemy : public GameObject
{
	friend class PatrolState;
	friend class ChaseState;
	friend class AttackState;
	friend class SearchState;
public:
	Enemy();
	~Enemy();
	void Update() override;
	void Draw() override;
	void ChangeState(StateType stateType);
	Player* GetTarget() { return target; }
	Pointf VNormal(Pointf a);
	Pointf GetDir();
	void Move();
private:
	StateBase* currentState;
	StateBase* nextState;
	int hImage_;//‰æ‘œID
	Point pos_;//ˆÊ’u
	DIR dir_;//ˆÚ“®•ûŒü
	Player* target;

	StateType currentStateType;
private:
	void DrawStateType();
};

