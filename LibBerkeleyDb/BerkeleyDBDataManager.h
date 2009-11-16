#pragma once

#include <db4/db_cxx.h>

// http://www.oracle.com/technology/documentation/berkeley-db/db/ref/toc.html
// http://www.oracle.com/technology/documentation/berkeley-db/db/api_cxx/frame.html

// Need to add transaction handling to the database

// m_DBEnv many need to be a pointer and placed on heap

class CBerkeleyDBDataManager {
public:

  CBerkeleyDBDataManager(void);
  ~CBerkeleyDBDataManager(void);

  DbEnv *GetDbEnv( void ) { return &m_DbEnv; }; 
  const char *GetBDBFileName( void ) { return m_BDBFileName; };

protected:

  static const char m_BDBFileName[];
  static unsigned int m_RefCount;
  static DbEnv m_DbEnv;

private:
};
