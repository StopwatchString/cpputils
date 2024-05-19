#include <iostream>

#include "Timer.h"

int main()
{
    static Timer timer;

    timer.start();
    timer.stop();
    std::cout << timer.getLast();


    return EXIT_SUCCESS;
}