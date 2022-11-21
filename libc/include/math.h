# define M_PI   3.14159265358979323846 

#define negate(a, b) (((a) ^ ((((int64_t)(b)) >= 0) - 1)) + (((int64_t)(b)) < 0))
#define abs(a) negate(a, a)

int max(int a, int b);
int min(int a, int b);
double pow(double x, double y);
double sqrt(double x);