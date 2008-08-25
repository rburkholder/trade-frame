#pragma once

class CTimeSource {
public:
  CTimeSource(void);
  ~CTimeSource(void);
  static void Internal( void ) {};
  static void External( void ) {};
protected:
private:
};
