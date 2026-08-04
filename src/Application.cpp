#include "../include/Application.hpp"
#include "../include/Board.hpp"

#include <ncurses.h>
#include <cstdlib>
#include <ctime>
#include <chrono>

static const std::string CONFIG_PATH = "puyo.conf";

Application::Application()
{
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    if (has_colors())
    {
        start_color();

        init_pair(1, COLOR_RED,     COLOR_RED);
        init_pair(2, COLOR_BLUE,    COLOR_BLUE);
        init_pair(3, COLOR_GREEN,   COLOR_GREEN);
        init_pair(4, COLOR_YELLOW,  COLOR_YELLOW);
        init_pair(5, COLOR_MAGENTA, COLOR_MAGENTA);

        if (COLORS >= 256)
        {
            init_pair(11, 196, 196);
            init_pair(12, 33,  33);
            init_pair(13, 46,  46);
            init_pair(14, 226, 226);
            init_pair(15, 129, 129);
        }
    }

    config.load(CONFIG_PATH);

    refresh();

    cchar_t vert, horiz, c1, c2, c3, c4;
    setcchar(&vert,  L"║", 0, 0, nullptr);
    setcchar(&horiz, L"═", 0, 0, nullptr);

    setcchar(&c1,  L"╔", 0, 0, nullptr);
    setcchar(&c2,  L"╗", 0, 0, nullptr);
    setcchar(&c3,  L"╚", 0, 0, nullptr);
    setcchar(&c4,  L"╝", 0, 0, nullptr);

    debugWin = newwin(10, 30, 8, 57);
    drawDebug(vert, horiz, c1, c2, c3, c4);

    scoreWin = newwin(3, 30, 1, 57);
    drawScore(vert, horiz, c1, c2, c3, c4, 0);

    highWin = newwin(3, 30, 4, 57);
    drawHighscore(vert, horiz, c1, c2, c3, c4);
}

void Application::drawDebug(cchar_t vert, cchar_t horiz, cchar_t c1, cchar_t c2, cchar_t c3, cchar_t c4) {
    wborder_set(debugWin, &vert, &vert, &horiz, &horiz, &c1, &c2, &c3, &c4);
    mvwprintw(debugWin, 0, 2, " Controls ");

    mvwprintw(debugWin, 2, 2, " [UP]   ");
    mvwprintw(debugWin, 3, 2, " [DOWN] ");
    mvwprintw(debugWin, 4, 2, " [LEFT] ");
    mvwprintw(debugWin, 5, 2, " [RIGHT]");

    mvwprintw(debugWin, 2, 11, "Rotate");
    mvwprintw(debugWin, 3, 11, "Soft drop");
    mvwprintw(debugWin, 4, 11, "Move left");
    mvwprintw(debugWin, 5, 11, "Move right");

    mvwprintw(debugWin, 7, 3, "[Q]");
    mvwprintw(debugWin, 7, 11, "Quit");

    wrefresh(debugWin);
}

void Application::drawScore(cchar_t vert, cchar_t horiz, cchar_t c1, cchar_t c2, cchar_t c3, cchar_t c4, int currentScore) {
    wborder_set(scoreWin, &vert, &vert, &horiz, &horiz, &c1, &c2, &c3, &c4);
    mvwprintw(scoreWin, 0, 2, "Score");
    mvwprintw(scoreWin, 1, 3, "%d", currentScore);
    wrefresh(scoreWin);
}


void Application::drawHighscore(cchar_t vert, cchar_t horiz, cchar_t c1, cchar_t c2, cchar_t c3, cchar_t c4) {
    wborder_set(highWin, &vert, &vert, &horiz, &horiz, &c1, &c2, &c3, &c4);
    mvwprintw(highWin, 0, 2, "Highscore");
    mvwprintw(highWin, 1, 3, "%d", config.highScore);
    wrefresh(highWin);
}

Application::~Application()
{
    delwin(scoreWin);
    delwin(debugWin);
    delwin(highWin);
    endwin();
}

Puyo Application::randomColor()
{
    switch (rand() % 5)
    {
        case 0: return Puyo::RED;
        case 1: return Puyo::BLUE;
        case 2: return Puyo::GREEN;
        case 3: return Puyo::YELLOW;
        default: return Puyo::PURPLE;
    }
}

GameMode Application::chooseMode()
{
    while (true)
    {
        werase(stdscr);
        mvprintw(2, 2, "PUYO PUYO ON TERMINAL");
        mvprintw(4, 2, "1 - Single Player");
        mvprintw(5, 2, "2 - Multiplayer");
        mvprintw(6, 2, "o - Options");
        mvprintw(7, 2, "q - Quit");
        refresh();

        timeout(-1);
        int key = getch();

        if (key == '1')
            return GameMode::SINGLE_PLAYER;
        if (key == '2')
            return GameMode::MULTIPLAYER;
        if (key == 'o')
        {
            runOptionsMenu();
            continue;
        }
        if (key == 'q')
        {
            endwin();
            exit(0);
        }
    }
}

bool Application::runConnectionSetup() {

    while (true) {
        werase(stdscr);
        mvprintw(2, 2, "PUYO NETWORK SETUP : ");
        mvprintw(4, 2, "h - Host a game");
        mvprintw(6, 2, "j - Join a game");
        mvprintw(8, 2, "q - Quit");
        refresh();

        timeout(-1);


        int key = getch();

        if (key == 'q') return false;

        if (key == 'h') {
            echo();
            curs_set(1);

            char portBuf[16];
            mvprintw(8, 2, "Port Host on : ");
            refresh();
            getnstr(portBuf, sizeof(portBuf) - 1);

            noecho();
            curs_set(0);

            int port = std::atoi(portBuf);
            if (port <= 0) {
                mvprintw(10, 2, "Invalid port. Press any key...");
                refresh();
                getch();
                continue;
            }

            werase(stdscr);
            mvprintw(2, 2, "Waiting for opponent to connect on port %d...", port);
            mvprintw(4, 2, "(Ctrl+C to cancel)");
            refresh();

            if (net.hostGame(port))
            {
                mvprintw(6, 2, "Connected! Press any key to start...");
                refresh();
                getch();
                return true;
            }
            else
            {
                mvprintw(6, 2, "Failed to host. Press any key...");
                refresh();
                getch();
                continue;
            }
        }
        else if (key == 'j')
        {
            echo();
            curs_set(1);

            char ipBuf[64];
            char portBuf[16];
            mvprintw(8, 2, "Host IP: ");
            refresh();
            getnstr(ipBuf, sizeof(ipBuf) - 1);

            mvprintw(9, 2, "Port: ");
            refresh();
            getnstr(portBuf, sizeof(portBuf) - 1);

            noecho();
            curs_set(0);

            int port = std::atoi(portBuf);
            if (port <= 0)
            {
                mvprintw(11, 2, "Invalid port. Press any key...");
                refresh();
                getch();
                continue;
            }

            werase(stdscr);
            mvprintw(2, 2, "Connecting to %s:%d...", ipBuf, port);
            refresh();

            if (net.joinGame(ipBuf, port))
            {
                mvprintw(4, 2, "Connected! Press any key to start...");
                refresh();
                getch();
                return true;
            }
            else
            {
                mvprintw(4, 2, "Failed to connect. Press any key...");
                refresh();
                getch();
                continue;
            }
        }
    }
}

void Application::runOptionsMenu()
{
    while (true)
    {
        werase(stdscr);
        mvprintw(2, 2, "OPTIONS");
        mvprintw(4, 2, "Color mode: %s",
                  config.colorMode == ColorMode::OFFICIAL ? "Official Puyo colors" : "Terminal colors");
        mvprintw(6, 2, "c - Toggle color mode");
        mvprintw(7, 2, "b - Back");
        refresh();

        timeout(-1);
        int key = getch();

        if (key == 'c')
        {
            config.colorMode = (config.colorMode == ColorMode::TERMINAL)
                                  ? ColorMode::OFFICIAL : ColorMode::TERMINAL;
            config.save(CONFIG_PATH);
        }
        else if (key == 'b')
        {
            return;
        }
    }
}

void Application::applyLayout(Board& board, Board* opponentBoard, bool multiplayer, int currentScore)
{
    cchar_t vert, horiz, c1, c2, c3, c4;
    setcchar(&vert,  L"║", 0, 0, nullptr);
    setcchar(&horiz, L"═", 0, 0, nullptr);

    setcchar(&c1,  L"╔", 0, 0, nullptr);
    setcchar(&c2,  L"╗", 0, 0, nullptr);
    setcchar(&c3,  L"╚", 0, 0, nullptr);
    setcchar(&c4,  L"╝", 0, 0, nullptr);



    int marginY = static_cast<int>(LINES * 0.03);
    int marginX = static_cast<int>(COLS  * 0.05);

    int boardY = marginY;
    int boardX = marginX;
    board.reposition(boardY, boardX);

    int nextX = boardX + board.windowWidth() + marginX;

    if (multiplayer && opponentBoard)
    {
        opponentBoard->reposition(boardY, nextX);
        nextX += opponentBoard->windowWidth() + marginX;
    }

    werase(scoreWin); wrefresh(scoreWin);
    werase(highWin);  wrefresh(highWin);
    werase(debugWin); wrefresh(debugWin);

    mvwin(scoreWin, boardY,     nextX);
    mvwin(highWin,  boardY + 3, nextX);
    mvwin(debugWin, boardY + 7, nextX);

    drawDebug(vert, horiz, c1, c2, c3, c4);
    drawScore(vert, horiz, c1, c2, c3, c4, currentScore);
    drawHighscore(vert, horiz, c1, c2, c3, c4);
}


void Application::run()
{
    int score = 0;

    GameMode mode = chooseMode();
    bool multiplayer = (mode == GameMode::MULTIPLAYER);

    if (multiplayer)
    {
        if (!runConnectionSetup())
            return;
    }


    werase(stdscr);
    refresh();

    touchwin(debugWin);
    wrefresh(debugWin);
    touchwin(scoreWin);
    wrefresh(scoreWin);
    touchwin(highWin);
    wrefresh(highWin);

    srand(static_cast<unsigned>(time(nullptr)));

    Board board(6, 12, 1, 1);
    board.setColorMode(config.colorMode);

    Board* opponentBoard = nullptr;
    std::vector<uint8_t> incomingBuf;
    if (multiplayer)
    {
        opponentBoard = new Board(6, 12, 1, 29);
        opponentBoard->setColorMode(config.colorMode);
        incomingBuf.resize(opponentBoard->cellCount());
    }

    applyLayout(board, opponentBoard, multiplayer, score); 


    int spawnX = 3, spawnY = 0;

    auto placePair = [&](const PuyoPair& p)
    {
        board.setCell(p.pivotX, p.pivotY, p.color1);
        board.setCell(p.secondX(), p.secondY(), p.color2);
    };
    auto clearPair = [&](const PuyoPair& p)
    {
        board.setCell(p.pivotX, p.pivotY, Puyo::EMPTY);
        board.setCell(p.secondX(), p.secondY(), Puyo::EMPTY);
    };
    auto pairFits = [&](const PuyoPair& p)
    {
        return board.isValidPosition(p.pivotX, p.pivotY)
            && board.isValidPosition(p.secondX(), p.secondY());
    };
    auto sendBoardState = [&]()
    {
        if (!multiplayer) return;
        std::vector<uint8_t> buf(board.cellCount());
        board.exportCells(buf.data());
        net.sendBytes(buf.data(), buf.size());
    };

    PuyoPair piece{ spawnX, spawnY, 2, randomColor(), randomColor() };
    placePair(piece);
    board.draw();
    sendBoardState();

    timeout(50);
    using clock = std::chrono::steady_clock;
    auto lastDrop = clock::now();
    const auto dropInterval = std::chrono::milliseconds(600);

    bool gameOver = false;

    while (true)
    {
        if (multiplayer && net.pollBytes(incomingBuf.data(), incomingBuf.size()))
        {
            opponentBoard->importCells(incomingBuf.data());
            opponentBoard->draw();
        }

        int key = getch();

        if (key == KEY_RESIZE)
        {
            clearok(curscr, TRUE);
            werase(stdscr);
            refresh();

            applyLayout(board, opponentBoard, multiplayer, score);

            board.draw();
            if (multiplayer)
                opponentBoard->draw();

            continue;
        }

        if (gameOver)
        {
            if (key == 'q' || key == '\n' || key == ' ')
                break;
            continue;
        }

        PuyoPair attempt = piece;
        bool tryMove = false;
        bool isGravityTick = false;

        auto now = clock::now();
        if (now - lastDrop >= dropInterval)
        {
            lastDrop = now;
            attempt.pivotY++;
            tryMove = true;
            isGravityTick = true;
        }

        if (key != ERR)
        {
            if (key == 'q')
                break;

            if (!isGravityTick)
            {
                switch (key)
                {
                    case KEY_LEFT:  attempt.pivotX--; tryMove = true; break;
                    case KEY_RIGHT: attempt.pivotX++; tryMove = true; break;
                    case KEY_DOWN:  attempt.pivotY++; tryMove = true; break;
                    case KEY_UP:    attempt.rotation = (attempt.rotation + 1) % 4; tryMove = true; break;
                    default: break;
                }
            }
        }

        if (!tryMove)
            continue;

        clearPair(piece);

        if (pairFits(attempt))
        {
            piece = attempt;
            placePair(piece);
            board.draw();
            sendBoardState();
        }
        else
        {
            placePair(piece);

            if (isGravityTick)
            {
                board.applyGravity();
                board.draw();
                sendBoardState();

                int chainCount = 0;
                int cellsCleared;
                while ((cellsCleared = board.clearMatches()) > 0)
                {
                    chainCount++;
                    score += 10 * cellsCleared * chainCount;
                    board.applyGravity();
                    board.draw();
                    sendBoardState();
                    napms(200);
                }

                mvwprintw(scoreWin, 0, 2, "Score");
                mvwprintw(scoreWin, 1, 3, "%d", score);
                wrefresh(scoreWin);

                if (score > config.highScore)
                {
                    config.highScore = score;
                    config.save(CONFIG_PATH);
                    mvwprintw(highWin, 1, 3, "%d   ", config.highScore);
                    wrefresh(highWin);
                }

                if (!pairFits(PuyoPair{ spawnX, spawnY, 2, Puyo::RED, Puyo::RED }))
                {
                    mvwprintw(debugWin, 8, 1, "GAME OVER - press q");
                    wrefresh(debugWin);
                    gameOver = true;
                    continue;
                }

                piece = PuyoPair{ spawnX, spawnY, 2, randomColor(), randomColor() };
                placePair(piece);
                board.draw();
                sendBoardState();
            }
            else
            {
                board.draw();
            }
        }
    }

    delete opponentBoard;
}
