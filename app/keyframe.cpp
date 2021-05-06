#include <iostream>
#include <boost/program_options.hpp>
#include "inc/common.h"
#include "inc/demuxer.h"
#include "inc/decoder.h"
#include "inc/packet.h"
#include "inc/frame.h"
#include "inc/scaler.h"

struct Config
{
  std::string input;
  std::string output;
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

  auto video_parameters = demux.stream_parameters(video_stream_index);
  Decoder decoder(video_parameters);

  std::cout << date_time() << " Opened " << config.input << std::endl;

  Statistics stats{};

  Frame frame;
  Scaler downscale;
  downscale
  .from(video_parameters->width, video_parameters->height, static_cast<AVPixelFormat>(video_parameters->format))
  .to(video_parameters->width/2, video_parameters->height/2, static_cast<AVPixelFormat>(video_parameters->format))
  .open(SWS_FAST_BILINEAR);
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

    decoder.put(std::move(packet));
    frame = decoder.get(); // key frame is always decoded instantly

    // save keyframe as a JPEG image
    Frame small = downscale(frame);
    try {
      save_jpeg(frame, config.output + ".jpg");
      save_jpeg(small, config.output + "-sm.jpg");
    } catch (const std::exception& e) {
      std::cerr << e.what() << std::endl;
    }
  }
}

void process_options(int argc, const char* argv[])
{
  namespace po = boost::program_options;

  po::options_description desc{"Options"};
  desc.add_options()
  ("help,h", "This help message")
  ("input,i", po::value<std::string>(), "Input video stream")
  ("output,o", po::value<std::string>(), "Name of the file (without extension) to save the latest I-frame to");

  po::variables_map vmap;
  po::store(parse_command_line(argc, argv, desc), vmap);
  po::notify(vmap);

  if (vmap.count("input")) {
    config.input = vmap["input"].as<std::string>();
  }
  if (vmap.count("output")) {
    config.output = vmap["output"].as<std::string>();
  }
  if (vmap.count("help") || config.input.empty() || config.output.empty()) {
    std::cout << desc << std::endl;
    exit(0);
  }
}
