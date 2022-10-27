#include <bits/types/struct_timespec.h>
#include <cstdlib>
#include <ctime>
#include <ctpl/ctpl_stl.h>
#include <filesystem>
#include <iostream>
#include <mutex>
#include <regex>
#include <sstream>
#include <string>
#include <sys/time.h>
#include <vector>

extern ctpl::thread_pool mfind_tpool;
extern std::vector<std::string> excludes;
extern std::vector<std::string> mfind_buffer;
extern std::mutex outputmutex;
extern std::string mfind_search;
extern std::regex re;
extern bool mfind_cancel;

bool hasEnding(std::string const &fullString, std::string const &ending);

bool hasEndings(std::string const &fullString,
                std::vector<std::string> const &endings);

void processDirectory(int id, std::string &directory);

void startFilesearch(std::string directory, std::string search = "");