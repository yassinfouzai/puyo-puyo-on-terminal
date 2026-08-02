#include "../include/Config.hpp"
#include <fstream>

bool Config::load(const std::string& path)
{
    std::ifstream in(path);
    if (!in.is_open())
        return false;

    std::string line;
    while (std::getline(in, line))
    {
        auto eq = line.find('=');
        if (eq == std::string::npos)
            continue;

        std::string key = line.substr(0, eq);
        std::string value = line.substr(eq + 1);

        if (key == "color_mode")
            colorMode = (value == "official") ? ColorMode::OFFICIAL : ColorMode::TERMINAL;
    }
    return true;
}

void Config::save(const std::string& path) const
{
    std::ofstream out(path);
    out << "color_mode=" << (colorMode == ColorMode::OFFICIAL ? "official" : "terminal") << "\n";
}
