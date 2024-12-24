#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "Strix/header/strix.h"

static inline void swap(int32_t *a, int32_t *b)
{
    int32_t temp = *a;
    *a = *b;
    *b = temp;
}

static int partition(int32_t *arr, int low, int high)
{
    int32_t pivot = arr[high];
    int i = low - 1;

    for (int j = low; j < high; j++)
    {
        if (arr[j] <= pivot)
        {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return i + 1;
}

void quicksort(int32_t *arr, int low, int high)
{
    if (low < high)
    {
        int pi = partition(arr, low, high);
        quicksort(arr, low, pi - 1);
        quicksort(arr, pi + 1, high);
    }
}

void sort_array(int32_t *arr, size_t n)
{
    if (n > 1)
    {
        quicksort(arr, 0, (int)n - 1);
    }
}

int main(void)
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

    strix_t *input_file_strix = strix_create((const char *)input_file_array);
    strix_arr_t *input_file_sep = strix_split_by_delim(input_file_strix, '\n');

    strix_free(input_file_strix);

    int32_t *first_array = (int32_t *)malloc(sizeof(int32_t) * input_file_sep->len);
    if (!first_array)
    {
        fclose(input_file);
        free(input_file_array);
        strix_free_strix_arr(input_file_sep);
        return EXIT_FAILURE;
    }

    int32_t *second_array = (int32_t *)malloc(sizeof(int32_t) * input_file_sep->len);
    if (!second_array)
    {
        fclose(input_file);
        free(input_file_array);
        free(first_array);
        strix_free_strix_arr(input_file_sep);
        return EXIT_FAILURE;
    }

    for (size_t counter = 0; counter < input_file_sep->len; counter++)
    {
        strix_t *current_line = input_file_sep->strix_arr[counter];
        strix_arr_t *both_integers = strix_split_by_substr(current_line, "   ");
        first_array[counter] = strix_to_signed_int(both_integers->strix_arr[0]);
        second_array[counter] = strix_to_signed_int(both_integers->strix_arr[1]);
        strix_free_strix_arr(both_integers);
    }

    sort_array(first_array, input_file_sep->len);
    sort_array(second_array, input_file_sep->len);

    uint64_t distance = 0;

    for (size_t counter = 0; counter < input_file_sep->len; counter++)
    {
        int64_t diff = first_array[counter] - second_array[counter];
        uint64_t mask = diff >> 63UL;

        distance += ((mask ^ diff) - mask);
        /*

        If mask is zero, diff is positive; then, xoring of diff with mask does nothing and then subtracting mask also does nothing
        If mask is one, diff is negative; then, xoring of diff with mask inverts all the bits of diff and then subtracting one(mask) gives the two's complement
        of diff, that is, -diff

        So all in all, this gives the absolute value of diff

         */
    }

    fprintf(stdout, "distance: %lu\n", distance);

    int64_t similarity = 0;

    for (size_t counter = 0; counter < input_file_sep->len; counter++)
    {
    }

    strix_free_strix_arr(input_file_sep);
    return EXIT_SUCCESS;
}
