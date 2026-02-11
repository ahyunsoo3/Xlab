#include <iostream>

class Counter {
public:
    int value;

    Counter(int initial) {
        value = initial;
    }
};

int main() {
    Counter c(10);   // constructor is called
    std::cout << "Counter value: " << c.value << std::endl;
    return 0;
}
