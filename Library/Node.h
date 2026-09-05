#pragma once

enum class NodeResult
{
	SUCCESS,
	FAILURE,
	RUNNING
};

class Node
{
public:
	virtual ~Node() = default;
	virtual NodeResult Tick() = 0;
};