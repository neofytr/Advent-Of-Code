#ifndef INT_HASH_TABLE_H
#define INT_HASH_TABLE_H

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define NUM_BUCKETS 128 // should always be a power of two

typedef struct int_hash_node
{
    int32_t key;
    int32_t value;
    struct int_hash_node *next_node;
} int_hash_node_t;

typedef struct
{
    int_hash_node_t *buckets[NUM_BUCKETS];
    size_t total_entries;
} int_hash_table_t;

int_hash_table_t *create_int_hash_table();
void destroy_int_hash_table(int_hash_table_t *table);
bool int_hash_table_insert(int_hash_table_t *table, int32_t key, int32_t value);
int_hash_node_t *int_hash_table_search(int_hash_table_t *table, int32_t key);
bool int_hash_table_delete(int_hash_table_t *table, int32_t key);
void int_hash_table_clear(int_hash_table_t *table);
size_t int_hash_table_size(int_hash_table_t *table);
void int_hash_table_print_stats(int_hash_table_t *table);

size_t get_int_hash(int32_t key)
{
    // Using a variation of Jenkins hash function for integers
    uint32_t hash = (uint32_t)key;
    hash = (hash + 0x7ed55d16) + (hash << 12);
    hash = (hash ^ 0xc761c23c) ^ (hash >> 19);
    hash = (hash + 0x165667b1) + (hash << 5);
    hash = (hash + 0xd3a2646c) ^ (hash << 9);
    hash = (hash + 0xfd7046c5) + (hash << 3);
    hash = (hash ^ 0xb55a4f09) ^ (hash >> 16);

    return hash % NUM_BUCKETS;
}

int_hash_table_t *create_int_hash_table()
{
    int_hash_table_t *table = (int_hash_table_t *)malloc(sizeof(int_hash_table_t));
    if (!table)
    {
        return NULL;
    }

    memset(table->buckets, 0, sizeof(table->buckets));
    table->total_entries = 0;

    return table;
}

void destroy_int_hash_table(int_hash_table_t *table)
{
    if (!table)
    {
        return;
    }

    int_hash_table_clear(table);
    free(table);
}

bool int_hash_table_insert(int_hash_table_t *table, int32_t key, int32_t value)
{
    if (!table)
    {
        return false;
    }

    if (int_hash_table_search(table, key))
    {
        return false; // key already exists
    }

    size_t hash = get_int_hash(key);

    int_hash_node_t *new_node = (int_hash_node_t *)malloc(sizeof(int_hash_node_t));
    if (!new_node)
    {
        return false;
    }

    new_node->key = key;
    new_node->value = value;
    new_node->next_node = table->buckets[hash];
    table->buckets[hash] = new_node;

    table->total_entries++;

    return true;
}

int_hash_node_t *int_hash_table_search(int_hash_table_t *table, int32_t key)
{
    if (!table)
    {
        return NULL;
    }

    size_t hash = get_int_hash(key);
    int_hash_node_t *current = table->buckets[hash];

    while (current)
    {
        if (current->key == key)
        {
            return current;
        }
        current = current->next_node;
    }

    return NULL;
}

bool int_hash_table_delete(int_hash_table_t *table, int32_t key)
{
    if (!table)
    {
        return false;
    }

    size_t hash = get_int_hash(key);
    int_hash_node_t *current = table->buckets[hash];
    int_hash_node_t *prev = NULL;

    while (current)
    {
        if (current->key == key)
        {
            if (prev)
            {
                prev->next_node = current->next_node;
            }
            else
            {
                table->buckets[hash] = current->next_node;
            }

            free(current);
            table->total_entries--;

            return true;
        }

        prev = current;
        current = current->next_node;
    }

    return false;
}

void int_hash_table_clear(int_hash_table_t *table)
{
    if (!table)
    {
        return;
    }

    for (size_t i = 0; i < NUM_BUCKETS; i++)
    {
        int_hash_node_t *current = table->buckets[i];
        while (current)
        {
            int_hash_node_t *temp = current;
            current = current->next_node;
            free(temp);
        }
        table->buckets[i] = NULL;
    }

    table->total_entries = 0;
}

size_t int_hash_table_size(int_hash_table_t *table)
{
    return table ? table->total_entries : 0;
}

void int_hash_table_print_stats(int_hash_table_t *table)
{
    if (!table)
    {
        return;
    }

    size_t max_chain_length = 0;
    size_t non_empty_buckets = 0;

    for (size_t i = 0; i < NUM_BUCKETS; i++)
    {
        size_t current_chain_length = 0;
        int_hash_node_t *current = table->buckets[i];

        while (current)
        {
            current_chain_length++;
            current = current->next_node;
        }

        if (current_chain_length > 0)
        {
            non_empty_buckets++;
        }

        if (current_chain_length > max_chain_length)
        {
            max_chain_length = current_chain_length;
        }
    }

    printf("Integer Hash Table Statistics:\n");
    printf("Total Entries: %zu\n", table->total_entries);
    printf("Number of Buckets: %d\n", NUM_BUCKETS);
    printf("Non-empty Buckets: %zu\n", non_empty_buckets);
    printf("Max Chain Length: %zu\n", max_chain_length);
}

#endif /* INT_HASH_TABLE_H */