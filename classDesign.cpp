#include <string>

class Person {
public:
    Person(std::string name, int age)
        : name_(std::move(name)), age_(age) {}

    const std::string& getName() const noexcept {
        return name_;
    }

    int getAge() const noexcept {
        return age_;
    }

private:
    std::string name_;
    int age_;
};


int main() {
    Person p = Person("Alice", 22);
    std::cout << p.getAge() << '\n';
}
