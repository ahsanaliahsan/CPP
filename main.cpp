#include <iostream>
#include <string>
#include <thread>
#include <chrono>

// Simple function to be run in a thread
void count_numbers(char threadname, int max) {
    for(int i = 1; i <= max; ++i) {
        std::cout << " ID: " << std::this_thread::get_id() << " Name: " << threadname << " counting: " << i << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main() {
    // Create two threads
    std::thread t1(count_numbers, 'A', 15);
    std::thread t2(count_numbers, 'B', 5);

    // Main thread message
    std::cout << "Main thread: Waiting for threads to finish..." << std::endl;

    // Wait for threads to finish
    t1.join();
    t2.join();

    std::cout << "Main thread: All done!" << std::endl;
    return 0;
}