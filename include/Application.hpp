#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "Board.hpp"
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
    WINDOW* highWin;
    Network net;
    Config config;
    
    void drawDebug(cchar_t vert, cchar_t horiz, cchar_t c1, cchar_t c2, cchar_t c3, cchar_t c4);
    void drawHighscore(cchar_t vert, cchar_t horiz, cchar_t c1, cchar_t c2, cchar_t c3, cchar_t c4);
    void drawScore(cchar_t vert, cchar_t horiz, cchar_t c1, cchar_t c2, cchar_t c3, cchar_t c4, int score);

    GameMode chooseMode();
    void runOptionsMenu();
    bool runConnectionSetup();
    Puyo randomColor();
    void applyLayout(Board& board, Board* opponentBoard, bool multiplayer, int currentScore);
};

#endif
