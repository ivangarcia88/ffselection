#include "mictools.h"

/*! ## Main function ##

Runs the MICTools main instance.*/
int main (int argc, char **argv)
{
	MICTools *app = new MICTools();
	app -> Run(argc, argv);
	delete app;
	return 0;
}