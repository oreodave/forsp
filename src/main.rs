mod reader;

use reader::Reader;
use std::fs;
use std::process::exit;

fn read_file(filename: &str) -> Result<Vec<char>, std::io::Error> {
    fs::read_to_string(filename).and_then(|contents| Ok(contents.chars().collect()))
}

fn main() {
    let filename = "test.txt";
    let buffer = read_file(filename);
    if let Err(e) = buffer {
        eprintln!("ERROR: Could not read `{}`: {}", filename, e);
        exit(1);
    }
    let buffer = buffer.unwrap();
    let mut reader = Reader::new(filename, &buffer);
    let mut i = 0;
    loop {
        let (token, new_reader) = reader.parse_token();
        if new_reader.at_end() {
            break;
        } else if token.is_none() {
            eprintln!(
                "Error: Couldn't parse (TODO: better messages)\n--> {}:{}:{}",
                new_reader.name, new_reader.position.line, new_reader.position.character
            );
            exit(1);
        } else {
            let token = token.unwrap();
            println!("{}: {:#?}", i, token);
            reader = new_reader;
            i += 1;
        }
    }
}
