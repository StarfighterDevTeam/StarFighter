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
	Terminal* t = CreateTerminal(sf::Vector2f(300, 440), PlayerAlliance);
	Wing* w = CreateWing(sf::Vector2f(400, 300), PlayerAlliance, 0);
	w->m_selected = true;
	(*CurrentGame).m_selected_entities.push_back(w);

	Node* node_a = CreateNode(sf::Vector2f(450, 540), PlayerAlliance);
	Node* node_b = CreateNode(sf::Vector2f(650, 340), PlayerAlliance);
	//CreateTerminal(sf::Vector2f(1920 - 600, 540), PlayerAlliance);

	//CreateNode(sf::Vector2f(1920 - 380, 650), EnemyAlliance);
	//CreateNode(sf::Vector2f(1920 - 300, 635), EnemyAlliance);
	//CreateNode(sf::Vector2f(1920 - 220, 610), EnemyAlliance);
	//CreateNode(sf::Vector2f(1920 - 250, 710), EnemyAlliance);

	Terminal* t2 = CreateTerminal(sf::Vector2f(1920 - 700, 800), EnemyAlliance);
	t2->CreateWing();
	CreateNode(sf::Vector2f(1920 - 1000, 800), PlayerAlliance);

	CreateLink(node_a, node_b);
}

Gameloop::~Gameloop()
{
	delete m_background;
}

void Gameloop::Update(sf::Time deltaTime)
{
	//Get mouse & keyboard inputs
	(*CurrentGame).m_hovered_entities.clear();
	(*CurrentGame).GetMouseInputs(deltaTime);
	if ((*CurrentGame).m_mouse_click == Mouse_LeftClick)
	{
		for (vector<L16Entity*>::iterator it = (*CurrentGame).m_selected_entities.begin(); it != (*CurrentGame).m_selected_entities.end(); it++)
		{
			(*it)->m_selected = false;
			(*it)->ResetColor();
		}
		(*CurrentGame).m_selected_entities.clear();
	}

	//Update objects
	(*CurrentGame).updateScene(deltaTime);

	//Create new links on right click
	L16Entity* selected = NULL;
	L16Entity* hovered = NULL;
	if ((*CurrentGame).m_selected_entities.size() == 1)
		selected = (*CurrentGame).m_selected_entities.front();

	if ((*CurrentGame).m_hovered_entities.size() == 1)
		hovered = (*CurrentGame).m_hovered_entities.front();

	if (hovered != NULL && selected != NULL && (*CurrentGame).m_mouse_click == Mouse_RightClick && selected != hovered && selected->m_alliance == PlayerAlliance && hovered->m_alliance == PlayerAlliance && hovered->m_L16_type == L16Entity_Node && selected->m_L16_type == L16Entity_Node)
	{
		//link already existing?
		bool found = false;
		Node* hovered_node = (Node*)hovered;
		Node* selected_node = (Node*)selected;
		for (vector<L16Entity*>::iterator it = hovered_node->m_linked_entities.begin(); it != hovered_node->m_linked_entities.end(); it++)
		{
			if ((*it) == selected_node)
			{
				found = true;
				break;
			}
		}

		if (found == false)
		{
			CreateLink(hovered_node, selected_node);
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
	(*CurrentGame).AddCircleObject(node);
	(*CurrentGame).m_nodes.push_back(node);

	return node;
}

Terminal* Gameloop::CreateTerminal(sf::Vector2f position, AllianceType alliance)
{
	Terminal* terminal = new Terminal(position, alliance);
	(*CurrentGame).AddCircleObject(terminal);
	(*CurrentGame).m_nodes.push_back(terminal);

	return terminal;
}

Link* Gameloop::CreateLink(Node* node_a, Node* node_b)
{
	Link* link = new Link(node_a, node_b);
	(*CurrentGame).AddLineObject(link);
	(*CurrentGame).m_links.push_back(link);

	node_a->m_linked_entities.push_back(node_b);
	node_b->m_linked_entities.push_back(node_a);

	return link;
}

Wing* Gameloop::CreateWing(sf::Vector2f position, AllianceType alliance, float heading)
{
	Wing* wing = new Wing(position, alliance, heading);
	(*CurrentGame).AddCircleObject(wing);
	(*CurrentGame).m_wings.push_back(wing);
	
	return wing;
}