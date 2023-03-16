// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>
#include <map>
#include <exception>
#include <functional>
#include <cmath>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <chrono>
#include <atomic>
#include "integrals.h"
#include "config_manager.h"
#include "functions.h"

namespace time_functions {
    inline std::chrono::high_resolution_clock::time_point get_current_time_fenced() {
        std::atomic_thread_fence(std::memory_order_seq_cst);
        auto res_time = std::chrono::high_resolution_clock::now();
        std::atomic_thread_fence(std::memory_order_seq_cst);
        return res_time;
    }

    template<class D>
    inline long long to_ms(const D &d) {
        return std::chrono::duration_cast<std::chrono::milliseconds>(d).count();
    }
}


int main(int argc, char *argv[]) {
    if (argc != 5) {
        std::cerr << "Wrong number of arguments" << std::endl;
        exit(1);
    }

    std::map<std::string, double> config;

    // trying to get the configuration
    try {
        config = get_config(argv[1]);
    }
    catch (const std::logic_error &error) {
        std::cerr << "Unable to read the configuration properly: " << std::endl;
        // the name of the absent argument
        std::cerr << error.what();
        exit(5);
    }
    catch (...) {
        std::cerr << "Unable to open config file" << std::endl;
        exit(3);
    }

    // trying to parse the number of the function

    int function_number, thread_count, points_count;

    // trying to parse function number
    try {
        function_number = std::stoi(argv[2]);
    }
    catch (std::invalid_argument &) {
        std::cerr << "Unable to parse the function number" << std::endl;
        exit(2);
    }
    catch (...) {
        std::cerr << "The function number is too large" << std::endl;
        exit(2);
    }

    if (function_number < 1 || function_number > 3) {
        std::cerr << "Invalid function number" << std::endl;
        exit(2);
    }

    // trying to parse thread_count
    try {
        thread_count = std::stoi(argv[3]);
    }
    catch (std::invalid_argument &) {
        std::cerr << "Unable to parse the number of threads" << std::endl;
        exit(2);
    }
    catch (...) {
        std::cerr << "The number of thread is too large" << std::endl;
        exit(2);
    }

    if (thread_count < 1) {
        std::cerr << "Wrong number of threads" << std::endl;
        exit(4);
    }

    // trying to parse points_count
    try {
        points_count = std::stoi(argv[4]);
    }
    catch (std::invalid_argument &) {
        std::cerr << "Unable to parse the number of points" << std::endl;
        exit(2);
    }
    catch (...) {
        std::cerr << "The number of thread is too large" << std::endl;
        exit(2);
    }

    if (points_count < thread_count) {
        std::cerr << "Number of points is less than number of threads" << std::endl;
        exit(4);
    }

    const std::function<double(double, double)> functions[] = {
            functions::f1,
            functions::f2,
            functions::f3,
    };

    auto start_time_point = time_functions::get_current_time_fenced();

    auto res = std::make_tuple(0, 0, 0);

    auto end_time_point = time_functions::get_current_time_fenced();
    auto dur = end_time_point - start_time_point;
    std::cout.precision(17);
    std::cout << std::fixed << std::get<0>(res) << std::endl;
    std::cout << std::fixed << std::get<1>(res) << std::endl;
    std::cout << std::fixed << std::get<2>(res) << std::endl;
    std::cout << std::fixed << time_functions::to_ms(dur) << std::endl;
    return 0;
}
