use crate::ptrtag::TaggedPtr;

pub struct List {
    car: TaggedPtr,
    cdr: TaggedPtr,
}

impl List {
    // pub fn cons(car: TaggedPtr, cdr: TaggedPtr) -> TaggedPtr {}
}
