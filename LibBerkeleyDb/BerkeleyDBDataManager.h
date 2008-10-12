#pragma once

#include <db_cxx.h>

// Need to add transaction handling to the database

class CBerkeleyDBDataManager {
public:
  CBerkeleyDBDataManager(void);
  ~CBerkeleyDBDataManager(void);
public:
  DbEnv *GetDbEnv( void ) { return &m_DbEnv; }; 
  const char *GetBDBFileName( void ) { return m_BDBFileName; };
protected:
  static const char m_BDBFileName[];
  static unsigned int m_RefCount;
  static DbEnv m_DbEnv;
private:
};
