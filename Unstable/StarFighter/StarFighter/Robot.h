#ifndef ROBOT_H_INCLUDED
#define ROBOT_H_INCLUDED

#include "Game.h"

#include "RobotSlot.h"

enum UI_ButtonType
{
	UI_Button_EndTurn,
	UI_Button_WeaponAttack_1,
	UI_Button_WeaponAttack_2,
	NB_UI_BUTTON_TYPES,
};

enum UI_WeaponStats
{
	UI_Weapon_Damage,
	UI_Weapon_HitChance,
	UI_Weapon_Unbalance,
	UI_Weapon_Fire,
	UI_Weapon_Electricity,
	UI_Weapon_Stun,
};

class Robot : public GameEntity
{
public:
	Robot();
	~Robot();

	int m_index;

	vector<CrewMember*> m_crew;
	vector<CrewMember*> m_crew_start;
	vector<RobotSlot> m_slots;

	int m_robot_map[NB_SLOT_INDEX][NB_SLOT_INDEX];

	bool SetModule(ModuleType type, SlotIndex index);
	bool SetEquipment(EquipmentType type, SlotIndex index);
	bool SetCrewMember(CrewType type, SlotIndex index);
	bool SetWeapon(WeaponType type, SlotIndex index);

	int m_health;
	int m_health_max;
	int GetHealthMax();
	int m_weight;
	int m_energy_cells;
	int m_energy_cells_available;
	int m_crew_max;
	int GetBalanceScore();
	int m_balance_bonus;
	int m_attack_speed_bonus;
	void GetWeightModifiers(int &balance_bonus, int &attack_speed_bonus);
	int m_unbalanced_counter;//0: balanced; 1: unbalanced until next turn; 2: unbalanced for 2 turns
	int m_unbalanced_value;
	bool m_shutdown_global;
	bool m_grounded;
	Module* m_grabbed;
	int m_guard_speed;
	int m_jammer_bonus;

	Robot* m_opponent;

	bool m_ready_to_change_phase;

	void Initialize();
	void Update();
	bool UpdateShudownGlobal();
	bool CheckShudownGlobalConditions();
	void DestroySlot(SlotIndex index);
	void ShutdownSlot(SlotIndex index);
	void UpdateCrew(SlotIndex index);
	int GetDistanceFromSlotToSlot(SlotIndex index, SlotIndex target_index);
	Module* GetShield();

	int GenerateEnergyCells();
	int HealCrewMembers();
	int RepairModules();
	void UpdateFirePropagation();
	void UpdateCooldowns();
	bool MoveCrewMemberToSlot(CrewMember* crew, RobotSlot* target_slot);
	bool SetEnergyCell(Module* module);
	bool SetEnergyCell(Equipment* equipment);
	bool RemoveEnergyCell(Module* module);
	bool RemoveEnergyCell(Equipment* equipment);
	bool SetWeaponAttackOnSlot(WeaponAttack* attack, SlotIndex target_index);
	bool SetEquipmentEffectOnSlot(EquipmentEffect* effect, SlotIndex target_index);
	bool SetMedicTarget(CrewMember* medic, CrewMember* target);
	bool SetEnergyCellsOnBalance();
	void ReloadWeapons();
	RobotSlot* GetEntityParentSlot(GameEntity* entity, UI_Type type);
	bool SetGuard(int speed, bool left);
	bool SetGrab(bool left, SlotIndex target_index);

	RobotSlot* m_grab_slot;
	RobotSlot* m_guard_slot;
	bool m_has_attacked;
	bool m_has_attacked_RightHand;
	bool m_has_attacked_LeftHand;
	bool m_has_guarded;
	bool m_has_grabbed_RightHand;
	bool m_has_grabbed_LeftHand;
	SlotIndex m_guard_index;

	//UI
	void InitializeUI();
	void UpdateUI();
	string GetCrewMemberName(CrewType type);

	vector<UI_Element> m_UI_slots;
	vector<vector<UI_Element> >m_UI_modules;
	vector<UI_Element> m_UI_crew;
	vector<UI_Element> m_UI_crew_sml;
	vector<UI_Element> m_UI_buttons;
	vector<UI_Element> m_UI_ec_available;
	SFText m_UI_stats;
	vector<vector<UI_Element> > m_UI_focus;
};

#endif //ROBOT_H_INCLUDED