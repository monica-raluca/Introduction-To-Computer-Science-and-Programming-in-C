//Birladeanu Raluca-Monica 315CA
#include <stdio.h>
#define MAX 400

void print_array(int n, int v[MAX])
{
	for (int i = 0; i < n * n; i++)
		printf("%d ", v[i]);
	printf("\n");
}

void line(int X, int n, int a[MAX][MAX], int v[MAX], int *sudoku, int *poss)
{
	int i;
	*sudoku = 0; //values on the sudoku board
	for (i = 0; i < n * n; i++)
		v[i] = 1;
	*poss = n * n; //possible values
	for (i = 0; i < n * n; i++)
		if (a[X - 1][i] != 0) {
			if (v[a[X - 1][i] - 1] == 1)
				(*poss)--;
			v[a[X - 1][i] - 1] = 0;
			(*sudoku)++;
		}
}

void column(int Y, int n, int a[MAX][MAX], int v[MAX], int *sudoku, int *poss)
{
	int i;
	*sudoku = 0;
	for (i = 0; i < n * n; i++)
		v[i] = 1;
	*poss = n * n;
	for (int i = 0; i < n * n; i++)
		if (a[i][Y - 1] != 0) {
			if (v[a[i][Y - 1] - 1] == 1)
				(*poss)--;
			v[a[i][Y - 1] - 1] = 0;
			(*sudoku)++;
		}
}

void box(int Z, int n, int a[MAX][MAX], int v[MAX], int *sudoku, int *poss)
{
	int i, j, counter = 0, pos_i, pos_j;
	*sudoku = 0;
	for (i = 0; i < n * n; i++)
		v[i] = 1;
	*poss = n * n;
	for (i = 0; i < n * n; i += n)
		for (j = 0; j < n * n; j += n)
			if (counter < Z) { //finds coordinates of Z
				counter++;
				pos_i = i;
				pos_j = j;
			}
	for (i = pos_i; i <= pos_i + n - 1; i++)
		for (j = pos_j; j <= pos_j + n - 1; j++)
			if (a[i][j] != 0) {
				if (v[a[i][j] - 1] == 1)
					(*poss)--;
				v[a[i][j] - 1] = 0;
				(*sudoku)++;
			}
}

void cell(int X, int Y, int n, int a[MAX][MAX], int v[MAX])
{
	int i, temp[MAX], sudoku = 0, poss = 0;
	if (a[X - 1][Y - 1] != 0) {
		for (i = 0; i < n * n; i++)
			v[i] = 0;
	} else {
		for (i = 0; i < n * n; i++)
			v[i] = 1;
		line(X, n, a, temp, &sudoku, &poss);
		for (i = 0; i < n * n; i++)
			v[i] = (v[i] & temp[i]);
		column(Y, n, a, temp, &sudoku, &poss);
		for (i = 0; i < n * n; i++)
			v[i] = (v[i] & temp[i]);
		int Z = (X - 1) / n * n + (Y - 1) / n;
		box(Z + 1, n, a, temp, &sudoku, &poss);
		for (i = 0; i < n * n; i++)
			v[i] = (v[i] & temp[i]);
	}
}

void place_value(int X, int Y, int C, int a[MAX][MAX])
{
	if (a[X - 1][Y - 1] == 0) //empty cell
		a[X - 1][Y - 1] = C;
}

int current_state(int n, int a[MAX][MAX])
{
	int line_index, column_index, sudoku = 0, poss = 0, v[MAX], all_sudoku = 0;
	for (line_index = 0; line_index < n * n; line_index++) {
		line(line_index + 1, n, a, v, &sudoku, &poss);
		if (sudoku + poss != n * n)
			return 1;
	}
	for (column_index = 0; column_index < n * n; column_index++) {
		column(column_index + 1, n, a, v, &sudoku, &poss);
		if (sudoku + poss != n * n)
			return 1;
	}
	for (line_index = 0; line_index < n * n; line_index += n)
		for (column_index = 0; column_index < n * n; column_index += n) {
			int Z = line_index / n * n + column_index / n;
			box(Z + 1, n, a, v, &sudoku, &poss);
			if (sudoku + poss != n * n)
				return 1;
			all_sudoku += sudoku; //all values in sudoku
		}
	if (all_sudoku == (n * n) * (n * n))
		return 2;
	else
		return 0;
}

int main(void)
{
	int i, j, N, M, task, X, Y, Z, C;
	int a[MAX][MAX], v[MAX], sudoku = 0, poss = 0;
	scanf("%d%d", &N, &M);
	for (i = 0; i < N * N; i++)
		for (j = 0; j < N * N; j++)
			scanf("%d", &a[i][j]);
	while (M) {
		scanf("%d", &task);
		if (task == 1) {
			scanf("%d", &X);
			line(X, N, a, v, &sudoku, &poss);
			print_array(N, v);
		} else if (task == 2) {
			scanf("%d", &Y);
			column(Y, N, a, v, &sudoku, &poss);
			print_array(N, v);
		} else if (task == 3) {
			scanf("%d", &Z);
			box(Z, N, a, v, &sudoku, &poss);
			print_array(N, v);
		} else if (task == 4) {
			scanf("%d%d", &X, &Y);
			cell(X, Y, N, a, v);
			print_array(N, v);
		} else if (task == 5) {
			scanf("%d%d%d", &X, &Y, &C);
			place_value(X, Y, C, a);
		} else if (task == 6) {
			printf("%d\n", current_state(N, a));
		}
		M--;
	}
	return 0;
}
