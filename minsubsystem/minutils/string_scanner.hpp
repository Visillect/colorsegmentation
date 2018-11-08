#pragma once
#ifndef MINUTILS_STRING_SCANNER_HPP_INCLUDED
#define MINUTILS_STRING_SCANNER_HPP_INCLUDED

#include <cstring>
#include <cstdio>

class StringScanner {
 public:
  StringScanner(): nextToken_(NULL), savePtr_(NULL) {
    strcpy(delimeters_, " \n\t\r");
  }

  void init(char *stringToParse) {
    nextToken_ = strtok_r(stringToParse, delimeters_, &savePtr_);
  }

  bool good() const {  return nextToken_ != NULL; }

  int setDelimeters(const char *delim) {
    int len = strlen(delim);
    if (len >= 256)
      return -1;

    strcpy(delimeters_, delim);
    return 0;
  }

  int nextInt() {
    int value = 0;
    if (nextToken_ != NULL)
    {
      sscanf(nextToken_, "%d", &value);
      skipToken();
    }
    return value;
  }

  float nextFloat() {
    float value = 0;
    if (nextToken_ != NULL)
    {
      sscanf(nextToken_, "%f", &value);
      skipToken();
    }
    return value;
  }

  double nextDouble() {
    double value = 0;
    if (nextToken_ != NULL)
    {
      sscanf(nextToken_, "%lf", &value);
      skipToken();
    }
    return value;
  }

  void skipToken() {
    if (nextToken_ != NULL)
      nextToken_ = strtok_r(NULL, delimeters_, &savePtr_);
  }

 private:

#if defined (ALCHEMY) || defined(_MSC_VER)
  char * strtok_r(char *s1, const char *s2, char **lasts)
  {
    char *ret;

    if (s1 == NULL)
      s1 = *lasts;
    while(*s1 && strchr(s2, *s1))
      ++s1;
    if(*s1 == '\0')
      return NULL;
    ret = s1;
    while(*s1 && !strchr(s2, *s1))
      ++s1;
    if(*s1)
      *s1++ = '\0';
    *lasts = s1;
    return ret;
  }
#endif

  char *nextToken_;
  char *savePtr_;
  char delimeters_[256];
};

#endif // #ifndef MINUTILS_STRING_PARSER_HPP_INCLUDED
