/*
This program generates mictools.so, a Python module that wraps
some of the functions of MICTools. 

So far mictools.so only provides access to the execution pipeline 
and allows to retrieve the resulting computations as a large Python
list of objects.
*/

#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <list>

#include "mictools.h"

using namespace std;

/*
Returns a Python list containing the results computed by mictools in the following format:
[result[0].var1, result[0].var2, result[0].pearson, result[0].parallelmic, result[0].samic, ... ,result[n].var1, result[n].var2, result[n].pearson, result[n].parallelmic, result[n].samic]
*/
boost::python::list GetRawResults(long mt_totalResults, mine_result *mt_results, vector<string> mt_rowNames)
{
	boost::python::list rawResults;
	string var1;
	string var2;

	for (long i = 0; i < mt_totalResults; i++)
	{
		var1 = mt_rowNames[mt_results[i].var1];
		var2 = mt_rowNames[mt_results[i].var2];
		
		rawResults.append(var1);
		rawResults.append(var2);
		rawResults.append(mt_results[i].pearson);
		rawResults.append(mt_results[i].parallelmic);
		rawResults.append(mt_results[i].samic);
	}

	return rawResults;
}

// Executes mictools with the given parameters (just as the normal app)
boost::python::list Run(int argc, boost::python::list& pyargv)
{
	boost::python::list rawResults;
	MICTools *app;
	char **argv;
	int i;

	// Transform Python list of strings to char**
	list<string> arg_list;

	for (i = 0; i < argc; i++)
		arg_list.push_back(boost::python::extract<string>(pyargv[i]));

	i = 0;
	argv = new char*[arg_list.size()];
	for (list<string>::iterator it = arg_list.begin(); it != arg_list.end(); it++)
	{
		argv[i] = (char*) it -> c_str();
		i++;
	}

	// Run the mictools command and extract raw results
	app = new MICTools();
	app -> Run(argc, argv);
	rawResults = GetRawResults(app -> mt_totalResults, app -> mt_results, app -> mt_rowNames);

	// Deallocates the resources used during the computation of mictools
	delete app;

	return rawResults;
}


// Boost-Python wrapper syntax
using namespace boost::python;
BOOST_PYTHON_MODULE(mictools)
{
  def("Run", Run);
}
