# SimpleOscilloscope
Простой 6-канальный цифровой логический осциллограф на основе Arduino. Частота около 1.5 МГц

## Установка
На arduino (желательно UNO) залить скетч из директории Arduino.

На компьютер под управлением Linux установить OpenCV, после чего самомстоятельно собрать исходник (из дериктории Linux). Либо, если у вас Ubuntu 16.04 64bit, можно после установки OpenCV запустить бинарник Linux/bin/SimpleOscilloscope_Ubuntu_16.04_64

## Запуск
После загрузки скетча на Arduino необходимо подключить его к компьютеру и запустить основную программу (например, SimpleOscilloscope_Ubuntu_16.04_64). При необходимости можно первым параметром передать tty (по умолчанию /dev/ttyUSB0).

## Управление
Клавиша пробел позваляет остановить/продолжить работу программы
Ползунок "Масштаб" позволяет масштабировать в диапазоне от 10 секунд до 10 мкс на все окно. Колесико мыши в окне "AnalizeScale" позволяет перемещаться вправо/влево.
Нажалие ЛКМ в окне "Analize" позволяет быстро перемещаться

## Модификации
При необходимости бОльшего числа каналов (легко увеличить до 8 каналов) можно немного изменить скетч на Arduino и подправить константу CHANNEL_COUNT в main.cpp, после чего пересобрать программу.

<div align="center">
  <img src="http://vova.tom.ru/github/SimpleOscilloscope/Analize.png"><br>
  <img src="http://vova.tom.ru/github/SimpleOscilloscope/AnalizeScale.png"><br>
</div>
