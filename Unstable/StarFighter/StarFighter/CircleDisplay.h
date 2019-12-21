#ifndef CIRCLEDISPLAY_H_INCLUDED
#define CIRCLEDISPLAY_H_INCLUDED

#include "GameObject.h"

enum CircleType
{
	Circle_Shield,
	Circle_Gravitation,
};

class CircleDisplay : public CircleShape
{
public :
	CircleDisplay(){};

	GameObject* m_target_object;
	CircleType m_circle_type;
};

#endif //CIRCLEDISPLAY_H_INCLUDED
