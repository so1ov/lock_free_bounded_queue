#include <iostream>
#include <thread>
#include <chrono>
#include <exception>
#include <vector>
#include <numeric>
#include <cmath>

#include "lock_free_bounded_queue.h"


int main() {
    constexpr int Sizes[] = {100, 200, 500, 1000, 8000, 30000};
    constexpr int Tries = 100;

    std::cout << "Size - N - Mean (ms) - StdDev (ms)" << std::endl;
    for (int i = 0; i < sizeof(Sizes) / sizeof(Sizes[0]); i++) {
        std::vector<double> tses;
        tses.reserve(Tries);
        for (int current_try = 0; current_try < Tries; current_try++) {
            std::chrono::time_point<std::chrono::high_resolution_clock> before = std::chrono::high_resolution_clock::now();

            auto writer = [](LockFreeBoundedQueue<int, -1>& q) {
                for (int i = 0; i < 300000; i++) {
                    q.push(i);
                }
            };

            auto reader = [](LockFreeBoundedQueue<int, -1>& q) {
                int prev = -1;
                int cur = -1;
                for (int i = 0; i < 300000; i++) {
                    cur = q.pop();
                    if (prev + 1 != cur) {
                        std::cout << "assertion failed: " << prev << " " << cur << std::endl;
                        std::terminate();
                    }

                    prev = cur;
                }
            };
            
            LockFreeBoundedQueue<int, -1> q{Sizes[i]};
            std::thread writer_thread{writer, std::ref(q)};
            std::thread reader_thread{reader, std::ref(q)};

            writer_thread.join();
            reader_thread.join();

            std::chrono::time_point<std::chrono::high_resolution_clock> after = std::chrono::high_resolution_clock::now();

            tses.push_back(std::chrono::duration<double, std::milli>(after - before).count());
        }

        double mean = std::accumulate(tses.begin(), tses.end(), 0.0) / tses.size();
        double sq_sum = std::inner_product(tses.begin(), tses.end(), tses.begin(), 0.0);
        double stddev = std::sqrt(sq_sum / tses.size() - mean * mean);

        std::cout << Sizes[i] << " " << Tries << " " << mean << " " << stddev << std::endl;
    }

    return 0;
}