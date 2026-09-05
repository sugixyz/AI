#include "BehaviorTree.h"

Sequence::Sequence()
{}

Sequence::~Sequence()
{
    for (auto child : children)
    {
        delete child;
    }
    children.clear();
}

NodeResult Sequence::Tick()
{
    for (auto child : children)
    {
        NodeResult result = child->Tick();

        if (result == NodeResult::FAILURE)return result;
        if (result == NodeResult::RUNNING)return result;
    }
    return NodeResult::SUCCESS;
}

void Sequence::AddChildren(Node* child)
{
    children.push_back(child);
}

Selector::Selector()
{}

Selector::~Selector()
{
    for (auto child : children)
    {
        delete child;
    }
    children.clear();
}

NodeResult Selector::Tick()
{
    for (auto child : children)
    {
        NodeResult result = child->Tick();

        if (result == NodeResult::SUCCESS)return result;
        if (result == NodeResult::RUNNING)return result;
    }
    return NodeResult::FAILURE;
}

void Selector::AddChildren(Node* child)
{
    children.push_back(child);
}