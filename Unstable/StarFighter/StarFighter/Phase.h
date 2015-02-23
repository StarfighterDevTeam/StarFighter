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
	PHASE_WEAPON_ANGLECONSTRAINT,//4
	PHASE_WEAPON_OFFSET,//5
	PHASE_WEAPON_2,//6
	PHASE_AMMO_2,//7
	PHASE_WEAPON_ANGLECONSTRAINT_2,//8
	PHASE_WEAPON_OFFSET_2,//9
	PHASE_WEAPON_3,//10
	PHASE_AMMO_3,//11
	PHASE_WEAPON_ANGLECONSTRAINT_3,//12
	PHASE_WEAPON_OFFSET_3,//13
	PHASE_PATTERN,//14
	PHASE_ANGSPEED,//15
	PHASE_RADIUS,//16
	PHASE_CLOCKWISE,//17
	PHASE_ROTATION_SPEED,//18
	PHASE_MODIFIER,//19
	PHASE_WAKEUP,//20
	PHASE_WELCOME_WEAPON,//21
	PHASE_WELCOME_AMMO,//22
	PHASE_CONDITION,//23
	PHASE_OPERATOR,//24
	PHASE_VALUE,//25
	PHASE_TRANSITION,//26
	PHASE_CONDITION_2,//27
	PHASE_OPERATOR_2,//28
	PHASE_VALUE_2,//29
	PHASE_TRANSITION_2,//30
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
	FaceTarget,//rotate to always face player
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
	float rotation_speed;
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