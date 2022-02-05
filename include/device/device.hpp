#pragma once

#include <memory>

#define TRY_NO 0
#define TRY_INFINITY -1

typedef short int sint;
typedef int (*DeviceEventOnRead)(unsigned int lenght);

struct __Device;
struct __DeviceI2C;
struct __DeviceUART;
struct __DeviceFake;

class Device
{
protected:
	std::shared_ptr<__Device> data;

public:
	Device();
	virtual ~Device();

	bool isActive();

	bool start(int tryTime = TRY_NO);
	bool stop();

	int available(); // Сколько байт еще не считано
	char read(); // Считать один байт
	long long getReadTime(); // Время считывания в ns следующего сообщения

	void write(const char* d, unsigned int lenght);

	void setOnRead(DeviceEventOnRead eventOnRead);
};




class DeviceUART : public Device
{
protected:
	std::shared_ptr<__DeviceUART> dataUART;

public:
	DeviceUART();
	DeviceUART(const char* fileName, bool needTime = false);
	~DeviceUART();
};

class DeviceFake : public Device
{
protected:
	std::shared_ptr<__DeviceFake> dataFake;

public:
	DeviceFake();
	~DeviceFake();
};
