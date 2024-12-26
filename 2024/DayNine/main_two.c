#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "../Strix/header/strix.h"

typedef struct
{
    size_t start;
    size_t length;
} FileBlock;

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
    size_t max_file_id = 0;

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
        max_file_id = curr_id - 1;

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

    for (int64_t file_id = max_file_id; file_id >= 0; file_id--)
    {
        FileBlock current_file = {0, 0};
        size_t i = 0;

        while (i < total_map_len && map[i] != file_id)
            i++;

        if (i == total_map_len)
            continue;

        current_file.start = i;

        while (i < total_map_len && map[i] == file_id)
        {
            current_file.length++;
            i++;
        }

        size_t best_pos = 0;
        size_t curr_free_start = 0;
        size_t curr_free_len = 0;
        bool found_space = false;

        for (i = 0; i < current_file.start; i++)
        {
            if (map[i] == -1)
            {
                if (curr_free_len == 0)
                    curr_free_start = i;
                curr_free_len++;

                if (curr_free_len >= current_file.length)
                {
                    best_pos = curr_free_start;
                    found_space = true;
                    break;
                }
            }
            else
            {
                curr_free_len = 0;
            }
        }

        if (found_space && best_pos < current_file.start)
        {
            for (i = 0; i < current_file.length; i++)
            {
                map[best_pos + i] = file_id;
            }

            for (i = 0; i < current_file.length; i++)
            {
                map[current_file.start + i] = -1;
            }

            /* for (size_t j = 0; j < total_map_len; j++)
            {
                fprintf(stdout, "%ld", map[j]);
            }
            printf("\n"); */
        }
    }

    size_t checksum = 0;
    for (size_t counter = 0; counter < total_map_len; counter++)
    {
        if (map[counter] == -1)
        {
            continue;
        }

        checksum += counter * map[counter];
    }

    fprintf(stdout, "part two: %zu\n", checksum);

    free(map);
    strix_free(input_strix);
    return EXIT_SUCCESS;
}