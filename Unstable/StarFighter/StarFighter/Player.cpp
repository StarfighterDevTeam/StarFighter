#include "Player.h"


void Player::Init(sf::RenderWindow* mainWindow)
{
	this->m_playerWindow = mainWindow;
	this->m_save_file = EDITOR_SAVE_FILE;
}