#ifndef CONFIG_H
#define CONFIG_H

#include <string>

struct Config
{
  std::string input;
  std::string output_filename;
  std::string keyframe_filename;
};

#endif // CONFIG_H