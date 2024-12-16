#include <windows.h>

extern int dll_main(int argc, char** argv);

int main(int argc, char** argv) {
    // Set the input and output code page to UTF-8 (65001)
    SetConsoleCP(65001);  // Set console input code page to UTF-8
    SetConsoleOutputCP(65001);  // Set console output code page to UTF-8

    return dll_main(argc, argv);
}

