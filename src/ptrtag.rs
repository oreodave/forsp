use crate::list::List;
use crate::symcache::Symbol;
use std::ptr;

#[derive(Debug, Clone, Copy)]
pub enum TypeTag {
    NIL = 0,
    Integer = 0b1,
    Symbol = 0b10,
    List = 0b110,
}

impl TypeTag {
    fn shift(&self) -> usize {
        match self {
            TypeTag::NIL => 0,
            TypeTag::Integer => 1,
            TypeTag::Symbol | TypeTag::List => 8,
        }
    }

    fn mask(&self) -> usize {
        (1 << self.shift()) - 1
    }

    fn is_tagged(&self, n: usize) -> bool {
        (n & self.mask()) == ((*self) as usize)
    }

    fn tag(&self, n: usize) -> usize {
        (n << self.shift()) | ((*self) as usize)
    }

    fn untag(&self, n: usize) -> usize {
        n >> self.shift()
    }
}

#[derive(Debug, Clone, Copy)]
pub struct TaggedPtr(*const u8);

impl TaggedPtr {
    fn new(ptr: *const u8, tag: TypeTag) -> Self {
        let num = ptr as usize;
        match tag {
            TypeTag::NIL => TaggedPtr(ptr::null()),
            _ => TaggedPtr(((num << tag.shift()) | (tag as usize)) as *const u8),
        }
    }

    pub fn value(&self) -> *const u8 {
        self.0
    }

    pub fn from_int(n: i64) -> TaggedPtr {
        let n = n as usize;
        let tagged = TypeTag::Integer.tag(n);
        TaggedPtr(tagged as *const u8)
    }

    pub fn as_int(self) -> Option<i64> {
        if !TypeTag::Integer.is_tagged(self.0.addr()) {
            return None;
        }
        let msb = self.0.addr() & (1 << ((std::mem::size_of::<usize>() * 8) - 1));
        let num = TypeTag::Integer.untag(self.0.addr());
        if msb != 0 {
            Some((num | (1 << ((std::mem::size_of::<usize>() * 8) - 1))) as i64)
        } else {
            Some(num as i64)
        }
    }

    pub fn from_sym(s: Symbol) -> TaggedPtr {
        let n = s.value();
        let tagged = TypeTag::Symbol.tag(n);
        TaggedPtr(tagged as *const u8)
    }

    pub fn as_sym<'a>(self) -> Option<Symbol> {
        if !TypeTag::Symbol.is_tagged(self.0.addr()) {
            None
        } else {
            let n = self.0 as usize;
            let n = TypeTag::Symbol.untag(n);
            Some(Symbol::new(n))
        }
    }

    pub fn from_list(lst: Box<List>) -> TaggedPtr {
        let raw = Box::into_raw(lst);
        let new_ptr = raw.map_addr(|addr| TypeTag::List.tag(addr));
        TaggedPtr(new_ptr.cast())
    }

    pub fn as_list(self) -> Option<Box<List>> {
        if !TypeTag::List.is_tagged(self.0.addr()) {
            None
        } else {
            let new_addr = self.0.map_addr(|addr| TypeTag::List.untag(addr));
            let new_addr = new_addr as *mut List;
            Some(unsafe { Box::from_raw(new_addr) })
        }
    }
}
