/* Copyright (C) 2013 Interactive Brokers LLC. All rights reserved. This code is subject to the terms
 * and conditions of the IB API Non-Commercial License or the IB API Commercial License, as applicable. */

#ifndef ibstring_h__INCLUDED
#define ibstring_h__INCLUDED

#include <stdlib.h>

// added 2014/08/24
typedef std::string IBString;

inline bool IsEmpty(const std::string& str)
{
	return str.empty();
};

inline void Empty(std::string& str)
{
	str.erase();
};

inline bool Compare(std::string str, const char* strToCompare)
{
	return str.compare(strToCompare);
};

inline bool Compare(std::string str, std::string strToCompare)
{
	return str.compare(strToCompare);
};

inline double Atof(std::string str)
{
	return atof(str.c_str());
};

inline int Atoi(std::string str)
{
	return atoi(str.c_str());
};

#ifdef  __AFXSTR_H__

inline bool IsEmpty(const CString& str)
{
	return str.IsEmpty();
};

inline void Empty(CString& str)
{
	str.Empty();
};

inline bool Compare(CString str, const char* strToCompare)
{
	return str.CompareNoCase(strToCompare);
};

inline bool Compare(CString str, CString strToCompare)
{
	return str.CompareNoCase(strToCompare);
};

inline double Atof(CString str)
{
	return atof(str);
};

inline int Atoi(CString str)
{
	return atoi(str);
};  
#endif //  __AFXSTR_H__


#endif


