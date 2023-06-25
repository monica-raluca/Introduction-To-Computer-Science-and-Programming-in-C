//Birladeanu Raluca-Monica 315CA
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(void)
{
	unsigned long long N, ps = 0, i, ai = 0, bi = 0;
	long long first_max_a, first_max_b, a_max = -1, b_max = -1;
	double n_a = 0, n_b = 0;
	scanf("%llu", &N);
	if (N != 0)
		scanf("%llo%llo", &ai, &bi);
	first_max_a = ai;
	first_max_b = bi;
	ps = ai * bi;
	n_a = ai * ai;
	n_b = bi * bi;
	for (i = 1; i < N; i++) {
		scanf("%llo%llo", &ai, &bi);
		ps += ai * bi;
		if ((unsigned int)first_max_a < ai) {
			a_max = first_max_a;
			first_max_a = ai;
		} else if (a_max < (signed int)ai && ai != (unsigned int)first_max_a) {
			a_max = ai;
		}
		if (a_max == first_max_a)
			a_max = -1;
		if ((unsigned int)first_max_b < bi) {
			b_max = first_max_b;
			first_max_b = bi;
		} else if (b_max < (signed int)bi && bi != (unsigned int)first_max_b) {
			b_max = bi;
		}
		if (b_max == first_max_b)
			b_max = -1;
		n_a += ai * ai;
		n_b += bi * bi;
	}
	printf("%llu\n%lld %lld\n", ps, a_max, b_max);
	printf("%.7F %.7F\n", sqrt(n_a), sqrt(n_b));
	return 0;
}
