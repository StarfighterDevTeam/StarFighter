#ifndef STRATAGEM_H_INCLUDED
#define STRATAGEM_H_INCLUDED

#include "Globals.h"
#include "GameObject.h"
#include "Game.h"
#include "SFTextPop.h"

#define MAX_CODES   10

enum ItemType
{
	ItemEmpty,
	ItemGlasses,
	ItemWallet,
	NB_ItemTypes,
};

class Stratagem
{
public :
	Stratagem();
	Stratagem(int nb_code);
	void Init();
	virtual ~Stratagem();

	int code[MAX_CODES];
};

#endif // STRATAGEM_H_INCLUDED
