#include <stdlib.h>
#include <stdio.h>
#include "../Strix/header/strix.h"
#include <math.h>

bool compute_perm(char *operators, int64_t len, int64_t index, int64_t result, int64_t *nums)
{
    if (index == len)
    {
        int64_t accum = nums[0];
        for (size_t counter = 0; counter < len; counter++)
        {
            if (operators[counter] == 0)
            {
                accum *= nums[counter + 1];
            }
            else if (operators[counter] == 1)
            {
                accum += nums[counter + 1];
            }
            else if (operators[counter] == 2)
            {
                accum = accum * pow(10, floor(log10(nums[counter + 1]) + 1)) + nums[counter + 1];
            }
        }

        return accum == result;
    }

    operators[index] = 0;
    if (compute_perm(operators, len, index + 1, result, nums))
    {
        return true;
    }

    operators[index] = 1;
    if (compute_perm(operators, len, index + 1, result, nums))
    {
        return true;
    }

    operators[index] = 2;
    if (compute_perm(operators, len, index + 1, result, nums))
    {
        return true;
    }

    return false;
}

bool compute_perm_first(char *operators, int64_t len, int64_t index, int64_t result, int64_t *nums)
{
    if (index == len)
    {
        int64_t accum = nums[0];
        for (size_t counter = 0; counter < len; counter++)
        {
            if (operators[counter] == 0)
            {
                accum *= nums[counter + 1];
            }
            else if (operators[counter] == 1)
            {
                accum += nums[counter + 1];
            }
        }

        return accum == result;
    }

    operators[index] = 0;
    if (compute_perm_first(operators, len, index + 1, result, nums))
    {
        return true;
    }

    operators[index] = 1;
    if (compute_perm_first(operators, len, index + 1, result, nums))
    {
        return true;
    }

    return false;
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

    strix_arr_t *lines = strix_split_by_delim(input_strix, '\n');
    if (!lines)
    {
        fprintf(stderr, "Failed to split input into lines\n");
        strix_free(input_strix);
        return EXIT_FAILURE;
    }

    int64_t one_total = 0;
    int64_t two_total = 0;

    for (size_t counter = 0; counter < lines->len; counter++)
    {
        strix_t *line = lines->strix_arr[counter];
        strix_arr_t *split = strix_split_by_delim(line, ':');
        if (!split || split->len != 2)
        {
            fprintf(stderr, "Invalid input format in line %zu\n", counter);
            continue;
        }

        int64_t result = strix_to_signed_int(split->strix_arr[0]);
        if (result == 0 && strix_get_error() != STRIX_SUCCESS)
        {
            fprintf(stderr, "Failed to parse result in line %zu\n", counter);
            continue;
        }

        strix_arr_t *num_split = strix_split_by_delim(split->strix_arr[1], ' ');
        if (!num_split)
        {
            fprintf(stderr, "Failed to split numbers in line %zu\n", counter);
            continue;
        }

        int64_t *nums = malloc(sizeof(int64_t) * num_split->len);
        if (!nums)
        {
            fprintf(stderr, "Failed to allocate memory for numbers in line %zu\n", counter);
            strix_free_strix_arr(num_split);
            continue;
        }

        for (size_t index = 0; index < num_split->len; index++)
        {
            nums[index] = strix_to_signed_int(num_split->strix_arr[index]);
            if (nums[index] == 0 && strix_get_error() != STRIX_SUCCESS)
            {
                fprintf(stderr, "Failed to parse number in line %zu\n", counter);
                free(nums);
                strix_free_strix_arr(num_split);
                continue;
            }
        }

        char *operators = calloc(num_split->len - 1, sizeof(char));
        if (!operators)
        {
            fprintf(stderr, "Failed to allocate memory for operators in line %zu\n", counter);
            free(nums);
            strix_free_strix_arr(num_split);
            continue;
        }

        if (compute_perm_first(operators, num_split->len - 1, 0, result, nums))
        {
            one_total += result;
        }

        if (compute_perm(operators, num_split->len - 1, 0, result, nums))
        {
            two_total += result;
        }

        free(operators);
        free(nums);
        strix_free_strix_arr(num_split);
    }

    strix_free_strix_arr(lines);
    strix_free(input_strix);

    printf("part_one: %ld part_two: %ld\n", one_total, two_total);

    return EXIT_SUCCESS;
}
