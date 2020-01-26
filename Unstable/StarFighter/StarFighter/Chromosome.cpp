#include "Chromosome.h"

extern Game* CurrentGame;

using namespace sf;

Chromosome::Chromosome()
{
	
}

Chromosome::~Chromosome()
{

}

void Chromosome::UpdateDNA(Wing* wing, Node* finish)
{
	m_dna[0] = Lerp(wing->getPosition().x, 0, (*CurrentGame).m_map_size.x, 0, 1);
	m_dna[1] = Lerp(wing->getPosition().y, 0, (*CurrentGame).m_map_size.y, 0, 1);

	m_dna[2] = Lerp(finish->getPosition().x, 0, (*CurrentGame).m_map_size.x, 0, 1);
	m_dna[3] = Lerp(finish->getPosition().y, 0, (*CurrentGame).m_map_size.y, 0, 1);

	L16Entity* missile = NULL;
	for (L16Entity* entity : (*CurrentGame).m_L16_entities)
	{
		if (entity->m_L16_type == L16Entity_Ballistic && entity->m_alliance == PlayerAlliance)
		{
			missile = entity;
			break;
		}
	}

	m_dna[4] = missile == NULL ? 0 : 1;
	m_dna[5] = wing->m_weapons.front()->IsReadyToFire() ? 0 : 1;
	m_dna[6] = missile == NULL ? 0 : Lerp(missile->getPosition().x, 0, (*CurrentGame).m_map_size.x, 0, 1);
	m_dna[7] = missile == NULL ? 0 : Lerp(missile->getPosition().y, 0, (*CurrentGame).m_map_size.y, 0, 1);

	int i = 0;
	for (L16Entity* entity : (*CurrentGame).m_L16_entities)
	{
		if (i < 10 && entity->m_L16_type == L16Entity_Node && entity->m_alliance == EnemyAlliance)// && entity->m_visible == true)
		{
			L16Entity* enemy_radar = entity;
			m_dna[8 + (3 * i) + 0] = 1;
			m_dna[8 + (3 * i) + 1] = Lerp(enemy_radar->getPosition().x, 0, (*CurrentGame).m_map_size.x, 0, 1);
			m_dna[8 + (3 * i) + 2] = Lerp(enemy_radar->getPosition().y, 0, (*CurrentGame).m_map_size.y, 0, 1);

			i++;
		}
	}

	for (int j = i; j < 10; j++)
	{
		m_dna[8 + (3 * j) + 0] = 0;
		m_dna[8 + (3 * j) + 1] = 0;
		m_dna[8 + (3 * j) + 2] = 0;
	}
}

void Chromosome::MagicFunction(float& output_direction_x, float& output_fire)
{
	//m_dna -> ouputs
}