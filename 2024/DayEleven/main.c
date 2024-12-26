#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../Strix/header/strix.h"

typedef struct
{
    uint64_t value;
    size_t result_len;
    uint64_t *result;
} CacheEntry;

typedef struct
{
    CacheEntry *entries;
    size_t size;
    size_t capacity;
} Cache;

Cache *create_cache(size_t capacity)
{
    Cache *cache = malloc(sizeof(Cache));
    if (!cache)
        return NULL;
    cache->entries = calloc(capacity, sizeof(CacheEntry));
    if (!cache->entries)
    {
        free(cache);
        return NULL;
    }
    cache->size = 0;
    cache->capacity = capacity;
    return cache;
}

void free_cache(Cache *cache)
{
    if (!cache)
        return;
    for (size_t i = 0; i < cache->size; i++)
    {
        free(cache->entries[i].result);
    }
    free(cache->entries);
    free(cache);
}

CacheEntry *find_in_cache(Cache *cache, uint64_t value)
{
    for (size_t i = 0; i < cache->size; i++)
    {
        if (cache->entries[i].value == value)
        {
            return &cache->entries[i];
        }
    }
    return NULL;
}

void add_to_cache(Cache *cache, uint64_t value, uint64_t *result, size_t result_len)
{
    if (cache->size >= cache->capacity)
        return;

    CacheEntry *entry = &cache->entries[cache->size++];
    entry->value = value;
    entry->result_len = result_len;
    entry->result = malloc(result_len * sizeof(uint64_t));
    if (entry->result)
    {
        memcpy(entry->result, result, result_len * sizeof(uint64_t));
    }
}

size_t transform_stone(uint64_t value, uint64_t *result, size_t max_capacity, Cache *cache)
{
    CacheEntry *cached = find_in_cache(cache, value);
    if (cached && cached->result)
    {
        memcpy(result, cached->result, cached->result_len * sizeof(uint64_t));
        return cached->result_len;
    }

    size_t result_len = 0;
    uint64_t temp_result[2];

    if (value == 0)
    {
        if (result_len + 1 > max_capacity)
            return 0;
        result[0] = 1;
        result_len = 1;
    }
    else
    {
        size_t digits = 0;
        uint64_t n = value;
        while (n)
        {
            digits++;
            n /= 10;
        }

        if (digits % 2 == 0)
        {
            uint64_t divisor = 1;
            for (size_t i = 0; i < digits / 2; i++)
                divisor *= 10;
            temp_result[0] = value / divisor;
            temp_result[1] = value % divisor;
            if (result_len + 2 > max_capacity)
                return 0;
            memcpy(result, temp_result, 2 * sizeof(uint64_t));
            result_len = 2;
        }
        else
        {
            if (result_len + 1 > max_capacity)
                return 0;
            result[0] = value * 2024;
            result_len = 1;
        }
    }

    add_to_cache(cache, value, result, result_len);
    return result_len;
}

int main()
{
    strix_t *input_strix = conv_file_to_strix("input.txt");
    if (!input_strix)
    {
        fprintf(stderr, "Error reading input file.\n");
        return EXIT_FAILURE;
    }

    strix_arr_t *arr = strix_split_by_delim(input_strix, ' ');
    if (!arr)
    {
        strix_free(input_strix);
        fprintf(stderr, "Error splitting input.\n");
        return EXIT_FAILURE;
    }
    strix_free(input_strix);

    size_t max_capacity = 1000000;
    uint64_t *current = malloc(max_capacity * sizeof(uint64_t));
    uint64_t *next = malloc(max_capacity * sizeof(uint64_t));

    Cache *cache = create_cache(10000);
    if (!current || !next || !cache)
    {
        fprintf(stderr, "Memory allocation failed.\n");
        free(current);
        free(next);
        free_cache(cache);
        strix_free_strix_arr(arr);
        return EXIT_FAILURE;
    }

    size_t curr_len = arr->len;
    for (size_t i = 0; i < arr->len; i++)
    {
        current[i] = strix_to_unsigned_int(arr->strix_arr[i]);
    }
    strix_free_strix_arr(arr);

    for (size_t iteration = 0; iteration < 75; iteration++)
    {
        size_t next_len = 0;
        for (size_t i = 0; i < curr_len && next_len < max_capacity; i++)
        {
            size_t result_len = transform_stone(current[i], &next[next_len], max_capacity - next_len, cache);
            next_len += result_len;
            if (next_len >= max_capacity)
                break;
        }
        uint64_t *temp = current;
        current = next;
        next = temp;
        curr_len = next_len;
    }

    printf("part two: %zu\n", curr_len);

    free(current);
    free(next);
    free_cache(cache);
    return EXIT_SUCCESS;
}
