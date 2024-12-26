#include <stdio.h>
#include <stdlib.h>
#include "../Strix/header/strix.h"

void follow_trail(char **map, int64_t start_row, int64_t start_col, size_t *total_trails, size_t total_cols, size_t total_rows)
{
    char curr_num = map[start_row][start_col];

    if (curr_num == 9)
    {
        (*total_trails)++;
        return;
    }

    if (start_row + 1 < total_rows && map[start_row + 1][start_col] == curr_num + 1)
    {
        follow_trail(map, start_row + 1, start_col, total_trails, total_cols, total_rows);
    }

    if (start_row - 1 >= 0 && map[start_row - 1][start_col] == curr_num + 1)
    {
        follow_trail(map, start_row - 1, start_col, total_trails, total_cols, total_rows);
    }

    if (start_col + 1 < total_cols && map[start_row][start_col + 1] == curr_num + 1)
    {
        follow_trail(map, start_row, start_col + 1, total_trails, total_cols, total_rows);
    }

    if (start_col - 1 >= 0 && map[start_row][start_col - 1] == curr_num + 1)
    {
        follow_trail(map, start_row, start_col - 1, total_trails, total_cols, total_rows);
    }
}

int main()
{
    strix_t *input_strix = conv_file_to_strix("input.txt");
    if (!input_strix)
    {
        return EXIT_FAILURE;
    }

    int64_t no_of_cols = strix_find(input_strix, "\n");
    if (no_of_cols <= 0)
    {
        strix_free(input_strix);
        return EXIT_FAILURE;
    }

    size_t newline_count = 0;
    for (size_t i = 0; i < input_strix->len; i++)
    {
        if (input_strix->str[i] == '\n')
            newline_count++;
    }
    int64_t no_of_rows = newline_count + 1;

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
            {
                free(map[j]);
            }
            free(map);
            strix_free(input_strix);
            return EXIT_FAILURE;
        }
    }

    size_t input_pos = 0;
    for (size_t i = 0; i < no_of_rows; i++)
    {
        for (size_t j = 0; j < no_of_cols; j++)
        {
            map[i][j] = input_strix->str[input_pos] - '0';
            input_pos++;
        }
        input_pos++;
    }

    fprintf(stdout, "Map:\n");
    for (size_t i = 0; i < no_of_rows; i++)
    {
        for (size_t j = 0; j < no_of_cols; j++)
        {
            fprintf(stdout, "%d", map[i][j]);
        }
        fprintf(stdout, "\n");
    }

    size_t total_trails = 0;

    for (size_t i = 0; i < no_of_rows; i++)
    {
        for (size_t j = 0; j < no_of_cols; j++)
        {
            if (map[i][j] == 0)
            {
                follow_trail(map, i, j, &total_trails, no_of_cols, no_of_rows);
            }
        }
    }

    fprintf(stdout, "total trails (part two): %zu\n", total_trails);

    for (size_t i = 0; i < no_of_rows; i++)
    {
        free(map[i]);
    }
    free(map);
    strix_free(input_strix);

    return EXIT_SUCCESS;
}