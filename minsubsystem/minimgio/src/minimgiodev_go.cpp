#include <cstdio>
#include <cstring>
#include <sstream>

#include <minimgio/minimgio.h>
#include <minimgio/device.h>

#include <minimgapi/minimgapi.h>
#include <minimgapi/imgguard.hpp>

#include <minbase/minresult.h>

#include <chrono>
#include <string>

#include <map>

typedef std::chrono::microseconds DurType;

int main(int argc, char *argv[])
{
  char deviceName[4096] = {0};
  auto t0 = std::chrono::steady_clock::now();
  int res = GetDeviceList(DSS_CAMERA, deviceName, sizeof(deviceName));
  auto t1 = std::chrono::steady_clock::now();
  auto durlist = std::chrono::duration_cast<DurType>(t1 - t0);
  if (res < 0 || strlen(deviceName) <= 0)
  {
    printf("Cannot get camera devices or no devices installed\n");
    return -1;
  };

  std::string currDeviceName;
  std::istringstream iss(deviceName);
  std::getline(iss, currDeviceName);

  currDeviceName += "\n2048x1536";

  char deviceURI[4096] = {0};
  t0 = std::chrono::steady_clock::now();
  res = OpenStream(DSS_CAMERA, currDeviceName.c_str(), deviceURI, sizeof(deviceURI));
  t1 = std::chrono::steady_clock::now();
  auto duropen = std::chrono::duration_cast<DurType>(t1 - t0);
  if (res < 0)
  {
    printf("Cannot open new device stream\n");
    return -1;
  }

  std::map<std::string, std::string> props;
  props[SP_GAIN] = "";
  props[SP_GAINAUTO] = "";
  props[SP_EXPOSURE] = "";
  props[SP_EXPOSUREAUTO] = "";
  props[SP_FOCUS] = "";
  props[SP_FOCUSAUTO] = "";
  props[SP_FRAMESIZE] = "";
  props[SP_FRAMERATE] = "";

  for (auto& prop_value_pair : props) {
    char value[32] = {0};
    const std::string& key = prop_value_pair.first;
    res = GetStreamProperty(deviceURI, key.c_str(), value, 32);
    if (0 == res) {
      prop_value_pair.second = value;
      printf("Got property %s, value is %s\n", key.c_str(), value);
    } else if (NOT_SUPPORTED == res)
      prop_value_pair.second = "NOT_SUPPORTED";
    else {
      CloseStream(deviceURI);
      printf("Cannot get property %s, returned %d\n", key.c_str(), res);
      return -1;
    }
  }

//  if (!::strcmp(props[SP_FOCUSAUTO].c_str(), "true")) {
//    if (0 > SetStreamProperty(deviceURI, SP_FOCUSAUTO, "false"))
//      printf("Can't disable autofocus\n");
//  }
//   if (::strcmp(props[SP_FOCUS].c_str(), "NOT_SUPPORTED")) {
//     if (0 > SetStreamProperty(deviceURI, SP_FOCUS, "4"))
//       printf("Can't set focus\n");
//   }
//   if (0 > SetStreamProperty(deviceURI, SP_FRAMESIZE, "1920x1080"))
//     printf("Can't set framesize\n");
//
//   if (0 > SetStreamProperty(deviceURI, SP_FRAMERATE, "15"))
//     printf("Can't set framerate\n");

  for (auto& prop_value_pair : props) {
    char value[32] = {0};
    const std::string& key = prop_value_pair.first;
    res = GetStreamProperty(deviceURI, key.c_str(), value, 32);
    if (0 == res) {
      prop_value_pair.second = value;
      printf("Got property %s, value is %s\n", key.c_str(), value);
    } else if (NOT_SUPPORTED == res)
      prop_value_pair.second = "NOT_SUPPORTED";
    else {
      CloseStream(deviceURI);
      printf("Cannot get property %s, returned %d\n", key.c_str(), res);
      return -1;
    }
  }

  DECLARE_GUARDED_MINIMG(frameImage);
  t0 = std::chrono::steady_clock::now();
  if (GetMinImageFileProps(&frameImage, deviceURI) < 0)
  {
    CloseStream(deviceURI);
    printf("Cannot get frame properties\n");
    return -1;
  };
  t1 = std::chrono::steady_clock::now();
  auto durprops = std::chrono::duration_cast<DurType>(t1 - t0);

  if (AllocMinImage(&frameImage) < 0)
  {
    CloseStream(deviceURI);
    printf("Cannot allocate frame image\n");
    return -1;
  };

  decltype(durlist) durgrab(0);
  auto t00 = std::chrono::steady_clock::now();
  int n_images = 100;
  for (int i = 0; i < n_images; i++)
  {

    char val[32] = {0};
    ::sprintf(val, "%d", i);
    SetStreamProperty(deviceURI, SP_FOCUS, val);

    bool ok_flag = true;

    t0 = std::chrono::steady_clock::now();
    if (LoadMinImage(&frameImage, deviceURI) < 0)
    {
      printf("Cannot get frame #%d.\n", i);
      ok_flag = false;
      continue;
    };
    t1 = std::chrono::steady_clock::now();
    if (ok_flag)
    durgrab += std::chrono::duration_cast<DurType>(t1 - t0);

    char imageName[4096] = {0};
    sprintf(imageName, "%06d.jpg", i);
    if (SaveMinImage(imageName, &frameImage) < 0)
    {
      printf("Cannot save frame #%d to %s\n", i, imageName);
      continue;
    }
  }
  auto t01 = std::chrono::steady_clock::now();
  auto durbatch = std::chrono::duration_cast<DurType>(t01 - t00);

  t0 = std::chrono::steady_clock::now();
  if (CloseStream(deviceURI) < 0)
  {
    printf("Error occurred while closing device stream\n");
    return -1;
  }
  t1 = std::chrono::steady_clock::now();
  auto durclose = std::chrono::duration_cast<DurType>(t1 - t0);

  printf("Getting device names list\t\t:\t%lu\tmicroseconds.\n",
         durlist.count());
  printf("Opening stream\t\t\t:\t%lu\tmicroseconds.\n",
         duropen.count());
  printf("Getting image props\t\t\t:\t%lu\tmicroseconds.\n",
         durprops.count());
  printf("Grabbing %d images (pure grab time)\t:\t%lu\tmicroseconds.\n",
         n_images, durgrab.count());
  printf("\t\t\tThat is\t\t%.3lf microseconds per image\n",
         durgrab.count() / (n_images + 0.));
  printf("\t\t\t     or\t\t%.3lf\tFPS.\n",
         1000000 * (n_images / (durgrab.count() + 0.)));
  printf("Total handling %d images (grab & save)\t:\t%lu\tmicroseconds.\n",
         n_images, durbatch.count());
  printf("\t\t\tThat is\t\t%.3lf microseconds per image\n",
         durbatch.count() / (n_images + 0.));
  printf("\t\t\t     or\t\t%.3lf\tFPS.\n",
         1000000 * (n_images / (durbatch.count() + 0.)));
  printf("Closing stream\t\t\t:\t%lu\tmicroseconds.\n",
         durclose.count());

  return 0;
}
