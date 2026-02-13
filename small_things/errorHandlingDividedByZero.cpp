#include <iostream>
#include <stdexcept>

double divide_bad_case(double a, double b) {
    return a / b;
}

double divide_good_case(double a, double b) {
    if (b == 0.0) {
        throw std::invalid_argument("Division by zero");
    }
    return a / b;
}

int main() {
    double x = 10.0;
    double y = 0.0;

    std::cout << "Bad case result: "
              << divide_bad_case(x, y) << std::endl;

    try {
        std::cout << "Good case result: "
                  << divide_good_case(x, y) << std::endl;
    }
    catch (const std::invalid_argument& e) {
        std::cout << "Caught exception: " << e.what() << "\n";
        
        // In Leetcode's Playground, following lines doesn't work.
        // std::cerr << "Caught exception: "
        //           << e.what() << std::endl; 
        
    }

    return 0;
}
