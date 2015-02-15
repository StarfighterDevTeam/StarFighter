#ifndef PHASE_H_INCLUDED
#define PHASE_H_INCLUDED

#include "Independant.h"
#include "Weapon.h"

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
	PHASE_MODIFIER,//11
	PHASE_WAKEUP,//12
	PHASE_CONDITION,//13
	PHASE_OPERATOR,//14
	PHASE_VALUE,//15
	PHASE_TRANSITION,//16
	PHASE_CONDITION_2,//17
	PHASE_OPERATOR_2,//18
	PHASE_VALUE_2,//19
	PHASE_TRANSITION_2,//20
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
};

enum Modifier
{
	NoModifier,
	Immune,
	Ghost,//no collision
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
	list<ConditionTransition*> transitions_list;
	string wake_up_name;
	bool hasWakeUp;
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