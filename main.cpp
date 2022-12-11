#include "mfind.hpp"
#include <bits/types/struct_timespec.h>
#include <cstddef>
#include <cstdlib>
#include <ctime>
//#include <ctpl/ctpl_stl.h>
#include <iostream>
#include <string>
#include <sys/time.h>
#include <vector>

#include <omp.h>

extern int tasks_created;

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

void printOut() {
  int ibuffer = 0;
  int buffer_last = 0;

  while (true) {
    int buffers = mfind_buffer.size();

    //std::cout << buffers << std::endl;
    
    if (ibuffer < buffers) {
      if (buffers) {
        std::cout << mfind_buffer[ibuffer++] << std::endl;
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
      if (tasks_created == 0) {
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

  mfind_search = ".*";

  omp_set_num_threads(8);
  
#pragma omp parallel
  {
#pragma omp single
    {
#pragma omp task
      startFilesearch(directory);
#pragma omp task
      printOut();
    }
  }
  
  return 0;
}
