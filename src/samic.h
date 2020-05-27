
double SAmic(int, long);
double RandomChoice(rng_config*);
double AceptanceProbability(double, double, double);

void *SAmicThread(void*);
bool PerformSAMIC(double**, int, int, mine_result*, long, int, config);