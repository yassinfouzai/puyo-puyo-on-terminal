#include "../include/Board.hpp"
#include <stdexcept>

Board::~Board()
{
    delwin(win);
}

Board::Board(int cols, int rows, int originY, int originX)
    : cols(cols), rows(rows)
{
    win = newwin(rows * cellH + 2, cols * cellW + 2, originY, originX);
    if (!win)
    {
        endwin();
        throw std::runtime_error("Failed to create ncurses window");
    }
    grid.resize(rows, std::vector<Puyo>(cols, Puyo::EMPTY));
}

void Board::drawCell(int x, int y)
{
    Puyo p = grid[y][x];
    char c = toChar(p);

    int pair = toColorPair(p, colorMode == ColorMode::OFFICIAL);
    if (pair != 0)
        wattron(win, COLOR_PAIR(pair));

    for (int dy = 0; dy < cellH; dy++)
    {
        for (int dx = 0; dx < cellW; dx++)
        {
            mvwaddch(win, y * cellH + 1 + dy, x * cellW + 1 + dx, c);
        }
    }

    if (pair != 0)
        wattroff(win, COLOR_PAIR(pair));
}

bool Board::isEmpty(int x, int y) const
{
    return grid[y][x] == Puyo::EMPTY;
}

bool Board::isValidPosition(int x, int y) const
{
    if (x < 0 || x >= cols || y < 0 || y >= rows)
        return false;
    return isEmpty(x, y);
}

void Board::setCell(int x, int y, Puyo puyo)
{
    if (x >= 0 && x < cols && y >= 0 && y < rows)
    {
        grid[y][x] = puyo;
    }
}

void Board::draw()
{
    werase(win);
    box(win, 0, 0);
    for (int y = 0; y < rows; y++)
    {
        for (int x = 0; x < cols; x++)
        {
            drawCell(x, y);
        }
    }
    wrefresh(win);
}

int Board::clearMatches()
{
    std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));
    int cellsCleared = 0;

    for (int y = 0; y < rows; y++)
    {
        for (int x = 0; x < cols; x++)
        {
            if (visited[y][x] || grid[y][x] == Puyo::EMPTY)
                continue;

            Puyo color = grid[y][x];
            std::vector<std::pair<int,int>> group;
            std::vector<std::pair<int,int>> stack{{x, y}};
            visited[y][x] = true;

            while (!stack.empty())
            {
                auto [cx, cy] = stack.back();
                stack.pop_back();
                group.push_back({cx, cy});

                const int dx[] = {0, 0, 1, -1};
                const int dy[] = {1, -1, 0, 0};
                for (int d = 0; d < 4; d++)
                {
                    int nx = cx + dx[d];
                    int ny = cy + dy[d];
                    if (nx >= 0 && nx < cols && ny >= 0 && ny < rows &&
                        !visited[ny][nx] && grid[ny][nx] == color)
                    {
                        visited[ny][nx] = true;
                        stack.push_back({nx, ny});
                    }
                }
            }

            if (group.size() >= 4)
            {
                for (auto& [gx, gy] : group)
                    grid[gy][gx] = Puyo::EMPTY;
                cellsCleared += group.size();
            }
        }
    }
    return cellsCleared;
}

void Board::applyGravity()
{
    for (int x = 0; x < cols; x++)
    {
        int writeY = rows - 1;
        for (int y = rows - 1; y >= 0; y--)
        {
            if (grid[y][x] != Puyo::EMPTY)
            {
                grid[writeY][x] = grid[y][x];
                if (writeY != y)
                    grid[y][x] = Puyo::EMPTY;
                writeY--;
            }
        }
    }
}

void Board::exportCells(uint8_t* out) const
{
    int idx = 0;
    for (int y = 0; y < rows; y++)
        for (int x = 0; x < cols; x++)
            out[idx++] = static_cast<uint8_t>(grid[y][x]);
}

void Board::importCells(const uint8_t* in)
{
    int idx = 0;
    for (int y = 0; y < rows; y++)
        for (int x = 0; x < cols; x++)
            grid[y][x] = static_cast<Puyo>(in[idx++]);
}
