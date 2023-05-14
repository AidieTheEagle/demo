use std::io;
mod draw_hangman;
mod draw_words;
mod read_words;


fn match_guess(c:char, secret:&mut Vec<char>, working:&mut Vec<char>, discard:&mut Vec<char>) -> bool {
    let mut found:bool = false;
    for i in 0..secret.len() {
        if c == secret[i] {
            working[i] = c;
            found = true;
        } else {
            continue;
        }
    }

    if !found {
        if !discard.contains(&c) {
            discard.push(c);
        }
    }

    return found;
}

fn check_chars(string:&mut String,secret:&mut Vec<char>,working:&mut Vec<char>,discard:&mut Vec<char>) -> u32{
    let mut mismatch_count:u32 = 0;
    for c in string.trim().chars() {
        if !match_guess(c.to_ascii_uppercase(), secret, working, discard) {
            mismatch_count = mismatch_count+1;
        }
    }
    return mismatch_count;
}


fn is_done(working:&mut Vec<char>) -> bool {
    for c in working {
        match c {
            '_'  => return false,
            _ => continue,
        }
    }

    return true;
}


fn main() {
    let mut secret:Vec<char> = read_words::get_word_as_vec();
    let mut working:Vec<char> = Vec::new();
    for _ in 0..secret.len() {
        working.push('_');
    }
    let mut discard:Vec<char> = Vec::new();

    let mut guess:String = String::new();

    let mut i:u32 = 0;
    loop {
        draw_hangman::draw(i);
        draw_words::draw_word(&mut working);
        draw_words::draw_word(&mut discard);

        if is_done(&mut working) {
            println!(" You win!");
            print!(" The word was:");
            draw_words::draw_word(&mut secret);
            println!(" You got: {} points\n", 10-i);
            break;
        }

        match i {
            10 => {
                println!(" You lose!");
                print!(" The word was:");
                draw_words::draw_word(&mut secret);
                println!(" You got: {} points\n", 10-i);
                break;
            },
            _ => ()
        }

        println!(" Please input a possible character\n");
        io::stdin().read_line(&mut guess).expect("could not read line from console");

        let mismatch_count = check_chars(&mut guess, &mut secret, &mut working, &mut discard);
        guess.clear();


        i = i+(mismatch_count%10);
        // if i > 10 {
        //     i = 10;
        // }
    }
    
}
