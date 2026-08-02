#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>

enum class ColorMode { TERMINAL, OFFICIAL };

struct Config
{
    ColorMode colorMode = ColorMode::TERMINAL;
    int highScore = 0;

    bool load(const std::string& path);
    void save(const std::string& path) const;
};

#endif
