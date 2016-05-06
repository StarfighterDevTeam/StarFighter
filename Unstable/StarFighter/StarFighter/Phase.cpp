#include "Phase.h"

Phase::Phase()
{
	m_welcomeWeapon = NULL;
	m_freeze_player = false;
	m_Pattern = new PatternBobby();
}

Phase::~Phase()
{
	for (std::vector<Weapon*>::iterator it = m_weapons_list.begin(); it != m_weapons_list.end(); it++)
	{
		delete (*it);
	}
	m_weapons_list.clear();

	delete m_welcomeWeapon;

	for (std::vector<Dialog*>::iterator it = m_dialogs.begin(); it != m_dialogs.end(); it++)
	{
		delete (*it);
	}
	m_dialogs.clear();

	for (std::vector<ConditionTransition*>::iterator it = m_transitions_list.begin(); it != m_transitions_list.end(); it++)
	{
		delete (*it);
	}
	m_transitions_list.clear();

	delete m_Pattern;
}

ConditionTransition::ConditionTransition(ConditionType condition, FloatCompare op, float value, std::string nextPhase_name)
{
	m_condition = condition;
	m_op = op;
	m_value = value;
	m_nextPhase_name = nextPhase_name;
}

ConditionTransition* Phase::ConditionLoader(vector<string> line_data, int index)
{
	//loading condition type
	ConditionType cond = NoCondition;
	if (line_data[index].compare("positionV") == 0)
	{
		cond = VerticalPosition;
	}
	else if (line_data[index].compare("positionH") == 0)
	{
		cond = HorizontalPosition;
	}
	else if (line_data[index].compare("phaseClock") == 0)
	{
		cond = phaseClock;
	}
	else if (line_data[index].compare("enemyClock") == 0)
	{
		cond = enemyClock;
	}
	else if (line_data[index].compare("life") == 0)
	{
		cond = LifePourcentage;
	}
	else if (line_data[index].compare("shield") == 0)
	{
		cond = ShieldPourcentage;
	}
	else if (line_data[index].compare("wakeUp") == 0)
	{
		cond = wakeUp;
	}
	else if (line_data[index].compare("proximity") == 0)
	{
		cond = EnemyProximity;
	}
	else if (line_data[index].compare("shotsFired") == 0)
	{
		cond = ShotsFired;
	}
	else if (line_data[index].compare("playerPosX") == 0)
	{
		cond = PlayerHorizontalPosition;
	}
	else if (line_data[index].compare("playerPosY") == 0)
	{
		cond = PlayerVerticalPosition;
	}

	//loading operator type
	FloatCompare op = ERROR_COMPARE;
	if (cond != wakeUp)
	{
		if (line_data[index + CONDITION_OPERATOR].compare("greater") == 0)
		{
			op = GREATER_THAN;
		}
		else if (line_data[index + CONDITION_OPERATOR].compare("lesser") == 0)
		{
			op = LESSER_THAN;
		}
		else if (line_data[index + CONDITION_OPERATOR].compare("equal") == 0)
		{
			op = EQUAL_TO;
		}
		else
		{
			LOGGER_WRITE(Logger::DEBUG, TextUtils::format("ERROR: Invalid operator found when loading condition of transition 1 of enemy phase named 'EnemyPhases.csv'. Please check config file"));
		}
	}

	float value = 0;
	if (cond != wakeUp)
	{
		value = stoi(line_data[index + CONDITION_VALUE]);
	}
	//and finally wrapping up all of this data in our list of transition conditions
	ConditionTransition* condition = new ConditionTransition(cond, op, value, line_data[index + CONDITION_TRANSITION]);

	return condition;
}