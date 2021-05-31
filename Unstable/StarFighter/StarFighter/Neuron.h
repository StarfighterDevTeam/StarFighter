#ifndef NEURON_H_INCLUDED
#define NEURON_H_INCLUDED



#define _USE_MATH_DEFINES
//#include <assert.h>
//#include <math.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <math.h>
//#include <cmath>
//#include <string>
//#include <cstdlib>
//#include <stdio.h>      /* printf, scanf, puts, NULL */
//#include <stdlib.h>     /* srand, rand */
//#include <iostream>		/* cin */
//#include <fstream>		/* ofstream */
//#include <sstream>		/*istringstream */
//#include <ctime>			/* clock */

#include "Globals.h"
#include "Data.h"

class Neuron
{
public:
	double m_value;
	double m_input_value;
	vector <double> m_weights;
	vector <double> m_deltaWeights;
	double m_error;
	double m_gradient;
};

enum LayerType
{
	InputLayer,		//0
	HiddenLayer,	//1
	OutpuLayer,		//2
};

class Layer
{
public:
	Layer(int nb_neuron, LayerType type);

	LayerType m_type;
	int m_nb_neurons;
	vector<Neuron> m_neurons;
};

#endif // NEURON_H_INCLUDED