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
std::regex re;

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

      if (std::regex_match(es, re)) {
        mfind_tpool.push(processDirectory, es);
      }
    }

    entries.push_back(es);
  }

  outputmutex.lock();
  for (auto &entry : entries) {
    mfind_buffer.push_back(entry);
  }
  outputmutex.unlock();
}

/*BROKEN FOR SOME REASON - DON'T USE*/
void startFilesearch(std::string directory, std::string search) {
  mfind_search = search;
  re = std::regex(mfind_search);
  /*mfind_tpool.stop();
  mfind_buffer = {};
  mfind_tpool.resize(std::thread::hardware_concurrency() * 4);*/
  mfind_tpool.push(processDirectory, directory);
}