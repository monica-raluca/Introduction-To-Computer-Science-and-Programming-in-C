//Birladeanu Raluca-Monica 315CA
#include <stdio.h>
#include <string.h>
#define NMAX 1000

int colour_check(int i, int j)
{
	if (i % 2 == j % 2)
		return 0; // white square
	else
		return 1; // black square
}

void strrev(char *s)
{
	char *s1, *s2;
	for (s1 = s, s2 = s + strlen(s) - 1; s1 < s2; s1++, s2--) {
		*s1 ^= *s2;
		*s2 ^= *s1;
		*s1 ^= *s2;
	}
}

void move_right(int i, int *j, int n, long long a[NMAX][NMAX])
{
	int aux, auxj = *j;
	if (*j + a[i][*j] < n) { //if it doesn't move out of the matrix
		*j += a[i][*j];
	} else {
		aux = a[i][*j];
		aux -= (n - 1 - *j); //get to last column
		if (aux % n == 0)
			*j = n - 1;
		else
			*j = aux % n - 1;
	}
	a[i][auxj] = 0; //mark as visited
}

void move_left(int i, int *j, int n, long long a[NMAX][NMAX])
{
	int aux, auxj = *j;
	if (*j - (-a[i][*j]) > 0) { //if it doesn't move out of the matrix
		*j -= (-a[i][*j]);
	} else {
		aux = -(a[i][*j]);
		aux -= *j;  //get to first column
		if (aux % n == 0)
			*j = 0;
		else
			*j = n - aux % n;
	}
	a[i][auxj] = 0; //mark as visited
}

void move_down(int *i, int j, int n, long long a[NMAX][NMAX])
{
	int aux, auxi = *i;
	if (*i + a[*i][j] < n) { //if it doesn't move out of the matrix
		*i += a[*i][j];
	} else {
		aux = a[*i][j];
		aux -= (n - 1 - *i); //get to last row
		if (aux % n == 0)
			*i = n - 1;
		else
			*i = aux % n - 1;
	}
	a[auxi][j] = 0; //mark as visited
}

void move_up(int *i, int j, int n, long long a[NMAX][NMAX])
{
	int aux, auxi = *i;
	if (*i - (-a[*i][j]) > 0) { //if it doesn't move out of the matrix
		*i -= (-a[*i][j]);
	} else {
		aux = (-a[*i][j]);
		aux -= *i; //get to first row
		if (aux % n == 0)
			*i = 0;
		else
			*i = n - aux % n;
	}
	a[auxi][j] = 0; //mark as visited
}

int main(void)
{
	int n, i, j, y_final = 0;
	long long a[NMAX][NMAX], distance = 0;
	char x_final[50] = "", c;
	scanf("%d", &n);
	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++)
			scanf("%lld", &a[i][j]);
	i = 0;
	j = 0;
	while (a[i][j] != 0) {
		if (colour_check(i, j) == 0) {
			if (a[i][j] > 0) {
				distance += a[i][j];
				move_right(i, &j, n, a);
			} else {
				distance += (-a[i][j]);
				move_left(i, &j, n, a);
			}
		} else {
			if (a[i][j] > 0) {
				distance += a[i][j];
				move_down(&i, j, n, a);
			} else {
				distance += (-a[i][j]);
				move_up(&i, j, n, a);
			}
		}
	}
	y_final = n - i;
	j++;
	i = 0;
	while (j) {
		if (j % 26 == 0 && j / 26 > 0)// if letter is Z
			c = (--j) % 26 + 'A';
		else
			c = j % 26 + 'A' - 1;
		strcat(x_final, &c);
		c = '\0';
		j /= 26;
		i++;
		x_final[i] = '\0';
	}
	strrev(x_final);
	printf("%lld\n%d %s\n", distance, y_final, x_final);
	return 0;
}
