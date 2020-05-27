#include <string>
#include <vector>
#include <set>

#include "customtypes.h"
#include "parallelmic.h"
#include "pearson.h"
#include "samic.h"

/*! ## MICTools class ##

This class implements the architecture of the MICTools software. This architecture consists on several 
layers and class level data structures. Each of the layers in the architecture provides access to
presentation, Input/Output and MIC Analysis functionalities. The class level data structures stated in the 
architecture are accessible from any of the previously mentioned layers, and they serve the purpose of 
storing configurations, input data and the resulting data from each of the performed algorithms. This design
allows to easily add new algorithms to improve MIC analysis.

The following diagram shows the general design of the architecture.
![Caption text](/home/arturo/workspace/mic/oracle-cinvestav-feature-selection/docs/mictools-architecture.png "MICTools Architecture.")

*/
class MICTools {
private:
	// Private variables
	unsigned int mt_targetIndex;
	unsigned int mt_totalRows;
	unsigned int mt_totalCols;
	bool mt_useDefaultOptions;
	bool mt_useKeysFile;
	bool mt_targetFound;
	bool mt_useTarget;
	bool mt_header;
	double **mt_inputMatrix;

	config mt_params;
	std::string mt_targetName;
	std::string mt_keysFileName;
	std::string mt_inputFileName;
	std::string mt_outputFileName;
	std::set<std::string> mt_keysNames; 

	// Private functions
	std::vector<std::string> Split(std::string&, const char*);
	void StoreRow(std::vector<std::string>, std::vector<std::vector<double> >*);
	void SetDefaultOptions();
	bool ParseArgs(int argc, char **argv);
	bool CheckCurrentConfig();
	bool autoSetMaxThreads();
	bool AllocatePairs();
	bool WriteCSV();
	bool ReadKeys();
	bool ParallelMIC();
	bool Pearson();
	bool ReadCSV();
	bool SAMIC();

public:
	// Public variables
	std::vector<std::string> mt_rowNames;
	std::vector<std::string> mt_colNames;
	mine_result *mt_results;
	long mt_totalResults;

	// Public functions
	MICTools();
	~MICTools();
	void Run(int, char**);
};