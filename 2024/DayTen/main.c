#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../Strix/header/strix.h"

void find_reachable_nines(char **map, int64_t start_row, int64_t start_col,
                          size_t total_cols, size_t total_rows,
                          bool **reachable_nines)
{
    char curr_num = map[start_row][start_col];

    if (curr_num == 9)
    {
        reachable_nines[start_row][start_col] = true;
        return;
    }

    const int directions[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

    for (int i = 0; i < 4; i++)
    {
        int64_t new_row = start_row + directions[i][0];
        int64_t new_col = start_col + directions[i][1];

        if (new_row < 0 || new_row >= total_rows || new_col < 0 || new_col >= total_cols)
            continue;

        if (map[new_row][new_col] == curr_num + 1)
        {
            find_reachable_nines(map, new_row, new_col, total_cols, total_rows,
                                 reachable_nines);
        }
    }
}

size_t count_reachable_nines(bool **reachable_nines, size_t rows, size_t cols)
{
    size_t count = 0;
    for (size_t i = 0; i < rows; i++)
    {
        for (size_t j = 0; j < cols; j++)
        {
            if (reachable_nines[i][j])
                count++;
        }
    }
    return count;
}

int main()
{
    strix_t *input_strix = conv_file_to_strix("input.txt");
    if (!input_strix)
        return EXIT_FAILURE;

    int64_t no_of_cols = strix_find(input_strix, "\n");
    if (!strix_delete_occurence(input_strix, "\n"))
    {
        strix_free(input_strix);
        return EXIT_FAILURE;
    }

    int64_t no_of_rows = input_strix->len / no_of_cols;
    char **map = malloc(sizeof(char *) * no_of_rows);
    if (!map)
    {
        strix_free(input_strix);
        return EXIT_FAILURE;
    }

    for (size_t i = 0; i < no_of_rows; i++)
    {
        map[i] = malloc(sizeof(char) * no_of_cols);
        if (!map[i])
        {
            for (size_t j = 0; j < i; j++)
                free(map[j]);
            free(map);
            strix_free(input_strix);
            return EXIT_FAILURE;
        }
    }

    for (size_t i = 0; i < no_of_rows; i++)
    {
        for (size_t j = 0; j < no_of_cols; j++)
        {
            map[i][j] = input_strix->str[i * no_of_cols + j] - '0';
        }
    }

    position_t *start_positions = strix_find_all(input_strix, "0");
    if (!start_positions)
    {
        for (size_t i = 0; i < no_of_rows; i++)
            free(map[i]);
        free(map);
        strix_free(input_strix);
        return EXIT_FAILURE;
    }

    bool **reachable_nines = malloc(sizeof(bool *) * no_of_rows);
    if (!reachable_nines)
        return EXIT_FAILURE;

    for (size_t i = 0; i < no_of_rows; i++)
    {
        reachable_nines[i] = calloc(no_of_cols, sizeof(bool));
        if (!reachable_nines[i])
            return EXIT_FAILURE;
    }

    size_t total_score = 0;
    for (size_t counter = 0; counter < start_positions->len; counter++)
    {
        for (size_t i = 0; i < no_of_rows; i++)
        {
            for (size_t j = 0; j < no_of_cols; j++)
            {
                reachable_nines[i][j] = false;
            }
        }

        size_t start_position = start_positions->pos[counter];
        int64_t start_row = start_position / no_of_cols;
        int64_t start_col = start_position % no_of_cols;

        find_reachable_nines(map, start_row, start_col, no_of_cols, no_of_rows,
                             reachable_nines);

        total_score += count_reachable_nines(reachable_nines, no_of_rows, no_of_cols);
    }

    fprintf(stdout, "Sum of trailhead scores: %zu\n", total_score);

    for (size_t i = 0; i < no_of_rows; i++)
    {
        free(reachable_nines[i]);
    }
    free(reachable_nines);

    for (size_t i = 0; i < no_of_rows; i++)
        free(map[i]);
    free(map);
    free(start_positions);
    strix_free(input_strix);

    return EXIT_SUCCESS;
}