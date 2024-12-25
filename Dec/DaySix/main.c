#include <stdlib.h>
#include <stdio.h>

#include "../Strix/header/strix.h"

int main()
{
    FILE *input_file = fopen("input.txt", "r");
    if (!input_file)
    {
        fprintf(stderr, "Failed to open input file\n");
        return EXIT_FAILURE;
    }

    int input_file_len = fseek(input_file, 0, SEEK_END);
    if (input_file_len < 0)
    {
        fprintf(stderr, "Failed to seek to end of file\n");
        fclose(input_file);
        return EXIT_FAILURE;
    }

    input_file_len = (int)ftell(input_file);
    if (input_file_len < 0)
    {
        fprintf(stderr, "Failed to get file length\n");
        fclose(input_file);
        return EXIT_FAILURE;
    }

    if (fseek(input_file, 0, SEEK_SET) < 0)
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

    size_t bytes_read = fread((void *)input_file_array, sizeof(uint8_t), input_file_len, input_file);
    if (bytes_read < (size_t)(input_file_len / sizeof(uint8_t)))
    {
        fprintf(stderr, "Failed to read file content\n");
        fclose(input_file);
        free(input_file_array);
        return EXIT_FAILURE;
    }

    strix_t *input_strix = strix_create((const char *)input_file_array);
    if (!input_strix)
    {
        fprintf(stderr, "Failed to create input strix\n");
        fclose(input_file);
        free(input_file_array);
        return EXIT_FAILURE;
    }
    free(input_file_array);
    fclose(input_file);

    strix_arr_t *lines_arr = strix_split_by_delim(input_strix, '\n');
    if (!lines_arr)
    {
        return EXIT_FAILURE;
    }

    size_t total_rows = lines_arr->len;
    strix_t **rows = lines_arr->strix_arr;

    size_t total_cols = rows[0]->len;
    strix_t **columns = (strix_t **)malloc(sizeof(strix_t *) * total_cols);
    if (!columns)
    {
        strix_free_strix_arr(lines_arr);
        return EXIT_FAILURE;
    }

    for (size_t counter = 0; counter < total_cols; counter++)
    {
        columns[counter] = strix_slice_by_stride(input_strix, counter, input_strix->len - 1, total_cols + 1);
        fprintf(stdout, STRIX_FORMAT "\n", STRIX_PRINT(columns[counter]));
        if (!columns[counter])
        {
            for (size_t i = 0; i < counter; i++)
            {
                strix_free(columns[i]);
            }
            strix_free_strix_arr(lines_arr);
            strix_free(input_strix);
            free(columns);
            return EXIT_FAILURE;
        }
    }

    strix_free(input_strix);

    for (size_t counter = 0; counter < total_cols; counter++)
    {
        strix_free(columns[counter]);
    }
    free(columns);
    strix_free_strix_arr(lines_arr);
}