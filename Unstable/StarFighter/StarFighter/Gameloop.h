#ifndef GAMELOOP_H_INCLUDED
#define GAMELOOP_H_INCLUDED

#include <fstream>
#include <iostream>
#include <sstream>
#include "Game.h"
#include "Player.h"
#include "SFUnitInfoPanel.h"
#include "SFContextInfoPanel.h"
#include "FileLoadUtils.h"
#include "Starship.h"
#include "StellarMap.h"

class GameObject;

class Gameloop
{
public:
	Gameloop();

	void Update(sf::Time deltaTime);
	void Draw();
	void UpdateCamera(sf::Time deltaTime);
	static void CreateSFPanel(SFPanelTypes panel_type, Player* player);
	static void DestroySFPanel(SFPanelTypes panel_type, Player* player);
	static void LoadCSVFiles();
};

#endif //GAMELOOP_H_INCLUDED