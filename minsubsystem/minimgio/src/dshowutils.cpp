/*

Copyright (c) 2011, Smart Engines Limited. All rights reserved.

All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY COPYRIGHT HOLDERS "AS IS" AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
SHALL COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of copyright holders.

*/

#pragma warning (disable : 4995)

#include "dshowutils.h"

#include <algorithm>
#include <comdef.h>
#include <cassert>
#include <string>
#include <cmath>

_COM_SMARTPTR_TYPEDEF(ICaptureGraphBuilder2, __uuidof(ICaptureGraphBuilder2));
_COM_SMARTPTR_TYPEDEF(IAMCameraControl, __uuidof(IAMCameraControl));
_COM_SMARTPTR_TYPEDEF(IAMVideoProcAmp, __uuidof(IAMVideoProcAmp));
_COM_SMARTPTR_TYPEDEF(IAMStreamConfig, __uuidof(IAMStreamConfig));
_COM_SMARTPTR_TYPEDEF(ICreateDevEnum, __uuidof(ICreateDevEnum));
_COM_SMARTPTR_TYPEDEF(IGraphBuilder, __uuidof(IFilterGraph));
_COM_SMARTPTR_TYPEDEF(IEnumMoniker, __uuidof(IEnumMoniker));
_COM_SMARTPTR_TYPEDEF(IPropertyBag, __uuidof(IPropertyBag));
_COM_SMARTPTR_TYPEDEF(IEnumPins, __uuidof(IEnumPins));
_COM_SMARTPTR_TYPEDEF(IMoniker, __uuidof(IMoniker));
_COM_SMARTPTR_TYPEDEF(IPin, __uuidof(IPin));

static void FreeMediaType(AM_MEDIA_TYPE &mt)
{
  if (mt.cbFormat != 0)
  {
    CoTaskMemFree((PVOID)mt.pbFormat);
    mt.cbFormat = 0;
    mt.pbFormat = NULL;
  }

  if (mt.pUnk != NULL)
  {
    mt.pUnk->Release();
    mt.pUnk = NULL;
  }
}

static void DeleteMediaType(AM_MEDIA_TYPE *pmt)
{
  if (pmt == NULL)
    return;

  FreeMediaType(*pmt);
  CoTaskMemFree((PVOID)pmt);
}

HRESULT GetCaptureDeviceList(std::string &deviceNameList)
{
  deviceNameList.clear();

  ICreateDevEnumPtr pDevEnum(CLSID_SystemDeviceEnum);
  if (pDevEnum == NULL)
    return E_FAIL;

  IEnumMonikerPtr pEnumMoniker = NULL;
  HRESULT hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnumMoniker, 0);
  if (hr != S_OK)
    return hr;

  pEnumMoniker->Reset();

  ULONG cFetched = 0;
  IMonikerPtr pMoniker = NULL;
  while (pEnumMoniker->Next(1, &pMoniker, &cFetched) == S_OK) 
  {
    IPropertyBagPtr pPropBag = NULL;
    HRESULT hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pPropBag);
    if (SUCCEEDED(hr)) 
    {
      _variant_t varName;
      HRESULT hr = pPropBag->Read(L"FriendlyName", &varName, 0);
      deviceNameList.append((_bstr_t)varName).append("\n");
    }
  }

  return S_OK;
}

HRESULT FindCaptureDevice(IBaseFilter *&pSourceFilter, const char *pDevName)
{
  if (pSourceFilter != NULL)
    return E_FAIL;

  ICreateDevEnumPtr pDevEnum(CLSID_SystemDeviceEnum);
  if (pDevEnum == NULL)
    return E_FAIL;

  IEnumMonikerPtr pEnumMoniker = NULL;
  HRESULT hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnumMoniker, 0);
  if (FAILED(hr))
    return hr;

  ULONG cFetched = 0;
  IMonikerPtr pMoniker = NULL;
  while (pEnumMoniker->Next(1, &pMoniker, &cFetched) == S_OK) 
  {
    IPropertyBagPtr pPropBag = NULL;
    HRESULT hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pPropBag);
    if (SUCCEEDED(hr)) 
    {
      _variant_t varName;
      HRESULT hr = pPropBag->Read(L"FriendlyName", &varName, 0);
      if (pDevName == NULL || strlen(pDevName) <= 0 || (_bstr_t)varName == _bstr_t(pDevName))
      {
        HRESULT hr = pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter, (void **)&pSourceFilter);
        if (SUCCEEDED(hr))
          pSourceFilter->AddRef();
        return S_OK;
      }
    }
  }

  return E_FAIL;
}

HRESULT GetCapturePin(IBaseFilter *pFilter, IPin *&pPin, int nPin)
{
  ICaptureGraphBuilder2Ptr pCaptureBuilder = NULL;
  HRESULT hr = pCaptureBuilder.CreateInstance(CLSID_CaptureGraphBuilder2);
  if (FAILED(hr))
    return hr;

  hr = pCaptureBuilder->FindPin(pFilter, PINDIR_OUTPUT, &PIN_CATEGORY_CAPTURE, NULL, TRUE, nPin, &pPin);
  if (FAILED(hr))
    return hr;

  return S_OK;
}

static HRESULT GetPin(IBaseFilter *pFilter, PIN_DIRECTION dirrequired, IPin *&pPin, int nPin)
{
  if (pPin != NULL)
    return E_FAIL;

  IEnumPinsPtr pEnum = NULL;
  HRESULT hr = pFilter->EnumPins(&pEnum);
  if(FAILED(hr)) 
    return hr;

  ULONG ulFound = 0;
  while(pEnum->Next(1, &pPin, &ulFound) == S_OK)
  {
    PIN_DIRECTION pindir = (PIN_DIRECTION)3;

    pPin->QueryDirection(&pindir);
    if(pindir == dirrequired)
    {
      if(nPin == 0)
        return S_OK;
      nPin--;
    } 

    pPin->Release();
    pPin = NULL;
  } 

  return E_FAIL;
}

HRESULT GetOutPin(IBaseFilter *pFilter, IPin *&pPin, int nPin)
{
  return GetPin(pFilter, PINDIR_OUTPUT, pPin, nPin);
}

HRESULT GetInPin(IBaseFilter *pFilter, IPin *&pPin, int nPin)
{
  return GetPin(pFilter, PINDIR_INPUT, pPin, nPin);
}

HRESULT GetGainCam(IBaseFilter *pSourceFilter, int *pValue, bool *pIsAuto)
{
  IAMVideoProcAmpPtr pVideoProcAmp = NULL;
  HRESULT hr = pSourceFilter->QueryInterface(IID_IAMVideoProcAmp, (void **)&pVideoProcAmp.GetInterfacePtr());
  if (FAILED(hr))
    return hr;

  long lValue = 0, lMin = 0, lMax = 0, lStep = 0, lDefault = 0, lFlag = 0;
  hr = pVideoProcAmp->GetRange(VideoProcAmp_Gain, &lMin, &lMax, &lStep, &lDefault, &lFlag);
  if (FAILED(hr))
    return hr;

  hr = pVideoProcAmp->Get(VideoProcAmp_Gain, &lValue, &lFlag);
  if (FAILED(hr))
    return hr;

  if (pValue != NULL)
    *pValue = static_cast<int>(lValue * 201 / (lMax - lMin + 1) + 0.5);
  if (pIsAuto != NULL)
    *pIsAuto = (lFlag == VideoProcAmp_Flags_Auto) ? true : false;
  return S_OK;
}

HRESULT SetGainCam(IBaseFilter *pSourceFilter, int value, bool isAuto)
{
  IAMVideoProcAmpPtr pVideoProcAmp = NULL;
  HRESULT hr = pSourceFilter->QueryInterface(IID_IAMVideoProcAmp, (void **)&pVideoProcAmp.GetInterfacePtr());
  if (FAILED(hr))
    return hr;

  long lValue = 0, lMin = 0, lMax = 0, lStep = 0, lDefault = 0, lFlag = 0;
  hr = pVideoProcAmp->GetRange(VideoProcAmp_Gain, &lMin, &lMax, &lStep, &lDefault, &lFlag);
  if (FAILED(hr))
    return hr;

  lValue = static_cast<long>(value * (lMax - lMin + 1) / 201 + 0.5);
  lFlag = isAuto ? VideoProcAmp_Flags_Auto : VideoProcAmp_Flags_Manual;
  hr = pVideoProcAmp->Set(VideoProcAmp_Gain, lValue, lFlag);
  if (FAILED(hr))
    return hr;

  return S_OK;
}

HRESULT GetExposureCam(IBaseFilter *pSourceFilter, double *pValue, bool *pIsAuto)
{
  IAMCameraControlPtr pCameraControl = NULL;
  HRESULT hr = pSourceFilter->QueryInterface(IID_IAMCameraControl, (void **)&pCameraControl.GetInterfacePtr());
  if (FAILED(hr))
    return hr;
  long lValue = 0, lFlag = 0;
  hr = pCameraControl->Get(CameraControl_Exposure, &lValue, &lFlag);
  if (FAILED(hr))
    return hr;

  if (pValue != NULL)
    *pValue = std::pow(2.0, lValue);
  if (pIsAuto != NULL)
    *pIsAuto = (lFlag == CameraControl_Flags_Auto) ? true : false;
  return S_OK;
}

HRESULT SetExposureCam(IBaseFilter *pSourceFilter, double value, bool isAuto)
{
  if (value <= 0)
    return S_FALSE;

  IAMCameraControlPtr pCameraControl = NULL;
  HRESULT hr = pSourceFilter->QueryInterface(IID_IAMCameraControl, (void **)&pCameraControl.GetInterfacePtr());
  if (FAILED(hr))
    return hr;

  long lValue = static_cast<long>(std::log(value) / std::log(2.0) + 0.5);
  long lFlag = isAuto ? CameraControl_Flags_Auto : CameraControl_Flags_Manual;
  hr = pCameraControl->Set(CameraControl_Exposure, lValue, lFlag);
  if (FAILED(hr))
    return hr;

  return S_OK;
}

HRESULT GetFocusCam(IBaseFilter *pSourceFilter, double *pValue, bool *pIsAuto)
{
  IAMCameraControlPtr pCameraControl = NULL;
  HRESULT hr = pSourceFilter->QueryInterface(IID_IAMCameraControl, (void **)&pCameraControl.GetInterfacePtr());
  if (FAILED(hr))
    return hr;

  long lMin = 0, lMax = 0, lStep = 0, lDefaut = 0, lFlags = 0;
  hr = pCameraControl->GetRange(CameraControl_Focus, &lMin, &lMax, &lStep, &lDefaut, &lFlags);
  if (FAILED(hr) || (lMax == lMin))
    return S_FALSE;

  long lValue = 0, lFlag = 0;
  hr = pCameraControl->Get(CameraControl_Focus, &lValue, &lFlag);
  if (FAILED(hr))
    return hr;

  if (pValue != NULL)
  {
    *pValue = static_cast<double>(100 * (lValue - lMin) / (lMax - lMin));
  }
  if (pIsAuto != NULL)
    *pIsAuto = (lFlag == CameraControl_Flags_Auto) ? true : false;
  return S_OK;
}

HRESULT SetFocusCam(IBaseFilter *pSourceFilter, double value, bool isAuto)
{
  if (value <= 0)
    return S_FALSE;

  IAMCameraControlPtr pCameraControl = NULL;
  HRESULT hr = pSourceFilter->QueryInterface(IID_IAMCameraControl, (void **)&pCameraControl.GetInterfacePtr());
  if (FAILED(hr))
    return hr;

  long lMin = 0, lMax = 0, lStep = 0, lDefaut = 0, lFlags = 0;
  hr = pCameraControl->GetRange(CameraControl_Focus, &lMin, &lMax, &lStep, &lDefaut, &lFlags);
  if (FAILED(hr) || (lMax == lMin))
    return S_FALSE;

  if (isAuto)
  {
    hr = pCameraControl->Set(CameraControl_Focus, lDefaut, CameraControl_Flags_Auto);
  }
  else
  {
    long lValue = static_cast<long>(value + 0.5);
    lValue = std::min(std::max(0l, lValue), 100l);
    lValue = (lMax - lMin) * lValue / 100 + lMin;
    
    long lCurrValue = 0;
    hr = pCameraControl->Get(CameraControl_Focus, &lCurrValue, &lFlags);
    if (FAILED(hr))
      return hr;
    
    while (std::abs(lCurrValue - lValue) > lStep && SUCCEEDED(hr))
    {
      lCurrValue += (lValue > lCurrValue) ? lStep : -lStep;
      hr = pCameraControl->Set(CameraControl_Focus, lCurrValue, CameraControl_Flags_Manual);
    }
  }

  return S_OK;
}

HRESULT GetFormatCam(IPin *pOutPin, int *pWidth, int *pHeight, int *pFps)
{
  IAMStreamConfigPtr pStreamConfig = NULL;
  HRESULT hr = pOutPin->QueryInterface(IID_IAMStreamConfig, (void **)&pStreamConfig.GetInterfacePtr());
  if (FAILED(hr))
    return hr;

  AM_MEDIA_TYPE *pMt = NULL;
  hr = pStreamConfig->GetFormat(&pMt);
  if (FAILED(hr))
    return hr;

  int fps = 0;
  int width = 0, height = 0;

  if (pMt->cbFormat == sizeof(VIDEOINFOHEADER))
  {
    VIDEOINFOHEADER *pVih = (VIDEOINFOHEADER *)pMt->pbFormat;
    fps = static_cast<int>(10000000.0 / pVih->AvgTimePerFrame + 0.5);
    height = std::abs(pVih->bmiHeader.biHeight);
    width = std::abs(pVih->bmiHeader.biWidth);  
  }
  else if (pMt->cbFormat == sizeof(VIDEOINFOHEADER2))
  {
    VIDEOINFOHEADER2 *pVih = (VIDEOINFOHEADER2 *)pMt->pbFormat;
    fps = static_cast<int>(10000000.0 / pVih->AvgTimePerFrame + 0.5);
    height = std::abs(pVih->bmiHeader.biHeight);
    width = std::abs(pVih->bmiHeader.biWidth);
  }
  else if (pMt->cbFormat > sizeof(VIDEOINFOHEADER))
  {
    VIDEOINFOHEADER *pVih = (VIDEOINFOHEADER *)pMt->pbFormat;
    fps = static_cast<int>(10000000.0 / pVih->AvgTimePerFrame + 0.5);
    height = std::abs(pVih->bmiHeader.biHeight);
    width = std::abs(pVih->bmiHeader.biWidth);
  }
  else
  {
    DeleteMediaType(pMt);
    return E_FAIL;
  }

  if (pWidth != NULL) *pWidth = width;
  if (pHeight != NULL)*pHeight = height;
  if (pFps != NULL)   *pFps = fps;

  DeleteMediaType(pMt);
  return S_OK;
}

HRESULT SetFormatCam(IPin *pOutPin, int width, int height, int fps)
{
  IAMStreamConfigPtr pStreamConfig = NULL;
  HRESULT hr = pOutPin->QueryInterface(IID_IAMStreamConfig, (void **)&pStreamConfig.GetInterfacePtr());
  if (FAILED(hr))
    return hr;

  int iCount = 0, iSize = 0;
  hr = pStreamConfig->GetNumberOfCapabilities(&iCount, &iSize);
  if (FAILED(hr))
    return hr;

  AM_MEDIA_TYPE *pBestMt = NULL, *pCurrMt = NULL;
  hr = pStreamConfig->GetFormat(&pBestMt);
  if (FAILED(hr))
    return hr;

  for (int i = 0; i < iCount; i++)
  {
    VIDEO_STREAM_CONFIG_CAPS vscc = {0};
    hr = pStreamConfig->GetStreamCaps(i, &pCurrMt, (BYTE *)&vscc);
    if (FAILED(hr))
    {
      DeleteMediaType(pBestMt);
      return hr;
    }

    if (pCurrMt->majortype != MEDIATYPE_Video && pCurrMt->majortype != MEDIASUBTYPE_RGB24)
    {
      DeleteMediaType(pCurrMt);
      continue;
    }

    if (pCurrMt->cbFormat == sizeof(VIDEOINFOHEADER2))
    {
      VIDEOINFOHEADER2 *pCurrVih = (VIDEOINFOHEADER2 *)pCurrMt->pbFormat;
      DeleteMediaType(pCurrMt);
      continue;    
    }

    assert(pCurrMt->cbFormat == sizeof(VIDEOINFOHEADER));
    VIDEOINFOHEADER *pCurrVih = (VIDEOINFOHEADER *)pCurrMt->pbFormat;
    VIDEOINFOHEADER *pBestVih = (VIDEOINFOHEADER *)pBestMt->pbFormat;
    BITMAPINFOHEADER &currBmp = pCurrVih->bmiHeader;
    BITMAPINFOHEADER &bestBmp = pBestVih->bmiHeader;

    int currDist = std::abs(currBmp.biWidth - width) + std::abs(std::abs(currBmp.biHeight) - height);
    int bestDist = std::abs(bestBmp.biWidth - width) + std::abs(std::abs(bestBmp.biHeight) - height);

    int currFps = static_cast<int>(10000000.0 / pCurrVih->AvgTimePerFrame + 0.5);
    int bestFps = static_cast<int>(10000000.0 / pBestVih->AvgTimePerFrame + 0.5);

    DWORD bestBitRate = pBestVih->dwBitRate;
    DWORD currBitRate = pCurrVih->dwBitRate;

    if (currDist > bestDist)
    {
      DeleteMediaType(pCurrMt);
      continue;
    }

    if (currDist < bestDist)
    {
      DeleteMediaType(pBestMt);
      pBestMt = pCurrMt;
      continue;      
    }


    if (std::abs(currFps - fps) > std::abs(bestFps - fps))
    {
      DeleteMediaType(pCurrMt);
      continue;
    }

    if (std::abs(currFps - fps) < std::abs(bestFps - fps))
    {
      DeleteMediaType(pBestMt);
      pBestMt = pCurrMt;
      continue;    
    }
   
    if (currBitRate <= bestBitRate)
    {
      DeleteMediaType(pCurrMt);
      continue;    
    }

    DeleteMediaType(pBestMt);
    pBestMt = pCurrMt;
  }

  hr = pStreamConfig->SetFormat(pBestMt);
  DeleteMediaType(pBestMt);
  return hr;

}