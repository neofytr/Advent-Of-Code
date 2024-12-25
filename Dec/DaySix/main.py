def parse_input_from_file(file_path):
    """Reads the map from a file and parses the grid and guard's initial state."""
    with open(file_path, 'r') as f:
        map_lines = f.read().strip().split('\n')
    
    grid = []
    guard_position = None
    direction_map = {'^': 0, '>': 1, 'v': 2, '<': 3}
    direction = None

    for r, line in enumerate(map_lines):
        row = []
        for c, char in enumerate(line):
            if char in direction_map:
                guard_position = (r, c)
                direction = direction_map[char]
                row.append('.')  # Replace guard with an empty space
            else:
                row.append(char)
        grid.append(row)
    
    return grid, guard_position, direction

def simulate_patrol(grid, start, direction):
    """Simulates the patrol and returns the set of visited positions."""
    visited = set()
    visited.add(start)

    # Movement vectors: [up, right, down, left]
    moves = [(-1, 0), (0, 1), (1, 0), (0, -1)]
    rows, cols = len(grid), len(grid[0])

    position = start
    while True:
        # Calculate the cell in front
        next_r = position[0] + moves[direction][0]
        next_c = position[1] + moves[direction][1]

        if 0 <= next_r < rows and 0 <= next_c < cols and grid[next_r][next_c] != '#':
            # Move forward
            position = (next_r, next_c)
            visited.add(position)
        else:
            # Turn right
            direction = (direction + 1) % 4

        # Check if the guard has left the grid
        if not (0 <= position[0] < rows and 0 <= position[1] < cols):
            break

    return visited

def count_visited_positions(file_path):
    """Counts distinct positions visited by the guard."""
    grid, start, direction = parse_input_from_file(file_path)
    visited_positions = simulate_patrol(grid, start, direction)
    return len(visited_positions)

# Example usage
file_path = 'input.txt'  # Replace with the actual file path
print("Distinct positions visited:", count_visited_positions(file_path))
