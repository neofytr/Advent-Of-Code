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
    bool **grid;
    size_t height;
    size_t width;
} AntiNodeGrid;

bool are_collinear(Position p1, Position p2, Position p3)
{
    return (p2.y - p1.y) * (p3.x - p1.x) == (p3.y - p1.y) * (p2.x - p1.x);
}

AntiNodeGrid create_grid(size_t height, size_t width)
{
    AntiNodeGrid grid;
    grid.height = height;
    grid.width = width;
    grid.grid = malloc(height * sizeof(bool *));
    for (size_t i = 0; i < height; i++)
    {
        grid.grid[i] = calloc(width, sizeof(bool));
    }
    return grid;
}

void free_grid(AntiNodeGrid *grid)
{
    for (size_t i = 0; i < grid->height; i++)
    {
        free(grid->grid[i]);
    }
    free(grid->grid);
}

void mark_antinodes(AntiNodeGrid *grid, PositionList *list)
{
    if (list->count < 2)
        return;

    for (size_t y = 0; y < grid->height; y++)
    {
        for (size_t x = 0; x < grid->width; x++)
        {
            Position p = {.y = y, .x = x};

            for (size_t i = 0; i < list->count; i++)
            {
                for (size_t j = i + 1; j < list->count; j++)
                {
                    if (are_collinear(p, list->positions[i], list->positions[j]))
                    {
                        grid->grid[y][x] = true;
                        goto next_point;
                    }
                }
            }
        next_point:;
        }
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

    PositionList antennas[128] = {0};

    for (size_t y = 0; y < height; y++)
    {
        for (size_t x = 0; x < width; x++)
        {
            char c = lines[y][x];
            if (c != '.')
            {
                PositionList *list = &antennas[c];
                if (list->count == 0)
                {
                    list->positions = malloc(sizeof(Position) * 100);
                }
                list->positions[list->count].y = y;
                list->positions[list->count].x = x;
                list->count++;
            }
        }
    }

    AntiNodeGrid antinodes = create_grid(height, width);

    for (int c = 0; c < 128; c++)
    {
        if (antennas[c].count >= 2)
        {
            mark_antinodes(&antinodes, &antennas[c]);
        }
    }

    size_t total = 0;
    for (size_t y = 0; y < height; y++)
    {
        for (size_t x = 0; x < width; x++)
        {
            if (antinodes.grid[y][x])
            {
                total++;
            }
        }
    }

    printf("Total antinodes: %zu\n", total);

    for (int i = 0; i < 128; i++)
    {
        if (antennas[i].count > 0)
        {
            free(antennas[i].positions);
        }
    }
    free_grid(&antinodes);

    return 0;
}