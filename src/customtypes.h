#include <vector>


/*! ## Configuration data structure ##

Stores the configuration needed to perform
the algorithms within the MICTools software.
*/
typedef struct config{
	// Excecution params
	int maxThreads;			/*!< Maximum number of threads to use in algorithms computation. */ 
	bool performSAMIC;		/*!< Sets MICTools to perform the __SAMIC__ algorithm. */ 
	bool performPearson;	/*!< Sets MICTools to perform the __Pearson__ algorithm. */ 
	bool performParallelMIC;	/*!< Sets MICTools to perform the __ParallelMIC__ algorithm. */ 

// Common algorithm params
	double alpha;			/*!< Sets the alpha value for ParallelMIC and SAMIC. */ 

	// ParallelMIC params
	double minPearson;		/*!< Min pearson value to be use in order to compute parallelmic */
	double maxPearson;		/*!< Max pearson value to be use in order to compute parallelmic */
	bool usePearson; 		/*!< Sets if pearson is going to be used in ParallelMIC computation for certain values. */
	double c;				/*!< Sets the clumps value for ParallelMIC. */ 

	// SAMIC params
	double coolingFactor;	/*!< Temperature cooling factor to be usesd in SAMIC */
	double minParallelMIC;		/*!< Min parallelmic value to be use in order to compute SAMIC */
	double maxParallelMIC;		/*!< Max parallelmic value to be use in order to compute SAMIC */
	double minTemp;			/*!< Min temperature value to be used in SAMIC */
	bool useParallelMIC;		/*!< Sets if ParallelMIC is going to be used in SAMIC computation for certain values. */
	int annealingNeighbors;	/*!< States how many candidiate neighbors to generate at each temperature in the simulated annealing stage. */
} config;

/*! ## Result data structure ##

Stores the results from the different
algorithms in MICTools for a given pair
of variables.*/
typedef struct mine_result{
	int var1;				/*!< Indicates the fist variable of the result pair. */ 
	int var2;				/*!< Indicates the second variable of the result pair. */ 
	double samic;			/*!< Indicates result of the SAMIC computation. */ 
	double pearson;			/*!< Indicates result of the Pearson computation. */ 
	double parallelmic;		/*!< Indicates result of the ParallelMIC computation. */ 
} mine_result;


typedef struct grid{
	double min_x;
	double min_y;
	double max_x;
	double max_y;
	double fitness;
	unsigned int nx;
	unsigned int ny;
	double *placements_x;
	double *placements_y;
} grid;


// Configuration structure for MarsagliaRNG
// function in grid.cpp
typedef struct rng_config{
	unsigned long x;
	unsigned long y;
	unsigned long z;
} rng_config;


// Store Score information of ParallelMIC
typedef struct _mic_score
{
	int m;
	int *p;
	double **I;
} parallelmic_score;


// Store data information of ParallelMIC
typedef struct parallelmic_data 
{
	int n;
	int *Gy;
	int *idx_x;				/*!< Position of x ordered by x */
	int *idx_y;				/*!< Position of y ordered by x */
	double *x; 				/*!< Value of x */
	double *y;				/*!< Value of y */
	double *x_x;			/*!< Value of x ordered by x */
	double *y_y;			/*!< Value of y ordered by y */
	double *x_y;			/*!< Value of x ordered by y */
	double *y_x;			/*!< Value of y ordered by x */
	parallelmic_score *score;
} parallelmic_data;
