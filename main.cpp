#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <future>
#include "utils.h" // Include the osyncstream definition if not in standard library

void slow(const std::string& name) {
    std::this_thread::sleep_for(std::chrono::seconds(7));
    std::osyncstream(std::cout) << name << std::endl;
}

void quick(const std::string& name) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::osyncstream(std::cout) << name << std::endl;
}

void work() {
    auto start = std::chrono::high_resolution_clock::now();

    std::promise<void> p_a2_done;
    std::future<void> f_a2_done = p_a2_done.get_future();

    auto t2 = std::async(std::launch::async, [&p_a2_done]() {
        quick("A2");
        p_a2_done.set_value();
        slow("A3");
    });

    auto t3 = std::async(std::launch::async, []() {
        quick("B2");
        quick("C2");
    });

    slow("A1");

    // Wait for A2 before starting B1
    f_a2_done.wait(); 
    quick("B1");

    // Wait for A3 (end of t2) before starting C1
    t2.wait();
    quick("C1");

    t3.wait();

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    
    std::osyncstream(std::cout) << "Work is done! Time: " << elapsed.count() << " sec" << std::endl;
}

int main() {
    work();
    return 0;
}
