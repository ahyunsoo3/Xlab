#include <iostream>

void case_copy() {
    int b = 10;
    int a = b;

    a = 7;
    std::cout << "[Copy]\n";
    std::cout << "a = " << a << "\n";
    std::cout << "b = " << b << "\n\n";
}

void case_reference() {
    int a = 10;
    int& b = a;

    b = 7;
    std::cout << "[Reference]\n";
    std::cout << "a = " << a << "\n";
    std::cout << "b = " << b << "\n\n";
}

void case_pointer() {
    int a = 10;
    int* b = &a;

    *b = 7;
    std::cout << "[Pointer]\n";
    std::cout << "a  = " << a << "\n";
    std::cout << "*b = " << *b << "\n\n";
}

int main() {
    case_copy();
    case_reference();
    case_pointer();
    return 0;
}


/* selectable main 
int main() {
    int choice;
    std::cout << "1=Copy, 2=Reference, 3=Pointer: ";
    std::cin >> choice;

    if (choice == 1) case_copy();
    else if (choice == 2) case_reference();
    else if (choice == 3) case_pointer();
    else std::cout << "Invalid choice\n";

    return 0;
}
*/
