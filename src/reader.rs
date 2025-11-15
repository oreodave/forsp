#[derive(Debug, Copy, Clone)]
pub struct Position {
    pub line: usize,
    pub character: usize,
    pub byte: usize,
}

#[derive(Clone, Copy)]
pub struct Reader<'a> {
    pub name: &'a str,
    pub position: Position,
    pub buffer: &'a Vec<char>,
}

// TODO: We need to fit this into our pointer tagging scheme better
#[derive(Debug)]
pub enum Obj {
    Nil,
    Integer(i64),
    Symbol(String),
    List(Vec<Token>),
}

#[derive(Debug)]
pub struct Token {
    pub position: Position,
    pub obj: Obj,
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

impl<'a> Reader<'a> {
    pub fn new(name: &'a str, buffer: &'a Vec<char>) -> Reader<'a> {
        Reader {
            name: name,
            position: Position {
                byte: 0,
                line: 1,
                character: 1,
            },
            buffer: buffer,
        }
    }

    pub fn at_end(&self) -> bool {
        self.position.byte >= self.buffer.len()
    }

    pub fn parse_token(mut self) -> (Option<Token>, Reader<'a>) {
        self = self.skip_whitespace();

        let c = self.peek_char();
        if c.is_none() {
            // FIXME:Error: Possible?
            return (None, self);
        }
        let c = c.unwrap();
        match c {
            ';' => self.skip_comments().parse_token(),
            '(' => self.parse_list(),
            '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9' => self.parse_integer(),
            _ => {
                // Symbol
                let token_iter = self.buffer[self.position.byte..]
                    .iter()
                    .take_while(|&&c| is_symbol_char(c));

                let token_contents: String = token_iter.collect();
                let token_length = token_contents.len();

                let token = Token {
                    position: self.position,
                    obj: Obj::Symbol(token_contents),
                };

                self = self.skip_chars(token_length);
                (Some(token), self)
            }
        }
    }

    fn space_left(self) -> usize {
        if self.at_end() {
            0
        } else {
            self.buffer.len() - self.position.byte
        }
    }

    fn peek_char(&self) -> Option<char> {
        if self.at_end() {
            None
        } else {
            Some(self.buffer[self.position.byte])
        }
    }

    fn peek_chars(&self, n: usize) -> Option<&'a [char]> {
        if self.at_end() || self.space_left() < n {
            None
        } else {
            Some(&self.buffer[self.position.byte..self.position.byte + n])
        }
    }

    fn skip_chars(mut self, n: usize) -> Reader<'a> {
        for _ in 0..n {
            match self.peek_char() {
                Some('\n') | Some('\r') => {
                    self.position.line += 1;
                    self.position.character = 0;
                }
                Some(_) => {
                    self.position.character += 1;
                }
                None => {
                    return self;
                }
            }
            self.position.byte += 1;
        }
        self
    }

    fn skip_char(self) -> Reader<'a> {
        self.skip_chars(1)
    }

    fn skip_comments(mut self) -> Reader<'a> {
        let c = self.peek_char();
        if c.is_none() || c.unwrap() != ';' {
            return self;
        }
        self.position.byte += 1;
        while let Some(c) = self.peek_char()
            && c != '\n'
            && c != '\r'
        {
            self = self.skip_char();
        }
        self
    }

    fn skip_whitespace(mut self) -> Reader<'a> {
        let mut c = self.peek_char();
        while c.is_some() && c.unwrap().is_whitespace() {
            self = self.skip_char();
            c = self.peek_char();
        }
        self
    }

    fn parse_integer(self) -> (Option<Token>, Reader<'a>) {
        let mut new_reader = self;
        // Keep peek, skipping until we don't find a numeric
        let mut c = new_reader.peek_char();
        while c.is_some() && c.unwrap().is_digit(10) {
            new_reader = new_reader.skip_char();
            c = new_reader.peek_char();
        }

        // TODO: Maybe consider not creating a new string here?
        let int_str: String = self.buffer[self.position.byte..new_reader.position.byte]
            .iter()
            .collect();
        match int_str.parse::<i64>() {
            Ok(x) => (
                Some(Token {
                    position: self.position,
                    obj: Obj::Integer(x),
                }),
                new_reader,
            ),
            Err(_e) =>
            // FIXME:Error: Propagate integer parsing error upwards
            {
                (None, self)
            }
        }
    }

    fn parse_list(mut self) -> (Option<Token>, Reader<'a>) {
        // TODO: We need to fit this into our pointer tagging scheme better
        let root_pos = self.position;
        let mut tokens: Vec<Token> = Vec::new();

        self = self.skip_char();
        while let Some(c_) = self.peek_char()
            && c_ != ')'
        {
            let (token, new_reader) = self.parse_token();
            if let Some(token) = token {
                tokens.push(token);
            } else {
                // FIXME:Error: Propagation
                return (None, new_reader);
            }
            self = new_reader;
        }

        if self.at_end() {
            // FIXME:Error: No end bracket
            (None, self)
        } else {
            self.skip_char();
            (
                Some(Token {
                    position: root_pos,
                    obj: Obj::List(tokens),
                }),
                self,
            )
        }
    }
}
