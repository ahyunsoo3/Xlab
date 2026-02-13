#include <array>
#include <iostream>
#include <cstddef>
#include <numeric>
#include <algorithm>

#if __cplusplus >= 202002L
#include <ranges>
#endif

template <std::size_t N>
void print_array(const std::array<int, N>& arr) {
    std::cout << "[ ";
    for (const auto& v : arr)
        std::cout << v << " ";
    std::cout << "]\n";
}

// C-style array (C / pre-C++98)
void c_style_array_case() {
    int arr[5];

    for (int i = 0; i < 5; ++i)
        arr[i] = i;

    std::cout << "[ ";
    for (int i = 0; i < 5; ++i)
        std::cout << arr[i] << " ";
    std::cout << "]\n";
}

// std::array (C++11)
void std_array_index_case() {
    std::array<int, 5> arr{};

    for (std::size_t i = 0; i < arr.size(); ++i)
        arr[i] = static_cast<int>(i);

    print_array(arr);
}

// Range-based for loop (C++11)
void range_based_for_case() {
    std::array<int, 5> arr{};

    int value = 0;
    for (auto& element : arr)
        element = value++;

    print_array(arr);
}

// std::iota (C++11)
void iota_case() {
    std::array<int, 5> arr{};
    std::iota(arr.begin(), arr.end(), 0);
    print_array(arr);
}

// Structured bindings (C++17)
void structured_bindings_case() {
#if __cplusplus >= 201703L
    std::array<int, 5> arr{0,1,2,3,4};
    auto [a,b,c,d,e] = arr;
    std::cout << "[ " << a << " " << b << " " << c << " "
              << d << " " << e << " ]\n";
#endif
}

// std::ranges::iota (C++20)
void ranges_iota_case() {
#if __cplusplus >= 202002L
    std::array<int, 5> arr{};
    std::ranges::iota(arr, 0);
    print_array(arr);
#endif
}

// std::views::iota + ranges::copy (C++20)
void views_iota_case() {
#if __cplusplus >= 202002L
    std::array<int, 5> arr{};
    auto view = std::views::iota(0, 5);
    std::ranges::copy(view, arr.begin());
    print_array(arr);
#endif
}

int main() {
    c_style_array_case();
    std_array_index_case();
    range_based_for_case();
    iota_case();
    structured_bindings_case();
    ranges_iota_case();
    views_iota_case();
}
