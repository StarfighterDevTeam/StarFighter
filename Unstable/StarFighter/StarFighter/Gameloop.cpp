#include "Gameloop.h"

extern Game* CurrentGame;

Gameloop::Gameloop()
{
	//background
	m_background = new GameObject(sf::Vector2f(960, 540), sf::Vector2f(0, 0), sf::Color::Black, sf::Vector2f(1920, 1080));
	(*CurrentGame).addToScene(m_background, BackgroundLayer, BackgroundObject);
	(*CurrentGame).m_map_size = m_background->m_size;

	//ship
	(*CurrentGame).m_playerShip = new Ship(sf::Vector2f(960, 540), sf::Vector2f(0, 0), sf::Color::White , sf::Vector2f(64, 64));
	(*CurrentGame).addToScene((*CurrentGame).m_playerShip, PlayerShipLayer, PlayerShip);
	(*CurrentGame).m_playerShip->setColor(sf::Color(255, 255, 255, 0));

	//node
	CreateTerminal(sf::Vector2f(200, 440), PlayerAlliance);
	Node* node_a = CreateNode(sf::Vector2f(450, 540), PlayerAlliance);
	Node* node_b = CreateNode(sf::Vector2f(650, 340), PlayerAlliance);
	CreateTerminal(sf::Vector2f(1920 - 600, 540), PlayerAlliance);

	CreateNode(sf::Vector2f(1920 - 380, 650), EnemyAlliance);
	CreateNode(sf::Vector2f(1920 - 300, 635), EnemyAlliance);
	CreateNode(sf::Vector2f(1920 - 220, 610), EnemyAlliance);
	CreateNode(sf::Vector2f(1920 - 250, 710), EnemyAlliance);

	CreateTerminal(sf::Vector2f(1920 - 700, 800), EnemyAlliance);
	CreateNode(sf::Vector2f(1920 - 900, 800), PlayerAlliance);

	CreateLink(node_a, node_b);
}

Gameloop::~Gameloop()
{
	delete m_background;

	for (vector<Node*>::iterator it = m_nodes.begin(); it != m_nodes.end(); it++)
	{
		delete *it;
	}
	m_nodes.clear();

	for (vector<Link*>::iterator it = m_links.begin(); it != m_links.end(); it++)
	{
		delete *it;
	}
	m_links.clear();
}

void Gameloop::Update(sf::Time deltaTime)
{
	//Get mouse & keyboard inputs
	(*CurrentGame).m_hovered_node = NULL;
	(*CurrentGame).GetMouseInputs(deltaTime);
	if ((*CurrentGame).m_mouse_click == Mouse_LeftClick)
	{
		if ((*CurrentGame).m_selected_node != NULL)
		{
			(*CurrentGame).m_selected_node->m_selected = false;
			(*CurrentGame).m_selected_node = NULL;
		}
	}

	//Update objects
	(*CurrentGame).updateScene(deltaTime);

	//Create new links on right click
	if ((*CurrentGame).m_hovered_node != NULL && (*CurrentGame).m_mouse_click == Mouse_RightClick && (*CurrentGame).m_selected_node != NULL && (*CurrentGame).m_selected_node != (*CurrentGame).m_hovered_node && (*CurrentGame).m_selected_node->m_alliance == PlayerAlliance && (*CurrentGame).m_hovered_node->m_alliance == PlayerAlliance)
	{
		//link already existing?
		bool found = false;
		for (vector<Node*>::iterator it = (*CurrentGame).m_hovered_node->m_linked_nodes.begin(); it != (*CurrentGame).m_hovered_node->m_linked_nodes.end(); it++)
		{
			if ((*it) == (*CurrentGame).m_selected_node)
			{
				found = true;
				break;
			}
		}

		if (found == false)
		{
			CreateLink((*CurrentGame).m_hovered_node, (*CurrentGame).m_selected_node);
		}
	}

	//Scroll camera
	UpdateCamera(deltaTime);
}

void Gameloop::Draw()
{
	(*CurrentGame).drawScene();
}

void Gameloop::UpdateCamera(sf::Time deltaTime)
{
	(*CurrentGame).m_view.move(sf::Vector2f((*CurrentGame).m_playerShip->m_speed.x * deltaTime.asSeconds(), (*CurrentGame).m_playerShip->m_speed.y * deltaTime.asSeconds()));

	//Map border constraints
	const float x = (*CurrentGame).m_view.getSize().x / 2;
	const float y = (*CurrentGame).m_view.getSize().y / 2;
	const float a = (*CurrentGame).m_playerShip->getPosition().x;
	const float b = (*CurrentGame).m_playerShip->getPosition().y;
	if (a < x)
		(*CurrentGame).m_view.setCenter(x, (*CurrentGame).m_view.getCenter().y);
	if (a >(*CurrentGame).m_map_size.x - x)
		(*CurrentGame).m_view.setCenter((*CurrentGame).m_map_size.x - x, (*CurrentGame).m_view.getCenter().y);
	if (b < y)
		(*CurrentGame).m_view.setCenter((*CurrentGame).m_view.getCenter().x, y);
	if (b >(*CurrentGame).m_map_size.y - y)
		(*CurrentGame).m_view.setCenter((*CurrentGame).m_view.getCenter().x, (*CurrentGame).m_map_size.y - y);
}

//Liaison 16
Node* Gameloop::CreateNode(sf::Vector2f position, AllianceType alliance)
{
	Node* node = new Node(position, alliance);
	(*CurrentGame).AddCircleObject(node, NodeType);
	m_nodes.push_back(node);

	return node;
}

Terminal* Gameloop::CreateTerminal(sf::Vector2f position, AllianceType alliance)
{
	Terminal* terminal = new Terminal(position, alliance);
	(*CurrentGame).AddCircleObject(terminal, NodeType);
	m_nodes.push_back(terminal);

	return terminal;
}

void Gameloop::CreateLink(Node* node_a, Node* node_b)
{
	Link* link = new Link(node_a, node_b);
	(*CurrentGame).AddLineObject(link);
	m_links.push_back(link);

	node_a->m_linked_nodes.push_back(node_b);
	node_b->m_linked_nodes.push_back(node_a);
}