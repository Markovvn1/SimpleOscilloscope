#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <termios.h>
#include <cstring>
#include <mutex>
#include <pthread.h>
#include <queue>
//#include <unistd.h>

#include "../timer.hpp"
#include "../logs/logs.hpp"
#include "device.hpp"

using namespace std;

#define MAX_TIME_WAIT 100 // Максимальное время ожидания получения ответа

typedef bool (*StartF)(__Device* data);
typedef bool (*StopF)(__Device* data);
typedef void (*SendF)(__Device* data, const char* d, unsigned int lenght);
typedef void (*UpdateF)(__Device* data);

struct __Device
{
	mutex lockData, lockQWrite, lockQRead;
	void* data; // I2C или UART
	pthread_t deviceThread;

	bool active = false;
	bool needStop;
	bool needTime = false;

	StartF startF = NULL;
	StopF stopF = NULL;
	SendF sendF = NULL;
	UpdateF updateF = NULL;

	DeviceEventOnRead eventOnRead = NULL;

	queue<char> qRead;
	queue<char> qWrite;
	queue<long long> qReadTime;
};

struct __DeviceI2C
{
	const char* fileName;
	int addr;

	unsigned char deviceId = 0;
};

struct __DeviceUART
{
	const char* fileName;
	int deviceId = 0;
};
