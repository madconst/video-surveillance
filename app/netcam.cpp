#include <iostream>
#include <queue>
#include <boost/program_options.hpp>
#include "inc/common.h"
#include "inc/demuxer.h"
#include "inc/decoder.h"
#include "inc/motion_detector.h"
#include "inc/packet.h"
#include "inc/recorder.h"

/* TODO: move to README.md
Frame sequence:
 I P P P ... I P P P ... I P P P ... I P P P ... I P P P ... I P P P ...
             |           ^           ^         | ^
             |    diff detected  diff detected | no more diffs
             |----------- recorded ------------|

Only i-frames are analyzed:
i-frame  1:
i-frame  2: <----------------------------------------------- actual record start
i-frame  3: motion detected (diff between i-frames 3 and 2)
i-frame  4: motion present
i-frame  5: no motion -> suspend recording
i-frame  6: motion present -> resume recording
i-frame  7: motion present <--------------------------------- actual record stop
i-frame  8: no motion -> suspend recording
i-frame  9: no motion
i-frame 10: no motion
i-frame 11: no motion -> stop recording (throw away record since last suspended)
*/

struct Config
{
  std::string input;
  std::string output_directory;
  std::string file_prefix;
  std::string video_format = "mp4";
} config;

struct Statistics
{
  size_t packets_total;
  size_t video_packets;
  size_t key_frames_total;
  size_t last_motion_key_frames;
  size_t chunks;
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

  std::cout << date_time() << " Opened " << config.input << std::endl;

  MotionDetector detect;

  size_t video_stream_index = demux.video_stream_index();
  Decoder decoder(demux.stream_parameters(video_stream_index));

  Recorder recorder;
  for (size_t i = 0; i < demux. num_of_streams(); ++i) {
    recorder.add_stream(demux.stream_parameters(i));
  }

  Statistics stats{};
  auto last_state = MotionState::ABSENT;

  Frame frame;
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
    ++stats.key_frames_total;
    decoder.put(packet);
    frame = decoder.get(); // key frame is always decoded instantly
    auto state = detect(av2cv(frame.get()));
    // 3 states: present, suspended, absent
    // 6 possible transitions, 5 allowed:
    // absent->present     => start recording
    // present->suspended  => suspend recording
    // suspended->present  => resume recording
    // suspended->absent   => stop recording
    // present->absent     => stop recording
    // absent->suspended - must not occur

    if (last_state == MotionState::ABSENT && state == MotionState::PRESENT) {
      // start recording video
      stats.last_motion_key_frames = 0;
      ++stats.chunks;
      std::string filename = config.output_directory + "/"
          + config.file_prefix + date_time("%Y%m%d_%H%M%S") + "_"
          + std::to_string(stats.chunks)
          + "." + config.video_format;
      try {
        recorder.start_recording(filename);
        std::cout << date_time() << " Motion detected, started recording " << filename << std::endl;
      } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
      }
    }

    if (state == MotionState::PRESENT) {
      std::string filename = config.output_directory + "/"
      + config.file_prefix + date_time("%Y%m%d_%H%M%S") + "_"
      + std::to_string(stats.chunks) + "_"
      + std::to_string(stats.last_motion_key_frames) + ".jpg";
      // save keyframe as a JPEG image
      try {
        save_jpeg(frame, filename);
        std::cout << date_time() << " Motion present, saved image " << filename << std::endl;
      } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
      }
    }

    if (last_state == MotionState::PRESENT && state == MotionState::SUSPENDED) {
      std::cout << date_time() << " Motion suspended" << std::endl;
      try {
        recorder.suspend();
      } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
      }
    }

    if (last_state == MotionState::SUSPENDED && state == MotionState::PRESENT) {
      std::cout << date_time() << " Motion resumed" << std::endl;
      try {
        recorder.resume();
      } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
      }
    }

    if (last_state != MotionState::ABSENT && state == MotionState::ABSENT) {
      recorder.stop_recording();
      std::cout << date_time() << " No motion, stopped recording" << std::endl;
    }

    if (state == MotionState::ABSENT) {
      recorder.flush(); // empty recorder on every new key frame if no motion
    }

    last_state = state;

    recorder.push(std::move(packet));
  }
}

void process_options(int argc, const char* argv[])
{
  namespace po = boost::program_options;

  po::options_description desc{"Options"};
  desc.add_options()
  ("help,h", "This help message")
  ("input,i", po::value<std::string>(), "Input video stream")
  ("output,o", po::value<std::string>(), "Output directory")
  ("view,v", po::value<std::string>(), "Name of a file to save the latest key frame to")
  ("prefix,p", po::value<std::string>(), "File prefix");

  po::variables_map vmap;
  po::store(parse_command_line(argc, argv, desc), vmap);
  po::notify(vmap);

  if (vmap.count("input")) {
    config.input = vmap["input"].as<std::string>();
  }
  if (vmap.count("output")) {
    config.output_directory = vmap["output"].as<std::string>();
  }
  if (vmap.count("prefix")) {
    config.output_directory = vmap["prefix"].as<std::string>();
  }
  if (vmap.count("help") || config.input.empty() || config.output_directory.empty()) {
    std::cout << desc << std::endl;
    exit(0);
  }
}
