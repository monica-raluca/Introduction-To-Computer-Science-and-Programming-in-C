//Birladeanu Raluca-Monica 315CA
#include <stdio.h>
#include <math.h>
#define NMAX 26

void find_max(int K, unsigned int fr[NMAX])
{
	int maxi, pos_max = 0, i, w[NMAX];
	for (i = 0; i < NMAX; i++)
		w[i] = fr[i]; //copies everything in another vector as to not modify fr
	while (K) { //finds K max values
		maxi = -1;
		for (i = 0; i < NMAX; i++)
			if (w[i] > maxi) {
				maxi = w[i]; //updates the max
				pos_max = i; //saves the position of the max each time
			}
	if (K != 1)
		printf("%c ", pos_max + 'a');
	else
		printf("%c", pos_max + 'a');
	w[pos_max] = -__INT_MAX__; //marks the max as found
	K--;
	}
}

int unique(unsigned int fr[NMAX])
{
	int counter = 0;
	for (int i = 0; i < NMAX; i++)
		if (fr[i] != 0)
			counter++; //counts each value that exists at least once
	if (counter >= 2)
		return 1; //at least 2 different values
	else
		return 0;
}

void simplification(int x, unsigned long long all_nr)
{
	int a = x;
	unsigned long long b = all_nr;
	unsigned long long c = a % b;
	/*calculates greatest common divisor*/
	while (a % b > 0) {
		a = b;
		b = c;
		c = a % b;
	}
	unsigned long long gcd = b;
	/*divides both nomerator and denominator by gcd*/
	printf("%llu/%llu", x / gcd, all_nr / gcd);
	printf("\n");
}

int freq(unsigned int fr[NMAX], unsigned long long all_nr)
{
	for (int i = 0; i < NMAX; i++)
		if (fr[i] >= (all_nr + 1) / 2 && fr[i] != 0) {
			printf("%c ", i + 'a');
			simplification(fr[i], all_nr);
			return 1;
		}
	return 0;
}

int main(void)
{
	unsigned int fr[NMAX] = {0};
	int K, nr_chars = 0;
	unsigned long long all_nr = 0;
	unsigned char c, L;
	scanf("%c", &c);
	while ((c >= 'a' && c <= 'z') || (c == 'Q' || c == 'T' || c == 'E')) {
		if (c != '\n') {
			if (c == 'Q') {
				scanf("\n%c", &L);
				printf("%c %d\n", L, fr[L - 'a']);
			} else if (c == 'T') {
				scanf("%d", &K);
				find_max(K, fr);
				printf("\n");
			} else if (c == 'E') {
				scanf("\n%c", &L);
				all_nr -= fr[L - 'a'];
				fr[L - 'a'] = 0;
			} else {
				fr[c - 'a']++;
				all_nr++;
				nr_chars++;
				if (nr_chars >= 5 && unique(fr) != 0 && freq(fr, all_nr) != 0)
					nr_chars = 0;
			}
		}
	scanf("\n%c", &c);
	}
	return 0;
}
