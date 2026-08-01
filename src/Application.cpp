#include "../include/Application.hpp"
#include "../include/Board.hpp"

#include <cstdio>
#include <ncurses.h>
#include <cstdlib>
#include <ctime>
#include <chrono>

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
        init_pair(1, COLOR_RED,    COLOR_RED);
        init_pair(2, COLOR_BLUE,   COLOR_BLUE);
        init_pair(3, COLOR_GREEN,  COLOR_GREEN);
        init_pair(4, COLOR_YELLOW, COLOR_YELLOW);
        //init_pair(5, COLOR_MAGENTA, COLOR_MAGENTA);
    }

    refresh(); 
    
    debugWin = newwin(10, 30, 1, 29 + 26 + 2);
    scrollok(debugWin, TRUE);
    box(debugWin, 0, 0);
    wrefresh(debugWin);
}


Application::~Application()
{
    delwin(debugWin);
    endwin();
}

Puyo Application::randomColor()
{
    switch (rand() % 4)
    {
        case 0: return Puyo::RED;
        case 1: return Puyo::BLUE;
        case 2: return Puyo::GREEN;
        case 3: return Puyo::YELLOW;
        default: return Puyo::PURPLE;
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


void Application::run()
{
    
    if (!runConnectionSetup()) return;

    werase(stdscr);
    refresh();


    srand(static_cast<unsigned>(time(nullptr)));

    Board board(6, 12, 1, 1);

    int spawnX = 3;
    int spawnY = 0;

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

    PuyoPair piece{ spawnX, spawnY, 2, randomColor(), randomColor() };
    placePair(piece);
    board.draw();

    timeout(50);


    using clock = std::chrono::steady_clock;
    auto lastDrop = clock::now();
    const auto dropInterval = std::chrono::milliseconds(600);


    
    bool gameOver = false;

    while (true)
    {
        int key = getch();


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
            wscrl(debugWin, 1);
            box(debugWin, 0, 0);
            mvwprintw(debugWin, 8, 1, "%-12s %4d", keyname(key), key);
            wrefresh(debugWin);

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
        }
        else
        {
            placePair(piece);
            if (isGravityTick)
            {
                board.applyGravity();
                board.draw();

                while (board.clearMatches())
                {
                    board.applyGravity();
                    board.draw();
                    napms(200); 
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
            }
            else
            {
                board.draw();
            }
        }
    }
}
