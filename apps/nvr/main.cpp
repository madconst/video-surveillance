#include <iostream>
#include <boost/program_options.hpp>
#include <fmt/format.h>
#include <av/demuxer.h>
#include <av/decoder.h>
#include <av/key_frame_saver.h>
#include <cv/motion_detector.h>
#include <av/muxer.h>
#include <cv/common.h>
#include "config.h"

Config process_options(int argc, const char* argv[]);
void run(const Config& config);

int main(int argc, const char* argv[])
{
  try {
    const auto config = process_options(argc, argv);
    run(config);
  } catch (const std::exception& e) {
    std::cerr << "Fatal error: " << e.what() << std::endl;
    exit(-1);
  }
}

Config process_options(int argc, const char* argv[])
{
  Config config{};
  namespace po = boost::program_options;
  po::options_description desc{"Options"};
  desc.add_options()
          ("help,h", "This help message")
          ("input,i", po::value<std::string>(&config.input), "Input video stream")
          ("output,o",
            po::value<std::string>(&config.output_filename)->default_value("out.mp4"),
            "Output video filename")
          ("keyframe,k",
            po::value<std::string>(&config.keyframe_filename)->default_value("keyframe"),
            "Name of a file to save the latest key frame to (without extension)");

  po::variables_map options;
  po::store(parse_command_line(argc, argv, desc), options);
  po::notify(options);

  if (options.contains("help") || !options.contains("input")) {
    std::cout << desc << std::endl;
    exit(0);
  }

  return config;
}

void run(const Config& config)
{
  AV::Demuxer dmx{};

  if (config.input.starts_with("rtsp://")) {
    dmx.set_option("rtsp_flags", "prefer_tcp");
  }

  dmx.open(config.input);

  const auto video_idx = dmx.video_stream_index();
  AV::Decoder decoder{dmx.stream_parameters(  video_idx)};
  AV::KeyFrameSaver kf_saver(config.keyframe_filename);
  kf_saver.open(dmx.stream_parameters(video_idx));

  CV::MotionDetector detect{};

  AV::Muxer mux{config.output_filename};
  for (size_t i = 0; i < dmx.num_of_streams(); ++i) {
    mux.add_stream(dmx.stream_parameters(i));
  }

  const size_t NUM_PACKETS_TO_PROCESS = 500; // for testing purposes
  for (size_t i = 0; i < NUM_PACKETS_TO_PROCESS; ++i) { // TODO: handle stop signal
    auto packet = dmx.read();
    mux.write(packet);
    if (packet.stream_index() != video_idx) {
      continue;
    }
    if (packet.is_corrupt()) {
      // TODO: warning?
      continue;
    }
    if (packet.is_keyframe()) {
      decoder.put(packet);
    }
    while (decoder.has_output()) {
      auto frame = decoder.get();
      kf_saver.push(frame);
      const auto score = detect(
              CV::grayscale_from_raw_data(frame->width, frame->height, frame->data[0], frame->linesize[0]));
      fmt::print("Motion score: {}\n", score);
    }
  }
}
