#include "Phase.h"

Phase::Phase()
{
	this->Pattern = new PatternBobby();
	this->angspeed = 0.0f;
	this->radius = 0.0f;
	this->vspeed = 0.0f;
	this->rotation_speed = 0.0f;
	this->hasTransition = false;
	this->hasWakeUp = false;
	this->hasWelcomeShot = false;
	this->welcomeWeapon = NULL;
}

Phase::~Phase()
{
	
}

Phase* Phase::Clone()
{
	Phase* new_phase = new Phase();

	*new_phase = *this;

	for (std::vector<Weapon*>::iterator it = (this->weapons_list.begin()); it != (this->weapons_list.end()); it++)
	{
		new_phase->weapons_list.push_back((*it)->Clone());
	}

	new_phase->Pattern = new PatternBobby();
	*(new_phase->Pattern) = *(this->Pattern);
	vector<ConditionTransition*> transitions_list;
	for (std::vector<ConditionTransition*>::iterator it = (this->transitions_list.begin()); it != (this->transitions_list.end()); it++)
	{
		new_phase->transitions_list.push_back(new ConditionTransition((*it)->condition, (*it)->op, (*it)->value, (*it)->nextPhase_name));
	}
	
	if(this->welcomeWeapon != NULL)
		new_phase->welcomeWeapon = this->welcomeWeapon->Clone();

	return new_phase;
}

ConditionTransition::ConditionTransition(ConditionType m_condition, FloatCompare m_op, float m_value, std::string m_nextPhase_name)
{
	this->condition = m_condition;
	this->op = m_op;
	this->value = m_value;
	this->nextPhase_name = m_nextPhase_name;
}

ConditionTransition* Phase::ConditionLoader(vector<string> line_data, int index)
{
	//loading condition type
	ConditionType cond = ConditionType::NoCondition;
	if (line_data[index].compare("positionV") == 0)
	{
		cond = ConditionType::VerticalPosition;
	}
	else if (line_data[index].compare("positionH") == 0)
	{
		cond = ConditionType::HorizontalPosition;
	}
	else if (line_data[index].compare("phaseClock") == 0)
	{
		cond = ConditionType::phaseClock;
	}
	else if (line_data[index].compare("enemyClock") == 0)
	{
		cond = ConditionType::enemyClock;
	}
	else if (line_data[index].compare("life") == 0)
	{
		cond = ConditionType::LifePourcentage;
	}
	else if (line_data[index].compare("shield") == 0)
	{
		cond = ConditionType::ShieldPourcentage;
	}
	else if (line_data[index].compare("wakeUp") == 0)
	{
		cond = ConditionType::wakeUp;
	}
	else if (line_data[index].compare("proximity") == 0)
	{
		cond = ConditionType::EnemyProximity;
	}
	else if (line_data[index].compare("shotsFired") == 0)
	{
		cond = ConditionType::ShotsFired;
	}

	//loading operator type
	FloatCompare op = FloatCompare::ERROR_COMPARE;
	if (cond != ConditionType::wakeUp)
	{
		if (line_data[index + CONDITION_OPERATOR].compare("greater") == 0)
		{
			op = FloatCompare::GREATHER_THAN;
		}
		else if (line_data[index + CONDITION_OPERATOR].compare("lesser") == 0)
		{
			op = FloatCompare::LESSER_THAN;
		}
		else if (line_data[index + CONDITION_OPERATOR].compare("equal") == 0)
		{
			op = FloatCompare::EQUAL_TO;
		}
		else
		{
			LOGGER_WRITE(Logger::Priority::DEBUG, TextUtils::format("ERROR: Invalid operator found when loading condition of transition 1 of enemy phase named 'EnemyPhases.csv'. Please check config file"));
		}
	}

	float value = 0;
	if (cond != ConditionType::wakeUp)
	{
		value = stoi(line_data[index + CONDITION_VALUE]);
	}
	//and finally wrapping up all of this data in our list of transition conditions
	ConditionTransition* condition = new ConditionTransition(cond, op, value, line_data[index + CONDITION_TRANSITION]);

	return condition;
}