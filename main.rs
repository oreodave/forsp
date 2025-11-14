use std::fs;
use std::process::exit;

fn read_file(filename: &str) -> Result<Vec<char>, std::io::Error> {
    fs::read_to_string(filename).and_then(|contents| Ok(contents.chars().collect()))
}

#[derive(Debug, Copy, Clone)]
struct Position {
    line: usize,
    character: usize,
}

// TODO: We need to fit this into our pointer tagging scheme better
#[derive(Debug)]
enum Obj {
    Nil,
    Symbol(String),
    Pop(String),
    Push(String),
    List(Vec<Token>),
}

#[derive(Debug)]
struct Token {
    position: Position,
    contents: Obj,
}

struct TokenStream {
    source_name: String,
    contents: Vec<Token>,
}

#[derive(Debug, Copy, Clone)]
struct ReadState {
    byte: usize,
    position: Position,
}

fn is_symbol_char(c: char) -> bool {
    if c.is_alphabetic() || c.is_digit(10) {
        true
    } else if c.is_ascii_punctuation() {
        match c {
            '(' | ')' | '$' | '^' | '.' => false,
            _ => true,
        }
    } else {
        false
    }
}

fn parse_token(stream: &Vec<char>, state: ReadState) -> (Option<Token>, ReadState) {
    if state.byte >= stream.len() {
        return (None, state);
    }
    let mut state = state;
    let c = stream[state.byte];
    if c == ';' {
        // Comments
        let mut c = c;
        while state.byte < stream.len() && !(c == '\n' || c == '\r') {
            state.byte += 1;
            if state.byte < stream.len() {
                c = stream[state.byte];
            }
        }
        // tail call
        parse_token(stream, state)
    } else if c.is_whitespace() {
        let mut c = c;
        while state.byte < stream.len() && c.is_whitespace() {
            if c == '\n' || c == '\r' {
                state.position.line += 1;
                state.position.character = 1;
            } else {
                state.position.character += 1;
            }
            state.byte += 1; // move to next character
            if state.byte < stream.len() {
                c = stream[state.byte]
            }
        }
        // tail call
        parse_token(stream, state)
    } else if c == '$' {
        // Parse the remainder as a token, then tag
        state.byte += 1;
        let (token, state) = parse_token(stream, state);
        match token {
            // NOTE: Supremely impressed by this level of pattern matching
            Some(Token {
                position: pos,
                contents: Obj::Symbol(contents),
            }) => (
                Some(Token {
                    position: pos,
                    contents: Obj::Pop(contents),
                }),
                state,
            ),
            // FIXME: Consider making an error here
            _ => (None, state),
        }
    } else if c == '^' {
        // Parse the remainder as a token, then tag
        state.byte += 1;
        let (token, state) = parse_token(stream, state);
        match token {
            // NOTE: Supremely impressed by this level of pattern matching
            Some(Token {
                position: pos,
                contents: Obj::Symbol(contents),
            }) => (
                Some(Token {
                    position: pos,
                    contents: Obj::Push(contents),
                }),
                state,
            ),
            _ => {
                // FIXME: Consider making an error here
                (None, state)
            }
        }
    } else if c == '(' {
        // TODO: We need to fit this into our pointer tagging scheme better
        let root_pos = state.position;
        let mut tokens: Vec<Token> = Vec::new();
        state.byte += 1;
        while state.byte < stream.len() && stream[state.byte] != ')' {
            let (token, new_state) = parse_token(stream, state);
            if let Some(token) = token {
                tokens.push(token);
            } else {
                return (None, new_state);
            }
            state = new_state;
        }

        println!("state.byte={}, c={}", state.byte, stream[state.byte]);

        if state.byte >= stream.len() {
            // FIXME: No end bracket, error
            (None, state)
        } else {
            state.byte += 1;
            (
                Some(Token {
                    position: root_pos,
                    contents: Obj::List(tokens),
                }),
                state,
            )
        }
    } else {
        // Token
        let token_iter = stream[state.byte..]
            .iter()
            .take_while(|&&c| is_symbol_char(c));

        let token_contents: String = token_iter.collect();
        let token_length = token_contents.len();

        let token = Token {
            position: state.position,
            contents: Obj::Symbol(token_contents),
        };

        state.byte += token_length;
        state.position.character += token_length;

        (Some(token), state)
    }
}

fn parse_tokens(stream_name: &str, stream: Vec<char>) -> TokenStream {
    let mut read_state = ReadState {
        position: Position {
            line: 1,
            character: 1,
        },
        byte: 0,
    };

    let mut token_stream = TokenStream {
        source_name: stream_name.to_string(),
        contents: Vec::new(),
    };

    while read_state.byte < stream.len() {
        let (possible_token, new_read_state) = parse_token(&stream, read_state);
        match possible_token {
            // FIXME: report NONEs more aggressively?
            None => break,
            Some(token) => {
                token_stream.contents.push(token);
            }
        }
        read_state = new_read_state;
    }

    token_stream
}

fn main() {
    let filename = "test.txt";
    let stream = read_file(filename);
    if let Err(e) = stream {
        eprintln!("ERROR: Could not read `{}`: {}", filename, e);
        exit(1);
    }
    let stream = stream.unwrap();
    // println!("{}: `{:?}`", filename, stream);
    let tokens = parse_tokens(filename, stream);
    for token in tokens.contents {
        println!(
            "{}:{}:{}:  `{:?}`",
            tokens.source_name, token.position.line, token.position.character, token.contents
        );
    }
}
