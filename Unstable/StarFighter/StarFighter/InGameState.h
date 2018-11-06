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
#define NB_MONSTER_SPELLS_MAX	6
#define SPELL_NB_COSTS_MAX		6
#define NB_MONSTERS				6

class Mage
{
public:
	Mage(){ m_timer = 0.f; for (int i = 0; i < NB_CARDS_HAND_MAX; i++) { m_hand_slots[i].m_status = CardSlot_Free; } };

	void InitCards();
	void ShuffleLibrary();
	void DrawCard(int nb_cards = 1);
	int GetFreeHandCardSlot();
	int GetFreeAltarCardSlot();
	bool PlayCard(int hand_slot, int altar_slot);
	void Attack();

	CardSlot m_hand_slots[NB_CARDS_HAND_MAX];
	CardSlot m_library_slot;
	CardSlot m_graveyard_slot;
	vector<Card> m_libary;
	vector<Card> m_graveyard;
	vector<Card> m_cards;
	CardSlot m_altar_slots[NB_CARDS_ALTAR];//to be shared among players
	CardSlot m_monster_spells_slots[NB_MONSTER_SPELLS_MAX][SPELL_NB_COSTS_MAX];//to be shared among players
	SFText m_monster_spells_names[NB_MONSTER_SPELLS_MAX];
	SFText m_monster_spells_descriptions[NB_MONSTER_SPELLS_MAX];

	float m_timer;
};

class Spell
{
public:
	Spell(int cost, int nb_costs);
	bool Effect();
	vector<Card> m_costs;
	string m_display_name;
	string m_description;

	CardSlotStatus m_status;
};

class Monster
{
public:
	Monster();

	void Attack();

	vector<Spell> m_spells;// [NB_MONSTER_SPELLS_MAX];
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

	Mage m_mage;
	void InitTable();
	void SummonMonster();

	vector<Monster> m_monsters;

private:
	sf::RenderWindow* mainWindow;
	sf::Text* framerate;
};



#endif //METAGAME_H_INCLUDED