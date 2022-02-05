#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <device/device.hpp>
#include <logs/logs.hpp>
#include <timer.hpp>
#include <unistd.h>
#include <cmath>

using namespace std;
using namespace cv;

#define CHANNEL_COUNT 6

#define WINDOW1_HEIGHT 30
#define WINDOW1_WIDTH 860
#define WINDOW1_BORDER1 20 // Вокруг всего изображения
#define WINDOW1_BORDER2 8 // Между каналами

#define WINDOW2_HEIGHT 100
#define WINDOW2_WIDTH 560
#define WINDOW2_BORDER1 20 // Вокруг всего изображения
#define WINDOW2_BORDER2 20 // Между каналами

#define IMAGE1_WIDTH WINDOW1_WIDTH + 2 * WINDOW1_BORDER1
#define IMAGE1_HEIGHT WINDOW1_HEIGHT * CHANNEL_COUNT + WINDOW1_BORDER2 * (CHANNEL_COUNT - 1) + 2 * WINDOW1_BORDER1

#define IMAGE2_WIDTH WINDOW2_WIDTH + 2 * WINDOW2_BORDER1
#define IMAGE2_HEIGHT WINDOW2_HEIGHT * CHANNEL_COUNT + WINDOW2_BORDER2 * (CHANNEL_COUNT - 1) + 2 * WINDOW2_BORDER1

// Количество наносекунд в окне
#define MAX_SCALE 10000000000LL
#define MIN_SCALE 10000LL


#define MAX_HISTORY 30000


class Events
{
private:
	vector<unsigned char> value;
	vector<long long> time;
	int iHistory = 0;
	int maxHistory = 0;

public:
	Events() {}
	Events(int maxHistory) : value(maxHistory), time(maxHistory), maxHistory(maxHistory) {}

	void addEvent(unsigned char value, long long time)
	{
		this->value[iHistory] = value;
		this->time[iHistory] = time;
		iHistory++; iHistory = iHistory >= maxHistory ? iHistory - maxHistory : iHistory;
	}

	void draw(Mat& image, Rect rect, long long startTime, long long scaleX, Scalar color, int thickness = 1)
	{
		long double scaleTime = rect.width / (long double)(scaleX);
		long long ctime = startTime * scaleTime;

		int oldx = rect.x + rect.width - 1;
		int oldy = 0;
		bool first = true;

		for (int i = (iHistory - 1 + maxHistory) % maxHistory; i != iHistory; i--, i = i < 0 ? i + maxHistory : i)
		{
			bool exi = false;
			long long tx = ctime - time[i] * scaleTime;

			if (tx < 0) continue;

			if (tx >= rect.width)
			{
				tx = rect.width - 1;
				exi = true;
			}

			int y = rect.y + (255 - value[i]) * (rect.height - 1) / 255;
			int x = rect.x + rect.width - 1 - tx;

			if (first)
				first = false;
			else
			{
				line(image, Point(oldx, oldy), Point(oldx, y), color, thickness);
			}

			line(image, Point(oldx, y), Point(x, y), color, thickness);

			if (exi) break;

			oldy = y;
			oldx = x;
		}
	}
};

long long offset = 0;
long long scale = MAX_SCALE;
bool programStart = false;

void checkOffset()
{
	if (offset > MAX_SCALE - scale) offset = MAX_SCALE - scale;
	if (offset < 0) offset = 0;
}

void myMouseCallback1(int event, int x, int y, int flags, void* param)
{
	if (!programStart) return;

	if (event == EVENT_MOUSEMOVE && ((flags & EVENT_FLAG_LBUTTON) == EVENT_FLAG_LBUTTON))
	{
		if (x < WINDOW1_BORDER1 || x >= IMAGE1_WIDTH - WINDOW1_BORDER1) return;
		long long tx = (IMAGE1_WIDTH - WINDOW1_BORDER1 - 1 - x) * MAX_SCALE / (WINDOW1_WIDTH - 1);
		offset = tx - scale / 2;

		checkOffset();
	}
}

void myMouseCallback2(int event, int x, int y, int flags, void* param)
{
	if (!programStart) return;

	if (event == EVENT_MOUSEHWHEEL)
	{
		int value = getMouseWheelDelta(flags);
		offset += value * scale / 40;

		checkOffset();
	}
}

int main(int argc, char* argv[])
{
	string file_name = "/dev/ttyUSB0";
	if (argc == 1)
		logWarning("Первым параметром не указан файл tty* для ardunio. Использется по умолчанию /dev/ttyUSB0");
	else
		file_name = argv[1];

	// Device* device = new DeviceUART(file_name.c_str(), true);
	Device* device = new DeviceFake();
	device->start(TRY_NO);
	if (!device->isActive())
	{
		logError("Не удалось установить соединение с arduino");
		return 1;
	}

	const long long scaleB = MIN_SCALE;
	const long double scaleA = pow(MAX_SCALE / (long double)scaleB, 1.0 / 400);

	vector<Rect> rectsBig(CHANNEL_COUNT);
	vector<Rect> rectsSmall(CHANNEL_COUNT);
	vector<Events> events(CHANNEL_COUNT, Events(MAX_HISTORY));

	rectsBig[0] = Rect(WINDOW1_BORDER1, WINDOW1_BORDER1, WINDOW1_WIDTH, WINDOW1_HEIGHT);
	rectsSmall[0] = Rect(WINDOW2_BORDER1, WINDOW2_BORDER1, WINDOW2_WIDTH, WINDOW2_HEIGHT);

	for (int i = 1; i < CHANNEL_COUNT; i++)
	{
		rectsBig[i] = Rect(WINDOW1_BORDER1, rectsBig[i - 1].y + rectsBig[i - 1].height + WINDOW1_BORDER2, WINDOW1_WIDTH, WINDOW1_HEIGHT);
		rectsSmall[i] = Rect(WINDOW2_BORDER1, rectsSmall[i - 1].y + rectsSmall[i - 1].height + WINDOW2_BORDER2, WINDOW2_WIDTH, WINDOW2_HEIGHT);
	}

	namedWindow("AnalizeScale");
	namedWindow("Analize");

	setMouseCallback("Analize", myMouseCallback1, NULL);
	setMouseCallback("AnalizeScale", myMouseCallback2, NULL);

	int ts = 0; createTrackbar("Масштаб", "AnalizeScale", &ts, 400);

	long long startTime = 0;
	bool sleepMode = false;
	long long sleepModeStart = 0;
	long long offsetTime = 0;

	char lastd = 0;
	while (true)
	{
		while (device->available())
		{
			if (!programStart) startTime = device->getReadTime() - MAX_SCALE;

			long long t = device->getReadTime() - startTime - offsetTime;
			char d = device->read();

			for (int i = 0; i < CHANNEL_COUNT; i++)
			{
				bool cd = (d >> i) & 1;
				bool ld = (lastd >> i) & 1;

				if ((cd == ld) && programStart) continue;
				if (sleepMode) continue;

				events[i].addEvent(cd * 255, t);
			}

			lastd = d;

			if (!programStart) programStart = true;
		}

		if (!programStart) continue;

		Mat frameBig(IMAGE1_HEIGHT, IMAGE1_WIDTH, CV_8UC3, Scalar(162, 185, 19));
		Mat frameSmall(IMAGE2_HEIGHT, IMAGE2_WIDTH, CV_8UC3, Scalar(162, 185, 19));

		long long ctime = sleepMode ? sleepModeStart : (getCTNanosecond() - startTime - offsetTime);
		scale = scaleB * pow(scaleA, 400 - ts);
		// printf("scale: %lld\n", scale);
		checkOffset();

		rectangle(frameBig,
				Point(WINDOW1_BORDER1 + WINDOW1_WIDTH - 1 - (offset + scale) * (WINDOW1_WIDTH - 1) / MAX_SCALE, 0),
				Point(WINDOW1_BORDER1 + WINDOW1_WIDTH - 1 - offset * (WINDOW1_WIDTH - 1) / MAX_SCALE, IMAGE1_HEIGHT - 1),
				Scalar(128, 147, 5), -1);

		for (int i = 0; i < CHANNEL_COUNT; i++)
		{
			events[i].draw(frameSmall, rectsSmall[i], ctime - offset, scale, Scalar(236, 252, 199));
			events[i].draw(frameBig, rectsBig[i], ctime, MAX_SCALE, Scalar(236, 252, 199));
		}

		long long xgrid_scale = 10;  // расстояние между временными метками в ns
		for (; xgrid_scale < MAX_SCALE; xgrid_scale *= 10) {
			if (scale / xgrid_scale < 21) break;
		}

		for (int i = 0; i <= scale / xgrid_scale; i++) {
			int x = WINDOW2_BORDER1 + WINDOW2_WIDTH - WINDOW2_WIDTH * (i * xgrid_scale) / scale;
			line(frameSmall, Point(x, WINDOW2_BORDER1), Point(x, IMAGE2_HEIGHT - WINDOW2_BORDER1), Scalar(21, 138, 171, 200), 1);
		}

		imshow("Analize", frameBig);
		imshow("AnalizeScale", frameSmall);
		setWindowTitle("AnalizeScale", "AnalizeScale (" + to_string(xgrid_scale / 1000) + " µs)");
		int key = waitKey(33);

		if (key == 32)
		{
			sleepMode = !sleepMode;

			if (sleepMode)
				sleepModeStart = getCTNanosecond() - startTime - offsetTime;
			else
				offsetTime = getCTNanosecond() - startTime - sleepModeStart;
		}
	}

	device->stop();
	delete device;

	return 0;
}
