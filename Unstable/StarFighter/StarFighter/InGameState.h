#ifndef METAGAME_H_INCLUDED
#define METAGAME_H_INCLUDED

#include "GameState.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "Game.h"
#include "Ship.h"
#include "SFPanelSpecific.h"
#include "FileLoadUtils.h"

class GameObject;

#define MAX_ROBOT_WEIGHT			25


enum SlotIndex
{
	Index_Head,
	Index_LegL,
	Index_LegR,
	Index_FootL,
	Index_FootR,
	Index_ShoulderL,
	Index_ShoulderR,
	Index_ForearmL,
	Index_ForearmR,
	Index_BodyU,
	Index_BodyM,
	Index_BodyD,
	NB_SLOT_INDEX,
};

enum CrewType
{
	Crew_Captain,
	Crew_Scientist,
	Crew_Mechanic,
	Crew_Pilot,
	Crew_Engineer,
	Crew_Warrior,
	Crew_Medic,
	Crew_Gunner,
	NB_CREW_TYPES,
};

class CrewMember
{
public:
	CrewMember(CrewType type);

	CrewType m_type;
	int m_health_max;
	int m_health;
	int m_steps;
	SlotIndex m_slot_index;

	void Update();
};

enum ModuleType
{
	Module_Infirmary,
	Module_Generator,
	Module_Sensors,
	Module_Stabilizers,
	Module_Head,
	Module_Radar,
	Module_Weapon,
	Module_CrewQuarter,
	Module_Deflectors,
	Module_Gadget,
	NB_MODULE_TYPES,
};

class Module
{
public:
	Module(ModuleType type);

	ModuleType m_type;

	int m_health;
	int m_health_max;
	int m_weight;
	int m_cells;
	int m_cells_max;
	int m_size;
	bool m_unique;
	int m_cooldown;
	int m_cooldown_timer;
	int m_crew_max;
};

enum EquipmentType
{
	Equipment_EnergeticWeapon,
	Equipment_HeavyPlate,
	Equipment_LightPlate,
	Equipment_GeneratorBooster,
	Equipment_CQExtension,
	Equipment_GadgetJammer,
	Equipment_WeaponsScope,
	Equipment_GadgetEMP,
	NB_EQUIPMENT_TYPES,
};

class Equipment
{
public:
	Equipment(EquipmentType type);

	EquipmentType m_type;
	ModuleType m_module_equipable;

	int m_health;
	int m_health_max;
	int m_weight;
	int m_cells;
	int m_cells_max;
	int m_size;
	bool m_unique;
	int m_cooldown;
	int m_cooldown_timer;
	int m_crew_max;
};

enum WeaponType
{
	Weapon_Fist,
	Weapon_Grab,
	Weapon_Guard,
	Weapon_BlasterRifle,
	Weapon_LaserCannon,
	Weapon_FireSword,
	Weapon_Hammer,
	Weapon_Gun,
	NB_WEAPON_TYPES,
};

class WeaponAttack
{
public:
	WeaponAttack();
	int m_speed;
	int m_damage;
	int m_energy_cost;
	int m_chance_of_hit;
	int m_chance_of_fire;
	int m_chance_of_electricity;
	int m_chance_of_stun;
	int m_balance;
	CrewType m_crew_required;
};

class Weapon
{
public:
	Weapon(WeaponType type);

	WeaponType m_type;

	vector<WeaponAttack> m_attacks;

	int m_weight;
	bool m_energetic;
	bool m_ranged;
};

enum SlotType
{
	Slot_Head,
	Slot_Leg,
	Slot_Foot,
	Slot_Shoulder,
	Slot_Forearm,
	Slot_Body,
	NB_SLOT_TYPES,
};

class RobotSlot
{
public:
	RobotSlot(SlotType type, int size, SlotIndex index){ m_size = size; m_type = type; m_index = index; m_module = NULL; m_weapon = NULL; };

	int m_size;
	SlotType m_type;
	SlotIndex m_index;

	vector<CrewMember> m_crew;
	Module* m_module;
	vector<Equipment*> m_equipments;
	Weapon* m_weapon;
};


class Robot
{
public:
	Robot();
	~Robot();

	vector<CrewMember> m_crew;
	vector<RobotSlot> m_slots;

	bool SetModule(ModuleType type, SlotIndex index);
	bool SetEquipment(EquipmentType type, SlotIndex index);
	bool SetCrewMember(CrewType type, SlotIndex index);
	bool SetWeapon(WeaponType type, SlotIndex index);

	int m_health;
	int m_health_max;
	int GetHealthMax();
	int m_weight;
	int m_crew_max;
	int m_crew_nb;
	int GetCrewMax();
	int m_balance;
	int m_balance_bonus;
	int m_attack_speed_bonus;
	void GetWeightModifiers(int &balance_bonus, int &attack_speed_bonus);
	
	void Initialize();
	void Update();
};


class InGameState : public GameState
{
public:
	GameObject* playerShip;

	void Initialize(Player player);
	void Update(sf::Time deltaTime);
	void Draw();
	void Release();
	void UpdateCamera(sf::Time deltaTime);
	static void CreateSFPanel(SFPanelTypes panel_type, Ship* playerShip);
	static void DestroySFPanel(Ship* playerShip);
	static void LoadCSVFile(string scenes_file);

	//ROBOT
	Robot m_robots[2];

	void InitRobots();

private:
	sf::RenderWindow* mainWindow;
	sf::Text* framerate;
};



#endif //METAGAME_H_INCLUDED