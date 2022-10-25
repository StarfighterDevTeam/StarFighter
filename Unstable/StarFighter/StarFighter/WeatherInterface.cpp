#include "WeatherInterface.h"

extern Game* CurrentGame;

#define WEATHER_INTERFACE_PANEL_SIZE_X			(64 * MAX_WEATHER_FORECASTS)
#define WEATHER_INTERFACE_PANEL_SIZE_Y			(64 * 2)

WeatherInterface::WeatherInterface()
{
	m_weather = NULL;
	m_panel = NULL;
}

void WeatherInterface::Init(const Weather* weather)
{
	m_weather = weather;

	//icons
	const float texture_size = 64.f;
	Texture* texture_sunny = TextureLoader::getInstance()->loadTexture("2D/weather_sunny.png", texture_size, texture_size);
	Texture* texture_mostlysunny = TextureLoader::getInstance()->loadTexture("2D/weather_mostlysunny.png", texture_size, texture_size);
	Texture* texture_cloudy = TextureLoader::getInstance()->loadTexture("2D/weather_cloudy.png", texture_size, texture_size);
	Texture* texture_rainy = TextureLoader::getInstance()->loadTexture("2D/weather_rainy.png", texture_size, texture_size);
	Texture* texture_tempest = TextureLoader::getInstance()->loadTexture("2D/weather_tempest.png", texture_size, texture_size);
	Texture* texture_foggy = TextureLoader::getInstance()->loadTexture("2D/weather_foggy.png", texture_size, texture_size);
	Texture* texture_wind = TextureLoader::getInstance()->loadTexture("2D/weather_wind.png", texture_size, texture_size);
	
	//background panel
	m_panel = new GameEntity(UI_None);
	m_panel->m_shape_container.setSize(sf::Vector2f(WEATHER_INTERFACE_PANEL_SIZE_X, WEATHER_INTERFACE_PANEL_SIZE_Y));
	m_panel->m_shape_container.setOrigin(sf::Vector2f(WEATHER_INTERFACE_PANEL_SIZE_X * 0.5f, WEATHER_INTERFACE_PANEL_SIZE_Y * 0.5));
	m_panel->m_shape_container.setFillColor((*CurrentGame).m_dico_colors[Color_VeryDarkGrey_Background]);
	m_panel->m_shape_container.setPosition(sf::Vector2f(REF_WINDOW_RESOLUTION_X * 0.5, REF_WINDOW_RESOLUTION_Y - WEATHER_INTERFACE_PANEL_SIZE_Y * 0.5));
	m_panel->m_position = m_panel->m_shape_container.getPosition();

	//weather forecasts
	for (int i = 0; i < MAX_WEATHER_FORECASTS; i++)
	{
		//winds
		m_winds[i] = new GameEntity(UI_WeatherWind);
		m_winds[i]->setAnimation(texture_wind, 1, 1);
		m_winds[i]->setPosition(sf::Vector2f(m_panel->m_position.x - m_panel->m_shape_container.getSize().x * 0.5 + texture_size * (0.5 + i),
			m_panel->m_position.y + m_panel->m_shape_container.getSize().y * 0.5 - texture_size * (0.5 + 1)));

		m_winds[i]->m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
		m_winds[i]->m_text.setCharacterSize(14);
		m_winds[i]->m_text.setColor(sf::Color::Cyan);
		m_winds[i]->m_text.setStyle(sf::Text::Bold);
		m_winds[i]->m_text.setPosition(sf::Vector2f(m_winds[i]->getPosition().x - m_winds[i]->m_size.x * 0.5 + 2, m_winds[i]->getPosition().y - texture_size * 0.5 - 2));

		//conditions
		m_conditions[i] = new GameEntity(UI_WeatherCondition);
		Texture* texture_condition = texture_sunny;
		switch (m_weather->m_forecasts[i].condition)
		{
			case Sunny:
			{
				texture_condition = texture_sunny;
				break;
			}
			case MostlySunny:
			{
				texture_condition = texture_mostlysunny;
				break;
			}
			case Cloudy:
			{
				texture_condition = texture_cloudy;
				break;
			}
			case Rainy:
			{
				texture_condition = texture_rainy;
				break;
			}
			case Foggy:
			{
				texture_condition = texture_foggy;
				break;
			}
			case Tempest:
			{
				texture_condition = texture_tempest;
				break;
			}
		}

		m_conditions[i]->setAnimation(texture_condition, 1, 1);
		m_conditions[i]->setPosition(sf::Vector2f(m_panel->m_position.x - m_panel->m_shape_container.getSize().x * 0.5 + texture_size * (0.5 + i),
			m_panel->m_position.y + m_panel->m_shape_container.getSize().y * 0.5 - texture_size * 0.5));
	}
}

WeatherInterface::~WeatherInterface()
{
	delete m_panel;
}

void WeatherInterface::Destroy()
{
	delete m_panel;
	m_panel = NULL;

	m_weather = NULL;
}

void WeatherInterface::Update()
{
	const float texture_size = 64.f;
	for (int i = 0; i < MAX_WEATHER_FORECASTS; i++)
	{
		ostringstream ss_wind;
		ss_wind << (int)m_weather->m_forecasts[i].windStrength << "kt";
		const float dir = m_weather->m_forecasts[i].windDirection;
		if (dir >=  337.5 || dir < 22.5)
			ss_wind << " N";
		if (dir >= 22.5 && dir < 67.5)
			ss_wind << " NW";
		if (dir >= 67.5 && dir < 112.5)
			ss_wind << " E";
		if (dir >= 112.5 && dir < 157.5)
			ss_wind << " SE";
		if (dir >= 157.5 && dir < 202.5)
			ss_wind << " S";
		if (dir >= 202.5 && dir < 247.5)
			ss_wind << " SW";
		if (dir >= 247.5 && dir < 292.5)
			ss_wind << " W";
		if (dir >= 292.5 && dir < 337.5)
			ss_wind << " NW";

		m_winds[i]->m_text.setString(ss_wind.str());
		m_winds[i]->m_text.setPosition(sf::Vector2f(m_winds[i]->getPosition().x - m_winds[i]->m_size.x * 0.5 + (texture_size - m_winds[i]->m_text.getGlobalBounds().width) * 0.5,
			m_winds[i]->getPosition().y - texture_size * 0.5 - 2));
	}
}

void WeatherInterface::Draw(sf::RenderTexture& screen)
{
	m_panel->Draw(screen);

	for (int i = 0; i < MAX_WEATHER_FORECASTS; i++)
	{
		m_winds[i]->Draw(screen);
		m_conditions[i]->Draw(screen);
	}
}