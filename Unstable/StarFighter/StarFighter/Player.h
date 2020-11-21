#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include "SFDialogPanel.h"

class Player
{
public:
	void Init(sf::RenderWindow* mainWindow);

	sf::RenderWindow* m_playerWindow;
	std::string m_currentSceneFile;
	
	bool reverse_scene;

};

#endif //PLAYER_H_INCLUDED