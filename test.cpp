#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>

int main()
{
    for (double i = 0.00; i <= 360.00; i += 0.01)
    {
        std::cout << std::fixed << std::setprecision(2) << i << std::endl;
        std::this_thread::sleep_for(std::chrono::microseconds(1666));
    }

    return 0;
}
