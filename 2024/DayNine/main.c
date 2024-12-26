#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "../Strix/header/strix.h"

int main()
{
    strix_t *input_strix = conv_file_to_strix("input.txt");
    if (!input_strix)
    {
        fprintf(stderr, "Failed to read input file\n");
        return EXIT_FAILURE;
    }

    if (!input_strix->len || input_strix->len & (2 - 1))
    {
        fprintf(stderr, "Invalid input format: length must be even\n");
        strix_free(input_strix);
        return EXIT_FAILURE;
    }

    size_t total_map_len = 0;
    for (size_t counter = 0; counter < input_strix->len; counter++)
    {
        if (!isdigit(input_strix->str[counter]))
        {
            fprintf(stderr, "Invalid input: non-digit character found\n");
            strix_free(input_strix);
            return EXIT_FAILURE;
        }
        size_t digit_value = input_strix->str[counter] - '0';
        if (SIZE_MAX - digit_value < total_map_len)
        {
            fprintf(stderr, "Integer overflow in total length calculation\n");
            strix_free(input_strix);
            return EXIT_FAILURE;
        }
        total_map_len += digit_value;
    }

    int64_t *map = (int64_t *)malloc(sizeof(int64_t) * total_map_len);
    if (!map)
    {
        fprintf(stderr, "Memory allocation failed\n");
        strix_free(input_strix);
        return EXIT_FAILURE;
    }

    size_t curr_id = 0;
    size_t curr_index = 0;

    for (size_t counter = 0; counter < input_strix->len; counter += 2)
    {
        size_t data_blocks = input_strix->str[counter] - '0';
        size_t free_blocks = input_strix->str[counter + 1] - '0';

        if (curr_index + data_blocks + free_blocks > total_map_len)
        {
            fprintf(stderr, "Block sizes exceed total map length\n");
            free(map);
            strix_free(input_strix);
            return EXIT_FAILURE;
        }

        for (size_t i = curr_index; i < data_blocks + curr_index; i++)
        {
            map[i] = curr_id;
        }

        curr_id++;

        curr_index += data_blocks;

        for (size_t i = curr_index; i < free_blocks + curr_index; i++)
        {
            map[i] = -1;
        }

        curr_index += free_blocks;
    }

    for (size_t counter = 0; counter < total_map_len; counter++)
    {
        fprintf(stdout, "%ld", map[counter]);
    }
    printf("\n");
    printf("\n");

    size_t front = 0;
    size_t end = total_map_len - 1;

    while (front <= end)
    {
        if (map[front] != -1)
        {
            front++;
            continue;
        }

        if (map[end] == -1)
        {
            end--;
            continue;
        }

        map[front] = map[end];
        map[end] = -1;
        front++;
        end--;
    }

    for (size_t counter = 0; counter < total_map_len; counter++)
    {
        fprintf(stdout, "%ld", map[counter]);
    }

    printf("\n");
    printf("\n");

    size_t checksum = 0;

    for (size_t counter = 0; counter < total_map_len; counter++)
    {
        if (map[counter] == -1)
        {
            break;
        }

        checksum += counter * map[counter];
    }

    fprintf(stdout, "part_one: %zu\n", checksum);
    free(map);
    strix_free(input_strix);

    return EXIT_SUCCESS;
}