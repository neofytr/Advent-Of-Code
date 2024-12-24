#include <stdio.h>
#include <stdlib.h>
#include "../Strix/header/strix.h"

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

    int64_t multiplication = 0;

    position_t *mul_positions = strix_find_all(input_strix, "mul(");
    if (!mul_positions)
    {
        printf("1\n");
        fclose(input_file);
        return EXIT_FAILURE;
    }

    for (size_t mul_counter = 0; mul_counter < mul_positions->len; mul_counter++)
    {
        size_t mul_pos = mul_positions->pos[mul_counter];
        size_t next_pos = mul_pos + 4;
        // it's guaranteed that at least nine elements are there even after the final mul(

        strix_t *slice = strix_slice(input_strix, next_pos, next_pos + 8);
        if (!slice)
        {
            printf("2\n");
            fclose(input_file);
            strix_position_free(mul_positions);
            return EXIT_FAILURE;
        }

        size_t comma_count = 0;
        for (size_t index = 0; index < slice->len; index++)
        {
            if (slice->str[index] == ')')
            {
                slice->len = index;
                break;
            }
        }

        strix_arr_t *digs = strix_split_by_delim(slice, ',');
        if (!digs)
        {
            printf("3\n");
            fclose(input_file);
            strix_position_free(mul_positions);
            strix_free(slice);
            return EXIT_FAILURE;
        }

        if (digs->len != 2)
        {
            continue;
        }

        int64_t first_num = strix_to_signed_int(digs->strix_arr[0]);
        int64_t second_num = strix_to_signed_int(digs->strix_arr[1]);

        if (strix_errno != STRIX_SUCCESS)
        {
            continue;
        }

        multiplication += first_num * second_num;
    }

    fprintf(stdout, "final sum product (part_one): %ld\n", multiplication);

    multiplication = 0;
    bool mul_enabled = true;

    position_t *do_positions = strix_find_all(input_strix, "do()");
    position_t *dont_positions = strix_find_all(input_strix, "don't()");

    if (!mul_positions)
    {
        printf("1\n");
        fclose(input_file);
        return EXIT_FAILURE;
    }

    for (size_t mul_counter = 0; mul_counter < mul_positions->len; mul_counter++)
    {
        size_t mul_pos = mul_positions->pos[mul_counter];

        if (do_positions && dont_positions)
        {
            size_t last_do = 0;
            size_t last_dont = 0;

            for (size_t i = 0; i < do_positions->len; i++)
            {
                if (do_positions->pos[i] < mul_pos && do_positions->pos[i] > last_do)
                {
                    last_do = do_positions->pos[i];
                }
            }

            for (size_t i = 0; i < dont_positions->len; i++)
            {
                if (dont_positions->pos[i] < mul_pos && dont_positions->pos[i] > last_dont)
                {
                    last_dont = dont_positions->pos[i];
                }
            }

            if (last_do == 0 && last_dont == 0)
            {
                mul_enabled = true;
            }
            else if (last_do > last_dont)
            {
                mul_enabled = true;
            }
            else
            {
                mul_enabled = false;
            }
        }

        if (!mul_enabled)
        {
            continue;
        }

        size_t next_pos = mul_pos + 4;
        strix_t *slice = strix_slice(input_strix, next_pos, next_pos + 8);
        if (!slice)
        {
            printf("2\n");
            fclose(input_file);
            strix_position_free(mul_positions);
            if (do_positions)
                strix_position_free(do_positions);
            if (dont_positions)
                strix_position_free(dont_positions);
            return EXIT_FAILURE;
        }

        size_t comma_count = 0;
        for (size_t index = 0; index < slice->len; index++)
        {
            if (slice->str[index] == ')')
            {
                slice->len = index;
                break;
            }
        }

        strix_arr_t *digs = strix_split_by_delim(slice, ',');
        if (!digs)
        {
            printf("3\n");
            fclose(input_file);
            strix_position_free(mul_positions);
            if (do_positions)
                strix_position_free(do_positions);
            if (dont_positions)
                strix_position_free(dont_positions);
            strix_free(slice);
            return EXIT_FAILURE;
        }

        if (digs->len != 2)
        {
            continue;
        }

        int64_t first_num = strix_to_signed_int(digs->strix_arr[0]);
        int64_t second_num = strix_to_signed_int(digs->strix_arr[1]);

        if (strix_errno != STRIX_SUCCESS)
        {
            continue;
        }

        multiplication += first_num * second_num;
    }

    fprintf(stdout, "final sum product (part_two): %ld\n", multiplication);

    if (do_positions)
        strix_position_free(do_positions);
    if (dont_positions)
        strix_position_free(dont_positions);
    strix_position_free(mul_positions);
    strix_free(input_strix);
    fclose(input_file);

    return EXIT_SUCCESS;
    return EXIT_SUCCESS;
}