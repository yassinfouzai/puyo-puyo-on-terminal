#ifndef APPLICATION
#define APPLICATION


#include <ncurses.h>
#include "Puyo.hpp"
#include "Network.hpp"

enum class GameMode { SINGLE_PLAYER, MULTIPLAYER };

class Application {
public:
    Application();
    ~Application();
    void run();

private:
    WINDOW* debugWin;
    WINDOW* scoreWin;
    Network net;

    GameMode chooseMode();
    Puyo randomColor();
    bool runConnectionSetup();
};

#endif // !APPLICATION
