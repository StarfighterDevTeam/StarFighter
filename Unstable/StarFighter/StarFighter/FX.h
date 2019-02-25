#ifndef FX_H_INCLUDED
#define FX_H_INCLUDED

#include "SFTextPop.h"
#include "Game.h"
#include "GameEntity.h"

enum FX_Type
{
	FX_Explosion,
	FX_Splash,
	FX_shrapnel,
};

class FX : public GameEntity
{
public:
	FX(FX_Type type);

	float m_FX_timer;
	bool m_can_be_seen;
	string m_texturename;
	FX_Type m_type;
	float m_delay_timer;

	void Update(Time deltaTime);
	void Draw(sf::RenderTexture& screen);
	FX* Clone();
};

#endif //FX_H_INCLUDED