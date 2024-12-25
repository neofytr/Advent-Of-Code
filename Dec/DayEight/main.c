#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

typedef struct
{
    int y;
    int x;
} Position;

typedef struct
{
    Position *positions;
    size_t count;
} PositionList;

typedef struct
{
    Position *positions;
    size_t count;
    size_t capacity;
} AntiNodeSet;

void add_antinode(AntiNodeSet *set, int y, int x, size_t height, size_t width)
{
    if (y >= 0 && y < (int)height && x >= 0 && x < (int)width)
    {
        for (size_t i = 0; i < set->count; i++)
        {
            if (set->positions[i].y == y && set->positions[i].x == x)
            {
                return;
            }
        }

        if (set->count == set->capacity)
        {
            set->capacity *= 2;
            set->positions = realloc(set->positions, sizeof(Position) * set->capacity);
        }

        set->positions[set->count].y = y;
        set->positions[set->count].x = x;
        set->count++;
    }
}

int main()
{
    FILE *file = fopen("input.txt", "r");
    if (!file)
        return 1;

    char lines[200][200];
    size_t height = 0, width = 0;
    char line[200];

    while (fgets(line, sizeof(line), file))
    {
        size_t len = strlen(line);
        if (line[len - 1] == '\n')
            line[--len] = '\0';
        if (width == 0)
            width = len;
        strcpy(lines[height++], line);
    }
    fclose(file);

    PositionList antennas[128] = {0}; // ASCII range

    AntiNodeSet antinodes = {
        .positions = malloc(sizeof(Position) * 1000),
        .count = 0,
        .capacity = 1000};

    for (size_t y = 0; y < height; y++)
    {
        for (size_t x = 0; x < width; x++)
        {
            char c = lines[y][x];
            if (c != '.')
            {
                PositionList *list = &antennas[c];

                for (size_t i = 0; i < list->count; i++)
                {
                    Position prev = list->positions[i];
                    int dY = y - prev.y;
                    int dX = x - prev.x;

                    add_antinode(&antinodes, y + dY, x + dX, height, width);
                    add_antinode(&antinodes, prev.y - dY, prev.x - dX, height, width);
                }

                if (list->count == 0)
                {
                    list->positions = malloc(sizeof(Position) * 10);
                }
                list->positions[list->count].y = y;
                list->positions[list->count].x = x;
                list->count++;
            }
        }
    }

    printf("Total antinodes: %zu\n", antinodes.count);

    for (int i = 0; i < 128; i++)
    {
        if (antennas[i].count > 0)
        {
            free(antennas[i].positions);
        }
    }
    free(antinodes.positions);

    return 0;
}