use std::fs::File;
use std::io::{self, BufRead, BufReader, BufWriter, Write};
use std::cmp;
use std::time::Instant;
use psutil::process::Process;

//Levenshtein Algorithm
fn lev(str_a: &str, str_b: &str) -> usize
{
    //Initialize variables
    let n = str_b.chars().count();
    let mut v0: Vec<usize> = (0..=n).collect();
    let mut v1: Vec<usize> = vec![0; n + 1];
    let mut deletion_cost: usize;
    let mut insertion_cost: usize;
    let mut substitution_cost: usize;
    let str_b_chars: Vec<char> = str_b.chars().collect();

    for (i, char_a) in str_a.chars().enumerate()
    {
        // calculate v1 (current row distances) from the previous row v0

        // first element of v1 is A[i + 1][0]
        // edit distance is delete (i + 1) chars from s to match empty t
        v1[0] = i + 1;

        for j in 0..n 
        {
            // calculating costs for A[i + 1][j + 1]
            deletion_cost = v0[j + 1] + 1;
            insertion_cost = v1[j] + 1;

            if char_a == str_b_chars[j] 
            {
                substitution_cost = v0[j];
            } 
            else 
            {
                substitution_cost = v0[j] + 1;
            }

            v1[j + 1] = cmp::min(deletion_cost, cmp::min(insertion_cost, substitution_cost));
        }

        // copy v1 (current row) to v0 (previous row) for next iteration
        std::mem::swap(&mut v0, &mut v1);
    }

    return v0[n];
}

fn main() -> io::Result<()>
{
    let start_time = Instant::now(); //Start time
    let process = Process::current().unwrap();
    let initial_memory = process.memory_info().unwrap().rss(); // Memory usage
    let args: Vec<String> = std::env::args().collect(); //Take input from run.sh
    let file = File::open(&args[2])?; //Open all words
    let reader = BufReader::new(file);
    let mut order: Vec<(String, usize)> = Vec::new();
    let output_file = File::create(args[3].clone())?;
    let mut writer = BufWriter::new(output_file);

    for line_result in reader.lines() 
    {
        let line = line_result?;
        let distance = lev(&args[1], &line); //Avoids having to clone line
        order.push((line, distance));
    }

    order.sort_by(|a, b| a.1.cmp(&b.1)); //Sort by usize in tuple

    // Write to the file
    for (str, dist) in order 
    {
        writeln!(writer, "{} {}", str, dist)?;
    }

    //Calculate total used memory
    let final_memory = process.memory_info().unwrap().rss();
    let memory_increase = (final_memory - initial_memory) as f64 / (1024.0 * 1024.0);

    //Calculate elapsed time
    let end_time = Instant::now();
    let duration = end_time.duration_since(start_time);

    println!("\nRust:");
    println!("Execution time: {:?}", duration);
    println!("Memory increase: {:.2} MiB\n", memory_increase);

    Ok(())
}