
double Sgmic(int, long);
double RandomChoice(rng_config*);
double AceptanceProbability(double, double, double);

void *SgmicThread(void*);
bool PerformSGMIC(double**, int, int, mine_result*, long, int, config);