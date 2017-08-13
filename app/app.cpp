#include <iostream>
#include <queue>
#include <ctime>
extern "C" {
  #include <libavformat/avio.h>
  #include <libavdevice/avdevice.h>
  #include <libavcodec/avcodec.h>
  #include <libavutil/imgutils.h>
}
#include "opencv2/opencv.hpp"
#include "inc/common.h"
#include "inc/demuxer.h"
#include "inc/decoder.h"
#include "inc/motion_detector.h"
#include "inc/packet.h"
#include "inc/recorder.h"

const std::string url = "rtsp://some.cam/url"
const std::string out_directory = "out";
const std::string file_prefix = "motion_";
const std::string file_format = "ts";

bool stop = false;

std::string get_date_time()
{
  // Get current date/time, format is YYYY-MM-DD.HH:mm:ss
  time_t     now = time(0);
  struct tm  tstruct;
  char       buf[80];
  tstruct = *localtime(&now);
  strftime(buf, sizeof(buf), "%Y-%m-%d_%H-%M-%S", &tstruct);
  return buf;
}

cv::Mat av2cv(AVFrame *frame)
{
  return cv::Mat(
    frame->height, frame->width,
    CV_8UC1, // grayscale
    frame->data[0],
    frame->linesize[0]);
}

struct Statistics
{
  size_t packets_total;
  size_t video_packets;
  size_t key_frames;
  size_t chunks;
};

int main()
{
  avdevice_register_all();
  avcodec_register_all();
  av_register_all();
  avformat_network_init();

  Demuxer demux;
  try {
    demux
    .set_option("rtsp_flags", "prefer_tcp")
    .set_option("rtsp_flags", "prefer_tcp")
    .open(url);
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    exit(-1);
  }

  MotionDetector detect;

  size_t video_stream_index = demux.video_stream_index();
  Decoder decoder(demux.stream_parameters(video_stream_index));

  Recorder recorder;
  for (size_t i = 0; i < demux.num_of_streams(); ++i) {
    recorder.add_stream(demux.stream_parameters(i));
  }

  Statistics stats{};

  AVFrame* frame = av_frame_alloc();
  while (!stop) {
    Packet packet;
    try {
      packet = demux.read();
    } catch (const std::exception& e) {
      std::cerr << e.what() << std::endl;
      break; // exit
    }
    ++stats.packets_total;
    if (packet.get()->stream_index != video_stream_index) {
      recorder.push(std::move(packet));
      continue;
    }
    if (packet.get()->flags & AV_PKT_FLAG_CORRUPT) {
      recorder.push(std::move(packet)); // pass through corrupt packets?
      continue;
    }
    ++stats.video_packets;
    if (!(packet.get()->flags & AV_PKT_FLAG_KEY)) {
      recorder.push(std::move(packet)); // pass through non-key video frames
      continue;
    }
    // process key frame
    ++stats.key_frames;
    int result = decoder.put(packet);
    if (result) { // AVERROR(EAGAIN), AVERROR(EINVAL), AVERROR(ENOMEM): https://ffmpeg.org/doxygen/3.2/group__lavc__decoding.html
      std::cerr << "Failed to put packet to decoder: " << av_error(result) << std::endl;
      continue;
    }
    av_frame_unref(frame);
    result = decoder.get(frame);
    if (result) { // AVERROR(EAGAIN), AVERROR_EOF, AVERROR(EINVAL)
      std::cerr << "Failed to get frame from decoder: " << av_error(result) << std::endl;
      continue;
    }
    recorder.flush(); // empty recorder on every new key frame
    if (detect(av2cv(frame))) {
      // motion detected
      if (!recorder.recording()) {
        ++stats.chunks;
        std::string filename = out_directory + "/"
            + file_prefix + std::to_string(stats.chunks)
            + "." + file_format;
        recorder.start_recording(filename);
        std::cout << "Recording " << filename << std::endl;
        std::cout.flush();
      }
    } else {
      if (recorder.recording()) {
        recorder.stop_recording();
        std::cout << " done" << std::endl;
      }
    }
    recorder.push(std::move(packet));
  }
  av_frame_free(&frame);
}

/*
no motion, no key => add packet to queue
no motion, key => flush queue
motion, no key => write
motion, key => flush buffer, create muxer, write

I P P .... I P P .... I P P P ... I P P P ...
^          ^         ^           |
|        motion detected         |
|                                |
|-----------recorded-------------|
*/
