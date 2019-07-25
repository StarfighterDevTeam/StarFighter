#include "Node.h"

extern Game* CurrentGame;

using namespace sf;

Node::Node(sf::Vector2f position, bool player) : GameObject(position, sf::Vector2f(0, 0), player ? sf::Color::Blue : sf::Color::Red, 16, 4)
{

}

Node::Node(sf::Vector2f position, bool player, float radius) : GameObject(position, sf::Vector2f(0, 0), player ? sf::Color::Blue : sf::Color::Red, radius, 4)
{

}