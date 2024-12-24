#ifndef C27DD5FD_D259_46F6_B426_018456343003
#define C27DD5FD_D259_46F6_B426_018456343003
#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include <stdint.h>

#define NUM_BUCKETS 128 // should always be a power of two

typedef struct hash_node
{
    int32_t key;
    int32_t index;
    int32_t *elements_in_front;
    size_t len;
    struct hash_node *next_node;
} hash_node_t;

typedef struct
{
    hash_node_t *buckets[NUM_BUCKETS];
    size_t total_entries;
} hash_table_t;

hash_table_t *create_hash_table();
void destroy_hash_table(hash_table_t *table);
bool hash_table_insert(hash_table_t *table, int32_t key, int32_t index, int32_t *elements_in_front, size_t len);
hash_node_t *hash_table_search(hash_table_t *table, int32_t key);
bool hash_table_delete(hash_table_t *table, int32_t key);
void hash_table_clear(hash_table_t *table);
size_t hash_table_size(hash_table_t *table);
void hash_table_prstats(hash_table_t *table);

size_t get_hash(int32_t key)
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

hash_table_t *create_hash_table()
{
    hash_table_t *table = (hash_table_t *)malloc(sizeof(hash_table_t));
    if (!table)
    {
        return NULL;
    }

    memset(table->buckets, 0, sizeof(table->buckets));
    table->total_entries = 0;

    return table;
}

void destroy_hash_table(hash_table_t *table)
{
    if (!table)
    {
        return;
    }

    hash_table_clear(table);
    free(table);
}

bool hash_table_insert(hash_table_t *table, int32_t key, int32_t index, int32_t *elements_in_front, size_t len)
{
    if (!table)
    {
        return false;
    }

    if (hash_table_search(table, key))
    {
        return false; // key already exists
    }

    size_t hash = get_hash(key);

    hash_node_t *new_node = (hash_node_t *)malloc(sizeof(hash_node_t));
    if (!new_node)
    {
        return false;
    }

    new_node->key = key;
    new_node->index = index;
    new_node->elements_in_front = elements_in_front;
    new_node->len = len;
    new_node->next_node = table->buckets[hash];
    table->buckets[hash] = new_node;

    table->total_entries++;

    return true;
}

hash_node_t *hash_table_search(hash_table_t *table, int32_t key)
{
    if (!table)
    {
        return NULL;
    }

    size_t hash = get_hash(key);
    hash_node_t *current = table->buckets[hash];

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

bool hash_table_delete(hash_table_t *table, int32_t key)
{
    if (!table)
    {
        return false;
    }

    size_t hash = get_hash(key);
    hash_node_t *current = table->buckets[hash];
    hash_node_t *prev = NULL;

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

void hash_table_clear(hash_table_t *table)
{
    if (!table)
    {
        return;
    }

    for (size_t i = 0; i < NUM_BUCKETS; i++)
    {
        hash_node_t *current = table->buckets[i];
        while (current)
        {
            hash_node_t *temp = current;
            current = current->next_node;
            free(temp->elements_in_front);
            free(temp);
        }
        table->buckets[i] = NULL;
    }

    table->total_entries = 0;
}

size_t hash_table_size(hash_table_t *table)
{
    return table ? table->total_entries : 0;
}

void hash_table_prstats(hash_table_t *table)
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
        hash_node_t *current = table->buckets[i];

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

#endif /* HASH_TABLE_H */

#endif /* C27DD5FD_D259_46F6_B426_018456343003 */
