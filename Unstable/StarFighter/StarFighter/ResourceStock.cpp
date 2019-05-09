#include "ResourceStock.h"

extern Game* CurrentGame;

Resource::Resource(ResourceType type, UI_Type ui_type) : GameEntity(ui_type)
{
	m_type = type;
	m_display_name = (*CurrentGame).m_dico_resources_names[type];
}

Resource::~Resource()
{
	
}