#include <mximg/image.h>
#include <minimgio/minimgio.h>


namespace mximg {

PImage Image::imread(std::string const& fileName)
{
  MinImg img = {0};
  if (GetMinImageFileProps(&img, fileName.c_str()) < 0)
    return PImage();
  if (AllocMinImage(&img) < 0)
    return PImage();
  if (LoadMinImage(&img, fileName.c_str()) < 0)
    return PImage();
  return createByOwning(img);
}

bool Image::imwrite(std::string const& fileName) const
{
  if (!img.pScan0)
    return false;
  if (SaveMinImage(fileName.c_str(), &img) < 0)
    return false;
  return true;
}

PImage createByCopy(MinImg const* toCopy)
{
  return Image::createByCopyImpl<Image>(toCopy);
}

PImage createByOwning(MinImg & toOwn)
{
  return Image::createByOwningImpl<Image>(toOwn);
}

PImage createByPrototype(MinImg const* prototype)
{
  return Image::createByPrototypeImpl<Image>(prototype);
}

PImage createByPrototype(int width, int height, int channels, MinTyp element_type)
{
  return Image::createByPrototypeImpl<Image>(width, height, channels, element_type);
}


} // ns mximg
