Communications Sub-Group

Ho Jun Hsian                2102699
Alicia Lee En Qing          2102573
Khansa Zilfa Shofia Ghazali 2102820

".ino" files are compiled in Arduino with M5Stack library, for the M5StickC Plus.

Files with "demo" are for demonstrating that UART and I2C works on Pico, M5StickC and ESP01.
These files do not require the car.

Files with "test" are for testing the latency and throughput of the UART/I2C protocols.
These are used in conjunction with the Pi Pico Logic Analyzer.

LatencyThroughput.xlsx contains the processed data from the Pi Pico Logic Analyzer.

LatencyChart.png and ThroughputChart.png are the charts for data collected above.

How the tests are conducted and the logic behind the tests are in the video submission.

Resources and references used to make this possible:
https://github.com/m5stack/M5Stack
https://github.com/m5stack/M5StickC-Plus/blob/master/examples/Advanced/MQTT/MQTT.ino
https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf
https://github.com/pico-coder/sigrok-pico
https://github.com/koendv/blackmagic-bluepill
https://lucidar.me/en/serialib/most-used-baud-rates-table/
