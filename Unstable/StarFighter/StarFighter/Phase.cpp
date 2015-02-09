#include "Phase.h"

Phase::Phase()
{
	this->hasTransition = false;
}

ConditionTransition::ConditionTransition(ConditionType m_condition, FloatCompare m_op, float m_value, std::string m_nextPhase_name)
{
	this->condition = m_condition;
	this->op = m_op;
	this->value = m_value;
	this->nextPhase_name = m_nextPhase_name;
}