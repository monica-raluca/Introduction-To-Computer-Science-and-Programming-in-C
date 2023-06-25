// Copyright Birladeanu Raluca-Monica 315CAb - 2022-2023
#define  _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <ctype.h>
#include <math.h>

/* Structure saving the picture info, containing a buffer,
the currently selected area and the necessary size for
both of them, as well as the magic word and type */
typedef struct {
	int *buffer, *sel_area;
	int width, height, max_value, depth;
	int sel_size, sel_width, sel_height, x1, y1, x2, y2, all;
	char magic_word[3], type[20];
} file_data;

/* Clamp function, which makes sure x is between min and max.
If it is smaller than the min value, it becomes min, if it
is larger than the max value, it becomes max */
void clamp(double *x, int min, int max)
{
	if (*x < min)
		*x = min;
	if (*x > max)
		*x = max;
}

/* Reads the file info, ignoring any comments it encounters.
The functiion reads the data in order, first the magic word,
then the width, height and, lastly, the max_value if it exists.
The depth and max_value are 1 by default, and modified if necessary. */
void file_info(file_data *file, FILE *file_pointer, long *temp_size)
{
	file->depth = 1;
	file->max_value = 1;
	char *line = NULL;
	size_t length = 0, ok = 0;
	ssize_t read;
	// Reads each line and ignores comments
	while ((read = getline(&line, &length, file_pointer)) != -1) {
		if (line[0] == '#') {
			continue;
		} else if (line[0] != '#' && ok == 0) {
			strncpy(file->magic_word, line, 2);
			file->magic_word[2] = '\0';
			ok = 1;
		} else if (line[0] != '#' && ok == 1) {
			int read_args = sscanf(line, "%d%d", &file->width, &file->height);
			if (!read_args) {
				perror("Error reading input");
				exit(1);
			}
			ok = 2;
			if (file->magic_word[1] == '1' || file->magic_word[1] == '4')
				break;
		} else if (line[0] != '#' && ok == 2 && (file->magic_word[1] == '2' ||
		file->magic_word[1] == '3' || file->magic_word[1] == '5' ||
		file->magic_word[1] == '6')) {
			int read_args = sscanf(line, "%d", &file->max_value);
			if (!read_args) {
				perror("Error reading input");
				exit(1);
			}
			break;
		}
	}
	free(line);
	// Saves the file type, depending on the magic word
	if (file->magic_word[1] == '1' || file->magic_word[1] == '4') {
		strcpy(file->type, "black_white");
	} else if (file->magic_word[1] == '2' || file->magic_word[1] == '5') {
		strcpy(file->type, "greyscale");
	} else {
		strcpy(file->type, "colour");
		file->depth = 3;
	}
	// At first, the selection has the same dimensions as the full image
	file->sel_height = file->height;
	file->sel_width = file->width;
	char test_char;
	// Ignores all comments between the max value and pixel matrix
	while ((test_char = fgetc(file_pointer)) == '#') {
		while (test_char != '\n')
			test_char = fgetc(file_pointer);
	}
	ungetc(test_char, file_pointer);
	// Saves the position where the pixel matrix starts
	*temp_size = ftell(file_pointer);
}

// Function that initializes all selection parameters
void initialize_selection(file_data *file)
{
	file->x1 = 0;
	file->y1 = 0;
	file->x2 = file->width;
	file->y2 = file->height;
	file->sel_height = file->height;
	file->sel_width = file->width;
}

file_data *load(char *filename)
{
	// Stores the file contents and sizes
	file_data *file = malloc(sizeof(file_data));
	if (!file) {
		perror("Error allocating memory");
		exit(1);
	}
	/* Open the file in binary mode to ensure
	no info is lost if the file is binary */
	FILE *file_pointer = fopen(filename, "rb");
	if (!file_pointer) {
		printf("Failed to load %s\n", filename);
		free(file);
		file = NULL;
		return file;
	}
	long temp_size;
	file_info(file, file_pointer, &temp_size);
	// Close the file and reopen it in the required mode
	fclose(file_pointer);
	if (file->magic_word[0] == 'P' && (file->magic_word[1] == '4' ||
									   file->magic_word[1] == '5' ||
									   file->magic_word[1] == '6')) {
		file_pointer = fopen(filename, "rb");
	} else if (file->magic_word[1] == '1' || file->magic_word[1] == '2' ||
	file->magic_word[1] == '3') {
		file_pointer = fopen(filename, "rt");
	}
	if (!file_pointer) {
		printf("Failed to load %s", filename);
		file = NULL;
		return file;
	}
	// Sets the file at the beginning of the pixel matrix
	fseek(file_pointer, temp_size, SEEK_SET);
	file->sel_size = file->height * file->depth * file->width;
	file->sel_area = malloc(file->sel_size * sizeof(int));
	initialize_selection(file);
	// Initializez file->all (which signifies if the whole pic was selected)
	file->all = 2;
	unsigned char temp_buffer;
	// Dynamically allocates the buffer memory
	file->buffer = (int *)calloc(file->height * file->depth *
	file->width, sizeof(int));
	if (!file->buffer) {
		perror("Error allocating memory");
		fclose(file_pointer);
		exit(1);
	}
	// Reads the content differently depending on the file type
	if (file->magic_word[0] == 'P' && (file->magic_word[1] == '4' ||
									   file->magic_word[1] == '5' ||
									   file->magic_word[1] == '6')) {
		for (int i = 0 ; i < file->height ; i++) {
			for (int j = 0 ; j < file->depth * file->width ; j++) {
				fread(&temp_buffer, sizeof(unsigned char), 1, file_pointer);
				file->buffer[i * file->width * file->depth + j] =
				(int)temp_buffer;
			}
		}
	} else if (file->magic_word[0] == 'P' && (file->magic_word[1] == '1' ||
	file->magic_word[1] == '2' || file->magic_word[1] == '3')) {
		for (int i = 0; i < file->height; i++)
			for (int j = 0; j < file->width * file->depth; j++)
				fscanf(file_pointer, "%d",
					   &file->buffer[i * file->width * file->depth + j]);
	}
	int k = 0;
	// Copies the file->buffer into the selection (assumes all pic is selected)
	for (int i = 0; i < file->height; i++)
		for (int j = 0; j < file->width * file->depth; j++)
			file->sel_area[k++] =
			file->buffer[i * (file->depth * file->width) + j];
	printf("Loaded %s\n", filename);
	fclose(file_pointer);
	return file;
}

// Swaps two variables using bitwise operator xor
void swap(int *x, int *y)
{
	*x ^= *y;
	*y ^= *x;
	*x ^= *y;
}

/* Checks if the select_pixels command is valid. Reads a test_char,
checks it and ungets that char if it is a valid parameter. (so the whole
parameter can then be read). Uses a counter which increases everytime
a valid parameter is read. If unvalid parameters are read, returns 0. */
int check_select(file_data *file, int *x1, int *y1, int *x2, int *y2)
{
	int count = 0;
	char test_char;
	while (1) {
		scanf("%c", &test_char);
		if (test_char == ' ') {
			continue;
		} else if (test_char == '\n' && count != 4) {
			printf("Invalid command\n");
			return 0;
		} else if (isalpha(test_char) > 0) {
			char test_string[100];
			fgets(test_string, sizeof(test_string), stdin);
			printf("Invalid command\n");
			return 0;
		} else if (test_char == '-') {
			ungetc(test_char, stdin);
			if (count == 0) {
				scanf("%d", x1);
				count++;
			} else if (count == 1) {
				scanf("%d", y1);
				count++;
			} else if (count == 2) {
				scanf("%d", x2);
				count++;
			} else if (count == 3) {
				scanf("%d", y2);
				count++;
			}
		} else if (isdigit(test_char) > 0 && count == 0) {
			ungetc(test_char, stdin);
			scanf("%d", x1);
			count++;
		} else if (isdigit(test_char) > 0 && count == 1) {
			ungetc(test_char, stdin);
			scanf("%d", y1);
			count++;
		} else if (isdigit(test_char) > 0 && count == 2) {
			ungetc(test_char, stdin);
			scanf("%d", x2);
			count++;
		} else if (isdigit(test_char) > 0 && count == 3) {
			ungetc(test_char, stdin);
			scanf("%d", y2);
			count++;
		} else if (count == 4 && test_char == '\n') {
			break;
		}
	}
	// Swaps the indices if necessary
	if (*x1 > *x2)
		swap(x1, x2);
	if (*y1 > *y2)
		swap(y1, y2);
	/* Tests if the indices are out of bounds or if a selection with
	the same parameters has already been made */
	if (*x1 < 0 || *y1 < 0 || *x2 > file->width || *y2 > file->height ||
		*x1 == *x2 || *y1 == *y2 || (*x1 == file->x1 && *x2 == file->x2 &&
		*y1 == file->y1 && *y2 == file->y2 && file->all == 0)) {
		printf("Invalid set of coordinates\n");
		return 0;
	}
	return 1;
}

// Selects an area, takes as input four coordinates
void select_pixels(file_data *file)
{
	int k = 0, x1, y1, x2, y2;
	// If no file is loaded, reads the rest of the line and exits the function
	if (!file) {
		printf("No image loaded\n");
		char test_string[100];
		fgets(test_string, sizeof(test_string), stdin);
		return;
	}
	if (check_select(file, &x1, &y1, &x2, &y2) == 0)
		return;
	// Saves the coordinates into the structure
	file->x1 = x1;
	file->y1 = y1;
	file->x2 = x2;
	file->y2 = y2;
	file->sel_size = (file->x2 - file->x1) *
	(file->y2 - file->y1) * file->depth;
	// If it would select the whole picture, set file->all to 0
	if (file->x1 == 0 && file->x2 == file->width &&
		file->y1 == 0 && file->y2 == file->height)
		file->all = 0;
	file->sel_width = file->x2 - file->x1;
	file->sel_height = file->y2 - file->y1;
	// Frees the previously selected area, allocates the new space
	if (file->sel_area) {
		free(file->sel_area);
		file->sel_area = NULL;
	}
	file->sel_area = malloc(file->sel_size * sizeof(int));
	if (!file->sel_area) {
		perror("Error allocating memory");
		exit(1);
	}
	// Copies only the required elements into the selection
	for (int i = file->y1; i < file->y2; i++)
		for (int j = file->x1 * file->depth; j < file->x2 * file->depth; j++)
			file->sel_area[k++] =
			file->buffer[i * file->depth * file->width + j];
	printf("Selected %d %d %d %d\n", file->x1, file->y1, file->x2, file->y2);
}

// Selects the whole picture
void select_all(file_data *file)
{
	if (!file) {
		printf("No image loaded\n");
		return;
	}
	// If there was an already made selection
	if (file->sel_area)
		free(file->sel_area);
	file->sel_area = NULL;
	file->sel_size = file->width * file->depth * file->height;
	file->sel_area = malloc(file->sel_size * sizeof(int));
	initialize_selection(file);
	// The file was all selected
	file->all = 1;
	if (!file->sel_area) {
		perror("Error allocating memory");
		exit(1);
	}
	for (int i = 0; i < file->height; i++)
		for (int j = 0; j < file->width * file->depth; j++)
			file->sel_area[i * file->width * file->depth + j] =
			file->buffer[i * file->width * file->depth + j];
	printf("Selected ALL\n");
}

// Creates the histogram for black and white images
void histogram(file_data *file)
{
	int star_total = 0, max_value = -1, freq[256] = {0};
	int bin_total, max_stars, histogram[256] = {0}, count = 0;
	// If no file is loaded, reads the rest of the line and exits the function
	if (!file) {
		printf("No image loaded\n");
		char test_string[100];
		fgets(test_string, sizeof(test_string), stdin);
		return;
	}
	// Tests if the command is valid
	char test_char;
	while (1) {
		scanf("%c", &test_char);
		if (test_char == ' ') {
			continue;
		} else if (test_char == '\n' && count != 2) {
			printf("Invalid command\n");
			return;
		} else if (count == 2 && isdigit(test_char) > 0) {
			char test_string[100];
			fgets(test_string, sizeof(test_string), stdin);
			printf("Invalid command\n");
			return;
		} else if (isdigit(test_char) > 0 && count == 0) {
			ungetc(test_char, stdin);
			scanf("%d", &max_stars);
			count++;
		} else if (isdigit(test_char) > 0 && count == 1) {
			ungetc(test_char, stdin);
			scanf("%d", &bin_total);
			count++;
		} else if (count == 2 && test_char == '\n') {
			break;
		}
	}
	// Tests if bin_total is a power of two
	if (bin_total != 1 && ((bin_total & (bin_total - 1)) != 0)) {
		printf("Invalid set of coordinates\n");
		return;
	}
	if (strcmp(file->type, "colour") == 0) {
		printf("Black and white image needed\n");
		return;
	}
	for (int i = 0; i < file->height * file->depth * file->width; i++)
		freq[file->buffer[i]]++;
	for (int i = 0; i < 256; i++)
		histogram[i / (256 / bin_total)] += freq[i];
	for (int i = 0; i < bin_total; i++)
		if (histogram[i] > max_value)
			max_value = histogram[i];
	for (int i = 0; i < bin_total; i++) {
		star_total = floor((histogram[i] * max_stars) / max_value);
		printf("%d\t|\t", star_total);
			while (star_total) {
				printf("*");
				star_total--;
			}
		printf("\n");
	}
}

// Crops the image based on the currently selected area
void crop(file_data *file)
{
	// If no file is loaded, reads the rest of the line and exits the function
	if (!file) {
		printf("No image loaded\n");
		char test_string[100];
		fgets(test_string, sizeof(test_string), stdin);
		return;
	}
	free(file->buffer);
	file->buffer = NULL;
	file->buffer = malloc(file->sel_size * sizeof(int));
	// Updates the dimensions, selects the selected coordinates
	file->width = file->sel_width;
	file->height = file->sel_height;
	file->x1 = 0;
	file->y1 = 0;
	file->x2 = file->width;
	file->y2 = file->height;
	if (!file->buffer) {
		perror("Error allocating memory");
		exit(1);
	}
	// Copies the required elements into the buffer
	for (int i = 0; i < file->sel_height; i++)
		for (int j = 0; j < file->sel_width * file->depth; j++)
			file->buffer[i * file->sel_width * file->depth + j] =
			file->sel_area[i * file->sel_width * file->depth + j];
	printf("Image cropped\n");
}

// Equalizes a black and white image
void equalize(file_data *file)
{
	int freq[256] = {0}, *equalized;
	if (!file) {
		printf("No image loaded\n");
		return;
	} else if (strcmp(file->type, "colour") == 0) {
		printf("Black and white image needed\n");
		return;
	}
	int area = file->width * file->height;
	for (int i = 0; i < area; i++)
		freq[file->buffer[i]]++;
	equalized = malloc(area * sizeof(int));
	// Copies the buffer elements into the temporary array equalized
	for (int i = 0; i < file->height; i++)
		for (int j = 0; j < file->width; j++)
			equalized[i * file->width + j] = file->buffer[i * file->width + j];
	for (int i = 0; i < area; i++) {
		double total = 0;
		for (int j = 0; j <= file->buffer[i]; j++)
			total += freq[j];
		double new_value = round((255 * total) / area);
		clamp(&new_value, 0, 255);
		equalized[i] = (int)new_value;
	}
	// Copies the equalized elements into the buffer
	for (int i = 0; i < file->height; i++)
		for (int j = 0; j < file->width; j++)
			file->buffer[i * file->width + j] = equalized[i * file->width + j];
	free(equalized);
	int k = 0;
	for (int i = file->y1; i < file->y2; i++)
		for (int j = file->x1; j < file->x2; j++)
			file->sel_area[k++] = file->buffer[i * file->width + j];
	printf("Equalize done\n");
}

// Applies the kernel specific for edge and modifies the image
void edge(file_data *file)
{
	int cols = file->width * file->depth, x1 = file->x1, y1 = file->y1;
	int x2 = file->x2, y2 = file->y2;
	double *temp;
	temp = malloc(cols * file->height * sizeof(double));
	for (int i = 0; i < file->height; i++)
		for (int j = 0; j < file->width * file->depth; j++)
			temp[i * cols + j] = file->buffer[i * cols + j];
	// Leaves edges of the image unchanged
	if (file->sel_size == file->height * file->depth * file->width) {
		x1++;
		y1++;
		x2--;
		y2--;
	} else {
		if (file->x1 == 0)
			x1++;
		if (file->x2 == file->width)
			x2--;
		if (file->y1 == 0)
			y1++;
		if (file->y2 == file->height)
			y2--;
	}
	// Applies kernel
	for (int i = y1; i < y2; i++)
		for (int j = x1 * file->depth; j < x2 * file->depth; j++) {
			temp[i * cols + j] = (-1) * (file->buffer[(i - 1) * cols + j - 3]
			+ file->buffer[(i - 1) * cols + j]
			+ file->buffer[(i - 1) * cols + j + 3]
			+ file->buffer[i * cols + j - 3]
			+ file->buffer[i * cols + j + 3]
			+ file->buffer[(i + 1) * cols + j - 3]
			+ file->buffer[(i + 1) * cols + j]
			+ file->buffer[(i + 1) * cols + j + 3])
			+ 8 * file->buffer[i * cols + j];
			clamp(&temp[i * cols + j], 0, 255);
		}
	for (int i = 0; i < file->height; i++)
		for (int j = 0; j < file->width * file->depth; j++)
			file->buffer[i * cols + j] = (int)temp[i * cols + j];
	free(temp);
}

// Applies the kernel specific for sharpen and modifies the image
void sharpen(file_data *file)
{
	int cols = file->width * file->depth, x1 = file->x1, y1 = file->y1;
	int x2 = file->x2, y2 = file->y2;
	double *temp;
	temp = malloc(cols * file->height * sizeof(double));
	for (int i = 0; i < file->height; i++)
		for (int j = 0; j < file->width * file->depth; j++)
			temp[i * cols + j] = file->buffer[i * cols + j];
	// Leaves edges of the image unchanged
	if (file->sel_size == file->height * file->depth * file->width) {
		x1++;
		y1++;
		x2--;
		y2--;
	} else {
		if (file->x1 == 0)
			x1++;
		if (file->x2 == file->width)
			x2--;
		if (file->y1 == 0)
			y1++;
		if (file->y2 == file->height)
			y2--;
	}
	for (int i = y1; i < y2; i++)
		for (int j = x1 * file->depth; j < x2 * file->depth; j++) {
			temp[i * cols + j] = (-1) * (file->buffer[(i - 1) * cols + j]
			+ file->buffer[i * cols + j - 3] + file->buffer[i * cols + j + 3]
			+ file->buffer[(i + 1) * cols + j])
			+ 5 * file->buffer[i * cols + j];
			clamp(&temp[i * cols + j], 0, 255);
		}
	for (int i = 0; i < file->height; i++)
		for (int j = 0; j < file->width * file->depth; j++)
			file->buffer[i * cols + j] = (int)temp[i * cols + j];
	free(temp);
}

// Applies the kernel specific for blur and modifies the image
void blur(file_data *file)
{
	int cols = file->width * file->depth, x1 = file->x1, y1 = file->y1;
	int x2 = file->x2, y2 = file->y2;
	double *temp;
	temp = malloc(cols * file->height * sizeof(double));
	for (int i = 0; i < file->height; i++)
		for (int j = 0; j < file->width * file->depth; j++)
			temp[i * cols + j] = file->buffer[i * cols + j];
	// Leaves edges of the image unchanged
	if (file->sel_size == file->height * file->depth * file->width) {
		x1++;
		y1++;
		x2--;
		y2--;
	} else {
		if (file->x1 == 0)
			x1++;
		if (file->x2 == file->width)
			x2--;
		if (file->y1 == 0)
			y1++;
		if (file->y2 == file->height)
			y2--;
	}
	double sum;
	for (int i = y1; i < y2; i++)
		for (int j = x1 * file->depth; j < x2 * file->depth; j++) {
			sum = file->buffer[(i - 1) * cols + j - 3] +
			file->buffer[(i - 1) * cols + j] +
			file->buffer[(i - 1) * cols + j + 3] +
			file->buffer[i * cols + j - 3] + file->buffer[i * cols + j] +
			file->buffer[i * cols + j + 3] +
			file->buffer[(i + 1) * cols + j - 3] +
			file->buffer[(i + 1) * cols + j] +
			file->buffer[(i + 1) * cols + j + 3];
			temp[i * cols + j] = round(sum / 9);
			clamp(&temp[i * cols + j], 0, 255);
		}
	for (int i = 0; i < file->height; i++)
		for (int j = 0; j < file->width * file->depth; j++)
			file->buffer[i * cols + j] = temp[i * cols + j];
	free(temp);
}

// Applies the kernel specific for gaussian blur and modifies the image
void gaussian_blur(file_data *file)
{
	int cols = file->width * file->depth, x1 = file->x1, y1 = file->y1;
	int x2 = file->x2, y2 = file->y2;
	double *temp;
	temp = malloc(cols * file->height * sizeof(double));
	for (int i = 0; i < file->height; i++)
		for (int j = 0; j < file->width * file->depth; j++)
			temp[i * cols + j] = file->buffer[i * cols + j];
	// Leaves edges of the image unchanged
	if (file->sel_size == file->height * file->depth * file->width) {
		x1++;
		y1++;
		x2--;
		y2--;
	} else {
		if (file->x1 == 0)
			x1++;
		if (file->x2 == file->width)
			x2--;
		if (file->y1 == 0)
			y1++;
		if (file->y2 == file->height)
			y2--;
	}
	double sum;
	for (int i = y1; i < y2; i++)
		for (int j = x1 * file->depth; j < x2 * file->depth; j++) {
			sum = file->buffer[(i - 1) * cols + j - 3] +
			file->buffer[(i - 1) * cols + j + 3] +
			file->buffer[(i + 1) * cols + j - 3] +
			file->buffer[(i + 1) * cols + j + 3] + 2 *
			(file->buffer[(i - 1) * cols + j] + file->buffer[i * cols + j - 3]
			+ file->buffer[i * cols + j + 3] +
			file->buffer[(i + 1) * cols + j]) + 4 * file->buffer[i * cols + j];
			temp[i * cols + j] = round(sum / 16);
			clamp(&temp[i * cols + j], 0, 255);
		}
	for (int i = 0; i < file->height; i++)
		for (int j = 0; j < file->width * file->depth; j++)
			file->buffer[i * cols + j] = temp[i * cols + j];
	free(temp);
}

/* Function which takes an apply parameter and
performs different operations based on it */
void apply_parameter(file_data *file, char parameter[20])
{
	if (strcmp(parameter, "EDGE") == 0) {
		edge(file);
	} else if (strcmp(parameter, "SHARPEN") == 0) {
		sharpen(file);
	} else if (strcmp(parameter, "BLUR") == 0) {
		blur(file);
	} else if (strcmp(parameter, "GAUSSIAN_BLUR") == 0) {
		gaussian_blur(file);
	} else {
		printf("APPLY parameter invalid\n");
		return;
	}
	printf("APPLY %s done\n", parameter);
}

// Checks if the apply command is valid
int check_apply(file_data *file, char parameter[20])
{
	int count = 0;
	char test_char;
	while (1) {
		scanf("%c", &test_char);
		if (test_char == ' ') {
			continue;
		} else if (test_char == '\n' && count != 1) {
			printf("Invalid command\n");
			return 0;
		} else if (isalpha(test_char) != 0 && count == 0) {
			ungetc(test_char, stdin);
			scanf("%s", parameter);
			break;
		}
	}
	if (strcmp(file->type, "colour") > 0) {
		printf("Easy, Charlie Chaplin\n");
		char test_string[100];
		fgets(test_string, sizeof(test_string), stdin);
		return 0;
	}
	return 1;
}

// Uses the specific image kernels to perform different functions
void apply(file_data *file)
{
	char parameter[20];
	// If no file is loaded, reads the rest of the line and exits the function
	if (!file) {
		printf("No image loaded\n");
		char test_string[100];
		fgets(test_string, sizeof(test_string), stdin);
		return;
	}
	if (check_apply(file, parameter) == 0)
		return;
	apply_parameter(file, parameter);
}

// Saves the current image in ascii or binary
void save(file_data *file)
{
	// If no file is loaded, reads the rest of the line and exits the function
	if (!file) {
		printf("No image loaded\n");
		char test_string[100];
		fgets(test_string, sizeof(test_string), stdin);
		return;
	}
	char file_name[100], test_char, format[10];
	int count = 0;
	scanf("%s", file_name);
	FILE *saved;
	while (1) {
		scanf("%c", &test_char);
		if (test_char == ' ') {
			continue;
		} else if (test_char == '\n' && count == 0) {
			saved = fopen(file_name, "wb");
			if (strcmp(file->magic_word, "P1") == 0)
				fprintf(saved, "P4\n");
			else if (strcmp(file->magic_word, "P2") == 0)
				fprintf(saved, "P5\n");
			else if (strcmp(file->magic_word, "P3") == 0)
				fprintf(saved, "P6\n");
			else
				fprintf(saved, "%s\n", file->magic_word);
			fprintf(saved, "%d %d\n", file->width, file->height);
			fprintf(saved, "%d\n", file->max_value);
			unsigned char temp_buffer;
			for (int i = 0 ; i < file->height ; i++) {
				for (int j = 0 ; j < file->depth * file->width ; j++) {
					temp_buffer = (unsigned char)file->buffer
					[i * file->depth * file->width + j];
					fwrite(&temp_buffer, sizeof(unsigned char), 1, saved);
				}
			}
			break;
		} else if (isalpha(test_char) != 0 && count == 0) {
			ungetc(test_char, stdin);
			scanf("%s", format);
			saved = fopen(file_name, "wt");
			if (strcmp(file->magic_word, "P4") == 0)
				fprintf(saved, "P1\n");
			else if (strcmp(file->magic_word, "P5") == 0)
				fprintf(saved, "P2\n");
			else if (strcmp(file->magic_word, "P6") == 0)
				fprintf(saved, "P3\n");
			else
				fprintf(saved, "%s\n", file->magic_word);
			fprintf(saved, "%d %d\n", file->width, file->height);
			fprintf(saved, "%d\n", file->max_value);
			for (int i = 0; i < file->height; i++) {
				for (int j = 0; j < file->width * file->depth; j++)
					fprintf(saved, "%d ", file->buffer
					[i * file->width * file->depth + j]);
				fprintf(saved, "\n");
			}
			break;
		}
	}
	fclose(saved);
	printf("Saved %s\n", file_name);
}

// Closes the program after freeing all its resources
void exit_program(file_data *file)
{
	if (!file) {
		printf("No image loaded\n");
	} else {
		free(file->buffer);
		free(file->sel_area);
	}
	free(file);
	file = NULL;
	exit(0);
}

// Rotates a black and white image counterclockwise by 90 degrees
void basic_rotate(file_data *file)
{
	int *rotated, k = 0;
	rotated = (int *)calloc(file->sel_height * file->sel_width, sizeof(int));
	if (file->sel_height == file->sel_width &&
		file->sel_size != file->height * file->width) {
		for (int i = file->sel_height - 1; i >= 0; i--)
			for (int j = 0; j < file->sel_width; j++)
				rotated[k++] = file->sel_area[j * file->sel_width + i];
		k = 0;
		for (int i = 0; i < file->sel_height; i++)
			for (int j = 0; j < file->sel_width; j++)
				file->sel_area[k++] = rotated[i * file->sel_width + j];
	} else if (file->sel_size == file->height * file->width) {
		for (int i = 0; i < file->sel_height; i++)
			for (int j = 0; j < file->sel_width; j++)
				rotated[(file->sel_width - 1 - j) * file->sel_height + i] =
				file->sel_area[i * file->sel_width + j];
		// Swaps the dimensions if the whole image is rotated
		swap(&file->sel_height, &file->sel_width);
		swap(&file->height, &file->width);
		swap(&file->y2, &file->x2);
		for (int i = 0; i < file->sel_height; i++)
			for (int j = 0; j < file->sel_width; j++)
				file->sel_area[i * file->sel_width + j] =
				rotated[i * file->sel_width + j];
	}
	k = 0;
	for (int i = file->y1; i < file->y2; i++)
		for (int j = file->x1; j < file->x2; j++)
			file->buffer[i * file->width + j] =
			file->sel_area[k++];
	free(rotated);
}

void colour_rotate(file_data *file)
{
	int *rotated, k = 0;
	rotated = (int *)calloc(file->sel_height *
	file->depth * file->sel_width, sizeof(int));
	if (file->sel_height == file->sel_width &&
		file->sel_size != file->height * file->width * file->depth) {
		for (int i = file->sel_height - 1; i >= 0; i--)
			for (int j = 0; j < file->sel_width * file->depth; j++)
				rotated[k++] = file->sel_area
				[j * file->sel_width * file->depth + i];
		k = 0;
	} else if (file->sel_size == file->height * file->width * file->depth) {
		for (int i = 0; i < file->sel_height; i++)
			for (int j = 0; j < file->sel_width * file->depth; j++) {
				rotated[(file->sel_width * file->depth - 1 - j) *
				file->sel_height + i] = file->sel_area[i * file->sel_width *
				file->depth + j];
				rotated[(file->sel_width * file->depth - 1 - j) *
				file->sel_height + i + 1] = file->sel_area[i *
				file->sel_width * file->depth + j + 1];
				rotated[(file->sel_width * file->depth - 1 - j) *
				file->sel_height + i + 2] = file->sel_area[i *
				file->sel_width * file->depth + j + 2];
			}
		swap(&file->sel_height, &file->sel_width);
		swap(&file->height, &file->width);
		swap(&file->y2, &file->x2);
		for (int i = 0; i < file->sel_height; i++)
			for (int j = 0; j < file->sel_width * file->depth; j++)
				file->sel_area[i * file->sel_width * file->depth + j] =
				rotated[i * file->sel_width * file->depth + j];
	}
	free(rotated);
}

// Rotates an image differently, depending on its type
void rotate(file_data *file)
{
	int angle;
	scanf("%d", &angle);
	if (!file) {
		printf("No image loaded\n");
		return;
	}
	if (angle % 90) {
		printf("Unsupported rotation angle\n");
		return;
	}
	printf("Rotated %d\n", angle);
	if ((file->sel_height == file->sel_width &&
		 file->sel_size != file->height * file->depth * file->width) ||
		 file->sel_size == file->height * file->depth * file->width) {
		if (angle < 0)
			angle = -angle;
		else
			angle = 360 - angle;
		if (angle == 0 || angle == 360 || angle == -360)
			return;
		if (strcmp(file->type, "colour") == 0) {
			while (angle >= 90) {
				colour_rotate(file);
				angle -= 90;
			}
		} else {
			while (angle >= 90) {
				basic_rotate(file);
				angle -= 90;
			}
		}
	} else {
		printf("The selection must be a square\n");
		return;
	}
}

int main(void)
{
	char file_name[100], command[50], test_string[5];
	int ok = 1;
	file_data *file;
	file = NULL;
	while (1) {
		scanf("%s", command);
		if (strcmp(command, "LOAD") == 0) {
			scanf("%s", file_name);
			if (file) {
				free(file->buffer);
				free(file->sel_area);
				free(file);
				file = NULL;
			}
			file = load(file_name);
		} else if (strcmp(command, "SELECT") == 0) {
			char test;
			ok = 1;
			scanf("%c", &test);
			if (test == '\n') {
				ok = 0;
				printf("Invalid command\n");
			} else {
				scanf("%c", &test);
			}
			if (isalpha(test) > 0 && ok == 1) {
				ungetc(test, stdin);
				scanf("%s", test_string);
				select_all(file);
			} else if (ok == 1) {
				ungetc(test, stdin);
				select_pixels(file);
			}
		} else if (strcmp(command, "EQUALIZE") == 0) {
			equalize(file);
		} else if (strcmp(command, "CROP") == 0) {
			crop(file);
		} else if (strcmp(command, "HISTOGRAM") == 0) {
			histogram(file);
		} else if (strcmp(command, "APPLY") == 0) {
			apply(file);
		} else if (strcmp(command, "SAVE") == 0) {
			save(file);
		} else if (strcmp(command, "EXIT") == 0) {
			exit_program(file);
		} else if (strcmp(command, "ROTATE") == 0) {
			rotate(file);
		} else {
			printf("Invalid command\n");
			char test_string[100];
			fgets(test_string, sizeof(test_string), stdin);
		}
	}
	return 0;
}
