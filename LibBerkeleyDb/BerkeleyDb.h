#pragma once

// http://www.oracle.com/technology/documentation/berkeley-db/db/ref/toc.html
// http://www.oracle.com/technology/documentation/berkeley-db/db/api_cxx/frame.html

#include <db_cxx.h>

#include <map>
using namespace std;

class CBerkeleyDb {
public:
  CBerkeleyDb(void);
  virtual ~CBerkeleyDb(void);
protected:
  Db *m_pDb;
private:
};

class CBerkeleyDbEnv {
  // a singleton class for maintaining a list of environments per database file
public:
  CBerkeleyDbEnv( void );
  virtual ~CBerkeleyDbEnv( void );
  DbEnv *GetDbEnv( const char *szHome );
  DbEnv *GetDbEnv( void ); 
protected:
  typedef map<const char *, DbEnv *> td_mapDbEnv;
  static td_mapDbEnv m_mapDbEnv;
  typedef pair <const char *, DbEnv *> m_pair_mapDbEnv;
  static unsigned int m_RefCount;
private:
};