// EZMP.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"
#include <stdio.h>
#include <time.h>
#include <chrono>
#include <ctime>

// TODO: This is an example of a library function
void fnEZMP()
{
    std::time_t t = std::time(0);   // get time now
    std::tm* now = std::localtime(&t);
    printf("%d", now->tm_sec);
}
