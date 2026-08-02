#ifndef PUYO_HPP
#define PUYO_HPP

enum class Puyo
{
    EMPTY,
    RED,
    BLUE,
    GREEN,
    YELLOW,
    PURPLE
};

inline char toChar(Puyo puyo)
{
    switch (puyo)
    {
        case Puyo::RED:    return 'R';
        case Puyo::BLUE:   return 'B';
        case Puyo::GREEN:  return 'G';
        case Puyo::YELLOW: return 'Y';
        case Puyo::PURPLE: return 'P';
        case Puyo::EMPTY:
        default:           return '.';
    }
}

inline int toColorPair(Puyo puyo, bool useOfficial)
{
    if (useOfficial)
    {
        switch (puyo)
        {
            case Puyo::RED:    return 11;
            case Puyo::BLUE:   return 12;
            case Puyo::GREEN:  return 13;
            case Puyo::YELLOW: return 14;
            case Puyo::PURPLE: return 15;
            default:           return 0;
        }
    }
    else
    {
        switch (puyo)
        {
            case Puyo::RED:    return 1;
            case Puyo::BLUE:   return 2;
            case Puyo::GREEN:  return 3;
            case Puyo::YELLOW: return 4;
            case Puyo::PURPLE: return 5;
            default:           return 0;
        }
    }
}

struct PuyoPair
{
    int pivotX, pivotY;
    int rotation;
    Puyo color1;
    Puyo color2;

    int secondX() const
    {
        switch (rotation)
        {
            case 1:  return pivotX + 1;
            case 3:  return pivotX - 1;
            default: return pivotX;
        }
    }

    int secondY() const
    {
        switch (rotation)
        {
            case 0:  return pivotY - 1;
            case 2:  return pivotY + 1;
            default: return pivotY;
        }
    }
};

#endif
