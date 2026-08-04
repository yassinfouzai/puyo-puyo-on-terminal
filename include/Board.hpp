#ifndef BOARD_HPP
#define BOARD_HPP

#include "Puyo.hpp"
#include "Config.hpp"
#include <ncurses.h>
#include <vector>
#include <cstdint>

class Board
{
public:
    Board(int cols, int rows, int originY = 1, int originX = 1);
    ~Board();

    bool isEmpty(int x, int y) const;
    bool isValidPosition(int x, int y) const;
    void setCell(int x, int y, Puyo puyo);
    void draw();

    int clearMatches();
    void applyGravity();

    int cellCount() const { return cols * rows; }
    void exportCells(uint8_t* out) const;
    void importCells(const uint8_t* in);

    void setColorMode(ColorMode mode) { colorMode = mode; }

    void reposition(int originY, int originX);
    int windowWidth() const;
    int windowHeight() const;

private:
    static const int cellW = 4;
    static const int cellH = 2;

    int cols, rows;
    WINDOW* win;
    std::vector<std::vector<Puyo>> grid;
    ColorMode colorMode = ColorMode::TERMINAL;

    void drawCell(int x, int y);
};

#endif
