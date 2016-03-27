#ifndef PHASE_H_INCLUDED
#define PHASE_H_INCLUDED

#include "GameObject.h"
#include "TextUtils.h"
#include "Dialog.h"

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
	PHASE_WEAPON_5,
	PHASE_AMMO_5,
	PHASE_WEAPON_OFFSET_5,
	PHASE_WEAPON_DELAY_5,
	PHASE_WEAPON_6,
	PHASE_AMMO_6,
	PHASE_WEAPON_OFFSET_6,
	PHASE_WEAPON_DELAY_6,
	PHASE_PATTERN,
	PHASE_ANGSPEED,
	PHASE_RADIUS,
	PHASE_CLOCKWISE,
	PHASE_ROTATION_SPEED,
	PHASE_MODIFIER,
	PHASE_MODIFIER_2,
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
	PHASE_DIALOG_NAME,
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
	ShotsFired,
	PlayerVerticalPosition,
	PlayerHorizontalPosition,
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
	GhostModifier,//no collision + alpha mask
	DeathModifier,//suicide
	FaceTarget,//rotate to always face player
	ResetFacing,//rotate to always face player
	Bouncing,//bounces on scene borders
	BouncingH,//bounces on horizontal scene borders only
	BouncingV,//bounces on horizontal scene borders only
	FreezePlayer,//disable player inputs
};

//forward declaration
class ConditionTransition;

class Phase
{
public:
	Phase();
	string m_display_name;
	string m_name;
	vector<Weapon*> m_weapons_list;
	PatternBobby* m_Pattern;
	vector <Modifier> m_modifiers;
	float m_angspeed;
	float m_radius;
	float m_vspeed;
	float m_rotation_speed;
	bool m_hasWakeUp;
	bool m_freeze_player;
	bool m_hasWelcomeShot;
	vector<ConditionTransition*> m_transitions_list;
	string m_wake_up_name;
	Weapon* m_welcomeWeapon;
	vector <Dialog*> m_dialogs;

	static ConditionTransition* ConditionLoader(vector<string> line_data, int index);
};

class ConditionTransition
{
public:
	ConditionTransition(ConditionType condition, FloatCompare op, float value, string nextPhase_name);

	ConditionType m_condition;
	FloatCompare m_op;
	float m_value;
	Phase* m_nextPhase;
	std::string m_nextPhase_name;
};

#endif// PHASE_H_INCLUDED