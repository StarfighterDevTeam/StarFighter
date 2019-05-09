#ifndef RESOURCESTOCK_H_INCLUDED
#define RESOURCESTOCK_H_INCLUDED

#include "Game.h"
#include "GameEntity.h"

struct Resource : GameEntity
{
public:
	Resource(ResourceType type, UI_Type ui_type);
	~Resource();
	
	ResourceType m_type;
	string m_display_name;
};

#endif //RESOURCESTOCK_H_INCLUDED