#include <stdlib.h>
#include <stdio.h>
#include "../Strix/header/strix.h"

#include <stdbool.h>
#include <string.h>

bool is_sequence_valid(const int32_t *line, size_t len)
{
    if (len <= 1)
        return true;

    bool should_increase = (line[1] - line[0]) > 0;

    for (size_t i = 0; i < len - 1; i++)
    {
        int32_t diff = line[i + 1] - line[i];

        if (should_increase)
        {
            if (diff < 1 || diff > 3)
                return false;
        }
        else
        {
            if (diff > -1 || diff < -3)
                return false;
        }
    }
    return true;
}

bool is_line_correct(int32_t *line, size_t len)
{
    if (is_sequence_valid(line, len))
        return true;

    int32_t temp[len];
    for (size_t i = 0; i < len; i++)
    {
        size_t new_len = 0;
        for (size_t j = 0; j < len; j++)
        {
            if (j != i)
            {
                temp[new_len++] = line[j];
            }
        }

        if (is_sequence_valid(temp, len - 1))
        {
            return true;
        }
    }

    return false;
}

int main()
{
    FILE *input_file = fopen("input.txt", "r");
    if (!input_file)
    {
        return EXIT_FAILURE;
    }
    int input_file_len = fseek(input_file, 0, SEEK_END);
    if (input_file_len < 0)
    {
        fclose(input_file);
        return EXIT_FAILURE;
    }
    input_file_len = (int)ftell(input_file);
    if (input_file_len < 0)
    {
        fclose(input_file);
        return EXIT_FAILURE;
    }
    if (fseek(input_file, 0, SEEK_SET) < 0)
    {
        fclose(input_file);
        return EXIT_FAILURE;
    }
    uint8_t *input_file_array = (uint8_t *)malloc(input_file_len * sizeof(uint8_t));
    if (!input_file_array)
    {
        fclose(input_file);
        return EXIT_FAILURE;
    }
    if (fread((void *)input_file_array, sizeof(uint8_t), input_file_len, input_file) < input_file_len / sizeof(uint8_t))
    {
        fclose(input_file);
        free(input_file_array);
        return EXIT_FAILURE;
    }
    strix_t *input_strix = strix_create((const char *)input_file_array);
    if (!input_strix)
    {
        fclose(input_file);
        free(input_file_array);
        return EXIT_FAILURE;
    }
    free(input_file_array);
    strix_arr_t *input_lines = strix_split_by_delim(input_strix, '\n');
    if (!input_lines)
    {
        fclose(input_file);
        strix_free(input_strix);
        return EXIT_FAILURE;
    }
    strix_free(input_strix);

    strix_t *line;
    strix_arr_t *line_num;
#define MAX_LINE_NUM 64
    int32_t *line_arr = (int32_t *)malloc(sizeof(int32_t) * MAX_LINE_NUM);
    if (!line_arr)
    {
        fclose(input_file);
        strix_free_strix_arr(input_lines);
        return EXIT_FAILURE;
    }

    int32_t correct_count_part_one = 0;
    int32_t correct_count_part_two = 0;

    for (size_t counter = 0; counter < input_lines->len; counter++)
    {
        line = input_lines->strix_arr[counter];
        line_num = strix_split_by_delim(line, ' ');
        if (!line_num)
        {
            fclose(input_file);
            strix_free_strix_arr(input_lines);
            free(line_arr);
            return EXIT_FAILURE;
        }

        if (line_num->len > MAX_LINE_NUM)
        {
            fclose(input_file);
            strix_free_strix_arr(input_lines);
            strix_free_strix_arr(line_num);
            free(line_arr);
            return EXIT_FAILURE;
        }

        for (size_t second_counter = 0; second_counter < line_num->len; second_counter++)
        {
            line_arr[second_counter] = strix_to_signed_int(line_num->strix_arr[second_counter]);
            if (strix_errno != STRIX_SUCCESS)
            {
                fclose(input_file);
                strix_free_strix_arr(input_lines);
                strix_free_strix_arr(line_num);
                free(line_arr);
                return EXIT_FAILURE;
            }
        }

        if (is_line_correct(line_arr, line_num->len))
        {
            correct_count_part_two++;
        }

        if (is_sequence_valid(line_arr, line_num->len))
        {
            correct_count_part_one++;
        }

        strix_free_strix_arr(line_num);
    }

    free(line_arr);
    strix_free_strix_arr(input_lines);
    fclose(input_file);

    fprintf(stdout, "Number of correct lines (part_one): %d\n", correct_count_part_one);
    fprintf(stdout, "Number of correct lines (part_two): %d\n", correct_count_part_two);

    return EXIT_SUCCESS;
}