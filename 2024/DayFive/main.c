#include <stdlib.h>
#include <stdio.h>

#include "../Strix/header/strix.h"
#include "./hash_table.h"
#include "../int_hash_table.h"

bool is_valid_update(const int32_t *nums_arr, size_t nums_len, hash_table_t *hash_table)
{
    bool already_seen[10000] = {false};

    for (size_t counter = 0; counter < nums_len; counter++)
    {
        int32_t curr_num = nums_arr[counter];
        already_seen[curr_num] = true;

        hash_node_t *searched_node = hash_table_search(hash_table, curr_num);
        if (searched_node)
        {
            for (size_t index = 0; index < searched_node->len; index++)
            {
                if (already_seen[searched_node->elements_in_front[index]])
                {
                    return false;
                }
            }
        }
    }

    return true;
}

int32_t mid_fixed_arr(const int32_t *nums_arr, size_t nums_len, hash_table_t *hash_table)
{
    int32_t *in_degree = (int32_t *)calloc(10000, sizeof(int32_t));
    int32_t *result = (int32_t *)malloc(nums_len * sizeof(int32_t));
    bool *exists = (bool *)calloc(10000, sizeof(bool));

    if (!in_degree || !result || !exists)
    {
        free(in_degree);
        free(result);
        free(exists);
        return -1;
    }

    for (size_t i = 0; i < nums_len; i++)
    {
        exists[nums_arr[i]] = true;
    }

    for (size_t i = 0; i < nums_len; i++)
    {
        hash_node_t *node = hash_table_search(hash_table, nums_arr[i]);
        if (node)
        {
            for (size_t j = 0; j < node->len; j++)
            {
                if (exists[node->elements_in_front[j]])
                {
                    in_degree[node->elements_in_front[j]]++;
                }
            }
        }
    }

    int32_t queue[10000];
    size_t queue_front = 0, queue_back = 0;

    for (size_t i = 0; i < nums_len; i++)
    {
        if (in_degree[nums_arr[i]] == 0)
        {
            queue[queue_back++] = nums_arr[i];
        }
    }

    size_t sorted_count = 0;
    while (queue_front < queue_back)
    {
        int32_t current = queue[queue_front++];
        result[sorted_count++] = current;

        hash_node_t *node = hash_table_search(hash_table, current);
        if (node)
        {
            for (size_t i = 0; i < node->len; i++)
            {
                int32_t next = node->elements_in_front[i];
                if (exists[next])
                {
                    in_degree[next]--;
                    if (in_degree[next] == 0)
                    {
                        queue[queue_back++] = next;
                    }
                }
            }
        }
    }

    int32_t middle_value = result[nums_len / 2];

    free(in_degree);
    free(result);
    free(exists);

    return middle_value;
}

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

    strix_arr_t *lines = strix_split_by_delim(input_strix, '\n');
    if (!lines)
    {
        fprintf(stderr, "Failed to split input into lines\n");
        strix_free(input_strix);
        return EXIT_FAILURE;
    }

    size_t update_start_line = 0;
    for (size_t counter = 0; counter < lines->len; counter++)
    {
        if (strix_find(lines->strix_arr[counter], "|") == -2)
        {
            update_start_line = counter;
            break;
        }
    }

    hash_table_t *order_hash_table = create_hash_table();
    if (!order_hash_table)
    {
        fprintf(stderr, "Failed to create hash table\n");
        strix_free_strix_arr(lines);
        strix_free(input_strix);
        return EXIT_FAILURE;
    }

    for (size_t counter = 0; counter < update_start_line; counter++)
    {
        strix_t *line = lines->strix_arr[counter];
        strix_arr_t *nums = strix_split_by_delim(line, '|');
        if (!nums)
        {
            fprintf(stderr, "Failed to split line by delimiter\n");
            hash_table_clear(order_hash_table);
            strix_free_strix_arr(lines);
            strix_free(input_strix);
            return EXIT_FAILURE;
        }

        int32_t num_one = strix_to_signed_int(nums->strix_arr[0]);
        int32_t num_two = strix_to_signed_int(nums->strix_arr[1]);

        hash_node_t *searched_node = hash_table_search(order_hash_table, num_one);
        if (!searched_node)
        {
#define MAX_ELTS_IN_FRONT 128
            int32_t *elements_in_front = (int32_t *)malloc(sizeof(int32_t) * MAX_ELTS_IN_FRONT);
            if (!elements_in_front)
            {
                fprintf(stderr, "Failed to allocate memory for elements array\n");
                strix_free_strix_arr(nums);
                hash_table_clear(order_hash_table);
                strix_free_strix_arr(lines);
                strix_free(input_strix);
                return EXIT_FAILURE;
            }
            size_t len = 0;
            elements_in_front[len++] = num_two;
            if (!hash_table_insert(order_hash_table, num_one, 0, elements_in_front, len))
            {
                fprintf(stderr, "Failed to insert into hash table\n");
                free(elements_in_front);
                strix_free_strix_arr(nums);
                hash_table_clear(order_hash_table);
                strix_free_strix_arr(lines);
                strix_free(input_strix);
                return EXIT_FAILURE;
            }
        }
        else
        {
            searched_node->elements_in_front[searched_node->len++] = num_two;
        }
        strix_free_strix_arr(nums);
    }

    size_t valid_update_count = 0;
    size_t mid_count = 0;
    size_t prev_invalid_mid_count = 0;

    for (size_t counter = update_start_line; counter < lines->len; counter++)
    {
        strix_t *line = lines->strix_arr[counter];
        strix_arr_t *nums = strix_split_by_delim(line, ',');
        if (!nums)
        {
            fprintf(stderr, "Failed to split update line\n");
            hash_table_clear(order_hash_table);
            strix_free_strix_arr(lines);
            strix_free(input_strix);
            return EXIT_FAILURE;
        }

#define MAX_LINE_LEN 128
        int32_t num_arr[MAX_LINE_LEN];
        size_t nums_len = nums->len;

        for (size_t i = 0; i < nums->len; i++)
        {
            num_arr[i] = strix_to_signed_int(nums->strix_arr[i]);
        }

        if (is_valid_update(num_arr, nums_len, order_hash_table))
        {
            valid_update_count++;
            mid_count += num_arr[nums->len / 2];
        }
        else
        {
            prev_invalid_mid_count += mid_fixed_arr(num_arr, nums_len, order_hash_table);
        }

        strix_free_strix_arr(nums);
    }

    fprintf(stdout, "valid update count: %zu, valid mid sum: %zu, prev invalid mid sum: %zu\n", valid_update_count, mid_count, prev_invalid_mid_count);

    hash_table_clear(order_hash_table);
    strix_free_strix_arr(lines);
    strix_free(input_strix);
    return EXIT_SUCCESS;
}