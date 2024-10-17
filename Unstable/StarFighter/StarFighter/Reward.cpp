#include "Reward.h"

extern Game* CurrentGame;

#include "Location.h"

Reward::Reward()
{
	m_DMS_location = NULL;
	m_combat_ship = NULL;
	m_dockyard = NULL;
}

Reward::~Reward()
{
	delete m_DMS_location;
}