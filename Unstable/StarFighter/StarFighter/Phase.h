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
	PHASE_NAME,
	PHASE_VSPEED,
	PHASE_WEAPON,
	PHASE_AMMO,
	PHASE_WEAPON_OFFSET,
	PHASE_WEAPON_DELAY,
	PHASE_WEAPON_2,
	PHASE_AMMO_2,
	PHASE_WEAPON_OFFSET_2,
	PHASE_WEAPON_DELAY_2,
	PHASE_WEAPON_3,
	PHASE_AMMO_3,
	PHASE_WEAPON_OFFSET_3,
	PHASE_WEAPON_DELAY_3,
	PHASE_WEAPON_4,
	PHASE_AMMO_4,
	PHASE_WEAPON_OFFSET_4,
	PHASE_WEAPON_DELAY_4,
	PHASE_PATTERN,
	PHASE_ANGSPEED,
	PHASE_RADIUS,
	PHASE_CLOCKWISE,
	PHASE_ROTATION_SPEED,
	PHASE_MODIFIER,
	PHASE_WAKEUP,
	PHASE_WELCOME_WEAPON,
	PHASE_WELCOME_AMMO,
	PHASE_CONDITION,
	PHASE_OPERATOR,
	PHASE_VALUE,
	PHASE_TRANSITION,
	PHASE_CONDITION_2,
	PHASE_OPERATOR_2,
	PHASE_VALUE_2,
	PHASE_TRANSITION_2,
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

enum BouncingType
{
	NoBouncing,
	BouncingEverywhere,
	BouncingHorizontal,
	BouncingVertical,
};

enum Modifier
{
	NoModifier,
	Immune,
	Ghost,//no collision + alpha mask
	Death,//suicide
	FaceTarget,//rotate to always face player
	ResetFacing,//rotate to always face player
	Bouncing,//bounces on scene borders
	BouncingH,//bounces on horizontal scene borders only
	BouncingV,//bounces on horizontal scene borders only
};

//forward declaration
class ConditionTransition;

class Phase
{
public:
	Phase();
	string display_name;
	vector<Weapon*> weapons_list;
	PatternBobby* Pattern;
	Modifier modifier;
	float angspeed;
	float radius;
	float vspeed;
	float rotation_speed;
	bool hasTransition;
	bool hasWakeUp;
	bool hasWelcomeShot;
	vector<ConditionTransition*> transitions_list;
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