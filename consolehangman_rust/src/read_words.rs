use std::fs;
use std::env;
use std::path::PathBuf;
use rand::Rng;

fn get_word() -> String {
    let mut path = PathBuf::new();
    path.push(env::current_dir().expect("could not load current directory"));
    path.push("src");
    path.push("words");
    path.set_extension("txt");
    let file_as_string = fs::read_to_string(path).expect("could not read file");
    let words:Vec<&str> = file_as_string.split_whitespace().collect();
    let index = rand::thread_rng().gen_range(0..words.len());

    let word = words[index].to_owned();
    return word;
}

pub fn get_word_as_vec() -> Vec<char> {
    let mut word:Vec<char> = Vec::new();
    for c in get_word().chars() {
        word.push(c.to_ascii_uppercase());
    }
    return word;
}