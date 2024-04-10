#include <iostream>

#include "lock_free_bounded_queue.h"

auto writer = [](LockFreeBoundedQueue<int, -1, 64>& q) {
    for (int i = 0; i < 1000000; i++) {
        q.push(i);
    }
};

auto reader = [](LockFreeBoundedQueue<int, -1, 64>& q) {
    int prev = -1;
    int cur = -1;
    for (int i = 0; i < 1000000; i++) {
        cur = q.pop();
        if (prev + 1 != cur) {
            std::cout << "assertion failed: " << prev << " " << cur << std::endl;
            return;
        }

        prev = cur;
    }
};

int main() {
    LockFreeBoundedQueue<int, -1, 64> q;
    std::thread writer_thread{writer, std::ref(q)};
    std::thread reader_thread{reader, std::ref(q)};

    writer_thread.join();
    reader_thread.join();
    return 0;
}