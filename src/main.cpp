#include "../include/Application.hpp"
#include <clocale>

int main()
{
    setlocale(LC_ALL, "");
    Application app;
    app.run();
    return 0;
}
