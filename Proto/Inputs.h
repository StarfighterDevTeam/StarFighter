#pragma once
#ifndef INPUTS_H_INCLUDED
#define INPUTS_H_INCLUDED

#include "SEInputs.h"

class Inputs : public SEInputs
{
public:
	bool isFiring(ControlerType device = AllControlDevices);
};

#endif // INPUTS_H_INCLUDED
