#include "Glow.h"

extern Game* CurrentGame;

using namespace sf;

Glow::Glow(GameObject* parent, sf::Color color, int glow_thickness, int stroke_size)
{
	assert(parent != NULL);

	collider_type = BackgroundObject;
	m_color = color;
	m_glow_radius = glow_thickness;
	textureName = parent->textureName + "_glow";

	const int W = parent->m_size.x + glow_thickness*2;
	const int H = parent->m_size.y + glow_thickness*2;

	sf::Uint8* pixels = new sf::Uint8[W * H * 4];

	ostringstream ss;
	
	for (int i = 0; i < W * H * 4; i += 4)
	{
		pixels[i] = color.r;		// R
		pixels[i + 1] = color.g;	// G
		pixels[i + 2] = color.b;	// B

		if (glow_thickness < 1 || (i / 4) <= W * (2 * glow_thickness + stroke_size) || (i / 4) >(H - 1 * (2 * glow_thickness + stroke_size))*W || (i / 4) % W <= 0 + ((2 * glow_thickness + stroke_size) - 1) || (i / 4) % W >= (W - 1 * (2 * glow_thickness + stroke_size)))
		{
			pixels[i + 3] = 255;
		}
		else
		{
			pixels[i + 3] = 0;
		}
	}

	//Add outter glow effect
	GlowEffect(glow_thickness, pixels, W, H, stroke_size);

	Init(parent->getPosition(), sf::Vector2f(0, 0), textureName, sf::Vector2f(W, H), 1, 1, pixels);
}

Glow::~Glow()
{
	
}

void Glow::update(sf::Time deltaTime)
{
	//const int W = m_size.x;
	//const int H = m_size.y;
	//
	//sf::Uint8* pixels = new sf::Uint8[W * H * 4];
	//
	//TextureLoader *loader;
	//loader = TextureLoader::getInstance();
	//
	//sf::Texture* texture = loader->getTexture(this->textureName);
	

	AnimatedSprite::update(deltaTime);
}