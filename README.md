# Build

In the source folder run:
```
mkdir build
cd build
cmake ..
make -j 4
```

## Dependencies

The project depends on the following libraries:

 * OpenCV 3
 * FFmpeg
 * Boost Program Options

# Run

```
$ mkdir out
$ ./app/netcam -i rtsp://some.cam/url -o out/
2017-08-14 22:22:43 Opened rtsp://some.cam/url
2017-08-14 22:22:49 Motion detected
2017-08-14 22:22:49 Started recording out//20170814_222249_1.mp4
2017-08-14 22:22:49 Saved image out//20170814_222249_1_1.jpg
2017-08-14 22:22:51 Saved image out//20170814_222251_1_2.jpg
2017-08-14 22:22:53 Saved image out//20170814_222253_1_3.jpg
2017-08-14 22:22:56 Saved image out//20170814_222256_1_4.jpg
2017-08-14 22:22:57 Saved image out//20170814_222257_1_5.jpg
2017-08-14 22:23:00 Stopped recording
```
