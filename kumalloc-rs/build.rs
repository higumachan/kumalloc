extern crate cc;

fn main(){
    println!("cargo:rustc-link-search=native=/usr/local/lib");
    cc::Build::new()
        .warnings(true)
        .flag("-Wall")
        .flag("-Wextra")
        .flag("-v")
        .flag("-g")
        .file("src/kumalloc/kumalloc.c")
        .include("src/kumalloc/include")
        .compile("libkumalloc.a");
}