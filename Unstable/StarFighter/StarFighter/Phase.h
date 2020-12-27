#ifndef PHASE_H_INCLUDED
#define PHASE_H_INCLUDED

#include "Loot.h"

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
	PHASE_WEAPON_OFFSET,
	PHASE_WEAPON_DELAY,
	PHASE_WEAPON_2,
	PHASE_WEAPON_OFFSET_2,
	PHASE_WEAPON_DELAY_2,
	PHASE_WEAPON_3,
	PHASE_WEAPON_OFFSET_3,
	PHASE_WEAPON_DELAY_3,
	PHASE_WEAPON_4,
	PHASE_WEAPON_OFFSET_4,
	PHASE_WEAPON_DELAY_4,
	PHASE_WEAPON_5,
	PHASE_WEAPON_OFFSET_5,
	PHASE_WEAPON_DELAY_5,
	PHASE_WEAPON_6,
	PHASE_WEAPON_OFFSET_6,
	PHASE_WEAPON_DELAY_6,
	PHASE_PATTERN,
	PHASE_PATTERN_SPEED,
	PHASE_PATTERN_CLOCKWISE,
	PHASE_PATTERN_STARTING_POINT,
	PHASE_PATTERN_WIDTH,
	PHASE_PATTERN_HEIGHT,
	PHASE_ROTATION_SPEED,
	PHASE_MODIFIER,
	PHASE_MODIFIER_2,
	PHASE_MODIFIER_3,
	PHASE_WAKEUP,
	PHASE_WELCOME_WEAPON,
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
	Rotation,
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
	ShootWhenAligned,//only shoots when aligned with target
	ResetFacing,//rotate to default direction
	Bouncing,//bounces on scene borders
	BouncingH,//bounces on horizontal scene borders only
	BouncingV,//bounces on horizontal scene borders only
	FreezePlayer,//disable player inputs
	KillBullets,//get all enemy bullets on screen to explode
};

class Phase;//forward declaration

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

class Phase
{
public:
	Phase();
	~Phase();
	Phase* Clone();

	string m_display_name;
	string m_name;
	vector<Weapon*> m_weapons_list;
	GeometryPattern* m_pattern;
	vector <Modifier> m_modifiers;
	float m_angspeed;
	float m_radius;
	float m_vspeed;
	float m_rotation_speed;
	bool m_freeze_player;
	vector<ConditionTransition*> m_transitions_list;
	string m_wake_up_name;
	Weapon* m_welcomeWeapon;
	vector <Dialog*> m_dialogs;

	static ConditionTransition* ConditionLoader(vector<string> line_data, int index);
};

#endif// PHASE_H_INCLUDED