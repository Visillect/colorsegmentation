#pragma once
#ifndef MINIMGIO_SRC_AVFOUNDATIONSUBSYSTEM_H_INCLUDED
#define MINIMGIO_SRC_AVFOUNDATIONSUBSYSTEM_H_INCLUDED

#include "subsystem.h"
#include "stream.h"

class AVFoundationSubSystem : public SubSystem
{
public:
  static SubSystem * Instance();
  int GetDeviceNameList(char *pDeviceNames, int size) const;
  int OpenStream(const char *pDeviceName, char *pURI, int size);

private:
  AVFoundationSubSystem();
  AVFoundationSubSystem(const AVFoundationSubSystem &);
  AVFoundationSubSystem &operator=(const AVFoundationSubSystem &);
};

#endif // #ifndef MINIMGIO_SRC_AVFOUNDATIONSUBSYSTEM_H_INCLUDED
