#include "Terminal.h"

extern Game* CurrentGame;

using namespace sf;

Terminal::Terminal(sf::Vector2f position, bool player) : Node(position, player, 32)
{

}