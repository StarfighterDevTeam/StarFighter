#include "FX.h"

extern Game* CurrentGame;

FX::FX(FX_Type type) : GameEntity(UI_None)
{
	m_type = type;
	m_delay_timer = 0.f;

	switch (type)
	{
		case FX_Explosion:
		{
			m_texturename = "2D/FX_explosion.png";
			Texture* texture = TextureLoader::getInstance()->loadTexture(m_texturename, 1600, 295);
			setAnimation(texture, 4, 1);
			break;
		}
		case FX_Splash:
		{
			m_texturename = "2D/FX_splash.png";
			Texture* texture = TextureLoader::getInstance()->loadTexture(m_texturename, 1360, 51);
			setAnimation(texture, 10, 1);
			break;
		}
		case FX_shrapnel:
		{
			m_texturename = "2D/FX_shrapnel.png";
			Texture* texture = TextureLoader::getInstance()->loadTexture(m_texturename, 400, 74);
			setAnimation(texture, 4, 1);
			break;
		}
	}

	m_FX_timer = TIME_BETWEEN_ANIMATION_FRAMES * m_frameNumber;
	m_can_be_seen = true;
}

FX* FX::Clone()
{
	FX* new_FX = new FX(m_type);
	new_FX->m_texturename = m_texturename;
	Texture* texture = TextureLoader::getInstance()->loadTexture(m_texturename, m_size.x, m_size.y);
	new_FX->setAnimation(texture, m_frameNumber, m_animationNumber);

	return new_FX;
}

void FX::Update(Time deltaTime)
{
	if (m_delay_timer > 0)
	{
		m_delay_timer -= deltaTime.asSeconds();
	}

	if (m_delay_timer <= 0)
	{
		if (m_FX_timer > 0)
		{
			m_FX_timer -= deltaTime.asSeconds();
		}

		if (m_FX_timer <= 0)
		{
			m_can_be_seen = false;
		}

		AnimatedSprite::update(deltaTime);
	}
}

void FX::Draw(sf::RenderTexture& screen)
{
	screen.draw(*this);
}