#ifndef PHASE_H_INCLUDED
#define PHASE_H_INCLUDED

#include "Independant.h"
#include "Weapon.h"
#include "TextUtils.h"

enum ConditionData
{
	CONDITION_TYPE,//0
	CONDITION_OPERATOR,//1
	CONDITION_VALUE,//2
	CONDITION_TRANSITION,//3
};

enum EnemyPhaseData
{
	PHASE_NAME,//0
	PHASE_VSPEED,//1
	PHASE_WEAPON,//2
	PHASE_AMMO,//3
	PHASE_WEAPON_2,//4
	PHASE_AMMO_2,//5
	PHASE_WEAPON_3,//6
	PHASE_AMMO_3,//7
	PHASE_PATTERN,//8
	PHASE_ANGSPEED,//9
	PHASE_RADIUS,//10
	PHASE_CLOCKWISE,//11
	PHASE_MODIFIER,//12
	PHASE_WAKEUP,//13
	PHASE_WELCOME_WEAPON,//14
	PHASE_WELCOME_AMMO,//15
	PHASE_CONDITION,//16
	PHASE_OPERATOR,//17
	PHASE_VALUE,//18
	PHASE_TRANSITION,//19
	PHASE_CONDITION_2,//20
	PHASE_OPERATOR_2,//21
	PHASE_VALUE_2,//22
	PHASE_TRANSITION_2,//23
};

enum ConditionType
{
	NoCondition,
	VerticalPosition,
	HorizontalPosition,
	phaseClock,
	enemyClock,
	LifePourcentage,
	ShieldPourcentage,
	wakeUp,
	EnemyProximity,
};

enum Modifier
{
	NoModifier,
	Immune,
	Ghost,//no collision + alpha mask
	Death,//suicide
};

//forward declaration
class ConditionTransition;

class Phase
{
public:
	Phase();
	string display_name;
	list<Weapon*> weapons_list;
	PatternBobby* Pattern;
	Modifier modifier;
	float angspeed;
	float radius;
	float vspeed;
	bool hasTransition;
	bool hasWakeUp;
	bool hasWelcomeShot;
	list<ConditionTransition*> transitions_list;
	string wake_up_name;
	static ConditionTransition* ConditionLoader(vector<string> line_data, int index);
	Weapon* welcomeWeapon;
};

class ConditionTransition
{
public:
	ConditionTransition(ConditionType m_condition, FloatCompare m_op, float m_value, string m_nextPhase_name);
	ConditionType condition;
	FloatCompare op;
	float value;
	Phase* nextPhase;
	std::string nextPhase_name;
};

#endif// PHASE_H_INCLUDED