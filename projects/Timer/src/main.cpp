#include "Timer.h"
#include <cstdlib>
#include <Windows.h>

int main()
{
    Timer t;

    t.start();

    Sleep(2000);

    t.stop(true);

    return EXIT_SUCCESS;
}