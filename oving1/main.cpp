#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <algorithm>

std::mutex mtx; // Mutex for thread-safe access to the shared vector
std::vector<int> primeNumbers; // Vector to store the prime numbers found by threads

// Function to check if a number is prime
bool isPrime(int num) {
    if (num <= 1) return false; // Numbers less than or equal to 1 are not prime
    for (int i = 2; i * i <= num; i++) {
        if (num % i == 0) return false; //If numb is divisible by any number between 2 and sqrt(num), it's not prime
    }
    return true; // If no divisors are found, num is prime
}

// Function for each thread to find prime numbers in a given range
void findPrimes(int start, int end) {
    for (int i = start; i <= end; i++) { // Loop through the range
        if (isPrime(i)) { // Check if the number is prime
            std::lock_guard<std::mutex> lock(mtx); // Lock the mutex while adding to the vector
            primeNumbers.push_back(i); // Add the prime number to the shared vector
        }
    }
}

int main() {
    int start, end, numThreads;

    // Ask the user for the range to search for prime numbers
    std::cout << "Choose start numb: ";
    std::cin >> start;

    std::cout << "Choose end numb: ";
    std::cin >> end;

    // Find out how many threads the hardware can support
    unsigned int availableThreads = std::thread::hardware_concurrency();
    std::cout << "Available threads: " << availableThreads << std::endl;

    // Ask the user for how many threads they want to use
    std::cout << "Choose numb of threads: ";
    std::cin >> numThreads;

    // Check if the entered number of threads is valid
    if (numThreads <= 0 || numThreads > static_cast<int>(availableThreads)) {
        std::cout << "Invalid number. Using max available " << availableThreads << std::endl;
        numThreads = availableThreads;
    }

    std::vector<std::thread> threads; // Vector to hold the threads
    int range = (end - start + 1) / numThreads; // Calculate the range each thread will cover
    int rangeStart, rangeEnd;
    for (int i = 0; i < numThreads; i++) {
        rangeStart = start + i * range; // Calculate the start of the range for this thread
        rangeEnd = (i == numThreads - 1) ? end : start + (i + 1) * range - 1; // Calculate the end of the range, ensuring the last thread covers any remainder
        threads.push_back(std::thread(findPrimes, rangeStart, rangeEnd)); // Create and start a new thread
    }

    for (auto &th : threads) {
        th.join(); // Wait for all threads to finish their task
    }

    std::sort(primeNumbers.begin(), primeNumbers.end()); // Sort the prime numbers found

    // Print the sorted list of prime numbers
    for (int num : primeNumbers) {
        std::cout << num << " ";
    }
    std::cout << std::endl;

    return 0;
}

