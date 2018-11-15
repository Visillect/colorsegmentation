#pragma once

#include <memory>
#include <string>
#include <cstring>
#include <utility>
#include <minbase/minimg.h>
#include <minbase/minresult.h>
#include <minimgapi/minimgapi.h>


namespace mximg
{

class Image;
using PImage = std::shared_ptr<const Image>;

class Image
{
public:
  Image(Image const&) = delete;
  Image & operator= (Image const&) = delete;

  operator MinImg const* () const
  {
    return &img;
  }

  MinImg const* operator -> () const
  {
    return &img;
  }

  virtual ~Image()
  {
    FreeMinImage(&img);
  }

  static PImage imread(std::string const& fileName);

  bool imwrite(std::string const& fileName) const;

protected:
  MinImg img;

  Image()
  {
    memset(&img, 0, sizeof(img));
  }

  template <class TSelf>
  static std::shared_ptr<TSelf> createByPrototypeImpl(int width, int height, int channels, MinTyp element_type)
  {
    using PSelf = std::shared_ptr<TSelf>;
    MinImg prototype = { 0 };
    if (NewMinImagePrototype(&prototype, width, height, channels, element_type, 0, AO_EMPTY) < 0)
      return PSelf();
    else
      return createByPrototypeImpl<TSelf>(&prototype);
  }

  template <class TSelf>
  static std::shared_ptr<TSelf> createByPrototypeImpl(MinImg const* prototype)
  {
    using PSelf = std::shared_ptr<TSelf>;
    if (AssureMinImagePrototypeIsValid(prototype) != NO_ERRORS)
      return PSelf();

    PSelf result(new TSelf);
    if (CloneMinImagePrototype(&result->img, prototype) < 0)
      return PSelf();
    else
      return result;
  }

  template <class TSelf>
  static std::shared_ptr<TSelf> createByCopyImpl(MinImg const* toCopy)
  {
    using PSelf = std::shared_ptr<TSelf>;
    PSelf result = createByPrototypeImpl<TSelf>(toCopy);
    if (!result)
      return result;
    else if (CopyMinImage(&result->img, toCopy) < 0)
      return PSelf();
    else
      return result;
  }

  template <class TSelf>
  static std::shared_ptr<TSelf> createByOwningImpl(MinImg & toOwn)
  {
    using PSelf = std::shared_ptr<TSelf>;
    if (AssureMinImageIsValid(&toOwn) != NO_ERRORS)
      return PSelf();
    MinImg myImg = {0};
    std::swap(myImg, toOwn);
    PSelf result(new TSelf);
    result.get()->img = myImg;
    return result;
  }

  friend PImage createByOwning(MinImg & toOwn);
  friend PImage createByCopy(MinImg const* toCopy);
  friend PImage createByPrototype(MinImg const* prototype);
  friend PImage createByPrototype(int width, int height, int channels, MinTyp element_type);
};

PImage createByOwning(MinImg & toOwn);
PImage createByCopy(MinImg const* toCopy);
PImage createByPrototype(MinImg const* prototype);
PImage createByPrototype(int width, int height, int channels, MinTyp element_type);

inline bool is_empty(PImage const& img)
{
  return !img || !*img || !(*img)->pScan0;
}

} // namespace mximg
