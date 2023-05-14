pub fn draw_word(word:&mut Vec<char>) {
    print!(" ");
    for c in word {
        print!("{c} ");
    }
    print!("\n");
}