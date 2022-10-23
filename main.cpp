#include "mfind.hpp"
#include <bits/types/struct_timespec.h>
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <ctpl/ctpl_stl.h>
#include <iostream>
#include <string>
#include <sys/time.h>
#include <vector>

timespec ts;
timespec old_ts = {0};

long getTimeDiff() {
  clock_gettime(CLOCK_REALTIME, &ts);

  long diff = 0;
  if (old_ts.tv_sec) {
    diff = ts.tv_nsec - old_ts.tv_nsec;
  }

  return diff;
}

void setTime() { clock_gettime(CLOCK_REALTIME, &old_ts); }

void printOut(int id) {
  int ibuffer = 0;
  int buffer_last = 0;

  while (true) {
    int buffers = mfind_buffer.size();

    if (ibuffer < buffers) {
      if (buffers) {
        std::cout << mfind_buffer[ibuffer++];
      }
    }

    if (outputmutex.try_lock()) {
      mfind_buffer.erase(mfind_buffer.begin(), mfind_buffer.begin() + ibuffer);
      outputmutex.unlock();
      ibuffer = 0;
    }

    if (buffers) {
      setTime();
    }

    if (!buffers) {
      if (getTimeDiff() > 300000000) {
        std::exit(0);
      }
    }

    buffer_last = buffers;
  }
}
int main(int argc, char **argv) {
  std::string directory;
  if (argc > 1) {
    directory = argv[1];
  } else {
    directory = ".";
  }
  startFilesearch(directory);
  // mfind_tpool.push(printOut);
  return 0;
}