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

enum ManaType
{
	Mana_Fire,		//0
	Mana_Water,		//1
	Mana_Air,		//2
	Mana_Earth,		//3
};

enum ManaValue
{
	Mana_1,			//0
	Mana_2,			//1
	Mana_3,			//2
};

class Card
{
public:
	Card(){};
	Card(ManaType type, ManaValue value) { m_type = type; m_value = value; }

	ManaType m_type;
	ManaValue m_value;
};

enum CardSlotStatus
{
	CardSlot_Free,		//0
	CardSlot_Occupied,	//1
	CardSlot_Burnt,		//2
};

class CardSlot
{
public:
	CardSlot(){ m_status = CardSlot_Free; }
	CardSlotStatus m_status;
	Card m_card;
	void GetCard(Card& card);
	static sf::Color GetManaColor(ManaType type);
	static sf::Color GetStatusColor(CardSlotStatus status);

	RectangleShape m_shape_container;
	RectangleShape m_shape;
	SFText m_text;
};

#define NB_CARDS_HAND		5
#define NB_CARDS_HAND_MAX	7
#define NB_CARDS_ALTAR		4
#define CARD_WIDTH			32
#define CARD_HEIGHT			64

class Mage
{
public:
	Mage(){ m_timer = 0.f; for (int i = 0; i < NB_CARDS_HAND_MAX; i++) { m_hand_slots[i].m_status = CardSlot_Free; } };

	void InitCards();
	void ShuffleLibrary();
	void DrawCard(int nb_cards = 1);
	int GetFreeHandCardSlot();
	int GetFreeAltarCardSlot();
	bool PlayCard(int hand_slot);

	CardSlot m_hand_slots[NB_CARDS_HAND_MAX];
	CardSlot m_library_slot;
	CardSlot m_graveyard_slot;
	vector<Card> m_libary;
	vector<Card> m_graveyard;
	vector<Card> m_cards;
	CardSlot m_altar_slots[NB_CARDS_ALTAR];//to be shared among players

	float m_timer;
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

private:
	sf::RenderWindow* mainWindow;
	sf::Text* framerate;
};



#endif //METAGAME_H_INCLUDED