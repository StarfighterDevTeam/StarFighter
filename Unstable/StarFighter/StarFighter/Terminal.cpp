#include "Terminal.h"

extern Game* CurrentGame;

using namespace sf;

Terminal::Terminal(sf::Vector2f position, AllianceType alliance) : Node(position, alliance, 32)
{

}