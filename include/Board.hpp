#ifndef BOARD_HPP
#define BOARD_HPP

#include <ncurses.h>
#include <vector>
#include "Puyo.hpp"

class Board
{
public:
    Board(int cols, int rows, int originY = 1, int originX = 1);
    ~Board();

    bool isEmpty(int x, int y) const;
    bool isValidPosition(int x, int y) const;
    void setCell(int x, int y, Puyo puyo);
    void draw();
    bool clearMatches();
    void applyGravity();

private:
    static const int cellW = 4;
    static const int cellH = 2;

    int cols, rows;
    WINDOW* win;
    std::vector<std::vector<Puyo>> grid;

    void drawCell(int x, int y);
};

#endif // !BOARD_HPP
