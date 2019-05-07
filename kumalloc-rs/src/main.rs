use std::alloc::{GlobalAlloc, Layout, alloc};
use std::ptr::null_mut;
use core::ffi::c_void;
use std::mem;
use std::fmt;




#[link(name = "kumalloc", kind = "static")]
extern "C" {
    fn kumalloc(size: usize) -> *mut c_void;
    fn kufree(p: *mut c_void);
}

#[repr(C)]
struct MemoryManageArea {
    flag: i8,
    size: usize,
    next: *mut MemoryManageArea,
    prev: *mut MemoryManageArea,
}

impl fmt::Debug for MemoryManageArea {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "MemoryManageArea(flag={}, size={}, next={}, prev={})", self.flag, self.size, self.next as usize, self.prev as usize)
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
    unsafe {
        let a: *mut u8 = kumalloc(10) as *mut u8;
        *a = 10;
        println!("{}", mem::size_of::<MemoryManageArea>() as isize);
        println!("{} {:?} {:?}", *a, a, *(a as *mut MemoryManageArea).offset(-1));
        kufree(a as *mut c_void);
        println!("{} {:?} {:?}", *a, a, *(a as *mut MemoryManageArea).offset(-1));
    }

    unsafe {
        assert!(!alloc(Layout::new::<u32>()).is_null())
    }
    let mut v = Vec::new();
    // This will allocate memory using the system allocator.
    v.push(1);
}
