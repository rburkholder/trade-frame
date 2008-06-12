#include "StdAfx.h"
#include "DbValueStream.h"

CDbValueStream::CDbValueStream(void) {
  setp( buf, buf + BufSize );
}

CDbValueStream::~CDbValueStream(void) {
}

int CDbValueStream::overflow(int_type meta) {
  throw std::runtime_error( "CDbValueStream overflow" );
}
