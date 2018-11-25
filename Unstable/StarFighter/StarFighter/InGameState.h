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


#define NB_CARDS_HAND			5
#define NB_CARDS_HAND_MAX		7
#define NB_CARDS_ALTAR			4
#define CARD_WIDTH				32
#define CARD_HEIGHT				64
#define MONSTER_WIDTH			256
#define MONSTER_HEIGHT			256
#define NB_MONSTER_SPELLS_MAX	6
#define SPELL_NB_COSTS_MAX		6
#define NB_MONSTERS				6
#define NB_PLAYERS_MAX			2

enum Actions
{
	Action_None,
	Action_ManaSuccess,
	Action_HandToAltar,
	Action_AltarToCurse,
	Action_DrawCard,
	Action_AltarToBlessing,
};

enum BlessingType
{
	Blessing_Heal,
	Blessing_Draw,
	Blessing_Telepathy,
	NB_BLESSING_TYPES,
};

enum MonsterType
{
	Monster_Squale,
	Monster_Kraken,
	Monster_Spiky,
	Monster_Golem,
	Monster_Ogre,
	Monster_Yeti,
	Monster_Blademaster,
	Monster_Wolf,
	Monster_Tatoo,
	Monster_Frankenstein,
	NB_MONSTER_TYPES,
};

enum CurseType
{
	Curse_NoEffect,
	Curse_BurnRandomPlayer,
	NB_CURSE_TYPES,
};
	
class Mage
{
public:
	Mage(int index){ m_timer = 0.f; for (int i = 0; i < NB_CARDS_HAND_MAX; i++) { m_hand_slots[i].m_status = CardSlot_Free; m_index = index; m_is_alive = true; } };

	void InitSlots(int player_index);
	void InitCards();
	void ShuffleLibrary();
	Actions DrawCard(int nb_cards = 1);
	int GetFreeHandCardSlot();
	int GetHandCount();

	CardSlot m_hand_slots[NB_CARDS_HAND_MAX];
	CardSlot m_library_slot;
	CardSlot m_graveyard_slot;
	CardSlot m_end_of_turn;
	vector<Card> m_libary;
	vector<Card> m_graveyard;
	vector<Card> m_cards;

	float m_timer;
	int m_index;
	bool m_is_alive;
};

class Curse
{
public:
	Curse(int cost, int nb_costs, int index, CurseType type);
	bool Effect();
	vector<Card> m_costs;
	string m_display_name;
	string m_description;

	CardSlotStatus m_status;
	int m_index;
	CurseType m_type;
};

class Blessing
{
public:
	Blessing(BlessingType type, int index);
	vector<Card> m_costs;
	string m_display_name;
	string m_description;

	CardSlotStatus m_status;
	int m_index;

	BlessingType m_type;
};

class Monster
{
public:
	Monster(MonsterType type = NB_MONSTER_TYPES);

	vector<Curse> m_curses;// [NB_MONSTER_SPELLS_MAX];
	MonsterType m_type;
	string m_display_name;
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

	vector<Mage> m_mages;
	void InitTable();
	void GetMonsterFilename(string& filename, int monster_type);
	void SummonMonster();
	void InitBlessings();
	int GetFreeAltarCardSlot();
	Actions PlayCard(int player_index, int hand_slot, int altar_slot);
	Actions AltarSpendMana(int player_index, CardStack stack, int slot);
	void UseAltarCard(int index);
	bool BurnPlayer(int player_index);
	bool BlessingPlayer(int player_index, BlessingType type);
	void EndOfTurn();
	void NewGame();
	bool AllPlayersDead();

	vector<Monster> m_monsters;
	vector<Blessing> m_blessings;

	CardSlot m_altar_slots[NB_CARDS_ALTAR];
	CardSlot m_altar_slot;
	CardSlot m_monster_slot;
	SFText m_monster_name;
	CardSlot m_monster_curses_slots[NB_MONSTER_SPELLS_MAX];
	CardSlot m_monster_curses_costs[NB_MONSTER_SPELLS_MAX][SPELL_NB_COSTS_MAX];
	SFText m_monster_curses_names[NB_MONSTER_SPELLS_MAX];
	SFText m_monster_curses_descriptions[NB_MONSTER_SPELLS_MAX];
	CardSlot m_blessing_slots[NB_BLESSING_TYPES];
	CardSlot m_blessing_costs[NB_BLESSING_TYPES][SPELL_NB_COSTS_MAX];
	SFText m_blessing_names[NB_BLESSING_TYPES];
	SFText m_blessing_descriptions[NB_BLESSING_TYPES];

	int m_current_player;

private:
	sf::RenderWindow* mainWindow;
	sf::Text* framerate;
};



#endif //METAGAME_H_INCLUDED