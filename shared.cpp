
#include <iostream>

#ifdef _WIN32
    #define dllexport __declspec(dllexport)
#else
    #define dllexport
#endif

extern "C" dllexport void print(char* c) {
    std::cout << c << std::endl;
}