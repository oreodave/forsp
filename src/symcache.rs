use std::collections::HashMap;

#[derive(Debug, Copy, Clone)]
pub struct Symbol(usize);

impl Symbol {
    pub fn new(x: usize) -> Self {
        Symbol(x)
    }

    // Method to get the raw pointer (for tagging purposes)
    pub fn value(&self) -> usize {
        self.0
    }
}

pub struct SymCache {
    cache: Vec<String>,
    rev_cache: HashMap<String, usize>,
}

impl SymCache {
    pub fn new() -> Self {
        SymCache {
            cache: Vec::new(),
            rev_cache: HashMap::new(),
        }
    }

    pub fn get(&self, symbol: Symbol) -> Option<&String> {
        if symbol.0 < self.cache.len() {
            Some(&self.cache[symbol.0])
        } else {
            None
        }
    }

    pub fn intern(&mut self, string: &str) -> Symbol {
        if let Some(&ptr) = self.rev_cache.get(string) {
            Symbol(ptr)
        } else {
            let ptr = self.cache.len();
            self.cache.push(string.to_string());
            self.rev_cache.insert(string.to_string(), ptr);
            Symbol(ptr)
        }
    }
}
