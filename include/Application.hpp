#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "Network.hpp"
#include "Puyo.hpp"
#include "Config.hpp"
#include <ncurses.h>

enum class GameMode { SINGLE_PLAYER, MULTIPLAYER };

class Application
{
public:
    Application();
    ~Application();
    void run();

private:
    WINDOW* debugWin;
    WINDOW* scoreWin;
    Network net;
    Config config;

    GameMode chooseMode();
    void runOptionsMenu();
    bool runConnectionSetup();
    Puyo randomColor();
};

#endif
