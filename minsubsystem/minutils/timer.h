#pragma once
#ifndef MINUTILS_TIMER_H_INCLUDED
#define MINUTILS_TIMER_H_INCLUDED

#if defined _WINDOWS
  #include <windows.h>
#elif defined _IOS
  #include <sys/time.h>
#endif // _WINDOWS


/**
 *
 */
class Timer
{
// Nested types
private:

#if defined _WINDOWS
  typedef SYSTEMTIME Time;
#elif defined _IOS
  typedef timeval Time;
#else
  struct Time
  {
    int unused;
  };
#endif

// Fields
private:
  Time m_start;

// Constructors/destructor
public:
  Timer()
  {
    Time zero = {0};
    m_start = zero;
  }

//  ~Timer();

private:
	Timer(const Timer&);

// Operators
private:
	Timer& operator = (const Timer&);

// Methods
public:
  void start()
  {
    getCurrentTime(m_start);
  }

  /**
   * Retrives time passed from start() in milliseconds
   */
  long time()
  {
    Time now = {0};
    getCurrentTime(now);
    return getTimeDiff(m_start, now);
  }

private:
  void getCurrentTime(Time& t)
  {
#if defined _WINDOWS
    GetSystemTime(&t);
#elif defined _IOS
    gettimeofday(&t, NULL);
#else
    Time zero = {0};
    t = zero;
#endif
  }

  long getTimeDiff(const Time& begin, const Time& end)
  {
    long result = 0;

#if defined _WINDOWS
    result += long(end.wHour-begin.wHour)     * 1000 /*msec*/ * 60 /*sec*/ * 60 /*min*/;
    result += long(end.wMinute-begin.wMinute) * 1000 /*msec*/ * 60 /*sec*/;
    result += long(end.wSecond-begin.wSecond) * 1000 /*msec*/;
    result += long(end.wMilliseconds-begin.wMilliseconds);
#elif defined _IOS
    Time sub = {0};
    timersub(&end, &begin, &sub);
    result += long(sub.tv_sec) * 1000 /*msec*/;
    result += long(sub.tv_usec) / 1000 /*microsec*/;
#endif

    return result;
  }
};


#endif // #ifndef MINUTILS_TIMER_H_INCLUDED
