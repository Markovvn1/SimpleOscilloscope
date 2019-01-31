#include "deviceStruct.hpp"

void* deviceProcess(void *ptr)
{
	__Device &deviceData = *((__Device*)ptr);

	while (!deviceData.needStop)
	{
		deviceData.updateF(&deviceData);
	}

	deviceData.needStop = false;

	return NULL;
}




Device::Device()
{
	data = shared_ptr<__Device>(new __Device);

	data->deviceThread = 0;
}

Device::~Device()
{
	if (data.use_count() != 1) return;

	stop();
}

bool Device::isActive()
{
	return data->active;
}

bool Device::start(int tryTime)
{
	if (isActive()) return true;

	long long endTime = getCTMillisecond() + tryTime;

	do
	{
		data->lockData.lock();
		if (data->startF)
			data->active = data->startF(data.get());
		data->lockData.unlock();

		if (!isActive()) continue;

		data->needStop = false;
		pthread_create(&data->deviceThread, NULL, deviceProcess, data.get());

		return true;
	}
	while (tryTime == TRY_INFINITY || getCTMillisecond() < endTime);

	return false;
}

bool Device::stop()
{
	if (!isActive()) return true;

	data->needStop = true;

	while (data->needStop)
	{
		usleep(1000);
	}

	data->lockData.lock();
	data->active = !data->stopF(data.get());
	data->lockData.unlock();

	return !isActive();
}

int Device::available()
{
	data->lockData.lock();
	int res = data->qRead.size();
	data->lockData.unlock();

	return res;
}

char Device::read()
{
	if (!available()) return 0;

	data->lockData.lock();

	char res = data->qRead.front();
	data->qRead.pop();
	if (data->needTime) data->qReadTime.pop();

	data->lockData.unlock();

	return res;
}

long long Device::getReadTime()
{
	if (!available()) return 0;
	if (!data->needTime) return 0;
	data->lockData.lock();
	long long res = data->qReadTime.front();
	data->lockData.unlock();
	return res;
}

void Device::write(const char* d, unsigned int lenght)
{
	data->lockData.lock();
	data->sendF(data.get(), d, lenght);
	data->lockData.unlock();
}

void Device::setOnRead(DeviceEventOnRead eventOnRead)
{
	data->lockData.lock();
	data->eventOnRead = eventOnRead;
	data->lockData.unlock();
}
