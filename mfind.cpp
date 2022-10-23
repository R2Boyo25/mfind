#include "mfind.hpp"
#include <string>

ctpl::thread_pool mfind_tpool(std::thread::hardware_concurrency() * 4);
std::vector<std::string> excludes = {
    "trash", ".git",  "cache",     "Cache",   "caches",       ".steam",
    "proc",  "build", "temp",      "tmp",     "node_modules", "site-packages",
    ".wine", "Steam", "Unity/Hub", "Android", ".cargo"};
std::vector<std::string> mfind_buffer = {};
std::mutex outputmutex;
std::string mfind_search;

bool hasEnding(std::string const &fullString, std::string const &ending) {
  if (fullString.length() >= ending.length()) {
    return (0 == fullString.compare(fullString.length() - ending.length(),
                                    ending.length(), ending));
  } else {
    return false;
  }
}

bool hasEndings(std::string const &fullString,
                std::vector<std::string> const &endings) {
  for (auto &ending : endings) {
    bool a = hasEnding(fullString, ending);
    if (a) {
      return a;
    }
  }
  return false;
}

void processDirectory(int id, std::string &directory) {
  std::vector<std::string> entries;
  for (auto &entry : std::filesystem::directory_iterator(directory)) {
    std::string es = entry.path().string();
    if (std::filesystem::is_directory(entry) &&
        !std::filesystem::is_symlink(entry)) {

      if (hasEndings(es, excludes)) {
        continue;
      }

      mfind_tpool.push(processDirectory, es);
    }

    entries.push_back(es);
  }

  /*std::stringstream outs;
  for (auto &entry : entries) {
    outs << entry << std::endl;
  }
  std::string out = outs.str();*/
  outputmutex.lock();
  for (auto &entry : entries) {
    mfind_buffer.push_back(entry);
  }
  outputmutex.unlock();
}

void startFilesearch(std::string directory, std::string search) {
  mfind_search = search;
  mfind_tpool.push(processDirectory, directory);
}