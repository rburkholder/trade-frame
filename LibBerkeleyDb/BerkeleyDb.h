#pragma once

#include <db4/db_cxx.h>

#include <map>

class CBerkeleyDb {
public:
  CBerkeleyDb(void);
  ~CBerkeleyDb(void);
protected:
  Db *m_pDb;
private:
};

class CBerkeleyDbEnv {
  // a singleton class for maintaining a list of environments per database file
public:
  CBerkeleyDbEnv( void );
  ~CBerkeleyDbEnv( void );
  DbEnv *GetDbEnv( const char *szHome );
  DbEnv *GetDbEnv( void ); 
protected:
  typedef std::pair <const char *, DbEnv *> mapDbEnv_pair_t;
  static unsigned int m_RefCount;
  typedef std::map<const char *, DbEnv *> mapDbEnv_t;
  static mapDbEnv_t m_mapDbEnv;
private:
};