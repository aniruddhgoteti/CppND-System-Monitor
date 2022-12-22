#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// helper function which parses stats of each process
// and returns them as a vector
vector<string> LinuxParser::GetStats(int pid) {
  string line;
  string stats;
  vector<string> stats_vector;

  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);

    while (linestream >> stats) {
      stats_vector.emplace_back(stats);
    }
  }
  return stats_vector;
}

float LinuxParser::MemoryUtilization() {
  string line;
  string key;
  string value;

  float memtotal;
  float memfree;
  float buffers;

  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "MemTotal:") {
        memtotal = std::stof(value);
      } else if (key == "MemFree:") {
        memfree = std::stof(value);
      } else if (key == "Buffers:") {
        buffers = std::stof(value);
      }
    }
  }

  auto total_used_mem = memtotal - (memfree + buffers);
  return (total_used_mem / memtotal);
}

long LinuxParser::UpTime() {
  string line;
  string time_in_sec;

  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> time_in_sec;
  }
  return std::stol(time_in_sec);
}

float LinuxParser::ActiveJiffies(int pid) {
  auto stats_vector = LinuxParser::GetStats(pid);

  auto utime = std::stof(stats_vector[13]);
  auto stime = std::stof(stats_vector[14]);
  auto cutime = std::stof(stats_vector[15]);
  auto cstime = std::stof(stats_vector[16]);

  auto total_time = utime + stime + cutime + cstime;
  return total_time;
}

vector<string> LinuxParser::CpuUtilization() {
  string line;
  string stats;
  string found_required_value;
  vector<string> cpu_measurements;

  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> stats;
      if (stats == "cpu") {
        for (int i = 0; i < 10; i++) {
          linestream >> found_required_value;
          cpu_measurements.emplace_back(found_required_value);
        }
      }
    }
  }
  return cpu_measurements;
}

// helper function to find key words while parsing
string LinuxParser::FindRequiredValueForKey(string key, string path) {
  string line;
  string stats;
  string found_required_value;

  std::ifstream stream(path);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> stats;
      if (stats == key) {
        linestream >> found_required_value;
        break;
      }
    }
  }
  return found_required_value;
}

int LinuxParser::TotalProcesses() {
  return std::stoi(LinuxParser::FindRequiredValueForKey(
      "processes", kProcDirectory + kStatFilename));
}

int LinuxParser::RunningProcesses() {
  return std::stoi(LinuxParser::FindRequiredValueForKey(
      "procs_running", kProcDirectory + kStatFilename));
}

// Processor Information Parsing
// Parse Command Line (cmdline) for a process
string LinuxParser::Command(int pid) {
  string cmdline;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, cmdline);
  }
  return cmdline;
}

string LinuxParser::Ram(int pid) {
  return std::to_string(
      std::stoi(LinuxParser::FindRequiredValueForKey(
          "VmSize:", kProcDirectory + std::to_string(pid) + kStatusFilename)) /
      1000);
}

string LinuxParser::Uid(int pid) {
  return LinuxParser::FindRequiredValueForKey(
      "Uid:", kProcDirectory + std::to_string(pid) + kStatusFilename);
}

string LinuxParser::User(int pid) {
  auto uid_ = LinuxParser::Uid(pid);

  string line, name, x, value;
  string found_required_value = "No name";

  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> name >> x >> value;
      if (value == uid_) {
        found_required_value = name;
        break;
      }
    }
  }
  return found_required_value;
}

long LinuxParser::UpTime(int pid) {
  auto stats_vector = LinuxParser::GetStats(pid);
  auto clock_ticks = std::stol(stats_vector[21]);
  auto uptime_in_seconds = clock_ticks / sysconf(_SC_CLK_TCK);
  return uptime_in_seconds;
}

float LinuxParser::ProcessorUtilization(int pid) {
  auto stats_vector = LinuxParser::GetStats(pid);
  long int uptime = LinuxParser::UpTime();
  auto hertz = sysconf(_SC_CLK_TCK);

  auto starttime = std::stof(stats_vector[21]);
  auto total_time = LinuxParser::ActiveJiffies(pid);
  float seconds = uptime - (starttime / hertz);

  return ((total_time / hertz) / seconds);
}
