use std::fs;
use std::fs::File;
use std::io::{self, BufRead, BufReader};
use std::cmp;
use std::time::Instant;
use psutil::process::Process;

//Remove first char from string
fn tail(s: &str) -> &str
{
    return s.chars().next().map(|c| &s[c.len_utf8()..]).unwrap_or("");
}

//Levenshtein Algorithm
fn lev(str_a: &str, str_b: &str) -> usize
{
    if str_b.len() == 0
    {
        return str_a.len();
    }
    else if str_a.len() == 0
    {
        return str_b.len();
    }
    else if str_a.chars().next().unwrap() == str_b.chars().next().unwrap() //Check first charachter of each string
    {
        return lev(tail(str_a), tail(str_b));
    }
    else
    {
        return 1 + cmp::min(lev(tail(str_a), str_b), cmp::min(lev(str_a, tail(str_b)), lev(tail(str_a), tail(str_b))));
    }
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

    for line_result in reader.lines() 
    {
        let line = line_result?;
        order.push((line.clone(), lev(&args[1], &line)));
    }

    order.sort_by(|a, b| a.1.cmp(&b.1)); //Sort by usize in tuple
    let output = order.iter().map(|(word, dist)| format!("{} {}", word, dist)).collect::<Vec<_>>().join("\n"); //Turn tuple into String
    fs::write("/home/spencerb/Documents/Misc-Development/Language-Race/Ordered/output-rust.txt", output)?; //Write to txt file

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