#include <iostream>
#include <string>

template<typename T>
T add(T a, T b) {
    return a + b;
}

int main() {
    int x = 3, y = 5;
    std::cout << add(x, y) << std::endl;

    double a = 2.5, b = 1.5;
    std::cout << add(a, b) << std::endl;

    std::string s1 = "Hello, ";
    std::string s2 = "World";
    std::cout << add(s1, s2) << std::endl;

    return 0;
}

/* With this template, we don't have to write the type explicitly (e.g., add<int>, add<std:string>)
The compiler can deduce T from arguments.
*/

/* - Additional case -

std::cout << add(3, 2.5); // error
-> Even though the compiler can deduce types, T must be the same type for all arguments 

e.g.
std::cout << add<double>(3, 2.5);

*/
