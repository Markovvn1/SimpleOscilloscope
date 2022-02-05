#include <device/deviceStruct.hpp>





bool startDeviceFake(__Device* data)
{
	return true;
}

void sendDeviceFake(__Device* data, const char* d, unsigned int lenght)
{

}

bool stopDeviceFake(__Device* data)
{
	return true;
}

void updateDeviceFake(__Device* data)
{
	__DeviceFake& dataFake = *((__DeviceFake*)data->data);

	usleep(10000);

	data->lockQRead.lock();
	data->qRead.push(rand() % 255);
	if (data->needTime)
		data->qReadTime.push(getCTNanosecond());
	data->lockQRead.unlock();
}

DeviceFake::DeviceFake()
{
	dataFake = shared_ptr<__DeviceFake>(new __DeviceFake);
	data->data = dataFake.get();
	data->startF = startDeviceFake;
	data->stopF = stopDeviceFake;
	data->sendF = sendDeviceFake;
	data->updateF = updateDeviceFake;

	data->needTime = true;
}

DeviceFake::~DeviceFake()
{
	if (dataFake.use_count() != 1) return;

	// Деструктор
}
