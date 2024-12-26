#include <stdlib.h>
#include <stdio.h>
#include "../Strix/header/strix.h"

size_t num_of_digits(uint64_t num)
{
    if (num == 0)
        return 1;
    size_t dig = 0;
    while (num)
    {
        dig++;
        num /= 10;
    }
    return dig;
}

void split_number(uint64_t num, uint64_t *left, uint64_t *right)
{
    size_t digits = num_of_digits(num);
    size_t half_digits = digits / 2;
    uint64_t divisor = 1;
    for (size_t i = 0; i < half_digits; i++)
    {
        divisor *= 10;
    }
    *left = num / divisor;
    *right = num % divisor;
}

int main()
{
    strix_t *input_strix = conv_file_to_strix("input.txt");
    if (!input_strix)
        return EXIT_FAILURE;

    strix_arr_t *arr = strix_split_by_delim(input_strix, ' ');
    if (!arr)
    {
        strix_free(input_strix);
        return EXIT_FAILURE;
    }
    strix_free(input_strix);

    if (arr->len == 0)
    {
        strix_free_strix_arr(arr);
        return EXIT_FAILURE;
    }

    size_t max_capacity = arr->len;
    for (int i = 0; i < 25; i++)
        max_capacity *= 2;

    uint64_t *current = calloc(max_capacity, sizeof(uint64_t));
    uint64_t *next = calloc(max_capacity, sizeof(uint64_t));

    if (!current || !next)
    {
        free(current);
        free(next);
        strix_free_strix_arr(arr);
        return EXIT_FAILURE;
    }

    size_t curr_len = arr->len;
    for (size_t i = 0; i < arr->len; i++)
    {
        current[i] = strix_to_unsigned_int(arr->strix_arr[i]);
        if (strix_errno != STRIX_SUCCESS)
        {
            free(current);
            free(next);
            strix_free_strix_arr(arr);
            return EXIT_FAILURE;
        }
    }
    strix_free_strix_arr(arr);

    for (size_t iteration = 0; iteration < 25; iteration++)
    {
        size_t next_len = 0;

        for (size_t i = 0; i < curr_len && next_len < max_capacity; i++)
        {
            if (current[i] == 0)
            {
                next[next_len++] = 1;
            }
            else if (num_of_digits(current[i]) % 2 == 0)
            {
                if (next_len + 2 > max_capacity)
                    break;
                uint64_t left, right;
                split_number(current[i], &left, &right);
                next[next_len++] = left;
                next[next_len++] = right;
            }
            else
            {
                next[next_len++] = current[i] * 2024;
            }
        }

        uint64_t *temp = current;
        current = next;
        next = temp;
        curr_len = next_len;
    }

    for (size_t i = 0; i < curr_len; i++)
    {
        printf("%lu ", current[i]);
    }
    printf("\n");

    free(current);
    free(next);
    return EXIT_SUCCESS;
}