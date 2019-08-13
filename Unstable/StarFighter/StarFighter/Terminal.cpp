#include "Terminal.h"

extern Game* CurrentGame;

using namespace sf;

Terminal::Terminal(sf::Vector2f position, AllianceType alliance) : Node(position, alliance, 32)
{
	m_is_terminal_node = true;

	m_radar_range = 500;
	m_radar_coverage = 120;
	m_radar_activated = true;
}