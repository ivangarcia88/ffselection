grid CreateGrid(int);

int GetMax1DPartition(double);
unsigned int GetPositionX(grid*, double);
unsigned int GetPositionY(grid*, double);

void PrintGridInfo(grid*);
void Reverse(double*, int);

bool EquipartitionX(grid*);
bool EquipartitionY(grid*);
bool CopyGrid(grid*, grid*);
bool ClearPlacements(grid*);
bool SortPlacementsX(grid*);
bool SortPlacementsY(grid*);
bool ArePlacementsXSorted(grid*);
bool ArePlacementsYSorted(grid*);
bool SetRandomPlacements(grid*, rng_config*);
bool SetPlacementX(grid*, unsigned int, double);
bool SetPlacementY(grid*, unsigned int, double);
bool SetResolution(grid*, unsigned int, unsigned int);
bool DiscoverMinMax(grid*, unsigned int, double*, double*);

unsigned long MarsagliaRNG(rng_config*);
double RandomPlacement(double, double, rng_config*);
double ComputeFitness(grid*, unsigned int, double*, double*);
