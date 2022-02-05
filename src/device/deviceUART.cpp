#include <device/deviceStruct.hpp>





bool startDeviceUART(__Device* data)
{
	__DeviceUART& dataUART = *((__DeviceUART*)data->data);

	dataUART.deviceId = open(dataUART.fileName, O_RDWR | O_NONBLOCK); // Открывает последовательный порт

	if(dataUART.deviceId < 0)
	{
		logError("Не удалось подключиться к COM-порту");
		return false;
	}

	tcflush(dataUART.deviceId, TCIOFLUSH); // Очистеть содержимое буфера

	struct termios options;

	options.c_iflag = 0;
	options.c_oflag = 0;
	options.c_cflag = CS8 | CREAD | HUPCL | CLOCAL;
	options.c_lflag = NOFLSH;

	// Установить скорость соединения (2000000 бодов в секунду)
	cfsetispeed(&options, B2000000);
	cfsetospeed(&options, B2000000);

	tcsetattr(dataUART.deviceId, TCSANOW, &options); // Применить новые настройки порта

	return true;
}

void sendDeviceUART(__Device* data, const char* d, unsigned int lenght)
{
	__DeviceUART& dataUART = *((__DeviceUART*)data->data);

	data->lockData.lock();
	if (dataUART.deviceId <= 0)
	{
		logError("Ошибка при отправке сообщения на arduino");
		data->lockData.unlock();
		return;
	}
	data->lockData.unlock();

	data->lockQWrite.lock();
	for (unsigned int i = 0; i < lenght; i++)
		data->qWrite.push(d[i]);
	data->lockQWrite.unlock();
}

bool stopDeviceUART(__Device* data)
{
	__DeviceUART& dataUART = *((__DeviceUART*)data->data);

	close(dataUART.deviceId);
	dataUART.deviceId = 0;

	return true;
}

void updateDeviceUART(__Device* data)
{
	__DeviceUART& dataUART = *((__DeviceUART*)data->data);

	data->lockQWrite.lock();

	if (data->qWrite.size())
	{
		char d = data->qWrite.front();
		data->lockQWrite.unlock();
		data->lockData.lock();

		if (write(dataUART.deviceId, &d, 1) == 1)
		{
			ioctl(dataUART.deviceId, TCSBRK, 1);
			data->lockData.unlock();
			data->lockQWrite.lock();
			data->qWrite.pop();
			data->lockQWrite.unlock();
		}
		else
		{
			data->lockData.unlock();
		}

	}
	else
	{
		data->lockQWrite.unlock();
	}

	//ioctl(dataUART.deviceId, TCSBRK, 1);


	char cData;
	data->lockData.lock();
	if (read(dataUART.deviceId, &cData, 1) != 1)
	{
		data->lockData.unlock();
		return;
	}
	data->lockData.unlock();

	data->lockQRead.lock();
	data->qRead.push(cData);
	if (data->needTime)
		data->qReadTime.push(getCTNanosecond());
	data->lockQRead.unlock();
}

DeviceUART::DeviceUART()
{
	dataUART = shared_ptr<__DeviceUART>(new __DeviceUART);
	data->data = dataUART.get();
	data->startF = startDeviceUART;
	data->stopF = stopDeviceUART;
	data->sendF = sendDeviceUART;
	data->updateF = updateDeviceUART;
}

DeviceUART::DeviceUART(const char* fileName, bool needTime) : DeviceUART()
{
	dataUART->fileName = fileName;
	data->needTime = needTime;
}

DeviceUART::~DeviceUART()
{
	if (dataUART.use_count() != 1) return;

	// Деструктор
}
