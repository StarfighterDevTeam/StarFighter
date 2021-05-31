#ifndef DATA_H_INCLUDED
#define DATA_H_INCLUDED

using namespace std;

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
#include <vector>

enum Label
{
	IS_NOT_ROMAIN,
	IS_ROMAIN,
	UNLABELLED,
};

enum Features
{
	RED,			//0
	GREEN,			//1
	BLUE,			//2
	NB_FEATURES,	//3
};

class Data
{
public:
	Data(vector<double> features, Label label);
	Data(Label label, double error_margin);

	vector<double> m_features;
	Label m_label;
};


#endif // DATA_H_INCLUDED