use std::alloc::{GlobalAlloc, Layout, alloc};
use std::ptr::{null_mut, null};
use core::ffi::c_void;
use std::mem;
use std::fmt;


#[repr(C)]
struct MemoryManageArea {
    flag: i8,
    size: usize,
    next: *mut MemoryManageArea,
    prev: *mut MemoryManageArea,
}


#[link(name = "kumalloc", kind = "static")]
extern "C" {
    fn kumalloc(size: usize) -> *mut c_void;
    fn kufree(p: *mut c_void);
    static heap: *mut u8;
    static manage_head_ptr: *mut MemoryManageArea;
}


impl fmt::Debug for MemoryManageArea {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "MemoryManageArea(flag={}, size={}, next={}, prev={})", self.flag, self.size, self.next as usize, self.prev as usize).and_then(|_| {
            if self.next != null_mut() {
                unsafe {
                    write!(f, " -> ");
                    return (*self.next).fmt(f);
                }
            }
            write!(f, "")
        })
    }
}


struct KumAllocator;

unsafe impl GlobalAlloc for KumAllocator {
    unsafe fn alloc(&self, _layout: Layout) -> *mut u8 {
        kumalloc(_layout.size()) as *mut u8
    }
    unsafe fn dealloc(&self, _ptr: *mut u8, _layout: Layout) {
        kufree(_ptr as *mut c_void)
    }
}

#[global_allocator]
static A: KumAllocator = KumAllocator;


fn main() {
    {
        let mut v = Vec::new();
        // This will allocate memory using the system allocator.
        for i in 0..20 {
            v.push(i);
        }
        unsafe {
            println!("{:?}", *manage_head_ptr);
        }
    }
    unsafe {
        println!("{:?}", *manage_head_ptr);
    }
}

#[cfg(test)]
mod test {
    use crate::manage_head_ptr;
    use std::ptr::null_mut;

    #[test]
    fn create_vector() {
        let mut v: Vec<u32> = Vec::new();

        unsafe {
            assert_ne!(manage_head_ptr, null_mut());
        }
    }

}
