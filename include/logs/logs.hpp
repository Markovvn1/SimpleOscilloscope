#pragma once

#include <string>
#include <iostream>


enum LCColor
{
	LC_BLACK = 0,
	LC_RED = 1,
	LC_GREEN = 2,
	LC_YELLOW = 3,
	LC_BLUE = 4,
	LC_MAGENTA = 5,
	LC_CYAN = 6,
	LC_WHITE = 7
};

/*
             foreground background
    black        30         40
    red          31         41
    green        32         42
    yellow       33         43
    blue         34         44
    magenta      35         45
    cyan         36         46
    white        37         47

    reset             0  (everything back to normal)
    bold/bright       1  (often a brighter shade of the same colour)
    underline         4
    inverse           7  (swap foreground and background colours)
    bold/bright off  21
    underline off    24
    inverse off      27

*/

std::string getLColor(int color, bool bright = false, bool background = false);

#define getLResetColor() "\033[0m"
#define logNone(msg)    std::cout                          << "  [L]: " << msg              << std::endl
#define logWait(msg)    std::cout << getLColor(LC_YELLOW)  << "  [L]: " << msg << getLResetColor() << std::endl
#define logWarning(msg) std::cout << getLColor(LC_MAGENTA) << "  [W]: " << msg << getLResetColor() << std::endl
#define logFocus(msg)   std::cout << getLColor(LC_BLUE)    << "  [M]: " << msg << getLResetColor() << std::endl

#define logError(msg)   std::cout << getLColor(LC_RED)     << "  [E]: " << msg << getLResetColor() << std::endl
#define logGood(msg)    std::cout << getLColor(LC_GREEN)   << "  [L]: " << msg << getLResetColor() << std::endl




class Logs
{
private:
	bool showLogsOnDestroy;
	std::string name;

public:
	Logs(const char* name);
	~Logs();

	void isOk();
	void isError();

	void set(bool value);
};

