
#ifndef STOPWATCH_H_
#define STOPWATCH_H_

#if _WIN32

/* Reduces build time by omitting extra system headers */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class Stopwatch {

  public:
    Stopwatch()
      : _active(false) {
      QueryPerformanceFrequency(&_frequency);
    }
    void start() {
      _active = true;
      QueryPerformanceCounter(&_start);
    }
    void stop() {
      _active = false;
      QueryPerformanceCounter(&_end);
    }
    inline void reset() {
      stop();
      start();
    }
    /**
     * @returns the elapsed time in usec
     */
    uint64_t elapsed() {
      if( _active )
        QueryPerformanceCounter(&_end);

      LARGE_INTEGER diff = _start.QuadPart - _end.QuadPart;

      return (uint64_t)( ((double)diff.QuadPart/(double)_frequency.QuadPart * 1000000ul )
    }


  private:
    bool _active;
    LARGE_INTEGER _frequency;
    LARGE_INTEGER _start, _end;

};

#else
# include <sys/time.h>
# include <stdint.h>

class Stopwatch {

  public:
    Stopwatch()
      : _active(false) {}
    void start() {
      _active = true;
      gettimeofday( &_start, NULL );
    }
    void stop() {
      _active = false;
      gettimeofday( &_end, NULL );
    }
    inline void reset() {
      stop();
      start();
    }
    /**
     * @returns the elapsed time in usec
     */
    uint64_t elapsed() {
      if( _active )
        gettimeofday( &_end, NULL );

      return (_end.tv_sec - _start.tv_sec ) * 1000000ul
        + ( _end.tv_usec - _start.tv_usec );
    }


  private:
    bool _active;
    struct timeval _start, _end;

};

#endif

#endif /* STOPWATCH_H_ */
