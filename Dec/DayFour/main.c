#include <stdlib.h>
#include <stdio.h>
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
}