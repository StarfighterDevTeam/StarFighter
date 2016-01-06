#include "Glow.h"

extern Game* CurrentGame;

using namespace sf;

Glow::Glow(GameObject* parent, sf::Color color, int glow_thickness, int stroke_size)
{
	sf::Uint8* pixels = CreateGlowFrame(parent, color, glow_thickness, stroke_size);

	const int W = parent->m_size.x + glow_thickness * 2;
	const int H = parent->m_size.y + glow_thickness * 2;

	Init(parent->getPosition(), sf::Vector2f(0, 0), m_textureName, sf::Vector2f(W, H), 1, 1, pixels);
}

sf::Uint8* Glow::CreateGlowFrame(GameObject* parent, sf::Color color, int glow_thickness, int stroke_size)
{
	assert(parent != NULL);

	const int W = parent->m_size.x + glow_thickness * 2;
	const int H = parent->m_size.y + glow_thickness * 2;

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

	return pixels;
}

Glow::Glow(GameObject* parent, sf::Color color, int glow_thickness, int stroke_size, float glow_animation_duration, int glow_min_thickness)
{
	m_collider_type = BackgroundObject;
	m_color = color;
	m_glow_radius = glow_thickness;
	m_glow_animation_duration = glow_animation_duration;

	//calculating the number of frames required to create an animation with the desired duration
	int unique_frames_number = (int)(ceil(glow_animation_duration / TIME_BETWEEN_ANIMATION_FRAMES));
	int frames_number = unique_frames_number * 2 - 2;
	if (frames_number < 1)
		frames_number = 1;

	//size of one frame
	const int W = parent->m_size.x + glow_thickness * 2;
	const int H = parent->m_size.y + glow_thickness * 2;

	//hardware limitation: TEXTURE_SIZE_LIMITATION x TEXTURE_SIZE_LIMITATION
	if (W*frames_number > TEXTURE_SIZE_LIMITATION)
	{
		frames_number = (int)(ceil(TEXTURE_SIZE_LIMITATION / W));
		if (frames_number % 2 != 0)
			frames_number--;
		unique_frames_number = frames_number / 2 + 1;
		printf("<!>Trying to create a texture that exceeds 8187x8187. Frame number reduced to %d.\n",frames_number);
	}
	if (H > TEXTURE_SIZE_LIMITATION)
	{
		frames_number = (int)(ceil(TEXTURE_SIZE_LIMITATION / H));
		if (frames_number % 2 != 0)
			frames_number--;
		unique_frames_number = frames_number / 2 + 1;
		printf("<!>Trying to create a texture that exceeds 8187x8187. Frame number reduced to %d.\n", frames_number);
	}

	//checking if texture laready exists. if not, create a fake one, to be filled few lines of code below
	TextureLoader *loader;
	loader = TextureLoader::getInstance();
	ostringstream ss;
	ss << W << "x" << H << "x" << frames_number << "_glow" << (int)color.r << "-" << (int)color.g << "-" << (int)color.b << "-" << (int)color.a;
	sf::Texture* texture = loader->getTexture(ss.str());

	//updating the newly created texture, if it does not exist yet
	if (!texture)
	{
		//creating and loading the texture
		sf::Uint8* pixels_animation = new sf::Uint8[W * H * frames_number * 4];
		texture = loader->loadTexture(ss.str(), W*frames_number, H, pixels_animation);

		//creating a transparent default background
		for (int p = 0; p < ((W * H * frames_number - 1) * 4) + 1; p++)// <!> very touchy because there can be heap corruption if we write outside the size of [W*H*frames_number*4] when i = p+3
		{
			pixels_animation[p] = 0;
			pixels_animation[p + 1] = 0;
			pixels_animation[p + 2] = 0;
			pixels_animation[p + 3] = 0;
		}
		texture->update(pixels_animation, W*frames_number, H, 0, 0);
		delete[] pixels_animation;

		//create and add as many frames as necessary for the complete glow animation
		for (int i = 1; i < unique_frames_number + 1; i++)
		{
			//"growing" part of the animation
			int thickness = (int)(floor(1.f * (glow_thickness - glow_min_thickness) * i / unique_frames_number));
			sf::Uint8* pixels_frame = CreateGlowFrame(parent, m_color, glow_min_thickness + thickness, stroke_size);
			int size_x = parent->m_size.x + (glow_min_thickness + thickness) * 2;
			int size_y = parent->m_size.y + (glow_min_thickness + thickness) * 2;
			int x_offset = glow_thickness - glow_min_thickness - thickness;
			int x = (i - 1)*W + x_offset;
			int y = glow_thickness - glow_min_thickness - thickness;
			texture->update(pixels_frame, size_x, size_y, x, y);

			//"diminishing" part of the animation
			if (i > 1 && i < unique_frames_number)//first and last frame must not be repeated
			{
				x = (frames_number - i + 1)*W + x_offset;
				texture->update(pixels_frame, size_x, size_y, x, y);
			}

			delete[] pixels_frame;
		}
	}

	setOrigin(parent->getOrigin().x + glow_thickness, parent->getOrigin().y + glow_thickness);
	Init(parent->getPosition(), sf::Vector2f(0, 0), texture, frames_number, 1);
}

Glow::~Glow()
{
	
}

void Glow::update(sf::Time deltaTime)
{
	AnimatedSprite::update(deltaTime);
}