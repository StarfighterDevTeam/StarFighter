#ifndef FX_H_INCLUDED
#define FX_H_INCLUDED

#include "Game.h"
#include "GameEntity.h"

class FX : public GameEntity
{
public:
	FX();

	float m_FX_timer;
	bool m_can_be_seen;
	string m_texturename;

	void Update(Time deltaTime);
	void Draw(sf::RenderTexture& screen);
	FX* Clone();
};

#endif //FX_H_INCLUDED