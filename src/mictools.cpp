#include <sys/time.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include <set>

#include "mictools.h"

using namespace std;


/*! ## MICTools class constructor##

Sets the common configuration for all of the algorithms of MICTools to a safe default value.
*/
MICTools::MICTools()
{
	mt_params.performParallelMIC = false;
	mt_params.performPearson = false;
	mt_params.performSAMIC = false;
	mt_params.maxThreads = 0;

	mt_params.usePearson = false;
	mt_params.minPearson = 0.0;
	mt_params.maxPearson = 1.0;
	mt_params.alpha = 0.6;
	mt_params.c = 15;

	mt_params.annealingNeighbors = 25;
	mt_params.coolingFactor = 0.9;
	mt_params.useParallelMIC = false;
	mt_params.minParallelMIC = 0.0;
	mt_params.maxParallelMIC = 1.0;
	mt_params.minTemp = 0.00001;

	mt_useDefaultOptions = false;
	mt_targetFound = false;
	mt_useKeysFile = false;
	mt_useTarget = false;
	mt_totalResults = 0;
	mt_targetIndex = 0;
	mt_totalRows = 0;
	mt_totalCols = 0;
	mt_results = NULL;
	mt_header = false;

	mt_targetName.clear();
	mt_keysNames.clear();
}


MICTools::~MICTools()
{
	unsigned int i;
	if (mt_results != NULL)
		delete []mt_results;

	if (mt_inputMatrix != NULL)
	{
		for (i = 0; i < mt_totalRows; i++)
			delete []mt_inputMatrix[i];
	}
	delete []mt_inputMatrix;
	mt_inputMatrix = NULL;
}


/*! 
Receives a string of arguments and runs the program as stated in the parsed options.
*/
void MICTools::Run(int argc, char** argv)
{
	if (ParseArgs(argc, argv))
	{
		// Print computation config and schedule
		printf("\n--- Oracle MICTools ---\n\n");
		printf("Input file: %s\n", mt_inputFileName.c_str());
		printf("Output file: %s\n", mt_outputFileName.c_str());	

		// Print the max amount of threads to use in the computation as AUTO
		// if no thread amount was explicitly specified by the user 
		if (mt_params.maxThreads == 0)
			printf("Max threads: AUTO\n\n");
		else
			printf("Max threads: %d\n\n", mt_params.maxThreads);
		
		if (mt_params.performPearson || mt_params.performParallelMIC || mt_params.performSAMIC)
		{
			printf("Analysis schedule:\n");
			if (mt_params.performPearson)
				printf("  - Pearson\n");
			if (mt_params.performParallelMIC)
				printf("  - ParallelMIC\n");
			if (mt_params.performSAMIC)
				printf("  - SAMIC\n");
			printf("\n");
		}

		printf("Reading %s...\n", mt_inputFileName.c_str());
		
		if(mt_useKeysFile)
		{
			printf("Keys file: %s\n", mt_keysFileName.c_str());
			if (!ReadKeys())
				return;
		}

		if(mt_useTarget)
			printf("Target variable: %s\n", mt_targetName.c_str());

		if (!ReadCSV())
			return;
		printf("Done!\n\n");
		
		printf("Total rows: %d\n", mt_totalRows);
		printf("Total cols: %d\n", mt_totalCols);

		if (!AllocatePairs())
			return;

		// If the maximum number of threads to use in the program was not provided by the user,
		// a reasonable amount of threads to assign to the task will be computed. The amount will
		// depend on the size of the dataset and the availability of processor cores.
		if (mt_params.maxThreads == 0)
			autoSetMaxThreads();

		struct timeval timeStart;		
		struct timeval timeEnd;
		unsigned long timeDiff;

		gettimeofday(&timeStart, NULL);
		if (mt_params.performPearson)
			Pearson();
		
		if (mt_params.performParallelMIC)
			ParallelMIC();

		if (mt_params.performSAMIC)
			SAMIC();
		gettimeofday(&timeEnd, NULL);

		timeDiff = 1e6 * (timeEnd.tv_sec - timeStart.tv_sec) + timeEnd.tv_usec - timeStart.tv_usec;
		printf("\nAnalysis completed in %lu ms\n", timeDiff / 1000);

		// If input file was specified call writting operation
		if (mt_outputFileName != "")
		{
			printf("\nWriting %s...\n", mt_outputFileName.c_str());
			WriteCSV();
		}
		printf("Done!\n\n");
	}
	else
	{
		printf("\n--- Oracle MICTools ---\n\n");
		printf("Usage: ./mictools [options]\n\n");
		printf("General options:\n");
		printf("    -a, --alpha\n        [double] Alpha value for MIC calculations. Alpha is a value in the range (0, 1].\n");
		printf("    -i, --input\n        [str] Input CSV file.\n");
		printf("    -o, --output\n        [str] Output results file.\n");
		printf("    -f, --keys_file\n        [str] Filter keys file. Restricts the generated pairs to be constructed only with those variables present in the file.\n");
		printf("    -g, --target\n        [str] Name of the variable against which all of the remaining variables will be paired.\n");
		printf("    -h, --header\n        Indicates that the input file contains a Header line.\n");
		printf("    -t, --max_threads\n        [int] Max number of threads to use during the computation.\n");
		printf("    -d, --default\n        Forces the program to run Pearson, ParallelMIC and SAMIC using their default parameters.\n\n");
		
		printf("Analysis options:\n");
		printf("    -P, --Pearson\n        Includes Pearson in the analysis schedule.\n");
		printf("    -R, --ParallelMIC\n        Includes ParallelMIC in the analysis schedule.\n");
		printf("    -S, --SAMIC\n        Includes SAMIC in the analysis schedule.\n\n");
		
		printf("ParallelMIC options:\n");
		printf("    -u, --clumps\n        [double] Number of clumps (must be larger than 0).\n");
		printf("    -p, --min_pearson\n        [double] Sets ParallelMIC to compute only those pairs whose Pearson coefficient absolute\n                 value is above the given value. This value must be in the range [0, 1].\n");
		printf("    -e, --max_pearson\n        [double] Sets ParallelMIC to compute only those pairs whose Pearson coefficient absolute\n                 value is below the given value. This value must be in the range [0, 1].\n\n");
		
		printf("SAMIC options:\n");
		printf("    -n, --neighbors\n        [int] SAMIC amount of candidate neighbors to consider for each temperature of the simulated annealing stage.\n");
		printf("    -c, --cooling\n        [double] Temperature cooling factor to be usesd in SAMIC. It is a value in the range (0, 1).\n");
		printf("    -m, --min_temp\n        [double] Minimum temperature value to be used in SAMIC. It is a value in the range (0, 1).\n");
		printf("    -j, --min_parallelmic\n        [double] SAMIC min ParallelMIC value. Restricts computation to pairs with ParallelMIC score above the given value.\n");
		printf("    -k, --max_parallelmic\n        [double] SAMIC max ParallelMIC value. Restricts computation to pairs with ParallelMIC score below the given value.\n\n");
	}
}

/*!
Set the suggested values of the parameters for the algorithms.
*/
void MICTools::SetDefaultOptions()
{
	mt_params.performPearson = true;
	mt_params.performParallelMIC = true;
	mt_params.performSAMIC = true;
	
	mt_params.usePearson = true;
	mt_params.minPearson = 0.2;
	mt_params.maxPearson = 1.0;
	
	mt_params.useParallelMIC = true;
	mt_params.minParallelMIC = 0.2;
	mt_params.maxParallelMIC = 0.9;
}

/*! 
Receives a string of arguments and parses them to set the program's common configuration variables.
Returns __true__ if all of the arguments could be parsed correctly or __false__ otherwise. To see
a full list of available argument flags please run the MICTools program without arguments.
*/
bool MICTools::ParseArgs(int argc, char **argv)
{
	optind = 0;
	int c;
	int optionIndex = 0;
	
	bool success = false;

	// Use getopt_long to parse argument options
	while (true)
	{
		static struct option long_options[] =
		{
			{"input",			required_argument, 0, 'i'},
			{"output",			required_argument, 0, 'o'},
			{"keys_file",		required_argument, 0, 'f'},
			{"target",			required_argument, 0, 'g'},
			{"alpha",			required_argument, 0, 'a'},
			{"clumps",			required_argument, 0, 'u'},
			{"cooling",			required_argument, 0, 'c'},
			{"min_temp",		required_argument, 0, 'm'},
			{"neighbors",		required_argument, 0, 'n'},
			{"max_threads",		required_argument, 0, 't'},
			{"min_pearson",		required_argument, 0, 'p'},
			{"max_pearson",		required_argument, 0, 'e'},
			{"min_parallelmic",	required_argument, 0, 'j'},
			{"max_parallelmic",	required_argument, 0, 'k'},
			{"default",			no_argument, 0, 'd'},
			{"header",			no_argument, 0, 'h'},
			{"SAMIC",			no_argument, 0, 'S'},
			{"Pearson",			no_argument, 0, 'P'},
			{"ParallelMIC",		no_argument, 0, 'R'},
			{0, 0, 0, 0}
		};

		// x: means that the parameter "x" is going to receive an argument
		c = getopt_long(argc, argv, "i:o:f:g:a:u:c:m:n:t:p:e:j:k:dhPRS", long_options, &optionIndex);

		// Break the cycle once all options have been found
		if (c == -1)
			break;

		switch (c)
		{
			// General options
			case 'i':
				mt_inputFileName = optarg;
				success = true;
				break;

			case 'o':
				mt_outputFileName = optarg;
				success = true;
				break;

			case 'f':
				mt_useKeysFile = true;
				mt_keysFileName = optarg;
				success = true;
				break;

			case 'g':
				mt_useTarget = true;
				mt_targetName = optarg;
				success = true;
				break;

			case 'd':
				mt_useDefaultOptions = true;
				success = true;
				break;

			case 't':
				mt_params.maxThreads = atoi(optarg);
				success = true;
				break;

			case 'h':
				mt_header = true;
				success = true;
				break;

			// Analysis options
			case 'P':
				mt_params.performPearson = true;
				success = true;
				break;

			case 'R':
				mt_params.performParallelMIC = true;
				success = true;
				break;

			case 'S':
				mt_params.performSAMIC = true;
				success = true;
				break;

			// ParallelMIC options
			case 'a':
				mt_params.alpha = atof(optarg);
				success = true;
				break;

			case 'u':
				mt_params.c = atof(optarg);
				success = true;
				break;

			case 'p':
				mt_params.minPearson = atof(optarg);
				mt_params.usePearson = true;
				success = true;
				break;
	
			case 'e':
				mt_params.maxPearson = atof(optarg);
				mt_params.usePearson = true;
				success = true;
				break;

			// SAMIC options
			case 'c':
				mt_params.coolingFactor = atof(optarg);
				success = true;
				break;

			case 'm':
				mt_params.minTemp = atof(optarg);
				success = true;
				break;

			case 'n':
				mt_params.annealingNeighbors = atoi(optarg);
				success = true;
				break;

			case 'j':
				mt_params.minParallelMIC = atof(optarg);
				mt_params.useParallelMIC = true;
				success = true;
				break;

			case 'k':
				mt_params.maxParallelMIC = atof(optarg);
				mt_params.useParallelMIC = true;
				success = true;
				break;
			default:
				break;
		}
	}
	
	// Check if the parsed configuration is valid
	if (success)
		success = CheckCurrentConfig();

	return success;
}


/*! 
Reads the common configuration data structure returning __true__ if all of its variables are set to safe values, 
returns __false__ otherwise.
*/
bool MICTools::CheckCurrentConfig()
{
	bool success = true;

	if (mt_useDefaultOptions)
	{
		printf("WARNING: Using the default parameters.\n");
		SetDefaultOptions();
	}

	if (mt_params.performSAMIC)
	{
		if (mt_params.coolingFactor <= 0 || mt_params.coolingFactor >= 1)
		{
			printf("  * The cooling factor option (-c) must be followed by a number in the range (0, 1].\n");
			success = false;
		}
		if (mt_params.annealingNeighbors < 0)
		{
			printf("  * The annealing neighbors option (-n) must be an integer equal or greater than 0.\n");
			success = false;
		}
		if (mt_params.useParallelMIC && mt_params.performParallelMIC)
		{
			if (mt_params.minParallelMIC < 0 || mt_params.minParallelMIC > 1)
			{
				printf("  * Min ParallelMIC option (-j) must be followed by a number in the range (0, 1].\n");
				success = false;
			}
			if (mt_params.maxParallelMIC < 0 || mt_params.maxParallelMIC > 1)
			{
				printf("  * Max ParallelMIC option (-k) must be followed by a number in the range (0, 1].\n");
				success = false;
			}
			if (mt_params.maxParallelMIC < mt_params.minParallelMIC)
			{
				printf("  * Max ParallelMIC option (-k) must be larger than the Min ParallelMIC option (-j).\n");
				success = false;
			}
		}
	}
	if (mt_params.performParallelMIC)
	{
		if (mt_params.alpha < 0 || mt_params.alpha > 1)
		{
			printf("  * Alpha option (-a) must be followed by a number in the range (0, 1].\n");
			success = false;
		}
		if (mt_params.c <= 0)
		{
			printf("  * Clumps option (-c) must be followed by a number larger than 0.\n");
			success = false;
		}
		if(mt_params.usePearson && mt_params.performPearson)
		{
			if (mt_params.minPearson < 0 || mt_params.minPearson > 1)
			{
				printf("  * ParallelMIC's minimum Pearson option (-p) must be in the range [0, 1].\n");
				success = false;
			}
			if (mt_params.maxPearson < 0 || mt_params.maxPearson > 1)
			{
				printf("  * Max Pearson option (-e) must be followed by a number in the range (0, 1].\n");
				success = false;
			}
			if (mt_params.maxPearson < mt_params.minPearson)
			{
				printf("  * Max Pearson option (-e) must be larger than the Min Pearson option (-p).\n");
				success = false;
			}
		}
	}
	if (!(mt_params.performPearson || mt_params.performParallelMIC || mt_params.performSAMIC))
	{
		printf("  * No analysis options specified. Please specify which algorithms to run using at least one of the following flags:\n    -P Pearson\n    -R ParallelMIC\n    -S SAMIC\n");
		success = false;
	}
	if (mt_params.maxThreads < 0 || mt_params.maxThreads > sysconf(_SC_NPROCESSORS_ONLN))
	{
		printf("  * Warning: The number of threads specified is not safe, it must be greater than zero and less or equal to %ld (the number of threads supported by your system).\n", sysconf(_SC_NPROCESSORS_ONLN));
	}
	if (mt_inputFileName == "")
	{
		printf("  * Input file not defined, use the (-i) option to define it.\n");
		success = false;
	}
	if (mt_useKeysFile)
	{
		if (mt_keysFileName == "")
		{
			printf("  * Keys file not defined, use the (-f) option to define it.\n");
			success = false;
		}
	}
	if (mt_useTarget)
	{
		if (mt_targetName == "")
		{
			printf("  * Target not defined, use the (-g) option to define it.\n");
			success = false;
		}	
	}

	return success;
}


/*! 
Receives a string __originalString__ and a char __delimiter__. Returns an array of the strings in which resulted from
the separation of the __originalString__ at each __delimiter__ found.
*/
vector<string> MICTools::Split(string& originalString, const char* delimiter)
{
	char *token;
	char *c_string;
	vector<string> resultVector;

	c_string = new char[originalString.size() + 1];
	strcpy(c_string, originalString.c_str());
	
	token = strtok(c_string, delimiter);
	while(token != NULL)
	{
		resultVector.push_back(token);
		token = strtok(NULL, delimiter);
	}

	delete token;
	delete[] c_string;
	return resultVector;
}

/*! 
Saves the contents of the results array __mt_results__ into a CSV file.
*/
bool MICTools::WriteCSV()
{
	int var1;
	int var2;
	if (mt_totalResults > 0)
	{
		if (mt_rowNames.size() > 0)
		{
			ofstream outputfile(mt_outputFileName.c_str());
			if (outputfile.is_open())
			{
				outputfile << "var1,var2,pearson,parallelmic,samic" << endl;
				for (unsigned int i = 0; i < mt_totalResults; i++)
				{
					var1 = mt_results[i].var1;
					var2 = mt_results[i].var2;
					outputfile << mt_rowNames[var1] << "," << mt_rowNames[var2] << ","  << mt_results[i].pearson << "," << mt_results[i].parallelmic << "," << mt_results[i].samic << endl;
				}
			}
			else
			{
				printf("ERROR: Output file could not be created!\n");
				return false;
			}
			outputfile.close();
		}
	}
	else
	{
		printf("ERROR: No results obtained from computation\n");
		return false;
	}
	return  true;
}


void MICTools::StoreRow(vector<string> splitedLine, vector<vector<double> > *inputDataVector)
{
	vector<double> currentRow;
	unsigned int i;

	currentRow.clear();
	
	// If use a target save the index of the target
	if (mt_useTarget)
	{
		if (mt_targetName.compare(splitedLine[0].c_str()) == 0)
		{
			mt_targetIndex = mt_totalRows;
			mt_targetFound = true;
		}

		// If use a target and a key file
		if (mt_useKeysFile)
		{
			// If the row name is different from a key and is not a target return
			if (mt_keysNames.find(splitedLine[0].c_str()) == mt_keysNames.end() && 
				mt_targetName.compare(splitedLine[0].c_str()) != 0)
				return;
		}
	}
	else if (mt_useKeysFile)
	{
		// If the row name is different from the keys, return
		if (mt_keysNames.find(splitedLine[0].c_str()) == mt_keysNames.end())
			return;
	}
	
	// Store the line
	for (i = 1; i < mt_totalCols; ++i)
		currentRow.push_back(atof(splitedLine[i].c_str()));
	inputDataVector -> push_back(currentRow);
	mt_rowNames.push_back(splitedLine[0]);
	mt_totalRows ++;
}

/*! 
Parses the CSV file specified in __mt_inputFileName__ to several data structures (mt_colNames, mt_rowNames, mt_inputMatrix, mt_totalRows, mt_totalCols).
*/
bool MICTools::ReadCSV()
{
	vector<vector<double> > inputDataVector;
	unsigned int i;
	unsigned int j;
	bool firstLine;
	string line;

	inputDataVector.clear();
	firstLine = true;
	mt_totalRows = 0;
	mt_totalCols = 0;

	ifstream inputFile (mt_inputFileName.c_str());
	if (inputFile.is_open())
	{
		// Read from file to dynamically generated vectors
		while (getline(inputFile, line))
		{
			vector<string> splitedLine = Split(line, ",");

			// If the current line is the first line
			if (firstLine)
			{
				mt_totalCols = splitedLine.size();
				firstLine = false;

				// If a header is expected
				if (mt_header)
				{
					for (i = 1; i < mt_totalCols; ++i)
						mt_colNames.push_back(splitedLine[i]);
				}
			}

			// Check if the line has as many columns as expected
			else if (!firstLine && splitedLine.size() != mt_totalCols)
			{
				printf("ERROR: Row with incorrect number of cols.\n");
				return false;
			}

			StoreRow(splitedLine, &inputDataVector);
		}

		// Copy data from vectors to permanent arrays
		mt_totalCols --;
		mt_inputMatrix = new double* [mt_totalRows];
		for (i = 0; i < mt_totalRows; i++)
		{
			mt_inputMatrix[i] = new double[mt_totalCols];
			for (j = 0; j < mt_totalCols; j++)
				mt_inputMatrix[i][j] = inputDataVector[i][j];
		}

		// Free resources
		inputDataVector.clear();
		inputFile.close();
	}
	else
	{
		printf("ERROR: Unable to open file %s\n", mt_inputFileName.c_str());
		return false;
	}
	return true;
}

/*! 
Read the keys and store in a set.
*/
bool MICTools::ReadKeys()
{
	string line;
	
	ifstream keysFile (mt_keysFileName.c_str());
	if (keysFile.is_open())
	{
		while (getline(keysFile, line))
			mt_keysNames.insert(line.c_str());

		keysFile.close();
	}
	else
	{
		printf("ERROR: Unable to open file %s\n", mt_keysFileName.c_str());
		return false;
	}

	return true;
}

/*! 
Allocates the memory needed to store the array of results.
*/
bool MICTools::AllocatePairs()
{
	unsigned int i;
	unsigned int j;
	int pair;
	long double allocatedBytes;

	if (mt_useTarget)
	{
		if (!mt_targetFound)
		{
			printf("\nTarget not found: %s\n", mt_targetName.c_str());
			return false;
		}
		mt_totalResults = mt_totalRows - 1;
	}
	else
	{
		mt_totalResults = mt_totalRows * (mt_totalRows - 1) / 2;
	}
	
	allocatedBytes = mt_totalResults * (int) sizeof(mine_result);
	if (allocatedBytes >= 1073741824)
		printf("\nAllocating %.2Lf GB to store %ld results...\n", allocatedBytes / 1073741824, mt_totalResults);
	else if (allocatedBytes >= 1048576)
		printf("\nAllocating %.2Lf MB to store %ld results...\n", allocatedBytes / 1048576, mt_totalResults);
	else if (allocatedBytes >= 1024)
		printf("\nAllocating %.2Lf KB to store %ld results...\n", allocatedBytes / 1024, mt_totalResults);
	else
		printf("\nAllocating %.2Lf KB to store %ld results...\n", allocatedBytes, mt_totalResults);

	
	mt_results = new mine_result[mt_totalResults];

	pair = 0;

	if (mt_useTarget)
	{
		for (i = 0; i < mt_totalRows; i++)
		{
			if (i != mt_targetIndex){
				mt_results[pair].var1 = mt_targetIndex;
				mt_results[pair].var2 = i;
				mt_results[pair].samic = 0;
				mt_results[pair].pearson = 0;
				mt_results[pair].parallelmic = 0;
				pair ++;
			}
		}
	}
	else
	{
		for (i = 0; i < mt_totalRows; i++)
		{
			for (j = i + 1; j < mt_totalRows; j++)
			{
				mt_results[pair].var1 = i;
				mt_results[pair].var2 = j;
				mt_results[pair].samic = 0;
				mt_results[pair].pearson = 0;
				mt_results[pair].parallelmic = 0;
				pair ++;
			}
		}
	}
	return true;
}


/*!
Automatically configure a reasonable amount of threads to assign to the MIC computing tasks.
The amount of threads depends on the size of the analysed dataset and the availability of processor cores.
*/
bool MICTools::autoSetMaxThreads()
{
	long int totalCores;
	int t;

	totalCores = sysconf(_SC_NPROCESSORS_ONLN);
	t = 1.0 / 500.0 * mt_totalResults + 1;

	if (t > totalCores)
		t = totalCores;
	if (t < 1)
		t = 1;

	mt_params.maxThreads = t;
	return true;
}


/*! 
Computes the Pearson correlation for each of the pairs allocated in the array of results.
*/
bool MICTools::Pearson()
{
	printf("\nPerforming Pearson analysis over %d thread(s)...\n", mt_params.maxThreads);
	return PerformPearson(mt_inputMatrix, mt_totalRows, mt_totalCols, mt_results, mt_totalResults, mt_params.maxThreads);
}


/*! 
Computes the ParallelMIC algorithm for each of the pairs allocated in the array of results.
*/
bool MICTools::ParallelMIC()
{
	printf("\nPerforming ParallelMIC analysis over %d thread(s)...\n", mt_params.maxThreads);
	if (mt_params.performParallelMIC)
	{
		printf("ParallelMIC params:\n");
		printf("  Alpha: %.2f\n", mt_params.alpha);
		printf("  Clumps: %.2f\n", mt_params.c);
		if (mt_params.usePearson)
		{
			printf("  Min Pearson: %.2f\n", mt_params.minPearson);
			printf("  Max Pearson: %.2f\n", mt_params.maxPearson);
		}
		return PerformParallelMIC(mt_inputMatrix, mt_totalRows, mt_totalCols, mt_results, mt_totalResults, mt_params.maxThreads, mt_params	);
	}
	return false;
}


/*! 
Computes the SAMIC algorithm for each of the pairs allocated in the array of results.
*/
bool MICTools::SAMIC()
{
	printf("\nPerforming SAMIC analysis over %d thread(s)...\n", mt_params.maxThreads);
	if (mt_params.performSAMIC)
	{
		printf("SAMIC params:\n");
		printf("  Alpha: %.2f\n", mt_params.alpha);
		printf("  Cooling factor: %.2f\n", mt_params.coolingFactor);
		printf("  Min temperature: %f\n", mt_params.minTemp);
		printf("  Annealing neighbors: %d\n", mt_params.annealingNeighbors);
		if (mt_params.useParallelMIC)
		{
			printf("  Min ParallelMIC: %.2f\n", mt_params.minParallelMIC);
			printf("  Max ParallelMIC: %.2f\n", mt_params.maxParallelMIC);
		}
		return PerformSAMIC(mt_inputMatrix, mt_totalRows, mt_totalCols, mt_results, mt_totalResults, mt_params.maxThreads, mt_params);
	}
	return false;
}
