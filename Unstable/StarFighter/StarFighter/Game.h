#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include "Globals.h"
#include "GameObject.h"
#include <list>
#include <vector>
#include "TextUtils.h"
#include <SFML/Audio.hpp>
#include "Includes/SimpleCollision.hpp"
#define _USE_MATH_DEFINES
#include <math.h>

#include "InputGuy.h"

#include "Glow.h"
#include "Stroboscopic.h"
#include "SFPanel.h"
#include "SFTextPop.h"

class Ship;

enum SFX_Bank
{
	SFX_Laser,
	NBVAL_SFX_BANK,
};

enum Music_Bank
{
	Music_Main,
	NBVAL_MUSIC_BANK,
};

enum FontsStyle
{
	Font_Arial,//0
	Font_Terminator,//1
	NBVAL_FontsStyle,//2
};

using namespace sf;


enum CardSlotStatus
{
	CardSlot_Free,		//0
	CardSlot_Occupied,	//1
	CardSlot_Burnt,		//2
	CardSlot_Temp,		//3
};

enum MouseAction
{
	Mouse_None,
	Mouse_LeftClick,
	Mouse_RightClick,
};

class CardSlot
{
public:
	CardSlot(){ m_status = CardSlot_Free; m_hovered = false; m_selected = false; }
	CardSlotStatus m_status;
	int m_index;
	void Update(MouseAction mouse_click);

	RectangleShape m_shape_container;
	RectangleShape m_shape;
	SFText m_text;

	bool m_hovered;
	bool m_selected;
};

//ROBOT
#define MAX_ROBOT_WEIGHT				25
#define MAX_ROBOT_ENERGY_CELLS		17

enum GamePhase
{
	Phase_GenerateEC,
	Phase_CrewMovement,
	Phase_AttackPlanning,
	Phase_HealCrew,
	Phase_RepairModules,
	Phase_EquipmentResolution,
	Phase_GrabResolution,
	Phase_GuardResolution,
	Phase_AttackResolution_12,
	Phase_AttackResolution_11,
	Phase_AttackResolution_10,
	Phase_AttackResolution_9,
	Phase_AttackResolution_8,
	Phase_AttackResolution_7,
	Phase_AttackResolution_6,
	Phase_AttackResolution_5,
	Phase_AttackResolution_4,
	Phase_AttackResolution_3,
	Phase_AttackResolution_2,
	Phase_AttackResolution_1,
	Phase_CooldownResolution,
	Phase_FireResolution,
	NB_GAME_PHASES,
	Phase_Execution,
	Phase_CounterAttack,
};

enum SlotIndex
{
	Index_Head,
	Index_LegL,
	Index_LegR,
	Index_FootL,
	Index_FootR,
	Index_ShoulderL,
	Index_ShoulderR,
	Index_HandL,
	Index_HandR,
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
	Crew_Any,
	NB_CREW_TYPES,
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
	Weapon_Shield,
	NB_WEAPON_TYPES,
};

enum WeaponAttackType
{
	WeaponAttack_Fist_1,
	WeaponAttack_Fist_2,
	WeaponAttack_Grab_1,
	WeaponAttack_Guard_1,
	WeaponAttack_BlasterRifle_1,
	WeaponAttack_BlasterRifle_2,
	WeaponAttack_LaserCannon_1,
	WeaponAttack_LaserCannon_2,
	WeaponAttack_FireSword_1,
	WeaponAttack_FireSword_2,
	WeaponAttack_Hammer_1,
	WeaponAttack_Hammer_2,
	WeaponAttack_Gun_1,
	WeaponAttack_Gun_2,
	NB_WEAPON_ATTACK_TYPES,
};

class Weapon;//foreward declaration
class WeaponAttack
{
public:
	WeaponAttack(WeaponAttackType type, Weapon* owner);

	WeaponAttackType m_type;
	int m_speed;
	int m_damage;
	int m_energy_cost;
	int m_chance_of_hit;
	int m_chance_of_fire;
	int m_chance_of_electricity;
	int m_chance_of_stun;
	int m_chance_of_unbalance;
	CrewType m_crew_required;

	int m_energy_cells;

	Weapon* m_owner;
};

enum EquipmentEffectType
{
	Effect_Radar,
	Effect_EMP,
	Effect_Jammer,
	Effect_JetPack,
	Effect_Repulsion,
	Effect_WeaponScopes,
	Effect_GeneratorBooster,
	NB_EQUIPMENT_ACTION_TYPES,
};

class Module;
class Equipment;
class EquipmentEffect
{
public:
	EquipmentEffect(EquipmentEffectType type, Module* owner_module, Equipment* owner_equipment);

	EquipmentEffectType m_type;
	Module* m_owner_module;
	Equipment* m_owner_equipment;
	CrewType m_crew_required;
	int m_energy_cost;
};

enum SlotType
{
	Slot_Head,
	Slot_Leg,
	Slot_Foot,
	Slot_Shoulder,
	Slot_Hand,
	Slot_Body,
	NB_SLOT_TYPES,
};

struct ActionAttack
{
	SlotIndex m_target_index;
	WeaponAttack* m_attack;
};

struct ActionEffect
{
	SlotIndex m_target_index;
	EquipmentEffect* m_effect;
};

enum DistanceCombat
{
	Distance_Ranged,
	Distance_Close,
};


enum UI_Type
{
	UI_Crew,
	UI_Slot,
	UI_Module,
	UI_Button,
	NB_UI_TYPES,
};

class UI_Element
{
public:
	UI_Element(){};
	RectangleShape m_shape_container;
	RectangleShape m_shape;
	UI_Type m_type;
	TeamAlliances m_team;

	void Update();

	void Draw(sf::RenderTexture& screen)
	{
		screen.draw(this->m_shape_container);
		screen.draw(this->m_shape);
	};
};

struct Game
{
public:
	void init(RenderWindow* window);
	RenderWindow* getMainWindow();
	void addToScene(GameObject *object, LayerType layer, GameObjectType type);
	void addToFeedbacks(RectangleShape* feedback);
	void addToFeedbacks(Text* text);
	void addToFeedbacks(SFText* text);
	void addToFeedbacks(SFPanel* panel);
	void removeFromFeedbacks(RectangleShape* feedback);
	void removeFromFeedbacks(Text* text);
	void removeFromFeedbacks(SFPanel* panel);
	void CreateSFTextPop(string text, FontsStyle font, unsigned int size, sf::Color color, sf::Vector2f position, PlayerTeams team, float distance_not_faded, float distance_faded, float total_pop_time, GameObject* follow_target, float offset_positionY);
	void changeObjectTypeAndLayer(GameObject *object, LayerType new_layer, GameObjectType new_type);

	void updateScene(Time deltaTime);
	void drawScene();
	void colisionChecksV2();
	void cleanGarbage();
	void collectGarbage();

	sf::RenderTexture m_mainScreen;
	sf::Vector2i m_screen_size;
	float m_hyperspeedMultiplier;
	float m_vspeed;
	sf::Vector2f m_scale_factor;

	Ship* m_playerShip;
	bool m_pause;
	bool m_window_has_focus;

	sf::Vector2f m_mouse_pos;
	MouseAction m_mouse_click;
	float m_mouse_click_timer;

	sf::View m_view;
	sf::Vector2f m_map_size;

	//Utiliary methods
	GameObject* GetClosestObjectTyped(const GameObject* ref_obj, GameObjectType type_of_closest_object, float dist_max = 1.f, float angle_delta_max = -1.f);
	GameObject* GetClosestObjectTyped(const sf::Vector2f position, GameObjectType type_of_closest_object, float dist_max = 1.f, float angle_delta_max = -1.f);
	std::vector<GameObject*> GetSceneGameObjectsTyped(GameObjectType type);

	//Fonts
	sf::Font* m_font[NBVAL_FontsStyle];

	//SFX
	int LoadSFX();
	void PlaySFX(SFX_Bank sfx_name);
	void SetSFXVolume(bool activate_sfx);

	sf::SoundBuffer m_soundBuffers[1];
	sf::Sound m_sounds[1];
	bool m_SFX_Activated;

	//Music
	sf::Music m_curMusic;
	bool m_Music_Activated;
	string m_next_music_name;
	Music_Bank m_curMusic_type;
	float m_music_fader;
	bool m_asking_music_fade_out;

	void SetMusicVolume(bool activate_music);
	void PlayMusic(Music_Bank music, string specific_filename = "");
	void ManageMusicTransitions(sf::Time deltaTime);

	//CSV data
	map<string, vector<string> > m_gameObjectsConfig;

	//ROBOT
	UI_Element* m_selected_ui;
	UI_Element* m_hovered_ui;
	UI_Element* m_target_ui;
	UI_Element* m_play_ui;

	GamePhase m_phase;
	int m_turn;
	vector<ActionAttack> m_attacks_list;
	vector<ActionEffect> m_effects_list;
	DistanceCombat m_distance;
	DistanceCombat m_distance_temp;

private:
	void AddGameObjectToVector(GameObject* pGameObject, vector<GameObject*>* vector);
	void AddGameObjectVectorToVector(vector<GameObject*> vector_slave, vector<GameObject*>* vector_master);
	void TransferGameObjectLayeredTempToSceneObjectsLayered(LayerType layer);
	void TransferGameObjectTypedTempToSceneObjectsTyped(GameObjectType collider_type);
	void AddSFTextToVector(SFText* pSFText, vector<SFText*>* vector);
	bool isVectorEmpty(vector <GameObject*>* vector);

	RenderWindow* m_window;
	std::list<RectangleShape*> m_sceneFeedbackBars;
	std::list<Text*> m_sceneFeedbackTexts;
	std::list<SFPanel*> m_sceneFeedbackSFPanels;
	std::vector<SFText*> m_sceneFeedbackSFTexts;
	std::vector<GameObject*> m_sceneGameObjects;
	std::vector<GameObject*> m_sceneGameObjectsLayered[NBVAL_Layer];
	std::vector<GameObject*> m_sceneGameObjectsTyped[NBVAL_GameObject];
	std::vector<GameObject*> m_sceneGameObjectsLayeredTemp[NBVAL_Layer];
	std::vector<GameObject*> m_sceneGameObjectsTypedTemp[NBVAL_GameObject];
	std::vector<GameObject*> m_garbage;
	std::vector<SFText*> m_garbageTexts;
};

#endif // GAME_H_INCLUDED