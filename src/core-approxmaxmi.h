double Eq1(int**, int, int, int);
double Eq2(int* , int, int, int);
double Eq3(int**, int*, int, int, int, int);
double Eq4(int**, int*, int, int, int, int);
double ApproxMaxMI(config, double*, double *, int);

int ClumpsPartition(double*, int, int*, int);
int EquipartitionAxis(double*, int, int, int*);

void Sort(double*, int*, int);
void Swap(double*, int*, int, int);
void Quicksort(double*, int*, int, int);
void DeallocateProblem (rapidmic_data*);
void AllocateProblem(rapidmic_data*, double);
void FixXpartition  (rapidmic_data*, double, int, double*, int*, int*);
void FixYpartition  (rapidmic_data*, double, int, double*, int*, int*);
void OptimizeAxis(double*, double*, int, int*, int, int*, int, int, double*);

rapidmic_score *ComputeScoreMIC(rapidmic_data*, double);
