#ifndef SEINGAMESTATE_H_INCLUDED
#define SEINGAMESTATE_H_INCLUDED

#include <fstream>
#include <iostream>
#include <sstream>
#include "SEGame.h"
#include "SEShip.h"
#include "FileLoadUtils.h"

class GameObject;

class SEInGameState
{
public:
	virtual void Initialize(sf::RenderWindow* window, SEShip* playerShip);
	virtual void Update(sf::Time deltaTime);
	virtual void Draw();
	virtual void Release();
	virtual void UpdateCamera(sf::Time deltaTime);
	virtual void CreateSFPanel(SFPanelTypes panel_type, SEShip* playerShip);
	virtual void DestroySFPanel(SEShip* playerShip);
	virtual void LoadCSVFile(string scenes_file);

private:
	sf::Text* framerate;
};

#endif //SEINGAMESTATE_H_INCLUDED