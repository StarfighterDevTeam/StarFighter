#include "Player.h"


void Player::Init(sf::RenderWindow* mainWindow)
{
	this->m_playerWindow = mainWindow;
	this->m_save_file = PLAYER_SAVE_FILE;
}