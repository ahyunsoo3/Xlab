#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <string>
#include <memory>

// =======================================================
// LAMBDA CASES (all in one source file)
// =======================================================
//
// Build/run (example):
//   g++ -std=c++17 -O2 -Wall lambda_cases.cpp && ./a.out
//
// Notes:
// - Most lambdas are demonstrated in isolated scopes to keep lifetimes clear.
// - One case intentionally shows a *dangerous* pattern (dangling reference);
//   it is NOT executed (commented out).
//

// Helper: runs a callable with no args
template <typename F>
void run(const char* title, F f) {
    std::cout << title << "\n";
    f();
    std::cout << "\n";
}

int main() {
    run("CASE 1: Basic lambda (no capture, no parameters)", []() {
        std::cout << "  Hello from a lambda!\n";
    });

    run("CASE 2: Lambda with Parameters + return value", []() {
        auto add = [](int a, int b) { return a + b; };
        std::cout << "  add(3, 4) = " << add(3, 4) << "\n";
    });

    run("CASE 3: Capture by value [x] (lambda gets a copy)", []() {
        int x = 10;
        auto show = [x]() { std::cout << "  captured x = " << x << "\n"; };
        x = 99;
        show();
        std::cout << "  current x outside = " << x << "\n";
    });
    // This means that, we can control `x` dynamically.

    run("CASE 4: Capture by reference [&x] (lambda uses original)", []() {
        int x = 10;
        auto inc = [&x]() { x++; };
        inc();
        inc();
        std::cout << "  x after two inc() = " << x << "\n";
    });
    // Unlike case 3, this case does not copy; it refers to the original.

    
    run("CASE 5: Mixed capture [=] and [&])", []() {
        int a = 1, b = 2;

        auto f = [=, &b]() { // a by value, b by reference
            // a++; // error: a is const copy inside lambda
            b++;
            std::cout << "  inside: a(copy)=" << a << ", b(ref)=" << b << "\n";
        };

        f();
        std::cout << "  outside: a=" << a << ", b=" << b << "\n";
    });
    //   [=] captures used vars by value, [&] captures by reference

    // =======================================================
    // CASE 6: mutable lambda (allows modifying value-captures)
    // =======================================================
    run("CASE 6: mutable (modify value-capture inside)", []() {
        int x = 5;
        auto bump_copy = [x]() mutable {
            x++;
            std::cout << "  inside (copy) x=" << x << "\n";
        };

        bump_copy(); // inside: 6
        bump_copy(); // inside: 6 again? No: same lambda object retains its own copy -> becomes 7
        std::cout << "  outside x=" << x << "\n"; // still 5
    });

    // =======================================================
    // CASE 7: Init-capture (C++14+) [name = expr]
    //   Useful for moving ownership into the lambda.
    // =======================================================
    run("CASE 7: Init-capture (move unique_ptr into lambda)", []() {
        auto p = std::make_unique<int>(42);

        auto owns = [ptr = std::move(p)]() {
            std::cout << "  ptr points to " << *ptr << "\n";
        };

        // p is now null (moved-from)
        std::cout << "  p is " << (p ? "non-null" : "null") << " after move\n";
        owns();
    });

    // =======================================================
    // CASE 8: Generic lambda (C++14+) using auto parameters
    // =======================================================
    run("CASE 8: Generic lambda (auto parameters)", []() {
        auto twice = [](auto v) { return v + v; };
        std::cout << "  twice(3) = " << twice(3) << "\n";
        std::cout << "  twice(std::string(\"Hi\")) = " << twice(std::string("Hi")) << "\n";
    });

    // =======================================================
    // CASE 9: Returning a lambda (factory pattern)
    // =======================================================
    run("CASE 9: Function returns a lambda", []() {
        auto make_adder = [](int base) {
            return [base](int x) { return base + x; };
        };

        auto add10 = make_adder(10);
        std::cout << "  add10(7) = " << add10(7) << "\n";
    });

    // =======================================================
    // CASE 10: Lambda as algorithm callback (std::for_each)
    // =======================================================
    run("CASE 10: Lambda with std::for_each", []() {
        std::vector<int> v{1, 2, 3, 4};
        int sum = 0;

        std::for_each(v.begin(), v.end(), [&sum](int x) { sum += x; });
        std::cout << "  sum = " << sum << "\n";
    });

    // =======================================================
    // CASE 11: Lambda as comparator (std::sort)
    // =======================================================
    run("CASE 11: Lambda as comparator (sort descending)", []() {
        std::vector<int> v{3, 1, 4, 2};
        std::sort(v.begin(), v.end(), [](int a, int b) { return a > b; });

        std::cout << "  sorted: ";
        for (int x : v) std::cout << x << " ";
        std::cout << "\n";
    });

    // =======================================================
    // CASE 12: Storing lambdas: `auto` vs `std::function`
    //   - `auto` keeps exact (unique) lambda type (fast, no type erasure).
    //   - `std::function` type-erases callables (flexible, some overhead).
    // =======================================================
    run("CASE 12: auto vs std::function", []() {
        auto exact = [](int x) { return x * 2; };
        std::function<int(int)> erased = [](int x) { return x * 3; };

        std::cout << "  exact(5)  = " << exact(5) << "\n";
        std::cout << "  erased(5) = " << erased(5) << "\n";
    });

    // =======================================================
    // CASE 13: Immediately-invoked lambda (IIFE style)
    // =======================================================
    run("CASE 13: Immediately invoked lambda", []() {
        int result = []() {
            int a = 2, b = 3;
            return a * b;
        }(); // <-- called immediately

        std::cout << "  result = " << result << "\n";
    });

    // =======================================================
    // CASE 14: Capturing `this` inside a class (C++11+)
    //   Demonstration uses a local struct.
    // =======================================================
    run("CASE 14: Capturing `this` in a class method", []() {
        struct Greeter {
            std::string name;
            explicit Greeter(std::string n) : name(std::move(n)) {}

            void greet() const {
                auto f = [this]() { std::cout << "  Hello, " << name << "\n"; };
                f();
            }
        };

        Greeter g("Lambda");
        g.greet();
    });

    // =======================================================
    // CASE 15: ⚠️ Common pitfall: returning a lambda capturing a local by reference
    //   This creates a dangling reference after the function returns.
    //   (Shown but NOT executed.)
    // =======================================================
    run("CASE 15: Pitfall (dangling reference) - shown only", []() {
        auto bad_factory = []() {
            int local = 123;
            // DO NOT DO THIS: local will die when bad_factory returns
            return [&local]() { std::cout << local << "\n"; };
        };

        std::cout << "  (Not calling the returned lambda: it would dangle.)\n";
        auto bad = bad_factory();
        (void)bad;

        // bad(); // <-- UB (undefined behavior). Don't do this.
    });

    // =======================================================
    // CASE 16: Safe version of CASE 15: capture by value
    // =======================================================
    run("CASE 16: Safe factory (capture by value)", []() {
        auto good_factory = []() {
            int local = 123;
            return [local]() { std::cout << "  local(copy) = " << local << "\n"; };
        };

        auto good = good_factory();
        good(); // safe
    });

    std::cout << "Done.\n";
    return 0;
}
