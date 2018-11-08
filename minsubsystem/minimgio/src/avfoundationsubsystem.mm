
#include "avfoundationsubsystem.h"
#include "avfoundationdevice.h"

#include "camstream.h"

#include <minbase/minresult.h>

AVFoundationSubSystem::AVFoundationSubSystem()
{

}

SubSystem * AVFoundationSubSystem::Instance()
{
	static AVFoundationSubSystem *pInstance = NULL;

	if(pInstance == NULL)
		pInstance = new AVFoundationSubSystem();

	return pInstance;
}

int AVFoundationSubSystem::GetDeviceNameList(char *pDeviceNames, int size) const
{
	if(pDeviceNames == NULL)
		return BAD_ARGS;

  std::string deviceName("iPhoneBackCamera\n");
  strncpy(pDeviceNames, deviceName.c_str(), size);

  return NO_ERRORS;
}

int AVFoundationSubSystem::OpenStream(const char *pDeviceName, char *pURI, int size)
{
  Device *pDevice = AVFoundationDevice::Instance(pDeviceName);
  if (pDevice == NULL)
    return INTERNAL_ERROR;

  Stream *pStream = new CamStream(pDevice);
  if (pStream == NULL)
    return INTERNAL_ERROR;

  snprintf(pURI, size, "dev://%p", pStream);
  return NO_ERRORS;
}
