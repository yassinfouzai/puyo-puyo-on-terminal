#ifndef APPLICATION
#define APPLICATION


#include <ncurses.h>
#include "Puyo.hpp"
#include "Network.hpp"

class Application {
public:
    Application();
    ~Application();
    void run();

private:
    WINDOW* debugWin;
    Network net;
    
    Puyo randomColor();
    bool runConnectionSetup();
};

#endif // !APPLICATION
