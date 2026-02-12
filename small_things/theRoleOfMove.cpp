int main() {
    std::string a = "Alice";
    std::string b = std::move(a);

    std::cout << a << " | " << b << '\n';
    
    a = "Bob";    
    std::cout << a << " | " << b << '\n';    
}
