#include "CombatInterface.h"

extern Game* CurrentGame;

CombatInterface::CombatInterface()
{
	
}

CombatInterface::~CombatInterface()
{

}

void CombatInterface::Init(Ship* enemy_ship)
{
	m_enemy_ship = enemy_ship;
}


void CombatInterface::Update(sf::Time deltaTime)
{
	
}

void CombatInterface::Draw(sf::RenderTexture& screen)
{
	

}