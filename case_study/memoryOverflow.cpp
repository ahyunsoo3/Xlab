#include <algorithm>
#include <cctype>
#include <iostream>
#include <string>
#include <vector>
#include <new>

/* Usage
./app 10GB
./app 20GB
./app 5GB
*/

struct Config {
    std::size_t target_bytes = 10ULL * 1024 * 1024 * 1024; // <-- change default here (10GB -> 20GB)
    std::size_t chunk_bytes  = 64ULL * 1024 * 1024;        // 64MB per chunk
    bool leak = true;                                      // true = leak, false = free
};

// Parse strings like: "20GB", "512MB", "4096KB", "100B"
static std::size_t parse_size(std::string s) {
    // remove spaces
    s.erase(std::remove_if(s.begin(), s.end(), ::isspace), s.end());
    // uppercase
    for (char& c : s) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));

    // split number + unit
    std::size_t i = 0;
    while (i < s.size() && (std::isdigit(static_cast<unsigned char>(s[i])) || s[i] == '.')) i++;
    if (i == 0) throw std::runtime_error("Size must start with a number (e.g., 20GB)");

    double value = std::stod(s.substr(0, i));
    std::string unit = s.substr(i);

    std::size_t mult = 1;
    if (unit.empty() || unit == "B") mult = 1;
    else if (unit == "KB") mult = 1024ULL;
    else if (unit == "MB") mult = 1024ULL * 1024;
    else if (unit == "GB") mult = 1024ULL * 1024 * 1024;
    else if (unit == "TB") mult = 1024ULL * 1024 * 1024 * 1024;
    else throw std::runtime_error("Unknown unit. Use B/KB/MB/GB/TB (e.g., 20GB)");

    return static_cast<std::size_t>(value * static_cast<double>(mult));
}

static void touch_pages(char* p, std::size_t bytes) {
    const std::size_t page = 4096;
    for (std::size_t i = 0; i < bytes; i += page) p[i] = 1; // commit pages
    if (bytes) p[bytes - 1] = 1;
}

static void print_usage(const char* prog) {
    std::cout
        << "Usage:\n"
        << "  " << prog << " [target=10GB] [chunk=64MB] [leak=1|0]\n"
        << "Examples:\n"
        << "  " << prog << " 20GB 128MB 1   # leak up to 20GB in 128MB chunks\n"
        << "  " << prog << " 5GB  64MB  0   # allocate then free\n";
}

int main(int argc, char** argv) {
    Config cfg;

    try {
        if (argc > 1) cfg.target_bytes = parse_size(argv[1]);
        if (argc > 2) cfg.chunk_bytes  = parse_size(argv[2]);
        if (argc > 3) cfg.leak         = (std::stoi(argv[3]) != 0);
        if (argc > 4) { print_usage(argv[0]); return 1; }

        if (cfg.chunk_bytes == 0) throw std::runtime_error("chunk must be > 0");
        if (cfg.target_bytes == 0) throw std::runtime_error("target must be > 0");

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        print_usage(argv[0]);
        return 1;
    }

    std::vector<char*> blocks;
    blocks.reserve(cfg.target_bytes / cfg.chunk_bytes + 1);

    std::size_t allocated = 0;

    while (allocated < cfg.target_bytes) {
        std::size_t this_chunk = std::min(cfg.chunk_bytes, cfg.target_bytes - allocated);

        char* p = nullptr;
        try {
            p = new char[this_chunk];
        } catch (const std::bad_alloc&) {
            std::cerr << "bad_alloc after ~" << (allocated / (1024.0 * 1024.0 * 1024.0)) << " GB\n";
            break;
        }

        touch_pages(p, this_chunk);
        blocks.push_back(p);
        allocated += this_chunk;

        std::cout << "Committed ~" << (allocated / (1024.0 * 1024.0 * 1024.0))
                  << " GB (" << blocks.size() << " blocks)\n";
    }

    std::cout << "\nDone. leak=" << cfg.leak << ". Press Enter to exit...\n";
    std::cin.get();

    if (!cfg.leak) {
        for (char* p : blocks) delete[] p;
        std::cout << "Freed all blocks.\n";
    } else {
        std::cout << "Intentionally leaked.\n";
    }
    return 0;
}
