#include "Phase.h"

Phase::Phase()
{
	m_welcomeWeapon = NULL;
	m_freeze_player = false;
	m_pattern = new GeometryPattern();
}

Phase::~Phase()
{
	for (Weapon* weapon : m_weapons_list)
		delete weapon;

	delete m_welcomeWeapon;

	for (Dialog* dialog : m_dialogs)
		delete dialog;

	for (ConditionTransition* condition : m_transitions_list)
		delete condition;

	delete m_pattern;
}

Phase* Phase::Clone()
{
	Phase* phase = new Phase();

	phase->m_display_name = m_display_name;
	phase->m_name = m_name;

	for (Weapon* weapon : m_weapons_list)
		phase->m_weapons_list.push_back(weapon->Clone());

	phase->m_pattern = new GeometryPattern(*m_pattern);

	for (Modifier modifier : m_modifiers)
		phase->m_modifiers.push_back(modifier);

	phase->m_angspeed = m_angspeed;
	phase->m_radius = m_radius;
	phase->m_vspeed = m_vspeed;
	phase->m_rotation_speed = m_rotation_speed;
	phase->m_freeze_player = m_freeze_player;

	for (ConditionTransition* condition : m_transitions_list)
		phase->m_transitions_list.push_back(condition);

	phase->m_wake_up_name = m_wake_up_name;

	if (phase->m_welcomeWeapon != NULL)
		phase->m_welcomeWeapon = m_welcomeWeapon->Clone();
	
	for (Dialog* dialog : m_dialogs)
		phase->m_dialogs.push_back(dialog);

	return phase;
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
		cond = VerticalPosition;
	else if (line_data[index].compare("positionH") == 0)
		cond = HorizontalPosition;
	else if (line_data[index].compare("phaseClock") == 0)
		cond = phaseClock;
	else if (line_data[index].compare("enemyClock") == 0)
		cond = enemyClock;
	else if (line_data[index].compare("life") == 0)
		cond = LifePourcentage;
	else if (line_data[index].compare("shield") == 0)
		cond = ShieldPourcentage;
	else if (line_data[index].compare("wakeUp") == 0)
		cond = wakeUp;
	else if (line_data[index].compare("proximity") == 0)
		cond = EnemyProximity;
	else if (line_data[index].compare("shotsFired") == 0)
		cond = ShotsFired;
	else if (line_data[index].compare("playerPosX") == 0)
		cond = PlayerHorizontalPosition;
	else if (line_data[index].compare("playerPosY") == 0)
		cond = PlayerVerticalPosition;
	else if (line_data[index].compare("rotation") == 0)
		cond = Rotation;
	else
		LOGGER_WRITE(Logger::DEBUG, TextUtils::format("ERROR: Invalid condition found when loading condition of transition of enemy phase named 'EnemyPhases.csv'. Please check config file"));


	//loading operator type
	FloatCompare op = ERROR_COMPARE;
	if (cond != wakeUp)
		if (line_data[index + CONDITION_OPERATOR].compare("greater") == 0)
			op = GREATER_THAN;
		else if (line_data[index + CONDITION_OPERATOR].compare("lesser") == 0)
			op = LESSER_THAN;
		else if (line_data[index + CONDITION_OPERATOR].compare("equal") == 0)
			op = EQUAL_TO;
		else
			LOGGER_WRITE(Logger::DEBUG, TextUtils::format("ERROR: Invalid operator found when loading condition of transition of enemy phase named 'EnemyPhases.csv'. Please check config file"));

	float value = 0;
	if (cond != wakeUp)
		value = stoi(line_data[index + CONDITION_VALUE]);

	//and finally wrapping up all of this data in our list of transition conditions
	ConditionTransition* condition = new ConditionTransition(cond, op, value, line_data[index + CONDITION_TRANSITION]);

	return condition;
}