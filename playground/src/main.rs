use std::thread;
use std::sync::{Mutex,Arc};
fn main(){
    let lock = Arc::new(Mutex::new(0));
    let mut threadlist = Vec::new();
    for _ in 0..109{
        let tempboi = Arc::clone(&lock);
        let thread1 = thread::spawn(move || {
            let mut num = tempboi.lock().unwrap();
            for _ in 0..100{
                *num += 1;
            }
        });
        threadlist.push(thread1);
    }
    for handle in threadlist{
        handle.join().unwrap();
    }
    dbg!(lock);
}
