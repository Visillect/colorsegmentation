#ifndef MINIMGIO_SRC_AVFOUNDATIONUTILS_H_INCLUDED
#define MINIMGIO_SRC_AVFOUNDATIONUTILS_H_INCLUDED

#import <Foundation/Foundation.h>

class ScopedAutoreleasePool
{
private:
  NSAutoreleasePool *pool;
public:
  ScopedAutoreleasePool() : pool([[NSAutoreleasePool alloc] init]) {}
  ~ScopedAutoreleasePool() { [pool drain]; }
};

#endif // #ifndef MINIMGIO_SRC_AVFOUNDATIONUTILS_H_INCLUDED
