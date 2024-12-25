#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "../Strix/header/strix.h"

typedef enum
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
} orientation_t;

typedef struct pointer
{
    int64_t pointer_row;
    int64_t pointer_col;
    orientation_t pointer_orientation;
} pointer_t;

static bool pointer_in_the_grid(const pointer_t *pointer, size_t total_rows, size_t total_cols);
static bool is_next_obstacle(const pointer_t *pointer, strix_t **rows, strix_t **cols, size_t total_rows, size_t total_cols);
static void turn_right(pointer_t *pointer);
static void move_ahead(pointer_t *pointer);
static void cleanup_resources(strix_t *input_strix, strix_t **columns, strix_arr_t *lines_arr, size_t total_cols);

static bool pointer_in_the_grid(const pointer_t *pointer, size_t total_rows, size_t total_cols)
{
    if (!pointer)
        return false;
    return (pointer->pointer_row > 0 && pointer->pointer_row < (int64_t)total_rows - 1 &&
            pointer->pointer_col > 0 && pointer->pointer_col < (int64_t)total_cols - 1);
}

static bool is_next_obstacle(const pointer_t *pointer, strix_t **rows, strix_t **cols, size_t total_rows, size_t total_cols)
{
    if (!pointer || !rows || !cols)
        return true;

    int64_t next_row = pointer->pointer_row;
    int64_t next_col = pointer->pointer_col;

    switch (pointer->pointer_orientation)
    {
    case UP:
        next_row--;
        break;
    case DOWN:
        next_row++;
        break;
    case LEFT:
        next_col--;
        break;
    case RIGHT:
        next_col++;
        break;
    }

    if (next_row < 0 || next_row >= (int64_t)total_rows ||
        next_col < 0 || next_col >= (int64_t)total_cols)
        return true;

    switch (pointer->pointer_orientation)
    {
    case UP:
    case DOWN:
        return rows[next_row]->str[pointer->pointer_col] == '#';
    case LEFT:
    case RIGHT:
        return cols[next_col]->str[pointer->pointer_row] == '#';
    default:
        return true;
    }
}

static void turn_right(pointer_t *pointer)
{
    if (!pointer)
        return;

    switch (pointer->pointer_orientation)
    {
    case UP:
        pointer->pointer_orientation = RIGHT;
        break;
    case RIGHT:
        pointer->pointer_orientation = DOWN;
        break;
    case DOWN:
        pointer->pointer_orientation = LEFT;
        break;
    case LEFT:
        pointer->pointer_orientation = UP;
        break;
    }
}

static void move_ahead(pointer_t *pointer)
{
    if (!pointer)
        return;

    switch (pointer->pointer_orientation)
    {
    case UP:
        pointer->pointer_row--;
        break;
    case DOWN:
        pointer->pointer_row++;
        break;
    case LEFT:
        pointer->pointer_col--;
        break;
    case RIGHT:
        pointer->pointer_col++;
        break;
    }
}

static void cleanup_resources(strix_t *input_strix, strix_t **columns, strix_arr_t *lines_arr, size_t total_cols)
{
    if (input_strix)
        strix_free(input_strix);

    if (columns)
    {
        for (size_t i = 0; i < total_cols; i++)
        {
            if (columns[i])
                strix_free(columns[i]);
        }
        free(columns);
    }

    if (lines_arr)
        strix_free_strix_arr(lines_arr);
}

int main()
{
    FILE *input_file = fopen("input.txt", "r");
    if (!input_file)
    {
        fprintf(stderr, "Failed to open input file\n");
        return EXIT_FAILURE;
    }

    if (fseek(input_file, 0, SEEK_END) != 0)
    {
        fprintf(stderr, "Failed to seek to end of file\n");
        fclose(input_file);
        return EXIT_FAILURE;
    }

    long input_file_len = ftell(input_file);
    if (input_file_len < 0)
    {
        fprintf(stderr, "Failed to get file length\n");
        fclose(input_file);
        return EXIT_FAILURE;
    }

    if (fseek(input_file, 0, SEEK_SET) != 0)
    {
        fprintf(stderr, "Failed to seek to start of file\n");
        fclose(input_file);
        return EXIT_FAILURE;
    }

    uint8_t *input_file_array = (uint8_t *)malloc((input_file_len + 1) * sizeof(uint8_t));
    if (!input_file_array)
    {
        fprintf(stderr, "Failed to allocate memory for file content\n");
        fclose(input_file);
        return EXIT_FAILURE;
    }
    input_file_array[input_file_len] = '\0';

    size_t bytes_read = fread(input_file_array, 1, input_file_len, input_file);
    if (bytes_read != (size_t)input_file_len)
    {
        fprintf(stderr, "Failed to read file content\n");
        free(input_file_array);
        fclose(input_file);
        return EXIT_FAILURE;
    }
    fclose(input_file);

    strix_t *input_strix = strix_create((const char *)input_file_array);
    free(input_file_array);
    if (!input_strix)
    {
        fprintf(stderr, "Failed to create input strix\n");
        return EXIT_FAILURE;
    }

    strix_arr_t *lines_arr = strix_split_by_delim(input_strix, '\n');
    if (!lines_arr)
    {
        fprintf(stderr, "Failed to split input into lines\n");
        strix_free(input_strix);
        return EXIT_FAILURE;
    }

    size_t total_rows = lines_arr->len;
    strix_t **rows = lines_arr->strix_arr;
    size_t total_cols = rows[0]->len;

    strix_t **columns = (strix_t **)malloc(sizeof(strix_t *) * total_cols);
    if (!columns)
    {
        fprintf(stderr, "Failed to allocate memory for columns\n");
        cleanup_resources(input_strix, NULL, lines_arr, 0);
        return EXIT_FAILURE;
    }

    for (size_t i = 0; i < total_cols; i++)
    {
        columns[i] = strix_slice_by_stride(input_strix, i, input_strix->len - 1, total_cols + 1);
        if (!columns[i])
        {
            fprintf(stderr, "Failed to create column %zu\n", i);
            cleanup_resources(input_strix, columns, lines_arr, i);
            return EXIT_FAILURE;
        }
    }

    int64_t pointer_pos = strix_find(input_strix, "^");
    if (pointer_pos < 0)
    {
        fprintf(stderr, "Failed to find starting position\n");
        cleanup_resources(input_strix, columns, lines_arr, total_cols);
        return EXIT_FAILURE;
    }

    pointer_t pointer = {
        .pointer_row = pointer_pos / (total_cols + 1),
        .pointer_col = pointer_pos % (total_cols + 1),
        .pointer_orientation = UP,
    };

    input_strix->str[pointer_pos] = '.';

    const size_t max_grid_size = 20000;
    bool *already_visited = (bool *)calloc(max_grid_size, sizeof(bool));
    if (!already_visited)
    {
        fprintf(stderr, "Failed to allocate memory for visited array\n");
        cleanup_resources(input_strix, columns, lines_arr, total_cols);
        return EXIT_FAILURE;
    }

    size_t visited_count = 1; // visited it's initial position

    while (pointer_in_the_grid(&pointer, total_rows, total_cols))
    {
        int64_t current_pos = pointer.pointer_row * total_cols + pointer.pointer_col;

        if (current_pos >= 0 && current_pos < (int64_t)max_grid_size && !already_visited[current_pos])
        {
            already_visited[current_pos] = true;
            visited_count++;
        }

        if (is_next_obstacle(&pointer, rows, columns, total_rows, total_cols))
        {
            turn_right(&pointer);
        }
        else
        {
            move_ahead(&pointer);
        }

        printf("row: %ld col: %ld\n", pointer.pointer_row, pointer.pointer_col);
    }

    printf("Total visited positions: %zu\n", visited_count);

    free(already_visited);
    cleanup_resources(input_strix, columns, lines_arr, total_cols);
    return EXIT_SUCCESS;
}