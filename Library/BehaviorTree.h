#pragma once
#include"Node.h"
#include<vector>

template<typename T>
class ActionNode : public Node
{
public:
	using ActionFunc = NodeResult(T::*)();
public:
	ActionNode(T* inst, ActionFunc func)
		:instance(inst),action(func)
	{}
	~ActionNode()
	{}
	NodeResult Tick() override
	{
		if (instance && action)
		{
			return (instance->*action)();
		}
		return NodeResult::FAILURE;
	}
private:
	T* instance;
	ActionFunc action;
};

template<typename T>
class ConditionNode : public Node
{
public:
	using ConditionFunc = bool(T::*)();
public:
	ConditionNode(T* inst, ConditionFunc func)
		:instance(inst), condition(func)
	{}
	~ConditionNode()
	{}
	NodeResult Tick() override
	{
		if (instance && condition)
		{
			bool result = (instance->*condition)();
			if (result)return NodeResult::SUCCESS;
			else return NodeResult::FAILURE;
		}
		return NodeResult::FAILURE;
	}
private:
	T* instance;
	ConditionFunc condition;
};

class Sequence : public Node
{
public:
	Sequence();
	~Sequence();
	NodeResult Tick() override;
	void AddChildren(Node* child);
private:
	std::vector<Node*> children;
};

class Selector : public Node
{
public:
	Selector();
	~Selector();
	NodeResult Tick() override;
	void AddChildren(Node* child);
private:
	std::vector<Node*> children;
};