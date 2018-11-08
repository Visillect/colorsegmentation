#pragma once
#ifndef MINUTILS_LIFETIME_H_INCLUDED
#define MINUTILS_LIFETIME_H_INCLUDED

#include <set>
#include <stdexcept>

/**
 *
 */
class LifeTime
{
  friend class LifeTimeUtils;

// Nested classes
public:
  /**
   *
   */
  struct Entry
  {
    friend class LifeTime;

  protected:
    virtual ~Entry() {}
  };

// Fields
private:
  std::set<Entry*> m_entries;

// Constructors/destructor
public:
  LifeTime() {}

  ~LifeTime()
  {
    killThemAll();
  }

private:
  LifeTime(const LifeTime&);

// Operators
private:
  LifeTime& operator = (const LifeTime&);

// Methods
public:
  void put(Entry* pEntry)
  {
    if (pEntry == NULL)
      throw std::invalid_argument("LifeTime: cannot put null entry");

    m_entries.insert(pEntry);
  }

  void kill(Entry* pEntry)
  {
    if (pEntry == NULL)
      // TODO: warn and return
      throw std::invalid_argument("LifeTime: cannot kill null entry");

    std::set<Entry*>::iterator it = m_entries.find(pEntry);
    if (it == m_entries.end())
      // TODO: warn and return
      throw std::invalid_argument("LifeTime: unknown entry to drop");

    delete *it;
    m_entries.erase(it);
  }

  void killThemAll()
  {
    std::set<Entry*>::reverse_iterator it = m_entries.rbegin();
    for (; it != m_entries.rend(); it++)
      delete *it;
    m_entries.clear();
  }

  void release(Entry* pEntry)
  {
    if (pEntry == NULL)
      // TODO: warn and return
      throw std::invalid_argument("LifeTime: cannot release null entry");

    std::set<Entry*>::iterator it = m_entries.find(pEntry);
    if (it == m_entries.end())
      // TODO: warn and return
      throw std::invalid_argument("LifeTime: unknown entry to release");

    m_entries.erase(it);
  }
};


/**
 *
 */
class LifeTimeUtils
{
// Fields
private:

// Methods
public:
  static void move(LifeTime& destination, LifeTime& source)
  {
    destination.m_entries.insert(source.m_entries.begin(), source.m_entries.end());
    source.m_entries.clear();
  }
};

#endif // #ifndef MINUTILS_LIFETIME_H_INCLUDED
