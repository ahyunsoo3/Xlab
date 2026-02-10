#include <iostream>
#include <thread>
#include <mutex>

// Case 1: Without mutex

void run_without_mutex() {
    int counter = 0;

    auto increment = [&counter]() {
        for (int i = 0; i < 1000000; i++) {
            counter++;   // NOT thread-safe
        }
    };

    std::thread t1(increment);
    std::thread t2(increment);

    t1.join();
    t2.join();

    std::cout << "[Without mutex] Counter: " << counter << std::endl;
}

// Case 2: With mutex

void run_with_mutex() {
    int counter = 0;
    std::mutex mtx;

    auto increment = [&counter, &mtx]() {
        for (int i = 0; i < 1000000; i++) {
            std::lock_guard<std::mutex> lock(mtx);
            counter++;
        }
    };

    std::thread t1(increment);
    std::thread t2(increment);

    t1.join();
    t2.join();

    std::cout << "[With mutex]    Counter: " << counter << std::endl;
}

int main() {
    run_without_mutex();
    run_with_mutex();
    return 0;
}

/*  Selectable main Function
int main() {
    int choice;
    std::cout << "1 = without mutex, 2 = with mutex: ";
    std::cin >> choice;

    if (choice == 1) run_without_mutex();
    else run_with_mutex();
}
*/


// < Insight > 

/* 1) Without a mutex, multiple threads may read from and write to the
same memory location at the same time, with no synchronization,
leading to race conditions and undefined behavior. [1]

2) Experiments in LeetCode's Playground show that correct results are common
at low iteration counts, but become increasingly rare as the count grows,
suggesting nondeterministic behavior due to concurrent execution. */




/* < Reference >
[1] Race condition: the program's outcome depends on the timing or order of those accesses.  */

