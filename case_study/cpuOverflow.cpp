#include <chrono>
#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <algorithm>

/* Usage
./app 25 20 4   # ~25% load on 4 threads for 20s
./app 80 10 1   # ~80% on 1 thread
*/

static void load_worker(std::atomic<bool>& stop, int percent) {
    percent = std::clamp(percent, 0, 100);

    const auto period = std::chrono::milliseconds(100); // control granularity
    auto busy = period * percent / 100;
    auto idle = period - busy;

    volatile std::uint64_t x = 0;
    while (!stop.load(std::memory_order_relaxed)) {
        auto t0 = std::chrono::steady_clock::now();
        while (std::chrono::steady_clock::now() - t0 < busy) {
            x = x * 1664525u + 1013904223u;
        }
        if (idle.count() > 0) std::this_thread::sleep_for(idle);
    }
}

int main(int argc, char** argv) {
    // Usage: ./cpu_load [percent=50] [seconds=10] [threads=1]
    int percent = (argc > 1) ? std::stoi(argv[1]) : 50;
    int seconds = (argc > 2) ? std::stoi(argv[2]) : 10;
    unsigned threads = (argc > 3) ? static_cast<unsigned>(std::stoul(argv[3])) : 1;

    std::atomic<bool> stop{false};
    std::vector<std::thread> pool;
    pool.reserve(threads);

    for (unsigned i = 0; i < threads; ++i)
        pool.emplace_back(load_worker, std::ref(stop), percent);

    std::this_thread::sleep_for(std::chrono::seconds(seconds));
    stop.store(true);

    for (auto& t : pool) t.join();

    std::cout << "Done: ~" << percent << "% for " << seconds
              << "s, threads=" << threads << "\n";
    return 0;
}
