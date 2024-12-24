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

    uint8_t *input_file_array = (uint8_t *)malloc(input_file_len * sizeof(uint8_t));
    if (!input_file_array)
    {
        fprintf(stderr, "Failed to allocate memory for file content\n");
        fclose(input_file);
        return EXIT_FAILURE;
    }

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

    int64_t total_count = 0;

    strix_arr_t *lines = strix_split_by_delim(input_strix, '\n');
    if (!lines)
    {
        fprintf(stderr, "Failed to split input into lines\n");
        strix_free(input_strix);
        return EXIT_FAILURE;
    }

    for (size_t counter = 0; counter < lines->len; counter++)
    {
        strix_t *line = lines->strix_arr[counter];
        if (!line)
        {
            fprintf(stderr, "Invalid line encountered\n");
            strix_free_strix_arr(lines);
            strix_free(input_strix);
            return EXIT_FAILURE;
        }
        total_count += (strix_count_substr(line, "XMAS") + strix_count_substr(line, "SAMX"));
    }

    strix_t *first_line = lines->strix_arr[0];
    if (!first_line)
    {
        fprintf(stderr, "First line is invalid\n");
        strix_free_strix_arr(lines);
        strix_free(input_strix);
        return EXIT_FAILURE;
    }

    for (size_t counter = 0; counter < first_line->len; counter++)
    {
        strix_t *column = strix_slice_by_stride(input_strix, counter, input_strix->len - 1, first_line->len + 1);
        if (!column)
        {
            fprintf(stderr, "Failed to create column slice\n");
            strix_free_strix_arr(lines);
            strix_free(input_strix);
            return EXIT_FAILURE;
        }
        total_count += (strix_count_substr(column, "XMAS") + strix_count_substr(column, "SAMX"));
        strix_free(column);
    }

    for (size_t counter = 0; counter < lines->len - 3; counter++)
    {
        strix_t *first = lines->strix_arr[counter];
        strix_t *second = lines->strix_arr[counter + 1];
        strix_t *third = lines->strix_arr[counter + 2];
        strix_t *fourth = lines->strix_arr[counter + 3];

        if (!first || !second || !third || !fourth)
        {
            fprintf(stderr, "Invalid line in forward diagonal search\n");
            strix_free_strix_arr(lines);
            strix_free(input_strix);
            return EXIT_FAILURE;
        }

        for (size_t index = 0; index < first->len - 3; index++)
        {
            if (first->str[index] == 'X' &&
                second->str[index + 1] == 'M' &&
                third->str[index + 2] == 'A' &&
                fourth->str[index + 3] == 'S')
            {
                total_count++;
            }

            if (first->str[index] == 'S' &&
                second->str[index + 1] == 'A' &&
                third->str[index + 2] == 'M' &&
                fourth->str[index + 3] == 'X')
            {
                total_count++;
            }
        }
    }

    for (size_t counter = 0; counter < lines->len - 3; counter++)
    {
        strix_t *first = lines->strix_arr[counter];
        strix_t *second = lines->strix_arr[counter + 1];
        strix_t *third = lines->strix_arr[counter + 2];
        strix_t *fourth = lines->strix_arr[counter + 3];

        if (!first || !second || !third || !fourth)
        {
            fprintf(stderr, "Invalid line in backward diagonal search\n");
            strix_free_strix_arr(lines);
            strix_free(input_strix);
            return EXIT_FAILURE;
        }

        for (size_t index = 3; index < first->len; index++)
        {
            if (first->str[index] == 'X' &&
                second->str[index - 1] == 'M' &&
                third->str[index - 2] == 'A' &&
                fourth->str[index - 3] == 'S')
            {
                total_count++;
            }

            if (first->str[index] == 'S' &&
                second->str[index - 1] == 'A' &&
                third->str[index - 2] == 'M' &&
                fourth->str[index - 3] == 'X')
            {
                total_count++;
            }
        }
    }

    fprintf(stdout, "first_part: %ld\n", total_count);

    total_count = 0;

    for (size_t i = 1; i < lines->len - 1; i++)
    {
        strix_t *prev_line = lines->strix_arr[i - 1];
        strix_t *curr_line = lines->strix_arr[i];
        strix_t *next_line = lines->strix_arr[i + 1];

        if (!prev_line || !curr_line || !next_line)
        {
            fprintf(stderr, "Invalid line encountered in X-MAS pattern search\n");
            strix_free_strix_arr(lines);
            strix_free(input_strix);
            return EXIT_FAILURE;
        }

        for (size_t j = 1; j < curr_line->len - 1; j++)
        {
            if (curr_line->str[j] == 'A')
            {
                if (prev_line->str[j - 1] == 'M' && next_line->str[j + 1] == 'S' &&
                    prev_line->str[j + 1] == 'M' && next_line->str[j - 1] == 'S')
                {
                    total_count++;
                }
                if (prev_line->str[j - 1] == 'S' && next_line->str[j + 1] == 'M' &&
                    prev_line->str[j + 1] == 'S' && next_line->str[j - 1] == 'M')
                {
                    total_count++;
                }
                if (prev_line->str[j - 1] == 'M' && next_line->str[j + 1] == 'S' &&
                    prev_line->str[j + 1] == 'S' && next_line->str[j - 1] == 'M')
                {
                    total_count++;
                }
                if (prev_line->str[j - 1] == 'S' && next_line->str[j + 1] == 'M' &&
                    prev_line->str[j + 1] == 'M' && next_line->str[j - 1] == 'S')
                {
                    total_count++;
                }
            }
        }
    }

    fprintf(stdout, "second_part: %ld\n", total_count);

    strix_free_strix_arr(lines);
    strix_free(input_strix);

    return EXIT_SUCCESS;
}