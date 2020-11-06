#include "seahash/v2/seahash.hpp"

#include <string>
#include <span>
#include <iostream>
#include <chrono>
#include <sstream>

auto main() -> int
{
    seahash::v2::Seahash hasher;

    std::string a = "to be or not to be or not to be ";
    std::string tmp;
    { // RAII block
        std::stringstream ss;
        for(int i = 0; i <  1024 * 1024 * 32; i ++) {
            ss << a;
        }
        tmp = ss.str();
    }
    std::cout << tmp.size() << "Bytes" << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    std::span<char> view = tmp;
    auto bytes = std::as_writable_bytes(view);
    auto result = hasher.hash(bytes); // 1988685042348123509
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << result << std::endl;
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;
}
