use std::collections::HashMap;

fn main() {
    let puzzle = include_str!("input.txt");
    println!("Part 1: {}", part1(&puzzle, 25));
    println!("Part 2: {}", part2(&puzzle, 75));
}

fn part1(input: &str, blinks: u64) -> usize {
    let mut stones: Vec<u64> = input.split_whitespace().map(|s| {
        s.parse::<u64>().unwrap()
    }).collect();

    for _ in 0..blinks {
        let mut v = vec![];
        for x in stones.into_iter() {
            match x {
                0 => v.push(1),
                x if (x.ilog10() + 1) % 2 == 0 => {
                    let digits = 1 + x.ilog10();
                    let left = x / 10_u64.pow(digits/2);
                    let right = x % 10_u64.pow(digits/2);
                    v.push(left);
                    v.push(right);
                },
                x => v.push(x * 2024)
            }
        };
        stones = v;
    }
    stones.len()
}


fn part2(input: &str, blinks: u8) -> u64 {
    let mut stones: HashMap<u64, u64> = input.split_whitespace().map(|s| {
        (s.parse::<u64>().unwrap(), 1)
    }).collect();

    for _ in 0..blinks {
        let mut y = HashMap::new();
        stones.into_iter().for_each(|(k,v)| {
            match k {
                0 => *y.entry(1).or_insert(0) += v,
                k if (k.ilog10() + 1) % 2 == 0 => {
                    let digits = 1 + k.ilog10();
                    *y.entry(k / 10_u64.pow(digits/2)).or_insert(0) += v;
                    *y.entry(k % 10_u64.pow(digits/2)).or_insert(0) += v;
                },
                k => *y.entry(k * 2024).or_insert(0) += v,
            };
        });
        stones = y;
    }

    stones.values().sum()
}

