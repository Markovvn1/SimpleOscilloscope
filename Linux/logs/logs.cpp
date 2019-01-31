#include "logs.hpp"

#define STORE_CURSOR        printf("\033[s")
#define RESET_CURSOR        printf("\033[u")

#define logLWait   std::cout << "[" << getLColor(LC_YELLOW) << " WAIT " << getLResetColor() << "] "
#define logLError  std::cout << "[" << getLColor(LC_RED)   << "FAILED" << getLResetColor() << "] "
#define logLGood   std::cout << "[" << getLColor(LC_GREEN) << "  OK  " << getLResetColor() << "] "




using namespace std;

string getLColor(int color, bool bright, bool background)
{
	return string("\033[") + string(bright ? "1" : "21") + string(";") + to_string((int)color + (background ? 40 : 30)) + string("m");
}


Logs::Logs(const char* name)
{
	showLogsOnDestroy = true;
	this->name = name;

	STORE_CURSOR;
	logLWait; std::cout << name << std::flush;
}

Logs::~Logs()
{
	if (showLogsOnDestroy) isError();
}



void Logs::isOk()
{
	showLogsOnDestroy = false;

	RESET_CURSOR;
	logLGood;
	std::cout << std::endl;
}

void Logs::isError()
{
	showLogsOnDestroy = false;

	RESET_CURSOR;
	logLError;
	std::cout << std::endl;
}

void Logs::set(bool value)
{
	if (value)
		isOk();
	else
		isError();
}
