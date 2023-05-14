

fn stage_0() {
    println!("  _________");
    println!(" |         |");
    println!(" |         |");
    println!(" |         |");
    println!(" |         |");
    println!(" |         |");
    println!(" |         |");
    println!(" |         |");
    println!(" |         |");
    println!(" |_________|");
    println!("");
}

fn stage_1() {
    println!("  _________");
    println!(" |         |");
    println!(" |         |");
    println!(" |         |");
    println!(" |         |");
    println!(" |         |");
    println!(" |         |");
    println!(" | ___     |");
    println!(" |         |");
    println!(" |_________|");
    println!("");
}

fn stage_2() {
    println!("  _________");
    println!(" |         |");
    println!(" |  |      |");
    println!(" |  |      |");
    println!(" |  |      |");
    println!(" |  |      |");
    println!(" |  |      |");
    println!(" | _|_     |");
    println!(" |         |");
    println!(" |_________|");
    println!("");
}

fn stage_3() {
    println!("  _________");
    println!(" |  ____   |");
    println!(" |  |      |");
    println!(" |  |      |");
    println!(" |  |      |");
    println!(" |  |      |");
    println!(" |  |      |");
    println!(" | _|_     |");
    println!(" |         |");
    println!(" |_________|");
    println!("");
}

fn stage_4() {
    println!("  _________");
    println!(" |  ____   |");
    println!(" |  |  |   |");
    println!(" |  |      |");
    println!(" |  |      |");
    println!(" |  |      |");
    println!(" |  |      |");
    println!(" | _|_     |");
    println!(" |         |");
    println!(" |_________|");
    println!("");
}

fn stage_5() {
    println!("  _________");
    println!(" |  ____   |");
    println!(" |  |  |   |");
    println!(" |  |  0   |");
    println!(" |  |      |");
    println!(" |  |      |");
    println!(" |  |      |");
    println!(" | _|_     |");
    println!(" |         |");
    println!(" |_________|");
    println!("");
}

fn stage_6() {
    println!("  _________");
    println!(" |  ____   |");
    println!(" |  |  |   |");
    println!(" |  |  0   |");
    println!(" |  |  |   |");
    println!(" |  |  |   |");
    println!(" |  |      |");
    println!(" | _|_     |");
    println!(" |         |");
    println!(" |_________|");
    println!("");
}

fn stage_7() {
    println!("  _________");
    println!(" |  ____   |");
    println!(" |  |  |   |");
    println!(" |  |  0   |");
    println!(" |  | /|   |");
    println!(" |  |  |   |");
    println!(" |  |      |");
    println!(" | _|_     |");
    println!(" |         |");
    println!(" |_________|");
    println!("");
}

fn stage_8() {
    println!("  _________");
    println!(" |  ____   |");
    println!(" |  |  |   |");
    println!(" |  |  0   |");
    println!(" |  | /|\\  |");
    println!(" |  |  |   |");
    println!(" |  |      |");
    println!(" | _|_     |");
    println!(" |         |");
    println!(" |_________|");
    println!("");
}

fn stage_9() {
    println!("  _________");
    println!(" |  ____   |");
    println!(" |  |  |   |");
    println!(" |  |  0   |");
    println!(" |  | /|\\  |");
    println!(" |  |  |   |");
    println!(" |  | /    |");
    println!(" | _|_     |");
    println!(" |         |");
    println!(" |_________|");
    println!("");
}

fn stage_10() {
    println!("  _________");
    println!(" |  ____   |");
    println!(" |  |  |   |");
    println!(" |  |  0   |");
    println!(" |  | /|\\  |");
    println!(" |  |  |   |");
    println!(" |  | / \\  |");
    println!(" | _|_     |");
    println!(" |         |");
    println!(" |_________|");
    println!("");
}

pub fn draw(stage: u32) {
    match stage {
        0 => stage_0(),
        1 => stage_1(),
        2 => stage_2(),
        3 => stage_3(),
        4 => stage_4(),
        5 => stage_5(),
        6 => stage_6(),
        7 => stage_7(),
        8 => stage_8(),
        9 => stage_9(),
        10 => stage_10(),
        _ => println!("invalide stage")
    }
}
