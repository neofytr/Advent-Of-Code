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

    uint8_t *input_file_array = (uint8_t *)malloc(input_file_len + 1);
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
        fprintf(stderr, "Failed to read file content (read %zu of %ld bytes)\n",
                bytes_read, input_file_len);
        free(input_file_array);
        fclose(input_file);
        return EXIT_FAILURE;
    }

    strix_t *input_strix = strix_create((const char *)input_file_array);
    if (!input_strix)
    {
        fprintf(stderr, "Failed to create input strix\n");
        free(input_file_array);
        fclose(input_file);
        return EXIT_FAILURE;
    }
    free(input_file_array);
    fclose(input_file);

    if (!strix_delete_occurence(input_strix, "\n"))
    {
        fprintf(stderr, "Failed to delete newline occurrences\n");
        strix_free(input_strix);
        return EXIT_FAILURE;
    }

    char_arr_t *char_arr = strix_find_unique_char(input_strix);
    if (!char_arr)
    {
        fprintf(stderr, "Failed to find unique characters\n");
        strix_free(input_strix);
        return EXIT_FAILURE;
    }

    bool *in_anti_arr = (bool *)calloc(input_strix->len, sizeof(bool));
    if (!in_anti_arr)
    {
        fprintf(stderr, "Failed to allocate anti array\n");
        strix_free_char_arr(char_arr);
        strix_free(input_strix);
        return EXIT_FAILURE;
    }

    for (size_t counter = 0; counter < char_arr->len; counter++)
    {
        char new_unique_char = char_arr->unique_char_arr[counter];
        if (new_unique_char == '.')
        {
            continue;
        }
        char str[2] = {new_unique_char, '\0'};
        position_t *all_positions = strix_find_all(input_strix, str);
        if (!all_positions)
        {
            fprintf(stderr, "Failed to find positions for character '%c'\n", new_unique_char);
            free(in_anti_arr);
            strix_free_char_arr(char_arr);
            strix_free(input_strix);
            return EXIT_FAILURE;
        }

        /* fprintf(stdout, "%c\n", new_unique_char);
        for (size_t i = 0; i < all_positions->len; i++)
        {
            fprintf(stdout, "%zu\n", all_positions->pos[i]);
        } */

        for (size_t i = 0; i < all_positions->len; i++)
        {
            for (size_t j = 0; j < i; j++)
            {
                int64_t pos_one = all_positions->pos[i] + (all_positions->pos[i] - all_positions->pos[j]);
                if (!in_anti_arr[pos_one] && pos_one >= 0 && pos_one < (int64_t)input_strix->len && pos_one >= all_positions->pos[all_positions->len - 1])
                {
                    in_anti_arr[pos_one] = true;
                }

                int64_t pos_two = all_positions->pos[j] - (all_positions->pos[i] - all_positions->pos[j]);
                if (pos_two >= 0 && pos_two < (int64_t)input_strix->len &&
                    !in_anti_arr[pos_two] && pos_two <= all_positions->pos[0])
                {
                    in_anti_arr[pos_two] = true;
                }

                // fprintf(stdout, "%ld %ld\n", all_positions->pos[j], all_positions->pos[i]);
            }
        }

        // strix_free_positions(all_positions);
    }

    size_t total = 0;
    for (size_t counter = 0; counter < input_strix->len; counter++)
    {
        if (in_anti_arr[counter])
        {
            // printf("%zu\n", counter);
            total++;
        }
    }

    printf("Total: %zu\n", total);

    free(in_anti_arr);
    strix_free_char_arr(char_arr);
    strix_free(input_strix);

    return EXIT_SUCCESS;
}