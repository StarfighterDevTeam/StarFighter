#include "Compass.h"

extern Game* CurrentGame;

Compass::Compass()
{
	//cadran
	m_cadran = new GameEntity(UI_Compass);
	m_cadran->m_shape_container.setSize(sf::Vector2f(COMPASS_CADRAN_SIZE_X, COMPASS_CADRAN_SIZE_Y));
	m_cadran->m_shape_container.setOrigin(sf::Vector2f(COMPASS_CADRAN_SIZE_X * 0.5f, COMPASS_CADRAN_SIZE_Y * 0.5f));
	m_cadran->m_shape_container.setPosition(sf::Vector2f(COMPASS_OFFSET_X + COMPASS_CADRAN_SIZE_X * 0.5, COMPASS_OFFSET_Y - COMPASS_CADRAN_SIZE_Y * 0.5f));
	m_cadran->m_shape_container.setFillColor(sf::Color(255, 255, 255, 255));
	m_cadran->m_shape_container.setOutlineThickness(4.f);
	m_cadran->m_shape_container.setOutlineColor(sf::Color(0, 0, 0, 255));
	sf::Vector2f cadran_pos = m_cadran->m_shape_container.getPosition();

	//cursor
	m_cursor = new GameEntity(UI_None);
	m_cursor->m_shape_container.setSize(sf::Vector2f(COMPASS_CURSOR_SIZE_X, COMPASS_CURSOR_SIZE_Y));
	m_cursor->m_shape_container.setOrigin(sf::Vector2f(COMPASS_CURSOR_SIZE_X * 0.5f, COMPASS_CURSOR_SIZE_Y * 0.5f));
	m_cursor->m_shape_container.setPosition(sf::Vector2f(cadran_pos.x, cadran_pos.y - (COMPASS_CURSOR_SIZE_Y - COMPASS_CADRAN_SIZE_Y) * 0.5f + 8.f));
	m_cursor->m_shape_container.setFillColor(sf::Color(255, 255, 255, 0));
	m_cursor->m_shape_container.setOutlineThickness(8.f);
	m_cursor->m_shape_container.setOutlineColor(sf::Color(63, 72, 204, 255));

	m_cursor->m_shape.setSize(sf::Vector2f(COMPASS_CURSOR_TIP_SIZE_X, COMPASS_CURSOR_TIP_SIZE_Y));
	m_cursor->m_shape.setOrigin(sf::Vector2f(COMPASS_CURSOR_TIP_SIZE_X * 0.5f, COMPASS_CURSOR_TIP_SIZE_Y * 0.5f));
	m_cursor->m_shape.setPosition(sf::Vector2f(cadran_pos.x, cadran_pos.y + (COMPASS_CADRAN_SIZE_Y * 0.5f)));
	m_cursor->m_shape.setFillColor(sf::Color(63, 72, 204, 255));

	m_marker_offset = 1.f * COMPASS_DEGREES_PER_MARKER / COMPASS_DEGREES_IN_CADRAN * COMPASS_CADRAN_SIZE_X;

	//markers
	for (int i = 0; i < 72; i++)
	{
		ostringstream ss;
		ss << i * COMPASS_DEGREES_PER_MARKER << "°";

		m_degree_markers[i] = new GameEntity(UI_None);
		m_degree_markers[i]->m_shape.setSize(sf::Vector2f(COMPASS_MARKER_SIZE_X, COMPASS_MARKER_SIZE_Y));
		m_degree_markers[i]->m_shape.setOrigin(sf::Vector2f(COMPASS_MARKER_SIZE_X * 0.5f, COMPASS_MARKER_SIZE_Y * 0.5f));
		m_degree_markers[i]->m_shape.setPosition(sf::Vector2f(cadran_pos.x - (COMPASS_CADRAN_SIZE_X * 0.5f) + i * m_marker_offset, cadran_pos.y + (COMPASS_CADRAN_SIZE_Y * 0.5f) - (COMPASS_MARKER_SIZE_Y * 0.5f)));
		m_degree_markers[i]->m_shape.setFillColor(sf::Color(0, 0, 0, 255));

		if (i % 9 == 0)
		{
			m_degree_markers[i]->m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
			m_degree_markers[i]->m_text.setCharacterSize(16);
			m_degree_markers[i]->m_text.setColor(sf::Color::Black);
			m_degree_markers[i]->m_text.setString(ss.str());

			ostringstream ss2;
			switch (i / 9)
			{
			case 0:
				ss2 << "N";
				break;
			case 1:
				ss2 << "NE";
				break;
			case 2:
				ss2 << "E";
				break;
			case 3:
				ss2 << "SE";
				break;
			case 4:
				ss2 << "S";
				break;
			case 5:
				ss2 << "SW";
				break;
			case 6:
				ss2 << "W";
				break;
			case 7:
				ss2 << "NW";
				break;
			}

			m_cardinal_pts[i / 9] = new GameEntity(UI_None);
			m_cardinal_pts[i / 9]->m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
			m_cardinal_pts[i / 9]->m_text.setCharacterSize(20);
			m_cardinal_pts[i / 9]->m_text.setColor(sf::Color::Black);
			m_cardinal_pts[i / 9]->m_text.setString(ss2.str());
		}

		if (i == 0)
		{
			m_marker_pos0 = m_degree_markers[i]->m_shape.getPosition();
			m_degree_markers[i]->m_shape.setFillColor(sf::Color(255, 0, 0, 255));
		}
	}
}

Compass::~Compass()
{
	delete m_cadran;
	delete m_cursor;

	for (int i = 0; i < 72; i++)
	{
		delete m_degree_markers[i];
	}

	for (int i = 0; i < 8; i++)
	{
		delete m_cardinal_pts[i];
	}
}

void Compass::Update(sf::Time deltaTime, float angle)
{
	m_cadran->update(deltaTime);
	m_cursor->update(deltaTime);

	for (int i = 0; i < 72; i++)
	{
		float x = m_marker_pos0.x + COMPASS_CADRAN_SIZE_X * 0.5f - m_marker_offset * (-i + (angle / COMPASS_DEGREES_PER_MARKER));//i0 = north
		if (x > m_marker_pos0.x + 1.5 * COMPASS_CADRAN_SIZE_X)
		{
			x -= 2.f * COMPASS_CADRAN_SIZE_X;
		}
		else if (x < m_marker_pos0.x - 0.5f * COMPASS_CADRAN_SIZE_X)
		{
			x += 2.f * COMPASS_CADRAN_SIZE_X;
		}

		m_degree_markers[i]->m_shape.setPosition(sf::Vector2f(x, m_marker_pos0.y));
		m_degree_markers[i]->m_text.SetPosition(sf::Vector2f(m_degree_markers[i]->m_shape.getPosition().x + 4.f, m_degree_markers[i]->m_shape.getPosition().y - m_degree_markers[i]->m_text.getCharacterSize() - 4.f));
		m_degree_markers[i]->update(deltaTime);

		if (i % 9 == 0)
		{
			m_cardinal_pts[i / 9]->m_text.SetPosition(sf::Vector2f(m_degree_markers[i]->m_shape.getPosition().x + 2.f, m_degree_markers[i]->m_shape.getPosition().y - m_degree_markers[i]->m_text.getCharacterSize() - 22.f));
			m_cardinal_pts[i / 9]->update(deltaTime);
		}
	}
}

void Compass::Draw(sf::RenderTexture& screen, float angle)
{
	m_cadran->Draw(screen);

	for (int i = 0; i < 72; i++)
	{
		float delta_angle = (i * COMPASS_DEGREES_PER_MARKER) - angle;

		if (delta_angle > 180)
			delta_angle -= 180.f * 2;
		else if (delta_angle < -180)
			delta_angle += 180.f * 2;

		if (abs(delta_angle) < 90)
		{
			m_degree_markers[i]->Draw(screen);

			if (i % 9 == 0)
			{
				m_cardinal_pts[i / 9]->Draw(screen);
			}
		}
	}

	m_cursor->Draw(screen);
}