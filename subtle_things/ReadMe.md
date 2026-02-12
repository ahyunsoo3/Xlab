# ULL vs LL

Comparing windows and Linux, even if they use same type names, the acutal sizes can be different because the platforms use differnet data models.

e.g)
**On Linux (LP64 model)**
- unsigned long = 8 bytes
- unsinged long long = 8 bytes

**On Windows (LLP64 model)**
- unsigned long = **4 bytes**
- unsigned long long = 8 bytes

Then, why don't we just use `ull`, but not `size_t`? <br>
The reason is that `size_t` matches what the standard library uses:
- `sizeof(...)` returns `std::size_t`
- `std::string::size()`, `std::vector::size()` return `std::size_t`

This seems to be traditional syntax, but even if we are using `ull` instead of `size_t`, 
there will be another traditional syntax, indicating that we cannot define which one is better than 
others unless implementing and complementing it in a practical way.
