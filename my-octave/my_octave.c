/// Copyright Birladeanu Raluca-Monica 315CAb 2022-2023

#include <stdio.h>
#include <stdlib.h>
#define MOD 10007
#define mult_size 7
#define dif_size 6
#define sum_size 8

typedef struct matrix {
	int **m_d;
	int r_sz, c_sz;
} matrix;

typedef struct resizable_array {
	matrix *arr;
	size_t used, size;
} res_arr;

// Initializes the array which will contain the matrices
void initialize_arr(res_arr *a)
{
	a->used = 0; // no space used yet
	a->size = 1; // size big enough to contain 1 matrix
	a->arr = (matrix *)malloc(sizeof(matrix) * a->size); // dynamic allocation
	if (!a->arr) {
		fprintf(stderr, "error allocating memory");
		exit(0);
	}
}

// Dynamically allocates a matrix with a specific number of rows and columns
int **dyn_matrix(int r_sz, int c_sz)
{
	int **matrix;
	int **temp = (int **)malloc(sizeof(int *) * r_sz);
	if (!temp) {
		fprintf(stderr, "error allocating memory");
		matrix = NULL;
		exit(0);
	}
	matrix = temp;
	for (int i = 0; i < r_sz; i++) {
		temp[i] = (int *)malloc(sizeof(int) * c_sz);
		if (!temp) {
			fprintf(stderr, "error allocating memory");
			for (int j = 0; j < i; j++)
				free(temp[j]);
			free(temp);
			matrix = NULL;
			exit(0);
		}
	}
	return matrix;
}

// Frees a dynamically allocated matrix
void free_matrix(int ***a, int size)
{
	for (int i = 0; i < size; i++)
		free((*a)[i]);
	free(*a);
}

/* Frees a resizable array and all
the matrices allocated inside of it */
void free_array(res_arr *a, int size, int counter)
{
	for (int i = 0; i < counter; i++)
		free_matrix(&a->arr[i].m_d, size);
	free(a->arr);
}

// If necessary, resizes array, then allocates space for the matrix to be added
void add_matrix(res_arr *a, size_t ind1, size_t ind2)
{
	if (a->used == a->size) { // if the arr is full
		a->size *= 2; // arr size doubles
		a->arr = (matrix *)realloc(a->arr, sizeof(matrix) * a->size);
		if (!a->arr) {
			fprintf(stderr, "error allocating memory");
			exit(0);
		}
	}
	a->arr[a->used].r_sz = a->arr[ind1].r_sz;
	a->arr[a->used].c_sz = a->arr[ind2].c_sz;
	a->arr[a->used].m_d = (int **)malloc(sizeof(int *) * a->arr[ind1].r_sz);
	if (!a->arr[a->used].m_d) {
		fprintf(stderr, "error allocating memory");
		exit(0);
	}
	for (int i = 0; i < a->arr[ind1].r_sz; i++) {
		a->arr[a->used].m_d[i] = (int *)malloc(sizeof(int) * a->arr[ind2].c_sz);
		if (!a->arr[a->used].m_d[i]) {
			fprintf(stderr, "error allocating memory");
			for (int j = 0; j < i; j++)
				free(a->arr[a->used].m_d[j]);
			free(a->arr[a->used].m_d);
			exit(0);
		}
	}
}

// Loads matrix into the array, which resizes if necessary
void load_matrix(res_arr *a)
{
	int i, j, r_sz, c_sz;
	scanf("%d%d", &r_sz, &c_sz);
	if (a->used == a->size) { // if the array is full
		a->size *= 2; // array size doubles
		a->arr = (matrix *)realloc(a->arr, sizeof(matrix) * a->size);
		if (!a->arr) {
			fprintf(stderr, "error allocating memory");
			exit(0);
		}
	}
	a->arr[a->used].r_sz = r_sz; // sets row size
	a->arr[a->used].c_sz = c_sz; // sets column size
	a->arr[a->used].m_d = (int **)malloc(sizeof(int *) * r_sz);
	if (!a->arr[a->used].m_d) {
		fprintf(stderr, "error allocating memory");
		exit(0);
	}
	for (i = 0; i < r_sz; i++) {
		a->arr[a->used].m_d[i] = (int *)malloc(sizeof(int) * c_sz);
		if (!a->arr[a->used].m_d[i]) {
			fprintf(stderr, "error allocating memory");
			for (j = 0; j < i; j++)
				free(a->arr[a->used].m_d[j]);
			free(a->arr[a->used].m_d);
			exit(0);
		}
	}
	for (i = 0; i < r_sz; i++)
		for (j = 0; j < c_sz; j++)
			scanf("%d", &a->arr[a->used].m_d[i][j]);
	a->used++; // Updates the counter for matrices in array
}

// Prints the dimensions of a given matrix
void print_dimensions(res_arr *a)
{
	size_t index;
	scanf("%ld", &index);
	if (index > a->used - 1 || a->used == 0)
		printf("No matrix with the given index\n");
	else
		printf("%d %d\n", a->arr[index].r_sz, a->arr[index].c_sz);
}

// Resizes matrix using Octave rules
void resize_matrix(res_arr *a)
{
	int l, c, *row, *column, i, j;
	size_t index;
	scanf("%ld", &index);
	scanf("%d", &l);
	int *row_temp = (int *)malloc(sizeof(int) * l);
	if (!row_temp) {
		fprintf(stderr, "error allocating memory");
		row = NULL;
		exit(0);
	}
	// Array saving row indices
	row = row_temp;
	for (i = 0; i < l; i++)
		scanf("%d", &row[i]);
	scanf("%d", &c);
	int *column_temp = (int *)malloc(sizeof(int) * c);
	if (!column_temp) {
		fprintf(stderr, "error allocating memory");
		column = NULL;
		exit(0);
	}
	// Array saving column indices
	column = column_temp;
	for (i = 0; i < c; i++)
		scanf("%d", &column[i]);
	if (index > a->used - 1 || a->used == 0) {
		printf("No matrix with the given index\n");
	} else {
		/* Dynamically allocate temporary matrix with as many
		rows and columns as the matrix we want to resize */
		int **temp = dyn_matrix(a->arr[index].r_sz, a->arr[index].c_sz);

		// Saves row size since it get modified
		int temp_row_sz = a->arr[index].r_sz;

		// Copy all elements of matrix to resize into a temporary matrix
		for (i = 0; i < temp_row_sz; i++)
			for (j = 0; j < a->arr[index].c_sz; j++)
				temp[i][j] = a->arr[index].m_d[i][j];

		// Resize desired matrix
		for (i = 0; i < a->arr[index].r_sz; i++)
			free(a->arr[index].m_d[i]);
		free(a->arr[index].m_d);
		a->arr[index].r_sz = l;
		a->arr[index].c_sz = c;
		a->arr[index].m_d = (int **)malloc(sizeof(int *) * l);
		if (!a->arr[index].m_d) {
			fprintf(stderr, "error allocating memory");
			exit(0);
		}
		for (i = 0; i < l; i++) {
			a->arr[index].m_d[i] = (int *)malloc(sizeof(int) * c);
			if (!a->arr[index].m_d[i]) {
				fprintf(stderr, "error allocating memory");
				for (j = 0; j < i; j++)
					free(a->arr[a->used].m_d[j]);
				free(a->arr[a->used].m_d);
				exit(0);
			}
		}

		// Copy just the elements of temp that are on the required position
		for (i = 0; i < l; i++)
			for (j = 0; j < c; j++)
				a->arr[index].m_d[i][j] = temp[row[i]][column[j]];
		free_matrix(&temp, temp_row_sz);
	}
	free(row);
	free(column);
}

/* Multiplies two given matrices and saves the
product matrix at the end of the matrix array */
void multiply_matrix(res_arr *a)
{
	int i, j, k;
	size_t ind1, ind2;
	scanf("%ld%ld", &ind1, &ind2);
	if (ind1 > a->used - 1 || ind2 > a->used - 1 || a->used == 0) {
		printf("No matrix with the given index\n");
	} else if (a->arr[ind1].c_sz != a->arr[ind2].r_sz) {
		printf("Cannot perform matrix multiplication\n");
	} else {
		int temp, **prod = dyn_matrix(a->arr[ind1].r_sz, a->arr[ind2].c_sz);
		for (i = 0; i < a->arr[ind1].r_sz; i++)
			for (j = 0; j < a->arr[ind2].c_sz; j++) {
				prod[i][j] = 0;
				for (k = 0; k < a->arr[ind1].c_sz; k++) {
					temp = a->arr[ind1].m_d[i][k] * a->arr[ind2].m_d[k][j];
					prod[i][j] += temp % MOD;
					if (prod[i][j] < 0)
						prod[i][j] += MOD;
				}
			}
		add_matrix(a, ind1, ind2);
		for (i = 0; i < a->arr[ind1].r_sz; i++)
			for (j = 0; j < a->arr[ind2].c_sz; j++) {
				a->arr[a->used].m_d[i][j] = prod[i][j] % MOD;
				if (a->arr[a->used].m_d[i][j] < 0)
					a->arr[a->used].m_d[i][j] += MOD;
			}
		a->used++;
		free_matrix(&prod, a->arr[ind1].r_sz);
	}
}

// Determines the sum of all elements in a given matrix
long long sum(res_arr *a, size_t index)
{
	int i, j;
	long long sum = 0;
	for (i = 0; i < a->arr[index].r_sz; i++)
		for (j = 0; j < a->arr[index].c_sz; j++)
			sum += a->arr[index].m_d[i][j];
	sum %= MOD;
	if (sum < 0)
		sum += MOD;
	return sum;
}

/* Sorts the arr of matrices in ascending order
by comparing the sum of all elements in each matrix */
void sort_arr(res_arr *a)
{
	size_t i, j;
	matrix temp;
	if (a->used != 0) {
		for (i = 0; i < a->used - 1; i++)
			for (j = i + 1; j < a->used; j++)
				if (sum(a, i) > sum(a, j)) {
					temp = a->arr[i];
					a->arr[i] = a->arr[j];
					a->arr[j] = temp;
				}
	}
}

// Swaps two variables using bitwise operator xor
void swap_variables(int *a, int *b)
{
	*a ^= *b;
	*b ^= *a;
	*a ^= *b;
}

/* Determines the transpose of a given matrix
and then modifies said matrix */
void transpose_matrix(res_arr *a)
{
	int i, j;
	size_t index;
	scanf("%ld", &index);
	if (index > a->used - 1 || a->used == 0) {
		printf("No matrix with the given index\n");
	} else {
		int **transpose = dyn_matrix(a->arr[index].c_sz, a->arr[index].r_sz);
		for (i = 0; i < a->arr[index].c_sz; i++)
			for (j = 0; j < a->arr[index].r_sz; j++)
				transpose[i][j] = a->arr[index].m_d[j][i];

		// Frees the space allocated to the matrix
		for (i = 0; i < a->arr[index].r_sz; i++)
			free(a->arr[index].m_d[i]);
		free(a->arr[index].m_d);

		// Changes matrix dimensions to transpose dimensions
		swap_variables(&a->arr[index].r_sz, &a->arr[index].c_sz);
		int new_r_sz = a->arr[index].r_sz, new_c_sz = a->arr[index].c_sz;

		// Allocates enough space for the transpose
		a->arr[index].m_d = (int **)malloc(sizeof(int *) * new_r_sz);
		if (!a->arr[index].m_d) {
			fprintf(stderr, "error allocating memory");
			exit(0);
		}
		for (i = 0; i < new_r_sz; i++) {
			a->arr[index].m_d[i] = (int *)malloc(sizeof(int) * new_c_sz);
			if (!a->arr[index].m_d[i]) {
				fprintf(stderr, "error allocating memory");
				for (j = 0; j < i; j++)
					free(a->arr[index].m_d[i]);
				free(a->arr[index].m_d);
				exit(0);
			}
		}
		for (i = 0; i < new_r_sz; i++)
			for (j = 0; j < new_c_sz; j++)
				a->arr[index].m_d[i][j] = transpose[i][j];
		free_matrix(&transpose, a->arr[index].r_sz);
	}
}

/* Multiplies two square matrices, storing the
result matrix into the first matrix input (**a) */
void mult_square_mat(int **a, int **b, int size)
{
	int i, j, k;
	int **result = dyn_matrix(size, size);
	for (i = 0; i < size; i++)
		for (j = 0; j < size; j++) {
			result[i][j] = 0;
			for (k = 0; k < size; k++) {
				result[i][j] += ((a[i][k] * b[k][j]) % MOD);
				if (result[i][j] < 0)
					result[i][j] += MOD;
			}
		}
	for (i = 0; i < size; i++)
		for (j = 0; j < size; j++) {
			a[i][j] = result[i][j] % MOD;
			if (a[i][j] < 0)
				a[i][j] += MOD;
		}
	free_matrix(&result, size);
}

/* Raises the given matrix to a given power,
modifying said matrix in place */
void raise_to_power(res_arr *a)
{
	int i, j, power;
	size_t index;
	scanf("%ld%d", &index, &power);
	if (index > a->used - 1 || a->used == 0) {
		printf("No matrix with the given index\n");
	} else if (power < 0) {
		printf("Power should be positive\n");
	} else if (a->arr[index].r_sz != a->arr[index].c_sz) {
		printf("Cannot perform matrix multiplication\n");
	} else {
		int **result = dyn_matrix(a->arr[index].r_sz, a->arr[index].c_sz);
		int row_size = a->arr[index].r_sz, column_size = a->arr[index].c_sz;
		// Result is initialized to the identity matrix
		for (i = 0; i < row_size; i++)
			for (j = 0; j < column_size; j++)
				if (i == j)
					result[i][j] = 1;
				else
					result[i][j] = 0;
		while (power > 0) {
			if (power & 1) // If the power is odd
				mult_square_mat(result, a->arr[index].m_d, row_size);
			power >>= 1; // Divides power by 2
			mult_square_mat(a->arr[index].m_d, a->arr[index].m_d, row_size);
			}
		for (i = 0; i < row_size; i++)
			for (j = 0; j < column_size; j++)
				a->arr[index].m_d[i][j] = result[i][j];
		free_matrix(&result, row_size);
	}
}

/* Deletes the given matrix from the array,
while also freeing the memory allocated for it */
void free_arr_matrix(res_arr *a)
{
	size_t index;
	scanf("%ld", &index);
	if (index > a->used - 1 || a->used == 0) {
		printf("No matrix with the given index\n");
	} else {
		// Frees data for matrix at given index
		for (int i = 0; i < a->arr[index].r_sz; i++)
			free(a->arr[index].m_d[i]);
		free(a->arr[index].m_d);

		// Moves all matrices as to not leave any blank spaces
		for (size_t i = index; i < a->used - 1; i++)
			a->arr[i] = a->arr[i + 1];
		a->used--; // Decreases the counter for places used in arr

		// Resizes array if necessary
		if (a->used == a->size / 2 && a->size != 1) {
			a->arr = (matrix *)realloc(a->arr, sizeof(matrix) * a->size / 2);
			if (!a->arr) {
				fprintf(stderr, "error allocating memory");
				exit(0);
			}
			a->size /= 2;
		}
	}
}

// Frees all resources and exits from the program
void free_all_resources(res_arr *a)
{
	for (size_t i = 0; i < a->used; i++) {
		for (int j = 0; j < a->arr[i].r_sz; j++)
			free(a->arr[i].m_d[j]);
		free(a->arr[i].m_d);
	}
	free(a->arr);
	exit(0);
}

// Returns the sum matrix of two matrices
int **add_strass(int **a1, int **a2, int size)
{
	int i, j, **sum = dyn_matrix(size, size);
	for (i = 0; i < size; i++)
		for (j = 0; j < size; j++) {
			sum[i][j] = (a1[i][j] + a2[i][j]) % MOD;
			if (sum[i][j] < 0)
				sum[i][j] += MOD;
		}
	return sum;
}

// Substracts two matrices and returns the resulted matrix
int **sub_strass(int **a1, int **a2, int size)
{
	int i, j, **temp = dyn_matrix(size, size);
	for (i = 0; i < size; i++)
		for (j = 0; j < size; j++) {
			temp[i][j] = (a1[i][j] - a2[i][j]) % MOD;
			if (temp[i][j] < 0)
				temp[i][j] += MOD;
		}
	return temp;
}

// Returns the product matrix of two matrices
int **mul_strass(int **a1, int **a2, int size)
{
	int i, j, k, **result = dyn_matrix(size, size);
	for (i = 0; i < size; i++)
		for (j = 0; j < size; j++) {
			result[i][j] = 0;
			for (k = 0; k < size; k++) {
				result[i][j] += ((a1[i][k] * a2[k][j]) % MOD);
				if (result[i][j] < 0)
					result[i][j] += MOD;
			}
		}
	return result;
}

/* Uses the Strassen method to multiply two given matrices
and returns the product matrix */
int **strassen(res_arr *a, int ind1, int ind2)
{
	int strassen_size = a->arr[ind1].r_sz; // Size of product matrix
	int **c = dyn_matrix(strassen_size, strassen_size);
	if (strassen_size == 1) { // Base case
		c[0][0] = a->arr[ind1].m_d[0][0] * a->arr[ind2].m_d[0][0];
	} else {
		int size = strassen_size / 2, i, j, **a11, **a12, **a21, **a22;
		int **b11, **b12, **b21, **b22, **c11, **c12, **c21, **c22;
		res_arr m, sum, dif;
		m.arr = (matrix *)malloc(sizeof(matrix) * mult_size);
		sum.arr = (matrix *)malloc(sizeof(matrix) * sum_size);
		dif.arr = (matrix *)malloc(sizeof(matrix) * dif_size);
		a11 = dyn_matrix(size, size);
		a12 = dyn_matrix(size, size);
		a21 = dyn_matrix(size, size);
		a22 = dyn_matrix(size, size);
		b11 = dyn_matrix(size, size);
		b12 = dyn_matrix(size, size);
		b21 = dyn_matrix(size, size);
		b22 = dyn_matrix(size, size);
		for (i = 0; i < size; i++)
			for (j = 0; j < size; j++) {
				a11[i][j] = a->arr[ind1].m_d[i][j];
				a12[i][j] = a->arr[ind1].m_d[i][size + j];
				a21[i][j] = a->arr[ind1].m_d[size + i][j];
				a22[i][j] = a->arr[ind1].m_d[size + i][size + j];
				b11[i][j] = a->arr[ind2].m_d[i][j];
				b12[i][j] = a->arr[ind2].m_d[i][size + j];
				b21[i][j] = a->arr[ind2].m_d[size + i][j];
				b22[i][j] = a->arr[ind2].m_d[size + i][size + j];
		}
		sum.arr[0].m_d = add_strass(a11, a22, size);
		sum.arr[1].m_d = add_strass(b11, b22, size);
		sum.arr[2].m_d = add_strass(a21, a22, size);
		dif.arr[0].m_d = sub_strass(b12, b22, size);
		dif.arr[1].m_d = sub_strass(b21, b11, size);
		sum.arr[3].m_d = add_strass(a11, a12, size);
		dif.arr[2].m_d = sub_strass(a21, a11, size);
		dif.arr[3].m_d = sub_strass(a12, a22, size);
		sum.arr[4].m_d = add_strass(b11, b12, size);
		sum.arr[5].m_d = add_strass(b21, b22, size);
		m.arr[0].m_d = mul_strass(sum.arr[0].m_d, sum.arr[1].m_d, size);
		m.arr[1].m_d = mul_strass(sum.arr[2].m_d, b11, size);
		m.arr[2].m_d = mul_strass(a11, dif.arr[0].m_d, size);
		m.arr[3].m_d = mul_strass(a22, dif.arr[1].m_d, size);
		m.arr[4].m_d = mul_strass(sum.arr[3].m_d, b22, size);
		m.arr[5].m_d = mul_strass(dif.arr[2].m_d, sum.arr[4].m_d, size);
		m.arr[6].m_d = mul_strass(dif.arr[3].m_d, sum.arr[5].m_d, size);
		dif.arr[4].m_d = sub_strass(m.arr[3].m_d, m.arr[4].m_d, size);
		dif.arr[5].m_d = sub_strass(m.arr[0].m_d, m.arr[1].m_d, size);
		sum.arr[6].m_d = add_strass(m.arr[0].m_d, m.arr[6].m_d, size);
		sum.arr[7].m_d = add_strass(m.arr[2].m_d, m.arr[5].m_d, size);
		c11 = add_strass(dif.arr[4].m_d, sum.arr[6].m_d, size);
		c12 = add_strass(m.arr[2].m_d, m.arr[4].m_d, size);
		c21 = add_strass(m.arr[1].m_d, m.arr[3].m_d, size);
		c22 = add_strass(dif.arr[5].m_d, sum.arr[7].m_d, size);
		for (i = 0; i < size; i++)
			for (j = 0; j < size; j++) {
				c[i][j] = c11[i][j];
				c[i][j + size] = c12[i][j];
				c[size + i][j] = c21[i][j];
				c[size + i][size + j] = c22[i][j];
			}
		free_matrix(&a11, size);
		free_matrix(&a12, size);
		free_matrix(&a21, size);
		free_matrix(&a22, size);
		free_matrix(&b11, size);
		free_matrix(&b12, size);
		free_matrix(&b21, size);
		free_matrix(&b22, size);
		free_matrix(&c11, size);
		free_matrix(&c12, size);
		free_matrix(&c21, size);
		free_matrix(&c22, size);
		free_array(&m, size, mult_size);
		free_array(&sum, size, sum_size);
		free_array(&dif, size, dif_size);
		}
	return c;
}

// Prints a given matrix from the matrix array
void print_matrix(res_arr *a)
{
	int i, j;
	size_t index;
	scanf("%ld", &index);
	if (index > a->used - 1 || a->used == 0) {
		printf("No matrix with the given index\n");
	} else  {
		for (i = 0; i < a->arr[index].r_sz; i++) {
			for (j = 0; j < a->arr[index].c_sz; j++)
				printf("%d ", a->arr[index].m_d[i][j]);
		printf("\n");
		}
	}
}

int main(void)
{
	res_arr a;
	size_t ind1, ind2;
	char operation;
	initialize_arr(&a);
	while (1) {
		scanf("\n%c", &operation);
		switch (operation) {
		case 'L': {
			load_matrix(&a);
			break;
		}
		case 'D': {
			print_dimensions(&a);
			break;
		}
		case 'P': {
			print_matrix(&a);
			break;
		}
		case 'C': {
			resize_matrix(&a);
			break;
		}
		case 'M': {
			multiply_matrix(&a);
			break;
		}
		case 'O': {
			sort_arr(&a);
			break;
		}
		case 'T': {
			transpose_matrix(&a);
			break;
		}
		case 'R': {
			raise_to_power(&a);
			break;
		}
		case 'F': {
			free_arr_matrix(&a);
			break;
		}
		case 'Q':{
			free_all_resources(&a);
			break;
		}
		case 'S':{
			scanf("%ld%ld", &ind1, &ind2);
			if (ind1 > a.used - 1 || ind2 > a.used - 1 || a.used == 0) {
				printf("No matrix with the given index\n");
			} else if (a.arr[ind1].r_sz != a.arr[ind1].c_sz ||
					   a.arr[ind2].r_sz != a.arr[ind2].c_sz ||
					   (((a.arr[ind1].r_sz) & (a.arr[ind1].r_sz - 1)) != 0) ||
					   (((a.arr[ind2].r_sz) & (a.arr[ind2].r_sz - 1)) != 0)) {
				printf("Cannot perform matrix multiplication\n");
			} else {
				int **c = strassen(&a, ind1, ind2);
				add_matrix(&a, ind1, ind2);
				for (int i = 0; i < a.arr[ind1].r_sz; i++)
					for (int j = 0; j < a.arr[ind2].c_sz; j++)
						a.arr[a.used].m_d[i][j] = c[i][j];
				a.used++;
				free_matrix(&c, a.arr[ind1].r_sz);
			}
			break;
		}
		default:{
			printf("Unrecognized command\n");
			break;
		}
		}
	}
	return 0;
}
