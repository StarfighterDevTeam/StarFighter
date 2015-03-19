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
	PHASE_WEAPON_OFFSET,//4
	PHASE_WEAPON_2,//5
	PHASE_AMMO_2,//6
	PHASE_WEAPON_OFFSET_2,//7
	PHASE_WEAPON_3,//8
	PHASE_AMMO_3,//9
	PHASE_WEAPON_OFFSET_3,//10
	PHASE_PATTERN,//11
	PHASE_ANGSPEED,//12
	PHASE_RADIUS,//13
	PHASE_CLOCKWISE,//14
	PHASE_ROTATION_SPEED,//15
	PHASE_MODIFIER,//16
	PHASE_WAKEUP,//17
	PHASE_WELCOME_WEAPON,//18
	PHASE_WELCOME_AMMO,//19
	PHASE_CONDITION,//20
	PHASE_OPERATOR,//21
	PHASE_VALUE,//22
	PHASE_TRANSITION,//23
	PHASE_CONDITION_2,//24
	PHASE_OPERATOR_2,//25
	PHASE_VALUE_2,//26
	PHASE_TRANSITION_2,//27
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
	ResetFacing,//rotate to always face player
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