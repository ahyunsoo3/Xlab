#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <cstdio>     // fopen, fclose, FILE

// =======================================================
// CASE 1: Destructor NOT needed (built-in types only)
// =======================================================
class Counter {
public:
    int value;
    Counter(int initial) : value(initial) {}
    // No destructor needed: int requires no cleanup
};

// =======================================================
// CASE 2: Destructor NOT needed (standard library types clean themselves)
// Rule of Zero
// =======================================================
class Data {
public:
    std::string name;
    std::vector<int> nums;
    std::unique_ptr<int> p;

    Data(std::string n)
        : name(std::move(n)), nums{1, 2, 3}, p(std::make_unique<int>(42)) {}
    // No destructor needed: string/vector/unique_ptr handle cleanup automatically
};

// =======================================================
// CASE 3: Destructor NEEDED (raw pointer ownership: new/delete)
// =======================================================
class RawOwner {
    int* ptr;
public:
    RawOwner(int v) : ptr(new int(v)) {}

    ~RawOwner() {                 // REQUIRED to avoid memory leak
        delete ptr;
    }
};

// =======================================================
// CASE 4: Destructor NEEDED (dynamic array: new[]/delete[])
// =======================================================
class ArrayOwner {
    int* arr;
public:
    ArrayOwner(size_t n) : arr(new int[n]) {}

    ~ArrayOwner() {               // REQUIRED
        delete[] arr;
    }
};

// =======================================================
// CASE 5: Destructor NEEDED (C-style resource: FILE*)
// =======================================================
class FileOwner {
    FILE* file;
public:
    FileOwner(const char* filename)
        : file(std::fopen(filename, "w")) {}

    ~FileOwner() {                // REQUIRED
        if (file) std::fclose(file);
    }
};

// =======================================================
// CASE 6: Destructor NEEDED (manual lock/unlock) - RAII style wrapper
// This is basically what std::lock_guard does.
// =======================================================
class ManualLockGuard {
    std::mutex& m;
public:
    ManualLockGuard(std::mutex& mtx) : m(mtx) {
        m.lock();
    }

    ~ManualLockGuard() {          // REQUIRED to guarantee unlock
        m.unlock();
    }
};

// =======================================================
// CASE 7: Destructor NOT needed (use std::lock_guard, which is RAII)
// =======================================================
class UsesStdLockGuard {
    std::mutex m;
public:
    void doWork() {
        std::lock_guard<std::mutex> lock(m);  // locks now, unlocks automatically
        // work...
    }
    // No destructor needed: std::mutex and std::lock_guard are already RAII
};

// =======================================================
// CASE 8: Polymorphic base class MUST have virtual destructor
// =======================================================
class Base {
public:
    virtual ~Base() = default;    // REQUIRED for safe delete via Base*
    virtual void hello() const { std::cout << "Base\n"; }
};

class Derived : public Base {
    int* ptr;
public:
    Derived() : ptr(new int(99)) {}

    ~Derived() override {         // cleans Derived resources
        delete ptr;
    }

    void hello() const override { std::cout << "Derived\n"; }
};

// =======================================================
// MAIN: demonstrate all cases in one run
// =======================================================
int main() {
    std::cout << "CASE 1: Counter (no destructor needed)\n";
    Counter c(10);
    std::cout << "  c.value = " << c.value << "\n\n";

    std::cout << "CASE 2: Data (Rule of Zero, no destructor needed)\n";
    Data d("example");
    std::cout << "  d.name = " << d.name << ", *d.p = " << *d.p << "\n\n";

    std::cout << "CASE 3: RawOwner (destructor needed for delete)\n";
    RawOwner ro(5);
    std::cout << "  RawOwner created\n\n";

    std::cout << "CASE 4: ArrayOwner (destructor needed for delete[])\n";
    ArrayOwner ao(100);
    std::cout << "  ArrayOwner created\n\n";

    std::cout << "CASE 5: FileOwner (destructor needed for fclose)\n";
    FileOwner fo("example_output.txt");
    std::cout << "  FileOwner created (writes possible)\n\n";

    std::cout << "CASE 6: ManualLockGuard (destructor needed for unlock)\n";
    std::mutex mtx;
    int sharedCounter = 0;
    {
        ManualLockGuard lock(mtx);   // locks here
        sharedCounter++;
    }                                // unlocks here (destructor)
    std::cout << "  sharedCounter = " << sharedCounter << "\n\n";

    std::cout << "CASE 7: UsesStdLockGuard (no destructor needed)\n";
    UsesStdLockGuard u;
    u.doWork();
    std::cout << "  did work\n\n";

    std::cout << "CASE 8: Polymorphism (virtual destructor needed)\n";
    Base* b = new Derived();
    b->hello();
    delete b; // safe because Base has virtual destructor
    std::cout << "  deleted via Base*\n\n";

    std::cout << "Done.\n";
    return 0;
}


/*  Basically, desturctor is fudamentall related to managing memory. 
In other words, we can understand of how to each resource is involved by understanding each case.
*/
