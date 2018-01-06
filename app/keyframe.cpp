#include <iostream>
#include <stdio.h>
#include <boost/program_options.hpp>
#include "inc/common.h"
#include "inc/demuxer.h"
#include "inc/decoder.h"
#include "inc/packet.h"
#include "inc/recorder.h"

struct Config
{
  std::string input;
  std::string output;
  std::string output_part;
} config;

struct Statistics
{
  size_t packets_total;
  size_t video_packets;
  size_t key_frames_total;
};

bool stop = false;

void process_options(int argc, const char* argv[]);

int main(int argc, const char* argv[])
{
  try {
    process_options(argc, argv);
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    exit(-1);
  }

  init_ffmpeg();

  Demuxer demux;
  try {
    demux
    .set_option("rtsp_flags", "prefer_tcp")
    .open(config.input);
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    exit(-1);
  }
  size_t video_stream_index = demux.video_stream_index();

  Decoder decoder(demux.stream_parameters(video_stream_index));

  std::cout << date_time() << " Opened " << config.input << std::endl;

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
      continue;
    }
    if (packet.get()->flags & AV_PKT_FLAG_CORRUPT) {
      continue;
    }
    ++stats.video_packets;
    if (!(packet.get()->flags & AV_PKT_FLAG_KEY)) {
      continue;
    }
    // process key frame
    ++stats.key_frames_total;
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

    // save keyframe as a JPEG image
    try {
      save_jpeg(frame, config.output_part);
      ::rename(config.output_part.c_str(), config.output.c_str());
      std::cout << date_time() << " Saved image " << config.output << std::endl;
    } catch (const std::exception& e) {
      std::cerr << e.what() << std::endl;
    }
  }
  av_frame_free(&frame);
}

void process_options(int argc, const char* argv[])
{
  namespace po = boost::program_options;

  po::options_description desc{"Options"};
  desc.add_options()
  ("help,h", "This help message")
  ("input,i", po::value<std::string>(), "Input video stream")
  ("output,o", po::value<std::string>(), "Name of the file to save the latest I-frame to");

  po::variables_map vmap;
  po::store(parse_command_line(argc, argv, desc), vmap);
  po::notify(vmap);

  if (vmap.count("input")) {
    config.input = vmap["input"].as<std::string>();
  }
  if (vmap.count("output")) {
    config.output = vmap["output"].as<std::string>();
    config.output_part = vmap["output"].as<std::string>() + ".part";
  }
  if (vmap.count("help") || config.input.empty() || config.output.empty()) {
    std::cout << desc << std::endl;
    exit(0);
  }
}
