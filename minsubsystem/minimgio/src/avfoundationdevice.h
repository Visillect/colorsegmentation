//#pragma once
#ifndef MINIMGIO_SRC_AVFOUNDATIONDEVICE_H_INCLUDED
#define MINIMGIO_SRC_AVFOUNDATIONDEVICE_H_INCLUDED

#include "stream.h"

#include <string>

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>

@class SampleBufferDelegateWrapper;

class AVFoundationDevice : public Device
{
public:
  static AVFoundationDevice * Instance(const char *pDeviceName);
  virtual ~AVFoundationDevice();

private:
  AVFoundationDevice(const char *pDeviceName);
  AVFoundationDevice(const AVFoundationDevice &);
  AVFoundationDevice &operator=(const AVFoundationDevice &);

public:
  int Take();
  int Release();
  int PushImage(const MinImg *pImg);
  int QueryImage(const MinImg *pImg);
  int QueryImagePropsEx(MinImg *pImg, ExtImgProps *pProps);
  int GetProperty(const char *pKey, char *pValue, int size);
  int SetProperty(const char *pKey, const char *pValue);

private:
  int counter;
  std::string deviceName;
    
  SampleBufferDelegateWrapper *pSampleBufferDelegateWrapper;
  AVCaptureSession *pCaptureSession;
  AVCaptureDeviceInput *pCaptureDeviceInput;
  AVCaptureVideoDataOutput *pCaptureVideoDataOutput;
  
private:
  int setupDevice();
  int clearDevice();
  
  AVCaptureDevice * backCamera() const;
};

@interface SampleBufferDelegateWrapper : NSObject
                <AVCaptureVideoDataOutputSampleBufferDelegate>
{
@public
  CMSampleBufferRef currentBuffer;
}

@end

#endif // #ifndef MINIMGIO_SRC_AVFOUNDATIONDEVICE_H_INCLUDED
