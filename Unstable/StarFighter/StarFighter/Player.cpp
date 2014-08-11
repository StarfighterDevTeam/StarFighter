#include "Player.h"


void Player::Init(sf::RenderWindow* mainWindow)
{
	this->m_playerWindow = mainWindow;
	this->reverse_scene = false;
}