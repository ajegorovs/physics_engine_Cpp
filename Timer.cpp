#include "Timer.h"

void Timer::line_init(const std::string& className) {
    std::cout << className << "() initializing ... " << std::endl;
    start_time = std::chrono::steady_clock::now();  // Start the timer
}

// End the timer and calculate the elapsed time
void Timer::line_end(const std::string& className) {
    auto end_time = std::chrono::steady_clock::now();  // Get the current time (end time)
    std::chrono::duration<double> elapsed_seconds = end_time - start_time;  // Calculate elapsed time
    std::cout << className << "() initializing finished! (" << elapsed_seconds.count() << " s)" << std::endl << std::string(60, '-') << std::endl;;

}