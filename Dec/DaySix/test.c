#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 150
#define OBSTACLE '#'
#define FREE '.'
#define START '^'

int DIRECTION_DELTAS[4][2] = {
	{-1, 0},  // Up
	{0, 1},   // Right
	{1, 0},   // Down
	{0, -1}   // Left
};

// Check if the cell is within bounds
int inside(int i, int j, int rows, int cols) {
	return i >= 0 && i < rows && j >= 0 && j < cols;
}

// Check if the cell is an obstacle
int is_obstacle(int i, int j, char matrix[MAX][MAX]) {
	return matrix[i][j] == OBSTACLE;
}

// Check if there's a loop starting from (si, sj) with direction tracking
int check_loop(int si, int sj, char matrix[MAX][MAX], int rows, int cols) {
	int delta_idx = 0;
	int ci = si, cj = sj;
	int visited[MAX][MAX][4] = {{{0}}};  // Track visited state with direction

	visited[ci][cj][delta_idx] = 1;

	while (1) {
		int di = DIRECTION_DELTAS[delta_idx][0];
		int dj = DIRECTION_DELTAS[delta_idx][1];
		int ni = ci + di, nj = cj + dj;

		if (!inside(ni, nj, rows, cols)) break;

		if (is_obstacle(ni, nj, matrix)) {
			delta_idx = (delta_idx + 1) % 4;
			continue;
		}

		ci = ni;
		cj = nj;

		if (visited[ci][cj][delta_idx]) return 1;

		visited[ci][cj][delta_idx] = 1;
	}

	return 0;
}

// Solve the second part of the problem
void solve_second(int si, int sj, char matrix[MAX][MAX], int visited[MAX][MAX], int rows, int cols) {
	int ans = 0;

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			if (!visited[i][j] || (i == si && j == sj)) continue;

			// Temporarily modify the matrix
			matrix[i][j] = OBSTACLE;

			// Check for a loop
			ans += check_loop(si, sj, matrix, rows, cols);

			// Restore the matrix
			matrix[i][j] = FREE;
		}
	}

	printf("%d\n", ans);
}

// Parse the input and initialize the matrix
void parse(char *data, int *si, int *sj, char matrix[MAX][MAX], int *rows, int *cols) {
	*rows = 0;
	*cols = 0;
	int found_start = 0;

	char *line = strtok(data, "\n");
	while (line) {
		int len = strlen(line);
		if (*cols == 0) *cols = len;

		for (int j = 0; j < len; j++) {
			matrix[*rows][j] = line[j];
			if (line[j] == START) {
				*si = *rows;
				*sj = j;
				matrix[*rows][j] = FREE;  // Treat START as FREE
				found_start = 1;
			}
		}

		(*rows)++;
		line = strtok(NULL, "\n");
	}

	if (!found_start) {
		fprintf(stderr, "No starting point found.\n");
		exit(1);
	}
}

int main() {
	char data[MAX * MAX];
	char matrix[MAX][MAX];
	int visited[MAX][MAX] = {{0}};
	int rows, cols;
	int si, sj;

	// Read the input data
	FILE *fp = fopen("input.txt", "r");
	if (!fp) {
		fprintf(stderr, "Failed to open input file.\n");
		return 1;
	}

	int idx = 0;
	while (fscanf(fp, "%c", &data[idx]) != EOF) idx++;
	data[idx] = '\0';
	fclose(fp);

	// Parse the input
	parse(data, &si, &sj, matrix, &rows, &cols);

	// Simulate the first solution (visit all reachable cells)
	int ci = si, cj = sj, delta_idx = 0;
	visited[ci][cj] = 1;

	while (1) {
		int di = DIRECTION_DELTAS[delta_idx][0];
		int dj = DIRECTION_DELTAS[delta_idx][1];
		int ni = ci + di, nj = cj + dj;

		if (!inside(ni, nj, rows, cols)) break;

		if (is_obstacle(ni, nj, matrix)) {
			delta_idx = (delta_idx + 1) % 4;
			continue;
		}

		ci = ni;
		cj = nj;
		visited[ci][cj] = 1;
	}

	// Solve the second part
	solve_second(si, sj, matrix, visited, rows, cols);

	return 0;
}
