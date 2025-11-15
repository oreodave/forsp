pub mod list;
pub mod ptrtag;
pub mod reader;
pub mod symcache;

// use reader::Reader;
use ptrtag::TaggedPtr;
use std::fs;
// use std::process::exit;
use symcache::SymCache;

fn read_file(filename: &str) -> Result<Vec<char>, std::io::Error> {
    fs::read_to_string(filename).and_then(|contents| Ok(contents.chars().collect()))
}

fn main() {
    let mut cache = SymCache::new();
    let sym = cache.intern("aryadev");
    let tptr = TaggedPtr::from_sym(sym);
    let as_sym = tptr.as_sym();
    println!(
        "{:?}[{:?}] => {:?} => {:?}[{:?}]",
        sym,
        cache.get(sym),
        tptr.value(),
        as_sym,
        cache.get(as_sym.unwrap())
    );

    let x: i64 = 1 << 61;
    let tptr = TaggedPtr::from_int(x);
    let as_int = tptr.as_int();
    println!("{} => {:?} => {:?}", x, tptr, as_int);

    // let filename = "test.txt";
    // let buffer = read_file(filename);
    // if let Err(e) = buffer {
    //     eprintln!("ERROR: Could not read `{}`: {}", filename, e);
    //     exit(1);
    // }
    // let buffer = buffer.unwrap();
    // let mut reader = Reader::new(filename, &buffer);
    // let mut i = 0;
    // loop {
    //     let (token, new_reader) = reader.parse_token();
    //     if new_reader.at_end() {
    //         break;
    //     } else if token.is_none() {
    //         eprintln!(
    //             "Error: Couldn't parse (TODO: better messages)\n--> {}:{}:{}",
    //             new_reader.name, new_reader.position.line, new_reader.position.character
    //         );
    //         exit(1);
    //     } else {
    //         let token = token.unwrap();
    //         println!("{}: {:#?}", i, token);
    //         reader = new_reader;
    //         i += 1;
    //     }
    // }
}
