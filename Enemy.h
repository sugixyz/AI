#pragma once
#include ".\Library\GameObject.h"
#include "global.h"

class Enemy;
class Player;

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
	enum State
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
	void ChangeState(StateBase* state);
	Player* GetTarget() { return target; }
	void Move();
private:
	StateBase* currentState;
	StateBase* nextState;
	int hImage_;//‰æ‘œID
	Point pos_;//ˆÊ’u
	DIR dir_;//ˆÚ“®•ûŒü
	Player* target;
};

