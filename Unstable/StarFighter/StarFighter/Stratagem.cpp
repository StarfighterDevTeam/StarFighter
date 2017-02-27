#include "Stratagem.h"

extern Game* CurrentGame;

using namespace sf;

Stratagem::Stratagem()
{
	Init();
}

Stratagem::Stratagem(int nb_code)
{
	Init();

	if (nb_code > MAX_CODES)
	{
		nb_code = MAX_CODES;
	}

	for (int i = 0; i < nb_code; i++)
	{
		code[i] = RandomizeIntBetweenValues(1, 4);
	}
}

void Stratagem::Init()
{
	for (int i = 0; i < MAX_CODES; i++)
	{
		code[i] = 0;
	}
}

Stratagem::~Stratagem()
{
	
}