// Created by Yap Chun Wei
// Version 1.0 (20 April 2006)
// Version 1.1 (22 April 2006)
	// - Fixed bugs with compound files not being able to write files more than 65535 bytes.
	// - Fixed bugs with reading and writing to Excel files containing many strings.
// Version 1.2 (30 April 2006)
	// - Added operator<< to pass BasicExcelCell to an output stream.
	// - Added Print() to BasicExcelWorksheet to print the worksheet to an output stream.
	// - Change BasicExcelCell Get functions to const functions.
	// - Rename BasicExcelWorksheet functions RenameWorkSheet() to Rename().
// Version 1.3 (10 May 2006)
	// - Fixed bugs with reading from Excel files containing Asian characters.
// Version 1.4 (13 May 2006)
	// - Fixed bugs with reading and writing to Excel files containing many strings.
// Version 1.5 (15 May 2006)
	// - Remove code for ExtSST because it was causing problems with reading and writing to Excel files containing many strings.
// Version 1.6 (16 May 2006)
	// - Optimized code for reading and writing.
// Version 1.7 (22 May 2006)
	// - Fixed code to remove some warnings.
	// - Fixed bug with BasicExcelWorksheet::Cell.
	// - Fixed bug with BasicExcel::UpdateWorksheets().
// Version 1.8 (23 May 2006)
	// - Fixed bug with reading Excel files containing many unicode strings.
	// - Fixed code to remove some warnings.
	// - Fixed variable code_ duplication in BoolErr.
	// - Minor changes to BasicExcelCell:Set functions.
// Version 1.9 (24 May 2006)
	// - Changed name_ in Style from SmallString to LargeString.
	// - Fixed bug in BasicExcelCell::GetString and BasicExcelCell::GetWString.
	// - Minor changes to functions in BasicExcel and BasicExcelWorksheet which checks for unicode.
	// - Minor change to SmallString::Read.
// Version 1.10 (30 May 2006)
	// - Fixed bug with reading Excel files containing many strings.
	// - Remove memory leaks.
// Version 1.11 (2 June 2006)
	// - Fixed bug with reading and writing Excel files containing many unicode and ANSI strings.
// Version 1.12 (6 June 2006)
	// - Fixed bug with reading and writing Excel files containing many unicode and ANSI strings.
// Version 1.13 (1 August 2006)
	// - Changed BasicExcelCell::Get() so that it will get a stored double as an integer or vice versa if necessary.
	// - Changed BasicExcelCell::Get() so that it will not cause any errors if a string is empty.
	// - Changed BasicExcelCell::SetString() and BasicExcelCell::SetWString() so that it will not save an empty string.
// Version 1.14 (6 August 2006)
	// - Fixed bug with reading Excel files that contain a null string.
// Version 2.0 (September 2009, Martin Fuchs)
	// - extended to maintain font and format information when reading and writing Excel sheets
	// - XLSFormatManager, ExcelFont and CellFormat to edit fonts and formats
// Version 2.1 (04.10.2009, Martin Fuchs)
	// - fixed memory leak in XLSFormatManager::get_font_idx()
	// - define macros and constants for cell and font properties
// Version 2.2 (07.11.2009, Martin Fuchs)
	// - fixed VS2008 problem when reading sheets with formula fields
	// - added BasicExcel::Close()
	// - added CellFormat::get/set_text_props() and get/set_borderlines()
// Version 2.3 (05.01.2010, Ami Castonguay/Martin Fuchs)
	// - fixed reference counting of Formula data structs
	// - support for shared formulas
	// - support for merged cells
	// - save formatting even if cell is empty
	// - flush fstream instead of closing it followed by open to prevent races in conjunction with virus scanners
	// - enable reading of XLS files exported by MacOS Numbers.app
// Version 2.4 (24.01.2010, Long Wenbiao/Martin Fuchs)
	// - add second set_borderlines() overload
	// - add ExcelFont::set_italic(), CellFormat::set_wrapping()
	// - handle COLINFO
	// - miscellaneous fixes
// Version 2.5 (01.01.2011, Martin Fuchs)
	// - dynamically allocate memory for unexpected high row/column values
	// - Unicode overloads for Load() and SaveAs()
	// - adjust to RKValues written by OpenOffice Calc
// Version 3.0 (23.01.2011, Martin Fuchs)
	// - portability fixes to enable using the code in 64 Bit development environments
	// - in a Windows environment use the Windows API instead of the old CompoundFile class to access compound document files
	// - reduced memory consumption in BasicExcel data handling
	// - return current value string from formula cells
	// - don't preserve empty rows/columns at the end of sheets

#ifndef BASICEXCEL_HPP
#define BASICEXCEL_HPP

//MF
#if defined(_MSC_VER) && _MSC_VER<=1200 // VC++ 6.0
#pragma warning(disable: 4786)

#define LONGINT __int64
#define LONGINT_CONST(x) x
#define COMPOUNDFILE

#else	// newer Microsoft compilers

#define LONGINT long long
#define LONGINT_CONST(x) x##LL
#define COMPOUNDFILE CompoundFile::

// Ray 20130922
//#ifdef _DEBUG
//#define _ITERATOR_DEBUG_LEVEL 0	// speedup iterator operations while debugging
//#endif
#endif


//MF type definitions of the Windows Compound Binary File Format (CBF) Specification

typedef unsigned char	BYTE;	//  8 bit unsigned integer

typedef unsigned short	WORD;	// 16 bit unsigned integer
typedef short			SHORT;	// 16 bit signed integer
typedef unsigned short	USHORT;	// 16 bit unsigned integer

#ifdef _MSC_VER
typedef unsigned long	DWORD;	// 32 bit unsigned integer
typedef long			LONG;	// 32 bit signed integer
typedef unsigned long	ULONG;	// 32 bit unsigned integer
#else
typedef unsigned int	DWORD;	// 32 bit unsigned integer
typedef int				LONG;	// 32 bit signed integer
typedef unsigned int	ULONG;	// 32 bit unsigned integer
#endif

typedef short	OFFSET;
typedef ULONG	SECT;
typedef ULONG	FSINDEX;
typedef USHORT	FSOFFSET;
typedef ULONG	DFSIGNATURE;
typedef WORD	DFPROPTYPE;
typedef ULONG	CBF_SID;	// renamed SID because of ambiguity with windows header files

#ifndef GUID_DEFINED
#define GUID_DEFINED
typedef struct _GUID {
	ULONG	Data1;
	USHORT	Data2;
	USHORT	Data3;
	BYTE	Data4[8];
} GUID;
#endif
typedef GUID CLSID;	// 16 bytes

struct TIME_T { // FILETYPE
    DWORD	dwLowDateTime;
    DWORD	dwHighDateTime;
};

#define	DIFSECT		0xFFFFFFFC
#define	FATSECT		0xFFFFFFFD
#define	ENDOFCHAIN	0xFFFFFFFE
#define	FREESECT	0xFFFFFFFF

#ifndef _WIN32
enum STGTY {
	STGTY_INVALID	= 0,
	STGTY_STORAGE	= 1,
	STGTY_STREAM	= 2,
	STGTY_LOCKBYTES	= 3,
	STGTY_PROPERTY	= 4,
	STGTY_ROOT		= 5
};
#endif

enum DECOLOR {
	DE_RED			= 0,
	DE_BLACK		= 1
};


#if _MSC_VER>=1400	// VS 2005
#define _CRT_SECURE_NO_WARNINGS //MF
#define _SCL_SECURE_NO_WARNINGS //MF
#endif

#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <map>
#include <vector>
#include <string>	//MF
using namespace std;

 // get facet from locale for GCC
#ifndef _USE
#define _USE(loc, fac) use_facet<fac >(loc)
#endif

#include <assert.h> //MF

#ifndef _MSC_VER
#include <string.h>
#endif

#define UTF16
#ifdef UTF16
#define SIZEOFWCHAR_T 2
#else
#define SIZEOFWCHAR_T sizeof(wchar_t)
#endif


	//MF string conversion functions

 // Courtesy of Tom Widmer (VC++ MVP)
inline std::string narrow_string(const std::wstring& str)
{
	std::string ret;

	if (!str.empty()) {
		ret.resize(str.length());

		typedef std::ctype<wchar_t> CT;
		CT const& ct = std::_USE(std::locale(), CT);

		ct.narrow(&str[0], &*str.begin()+str.size(), '?', &ret[0]);
	}

	return ret;
}

inline std::wstring widen_string(const std::string& str)
{
	std::wstring ret;

	if (!str.empty()) {
		ret.resize(str.length());

		typedef std::ctype<wchar_t> CT;
		CT const& ct = std::_USE(std::locale(), CT);

		ct.widen(&str[0], &*str.begin()+str.size(), &ret[0]);
	}

	return ret;
}


#ifdef _WIN32

#include <objbase.h>

#pragma comment(lib, "ole32")

// MF

namespace WinCompFiles
{

enum CF_RESULT {
	INVALID_SIZE = -6,
	FILE_NOT_FOUND = -4,
	DIRECTORY_NOT_EMPTY = -3,
	DIRECTORY_NOT_FOUND = -2,
	INVALID_PATH = -1,
	SUCCESS = 1
};

struct CompoundFile
{
	CompoundFile();
	~CompoundFile();

	// Compound File functions
	bool Create(const wchar_t* filename);
	bool Open(const wchar_t* filename, ios_base::openmode mode=ios_base::in|ios_base::out);
	bool Close();
	bool IsOpen();

	// File functions
	CF_RESULT MakeFile(const wchar_t* path);
	CF_RESULT FileSize(const wchar_t* path, ULONGLONG& size);
	CF_RESULT ReadFile(const wchar_t* path, char* data, ULONG size);
	CF_RESULT ReadFile(const wchar_t* path, vector<char>&data);
	CF_RESULT WriteFile(const wchar_t* path, const char* data, ULONG size);
	CF_RESULT WriteFile(const wchar_t* path, const vector<char>&data, ULONG size);

	// ANSI char functions
	bool Create(const char* filename);
	bool Open(const char* filename, ios_base::openmode mode=ios_base::in|ios_base::out);

	CF_RESULT MakeFile(const char* path);
	CF_RESULT FileSize(const char* path, ULONGLONG& size);
	CF_RESULT ReadFile(const char* path, char* data, ULONG size);
	CF_RESULT ReadFile(const char* path, vector<char>& data);
	CF_RESULT WriteFile(const char* path, const char* data, ULONG size);
	CF_RESULT WriteFile(const char* path, const vector<char>& data, ULONG size);

private:
	IStorage*	_pStg;
};

} // namespace WinCompFiles

#endif

namespace YCompoundFiles
{

struct LittleEndian
{
#if defined(_MSC_VER) && _MSC_VER<=1200 // VC++ 6.0
	#define READWRITE(Type) \
	static void Read(const char* buffer, Type& retVal, size_t pos=0, int bytes=0)	\
	{	\
		retVal = Type(0);	\
		if (bytes == 0) bytes = sizeof(Type);	\
		for (size_t i=0; i<bytes; ++i)	\
		{	\
			retVal |= ((Type)((unsigned char)buffer[pos+i])) << 8*i;	\
		}	\
	}	\
	static void ReadString(const char* buffer, Type* str, size_t pos=0, int bytes=0)	\
	{	\
		for (size_t i=0; i<bytes; ++i) Read(buffer, str[i], pos+i*sizeof(Type));	\
	}	\
	static void Write(char* buffer, Type val, size_t pos=0, int bytes=0)	\
	{	\
		if (bytes == 0) bytes = sizeof(Type);	\
		for (size_t i=0; i<bytes; ++i)	\
		{	\
			buffer[pos+i] = (unsigned char)val; \
			val >>= 8;	\
		}	\
	}	\
	static void WriteString(char* buffer, Type* str, size_t pos=0, int bytes=0)	\
	{	\
		for (size_t i=0; i<bytes; ++i) Write(buffer, str[i], pos+i*sizeof(Type));	\
	}	\
	static void Read(const vector<char>& buffer, Type& retVal, size_t pos=0, int bytes=0)	\
	{	\
		retVal = Type(0);	\
		if (bytes == 0) bytes = sizeof(Type);	\
		for (size_t i=0; i<bytes; ++i)	\
		{	\
			retVal |= ((Type)((unsigned char)buffer[pos+i])) << 8*i;	\
		}	\
	}	\
	static void ReadString(const vector<char>& buffer, Type* str, size_t pos=0, int bytes=0)	\
	{	\
		for (size_t i=0; i<bytes; ++i) Read(buffer, str[i], pos+i*sizeof(Type));	\
	}	\
	static void Write(vector<char>& buffer, Type val, size_t pos=0, int bytes=0)	\
	{	\
		if (bytes == 0) bytes = sizeof(Type);	\
		for (size_t i=0; i<bytes; ++i)	\
		{	\
			buffer[pos+i] = (unsigned char)val; \
			val >>= 8;	\
		}	\
	}	\
	static void WriteString(vector<char>& buffer, Type* str, size_t pos=0, int bytes=0)	\
	{	\
		for (size_t i=0; i<bytes; ++i) Write(buffer, str[i], pos+i*sizeof(Type));	\
	}	\

	READWRITE(char)
	READWRITE(unsigned char)
	READWRITE(short)
	READWRITE(int)
	READWRITE(unsigned int)
	READWRITE(long)
	READWRITE(unsigned long)
	READWRITE(__int64)
	READWRITE(unsigned __int64)

	#undef READWRITE


	static void Read(const char* buffer, wchar_t& retVal, size_t pos=0, int bytes=0)
	{
		retVal = wchar_t(0);

		if (bytes == 0)
			bytes = SIZEOFWCHAR_T;

		for (int i=0; i<bytes; ++i)
			retVal |= ((wchar_t)((unsigned char)buffer[pos+i])) << 8*i;
	}

	static void ReadString(const char* buffer, wchar_t* str, size_t pos=0, int bytes=0)
	{
		for (int i=0; i<bytes; ++i)
			Read(buffer, str[i], pos+i*SIZEOFWCHAR_T);
	}

	static void Write(char* buffer, wchar_t val, size_t pos=0, int bytes=0)
	{
		if (bytes == 0)
			bytes = SIZEOFWCHAR_T;

		for (int i=0; i<bytes; ++i) {
			buffer[pos+i] = (unsigned char)val;
			val >>= 8;
		}
	}

	static void WriteString(char* buffer, wchar_t* str, size_t pos=0, int bytes=0)
	{
		for (int i=0; i<bytes; ++i)
			Write(buffer, str[i], pos+i*SIZEOFWCHAR_T);
	}

	static void Read(const vector<char>& buffer, wchar_t& retVal, size_t pos=0, int bytes=0)
	{
		retVal = wchar_t(0);
		if (bytes == 0) bytes = SIZEOFWCHAR_T;
		for (int i=0; i<bytes; ++i)
		{
			if (pos+i < buffer.size())	//MF
				retVal |= ((wchar_t)((unsigned char)buffer[pos+i])) << 8*i;
		}
	}

	static void ReadString(const vector<char>& buffer, wchar_t* str, size_t pos=0, int bytes=0)
	{
		for (int i=0; i<bytes; ++i) Read(buffer, str[i], pos+i*SIZEOFWCHAR_T);
	}

	static void Write(vector<char>& buffer, wchar_t val, size_t pos=0, int bytes=0)
	{
		if (bytes == 0)
			bytes = SIZEOFWCHAR_T;

		for (int i=0; i<bytes; ++i) {
			buffer[pos+i] = (unsigned char)val;
			val >>= 8;
		}
	}

	static void WriteString(vector<char>& buffer, wchar_t* str, size_t pos=0, int bytes=0)
	{
		for (int i=0; i<bytes; ++i)
			Write(buffer, str[i], pos+i*SIZEOFWCHAR_T);
	}
#else
	template<typename Type>
	static void Read(const char* buffer, Type& retVal, size_t pos=0, int bytes=0)
	{
		retVal = Type(0);

		if (bytes == 0)
			bytes = sizeof(Type);

		for (int i=0; i<bytes; ++i)
			retVal |= ((Type)((unsigned char)buffer[pos+i])) << 8*i;
	}

	template<typename Type>
	static void ReadString(const char* buffer, Type* str, size_t pos=0, int bytes=0)
	{
		for (int i=0; i<bytes; ++i)
			Read(buffer, str[i], pos+i*sizeof(Type));
	}

	template<typename Type>
	static void Write(char* buffer, Type val, size_t pos=0, int bytes=0)
	{
		if (bytes == 0)
			bytes = sizeof(Type);

		for (int i=0; i<bytes; ++i) {
			buffer[pos+i] = (unsigned char)val;
			val >>= 8;
		}
	}

	template<typename Type>
	static void WriteString(char* buffer, Type* str, size_t pos=0, int bytes=0)
	{
		for (int i=0; i<bytes; ++i)
			Write(buffer, str[i], pos+i*sizeof(Type));
	}

	template<typename Type>
	static void Read(const vector<char>& buffer, Type& retVal, size_t pos=0, int bytes=0)
	{
		retVal = Type(0);

		if (bytes == 0)
			bytes = sizeof(Type);

		for (int i=0; i<bytes; ++i)
			retVal |= ((Type)((unsigned char)buffer[pos+i])) << 8*i;
	}

	template<typename Type>
	static void ReadString(const vector<char>& buffer, Type* str, size_t pos=0, int bytes=0)
	{
		for (int i=0; i<bytes; ++i)
			Read(buffer, str[i], pos+i*sizeof(Type));
	}

	template<typename Type>
	static void Write(vector<char>& buffer, Type val, size_t pos=0, int bytes=0)
	{
		if (bytes == 0)
			bytes = sizeof(Type);

		for (int i=0; i<bytes; ++i) {
			buffer[pos+i] = (unsigned char)val;
			val >>= 8;
		}
	}

	template<typename Type>
	static void WriteString(vector<char>& buffer, Type* str, size_t pos=0, int bytes=0)
	{
		for (int i=0; i<bytes; ++i) Write(buffer, str[i], pos+i*sizeof(Type));
	}


	static void Read(const char* buffer, wchar_t& retVal, size_t pos=0, int bytes=0)
	{
		retVal = wchar_t(0);
		if (bytes == 0)
			bytes = SIZEOFWCHAR_T;

		for (int i=0; i<bytes; ++i)
			retVal |= ((wchar_t)((unsigned char)buffer[pos+i])) << 8*i;
	}

	static void ReadString(const char* buffer, wchar_t* str, size_t pos=0, int bytes=0)
	{
		for (int i=0; i<bytes; ++i)
			Read(buffer, str[i], pos+i*SIZEOFWCHAR_T);
	}

	static void Write(char* buffer, wchar_t val, size_t pos=0, int bytes=0)
	{
		if (bytes == 0)
			bytes = SIZEOFWCHAR_T;

		for (int i=0; i<bytes; ++i) {
			buffer[pos+i] = (unsigned char)val;
			val >>= 8;
		}
	}

	static void WriteString(char* buffer, wchar_t* str, size_t pos=0, int bytes=0)
	{
		for (int i=0; i<bytes; ++i) Write(buffer, str[i], pos+i*SIZEOFWCHAR_T);
	}

	static void Read(const vector<char>& buffer, wchar_t& retVal, size_t pos=0, int bytes=0)
	{
		retVal = wchar_t(0);

		if (bytes == 0)
			bytes = SIZEOFWCHAR_T;

		for (int i=0; i<bytes; ++i) {
			if (pos+i < (int)buffer.size()) //MF
				retVal |= ((wchar_t)((unsigned char)buffer[pos+i])) << 8*i;
		}
	}

	static void ReadString(const vector<char>& buffer, wchar_t* str, size_t pos=0, int bytes=0)
	{
		for (int i=0; i<bytes; ++i) Read(buffer, str[i], pos+i*SIZEOFWCHAR_T);
	}

	static void Write(vector<char>& buffer, wchar_t val, size_t pos=0, int bytes=0)
	{
		if (bytes == 0)
			bytes = SIZEOFWCHAR_T;

		for (int i=0; i<bytes; ++i) {
			buffer[pos+i] = (unsigned char)val;
			val >>= 8;
		}
	}

	static void WriteString(vector<char>& buffer, wchar_t* str, size_t pos=0, int bytes=0)
	{
		for (int i=0; i<bytes; ++i)
			Write(buffer, str[i], pos+i*SIZEOFWCHAR_T);
	}
#endif
};


#ifndef _WIN32

class Block
// PURPOSE: In charge of handling blocks of data from a file
{
public:
	Block();

// File handling functions
	bool Create(const wchar_t* filename);
	bool Open(const wchar_t* filename, ios_base::openmode mode=ios_base::in | ios_base::out);
	bool Close();
	bool IsOpen();

// Block handling functions
	bool Read(SECT index, char* block);
	bool Write(SECT index, const char* block);
	bool Swap(SECT index1, SECT index2);
	bool Move(SECT from, SECT to);
	bool Insert(SECT index, const char* block);
	bool Erase(SECT index);
	bool Erase(vector<SECT>& indices);

// Misc functions
	ULONG GetBlockSize() const {return blockSize_;}

	void SetBlockSize(ULONG size)
	{
		blockSize_ = size;
		indexEnd_ = fileSize_/blockSize_ + (fileSize_%blockSize_? 1: 0);
	}

protected:
	vector<char> filename_;
	ios_base::openmode mode_;
	fstream	file_;
	ULONG	blockSize_;
	SECT	indexEnd_;
	ULONG	fileSize_;
};

enum {
	DUPLICATE_PROPERTY=-6,
	NAME_TOO_LONG=-5, FILE_NOT_FOUND=-4,
	DIRECTORY_NOT_EMPTY=-3, DIRECTORY_NOT_FOUND=-2,
	INVALID_PATH=-1,
	SUCCESS=1
};

class CompoundFile
{
public:
	CompoundFile();
	~CompoundFile();

// User accessible functions
public:
	// Compound File functions
	bool Create(const wchar_t* filename);
	bool Open(const wchar_t* filename, ios_base::openmode mode=ios_base::in | ios_base::out);
	bool Close();
	bool IsOpen();

	// Directory functions
	int ChangeDirectory(const wchar_t* path);
	int MakeDirectory(const wchar_t* path);

	// File functions
	int MakeFile(const wchar_t* path);
	int FileSize(const wchar_t* path, ULONG& size);
	int ReadFile(const wchar_t* path, char* data);
	int ReadFile(const wchar_t* path, vector<char>&data);
	int WriteFile(const wchar_t* path, const char* data, ULONG size);
	int WriteFile(const wchar_t* path, const vector<char>&data, ULONG size);

	// ANSI char functions
	bool Create(const char* filename);
	bool Open(const char* filename, ios_base::openmode mode=ios_base::in | ios_base::out);
	int ChangeDirectory(const char* path);
	int MakeDirectory(const char* path);
	int MakeFile(const char* path);
	int FileSize(const char* path, ULONG& size);
	int ReadFile(const char* path, char* data);
	int ReadFile(const char* path, vector<char>& data);
	int WriteFile(const char* path, const char* data, ULONG size);
	int WriteFile(const char* path, const vector<char>& data, ULONG size);

// Protected functions and data members
protected:
	// General functions and data members
	void IncreaseLocationReferences(vector<SECT> indices);
	void DecreaseLocationReferences(vector<SECT> indices);
	void SplitPath(const wchar_t* path, wchar_t*& parentpath, wchar_t*& propertyname);
	vector<char> block_;
	Block file_;

	// Header related functions and data members
	bool LoadHeader();
	void SaveHeader();
	class Header
	{
	public:
		Header();
		void Write(char* block);
		void Read(char* block);

		LONGINT	_abSig;				// 0x0000 Magic number identifying this as a compound file system
		CLSID	_clid;				// 0x0008 class id (set with WriteClassStgm retrieved with GetClassFile/ReadClassStg)
		USHORT	_ulMinorVersion;	// 0x0018 minor version of the format: 33 is written by reference implementation
		USHORT	_uDllVersion;		// 0x001A major version of the dll/format: 3 is written by reference implementation
		USHORT	_uByteOrder;		// 0x001C 0xFFFE: indicates Intel byte ordering
		USHORT	_uSectorShift;		// 0x001E size of sectors in power-of-two, typically 9, indicating 512-byte sectors
		USHORT	_uMiniSectorShift;	// 0x0020 size of mini-sectors in power-of-two, typically 67, indicating 64-byte mini-sectors
		USHORT	_usReserved;		// 0x0022 reserved, must be zero
		ULONG	_ulReserved1;		// 0x0024 reserved, must be zero
		ULONG	_ulReserved2;		// 0x0028 reserved, must be zero
		FSINDEX	_csectFat;			// 0x002C number of SECTs in the FAT chain			/ "Number of elements in the BAT array"
		SECT	_sectDirStat;		// 0x0030 first SECT in the directory chain			/ "Block index of the first block of the property table"
		DFSIGNATURE _signature;		// 0x0034 signature used for transactioning: must be zero, not supported by reference implementation
		ULONG	_ulMiniSectorCutOff;// 0x0038 maximum size for mini-streams: typically 4096 bytes
		SECT	_sectMiniFatStart;	// 0x003C first SECT in the mini-FAT chain			/ "Block index of first big block containing the small block allocation table (SBAT)"
		FSINDEX	_csectMiniFat;		// 0x0040 number of SECTs in the mini-FAT chain		/ "Number of big blocks holding the SBAT"
		SECT	_sectDifStart;		// 0x0044 first SECT in the DIFG chain				/ "Block index of the first block in the Extended Block Allocation Table (XBAT)"
		FSINDEX	_csectDif;			// 0x0048 number of SECTs in the DIF chain			/ "Number of elements in the Extended Block Allocation Table (to be added to the BAT)"
		SECT	_sectFat[109];		// 0x004C..0x01FF the SECTs of the first 109 FAT sectors	/ "Array of block indices constituting the Block Allocation Table (BAT)"

		ULONG	bigBlockSize_;
		ULONG	smallBlockSize_;

	private:
		void Initialize();
	};
	Header header_;

	// BAT related functions and data members
	void LoadBAT();
	void SaveBAT();
	ULONG DataSize(SECT startIndex, bool isBig);
	ULONG ReadData(SECT startIndex, char* data, bool isBig);
	SECT WriteData(const char* data, ULONG size, SECT startIndex, bool isBig);
	void GetBlockIndices(SECT startIndex, vector<SECT>& indices, bool isBig);
	SECT GetFreeBlockIndex(bool isBig);
	void ExpandBATArray(bool isBig);
	void LinkBlocks(SECT from, SECT to, bool isBig);
	void FreeBlocks(vector<SECT>& indices, bool isBig);
	vector<SECT> blocksIndices_;
	vector<SECT> sblocksIndices_;

	// Properties related functions and data members
	class DirectoryEntry // struct StructuredStorageDirectoryEntry
	{
	public:
		DirectoryEntry();
		void Write(char* block);
		void Read(char* block);
		friend bool operator==(const COMPOUNDFILE DirectoryEntry& lhs, const COMPOUNDFILE DirectoryEntry& rhs)
		{
			return (!wcscmp(lhs.name_, rhs.name_));
		}
		friend bool operator< (const COMPOUNDFILE DirectoryEntry& lhs, const COMPOUNDFILE DirectoryEntry& rhs)
		{
			size_t maxLen1 = wcslen(lhs.name_);
			size_t maxLen2 = wcslen(rhs.name_);
			if (maxLen1 < maxLen2) return true;
			else if (maxLen1 > maxLen2) return false;
			else
			{
				int result = wcscmp(lhs.name_, rhs.name_);
				if (result <= 0) return true;
				else return false;
			}
		}
		friend bool operator!=(const COMPOUNDFILE DirectoryEntry& lhs, const COMPOUNDFILE DirectoryEntry& rhs) {return !(lhs == rhs);}
		friend bool operator> (const COMPOUNDFILE DirectoryEntry& lhs, const COMPOUNDFILE DirectoryEntry& rhs) {return (rhs < lhs);}
		friend bool operator<=(const COMPOUNDFILE DirectoryEntry& lhs, const COMPOUNDFILE DirectoryEntry& rhs) {return !(rhs < lhs);}
		friend bool operator>=(const COMPOUNDFILE DirectoryEntry& lhs, const COMPOUNDFILE DirectoryEntry& rhs) {return !(lhs < rhs);}

		wchar_t name_[32];				// 0x00..0x3E the element name in Unicode, padded with zeros to fill the array	/ "A unicode null-terminated uncompressed 16bit string (lblocke the high bytes) containing the name of the property"
		WORD	_cb_namesize;			// 0x40 length of the element name in characters, not bytes						/ "Number of characters in the NAME field"
		BYTE	_mse;					// 0x42 type of object: value taken from the STGTY enumeration					/ "DirectoryEntry type (directory, file, or root) Byte 1 (directory), 2 (file), or 5 (root entry)"
		BYTE	_bflags;				// 0x43 value taken form DECOLOR enumeration / "Node color"
		CBF_SID	_sidLeftSib;			// 0x44 SID of the left-sibling of this entry in the directory tree				/ "Previous property index"
		CBF_SID	_sidRightSib;			// 0x48 SID of the right-sibling of this entry in the directroy tree			/ "Next property index"
		CBF_SID	_sidChild;				// 0x4C SID of the child acting as the root of all the children of this element (if _mse=STGTY_STORAGE) / "First child property index"
		GUID	_clsId;					// 0x50 CLSID if this storage (if _mse=STGTY_STORAGE)
		DWORD	_dwUserFlags;			// 0x60 user flags of this storage (if _mse=STGTY_STORAGE)
		TIME_T	_time[2];				// 0x64 create/modify time stamps (if _mse=STGTY_STORAGE)
		SECT	_sectStart; 			// 0x74 starting SECT of the stream (if _mse=STGTY_STORAGE)						/ "Starting block of the file, used as the first block in the file and the pointer to the next block from the BAT"
		ULONG	_ulSize;				// 0x78 size of stream in bytes (if _mse=STGTY_STORAGE)							/ "Actual size of the file this property points to. (used to truncate the blocks to the real size)."
		DFPROPTYPE _dptPropType;		// 0x7C reserved for future use, must be zero
	};

	class PropertyTree
	{
	public:
		PropertyTree();
		~PropertyTree();
		PropertyTree* parent_;
		DirectoryEntry* self_;
		SECT index_;
		vector<PropertyTree*> children_;
	};
	void LoadProperties();
	void SaveProperties();
	int MakeProperty(const wchar_t* path, DirectoryEntry* property);
	PropertyTree* FindProperty(SECT index);
	PropertyTree* FindProperty(const wchar_t* path);
	PropertyTree* FindProperty(PropertyTree* parentTree, wchar_t* name);
	void InsertPropertyTree(PropertyTree* parentTree, DirectoryEntry* property, SECT index);
	void DeletePropertyTree(PropertyTree* tree);
	void UpdateChildrenIndices(PropertyTree* parentTree);
	void IncreasePropertyReferences(PropertyTree* parentTree, SECT index);
	void DecreasePropertyReferences(PropertyTree* parentTree, SECT index);
	PropertyTree* propertyTrees_;
	PropertyTree* currentDirectory_;
	vector<DirectoryEntry*> dirEntries_;
	vector<PropertyTree*> previousDirectories_;
};

#endif // _WIN32

} // namespace YCompoundFiles


 // reference counting to implement smart pointers

namespace RefCount
{

 // reference counter for SmartPtr managed objects
struct RefCnt
{
	 // On construction the reference counter is initialized with an usage count of 0.
	RefCnt()
	 :	_ref_cnt(0)
	{
	}

	int _ref_cnt;
};

 // reference counting smart pointer
template<typename T> struct SmartPtr
{
	 // default constructor
	SmartPtr()
	 :	_ptr(NULL)
	{
	}

	 // The initialized SmartPtr constructor increments the reference counter in struct RefCnt.
	SmartPtr(T* p)
	 :	_ptr(p)
	{
		if (p)
			++_ptr->_ref_cnt;
	}

	 // The copy constructor increments the reference counter.
	SmartPtr(const SmartPtr& other)
	 :	_ptr(other._ptr)
	{
		if (_ptr)
			++_ptr->_ref_cnt;
	}

	 // The destructor decreases the reference counter and
	 // frees the managed memory as the counter reaches zero.
	~SmartPtr()
	{
		if (_ptr) {
			if (!--_ptr->_ref_cnt)
				delete _ptr;
		}
	}

	 // The assignment operator increments the reference counter.
	SmartPtr& operator=(T* p)
	{
		if (_ptr) {
			if (!--_ptr->_ref_cnt)
				delete _ptr;

			_ptr = NULL;
		}

		if (p) {
			_ptr = p;

			++_ptr->_ref_cnt;
		}

		return *this;
	}

	 // operator bool() to check for non-empty smart pointers
	operator bool() const {return _ptr != NULL;}

	 // operator!() to check for empty smart pointers
	bool operator!() const {return !_ptr;}

	 // operator->() to access the managed objects
	T* operator->() {return _ptr;}
	const T* operator->() const {return _ptr;}

	 // Derefence pointed memory
	T& operator*() {return *_ptr;}
	const T& operator*() const {return *_ptr;}

private:
	T* _ptr;
};

} // namespace RefCount


//MF
namespace ExcelFormat {
struct CellFormat;
}


namespace YExcel
{
using namespace YCompoundFiles;

#ifdef _WIN32
using namespace WinCompFiles;
#endif

struct CODE
{
	enum {	FORMULA=0x0006, 		//Token array and the result of a formula cell.
			YEOF=0x000A,			//End of a record block with leading BOF record.
			CALCCOUNT=0x000C,		//Maximum number of times the forumlas should be iteratively calculated
			CALCMODE=0x000D,		//Calculate formulas manually, automatically, or automatically except for multiple table operations
			PRECISION=0x000E,		//Whether formulas use the real cell values for calculation or the values displayed on the screen.
			REFMODE=0x000F, 		//Method used to show cell addresses in formulas.
			DELTA=0x0010,			//Maximum change of the result to exit an iteration.
			ITERATION=0x0011,		//Whether iterations are allowed while calculating recursive formulas.
			PROTECT=0x0012, 		//Whether worksheet or a workbook is protected against modification.
			PASSWORD=0x0013,		//16-bit hash value, calculated from the worksheet or workbook protection password.
			HEADER=0x0014,			//Page header string for the current worksheet.
			FOOTER=0x0015,			//Page footer string for the current worksheet.
			EXTERNSHEET=0x0017, 	//List with indexes to SUPBOOK records
			NAME=0x0018,			//Name and token array of an internal defined name.
			WINDOWPROTECT=0x0019,	//Whether the window configuration of the document is protected.
			SELECTION=0x001D,		//Addresses of all selected cell ranges and position of the active cell for a pane in the current sheet.
			DATEMODE=0x0022,		//Base date for displaying date values.
			EXTERNNAME=0x0023,		//Name of an external defined name, name of an add-in function, a DDE item or an OLE object storage identifier.
			LEFTMARGIN=0x0026,		//Left page margin of the current worksheet.
			RIGHTMARGIN=0x0027, 	//Right page margin of the current worksheet.
			TOPMARGIN=0x0028,		//Top page margin of the current worksheet.
			BOTTOMMARGIN=0x0029,	//Bottom page margin of current worksheet
			PRINTHEADERS=0x002A,	//Whether row and column headers (the areas with row numbers and column letters) will be printed.
			PRINTGRIDLINES=0x002B,	//Whether sheet grid lines will be printed.
			FILEPASS=0x002F,		//Information about the read/write password of the file.
			FONT=0x0031,			//Information about a used font, including character formatting.
			TABLE=0x0036,			//Information about a multiple operation table in the sheet.
			CONTINUE=0x003C,		//Continue from previous record
			WINDOW1=0x003D, 		//General settings for the workbook global settings.
			BACKUP=0x0040,			//Make backup of file while saving?
			PANE=0x0041,			//Position of window panes.
			CODEPAGE=0x0042,		//Text encoding used to encode byte strings
			DCONREF=0x0051,
			DEFCOLWIDTH=0x0055, 	//Default column width for columns that do not have a specific width set
			XCT=0x0059, 			//Number of immediately following CRN records.
			CRN=0x005A, 			//Contents of an external cell or cell range.
			FILESHARING=0x005B, 	//Information about write protection, for instance the write protection password.
			WRITEACCESS=0x005C, 	//Name of the user that has saved the file.
			UNCALCED=0x005E,		//Formulas have not been recalculated before the document was saved.
			SAVERECALC=0x005F,		//"Recalculate before save" option
			OBJECTPROTECT=0x0063,	//Whether objects of the current sheet are protected.
			COLINFO=0x007D, 		//Width for a given range of columns
			GUTS=0x0080,			//Layout of outline symbols.
			WSBOOL=0x0081,			//16-bit value with boolean options for the current sheet.
			GRIDSET=0x0082, 		//Whether option to print sheet grid lines has ever been changed.
			HCENTER=0x0083, 		//Sheet is centred horizontally when printed.
			VCENTER=0x0084, 		//Whether sheet is centred vertically when printed.
			BOUNDSHEET=0x0085,		//Sheet inside of the workbook
			WRITEPROT=0x0086,		//Whether file is write protected.
			COUNTRY=0x008C, 		//User interface language of the Excel version that has saved the file, system regional settings at the time the file was saved.
			HIDEOBJ=0x008D, 		//Whether and how to show objects in the workbook.
			SORT=0x0090,			//Last settings from the "Sort" dialogue for each sheet.
			PALETTE=0x0092, 		//Definition of all user-defined colours available for cell and object formatting.
			SETUP=0x00A1,			//Page format settings of the current sheet.
			SHRFMLA=0x00BC, 		//Token array of a shared formula.
			MULRK=0x00BD,			//Cell range containing RK value cells. All cells are located in the same row.
			MULBLANK=0x00BE,		//Cell range of empty cells. All cells are located in the same row.
			DBCELL=0x00D7,			//Relative offsets to calculate stream position of the first cell record for each row.
			BOOKBOOL=0x00DA,		//Save values linked from external workbooks records and XCT records?
			SCENPROTECT=0x00DD, 	//Whether scenarios of the current sheet are protected.
			XF=0x00E0,				//Formatting information for cells, rows, columns or styles.
			MERGECELLS=0x00E5, 		//All merged cell ranges of the current sheet.
			SST=0x00FC, 			//List of all strings used anywhere in the workbook.
			LABELSST=0x00FD,		//Cell that contains a string.
			EXTSST=0x00FF,			//Create a hash table with stream offsets to the SST record to optimise string search operations.
			LABELRANGES=0x015F, 	//Addresses of all row and column label ranges in the current sheet.
			USESELFS=0x0160,		//Whether formulas in the workbook can use "natural language formulas".
			DSF=0x0161, 			//Whether file contains an addition BIFF5/BIFF7 workbook stream.
			SUPBOOK=0x01AE, 		//URL of an external document and a list of sheet names inside this document.
			CONDFMT=0x01B0, 		//List of cell range addresses for all cells with equal conditional formatting.
			CF=0x01B1,				//Condition and the formatting attributes applied to the cells specified in the CONDFMT record, if the condition is met
			DVAL=0x01B2,			//List header of the data validity table in the current sheet.
			HLINK=0x01B8,			//One cell address or a cell range where all cells contain the same hyperlink.
			DV=0x01BE,				//Data validity settings and a list of cell ranges which contain these settings.
			DIMENSIONS=0x0200,		//Range address of the used area in the current sheet.
			BLANK=0x0201,			//Empty cell, contains cell address and formatting information
			NUMBER=0x0203,			//Cell that contains a floating-point value.
			BOOLERR=0x0205, 		//Error value cell
			STRING=0x0207,			//Result of a string formula.
			ROW=0x0208, 			//Properties of a single row in the sheet.
			INDEX=0x020B,			//Range of used rows and stream positions of several records of the current sheet.
			ARRAY=0x0221,			//Token array of an array formula
			WINDOW2=0x023E, 		//Additional settings for the window of a specific worksheet.
			RK=0x027E,				//Cell that contains an RK value (encoded integer or floating point value).
			STYLE=0x0293,			//Name of a user-defined cell style or specific options for a built-in cell style.
			FORMAT=0x041E,			//Number format.
			SHRFMLA1=0x04BC,		//Token array of a shared formula (added).
			QUICKTIP=0x0800,		//Cell range and text for a tool tip.
			BOF=0x0809, 			//Beginning of file
			SHEETLAYOUT=0x0862, 	//Colour of the tab below the sheet containing the sheet name.
			SHEETPROTECTION=0x0867, //Additional options for sheet protection.
			RANGEPROTECTION=0x0868,	//Information about special protected ranges in a protected sheet.
			SXFORMULA=0x0103,		//PivotTable Formula Record
		};
};

class Record
{
public:
	Record();
	virtual ~Record();
	virtual ULONG Read(const char* data);
	virtual ULONG Write(char* data);
	virtual ULONG DataSize();
	virtual ULONG RecordSize();
	USHORT code_;
	vector<char> data_;
	ULONG	dataSize_;
	ULONG	recordSize_;
	vector<ULONG> continueIndices_;
};

struct BOF : public Record
{
	BOF();
	virtual ULONG Read(const char* data);
	virtual ULONG Write(char* data);
	USHORT version_;
	USHORT type_;
	USHORT buildIdentifier_;
	USHORT buildYear_;
	ULONG fileHistoryFlags_;
	ULONG lowestExcelVersion_;
};

struct YEOF : public Record
{
	YEOF();
};

 // String with 1 byte length field
struct SmallString
{
	SmallString();
	~SmallString();
	SmallString(const SmallString& s);

	SmallString& operator=(const SmallString& s);
	const SmallString& operator=(const char* str);
	const SmallString& operator=(const wchar_t* str);

	void Reset();
	ULONG Read(const char* data);
	ULONG Write(char* data);
	ULONG DataSize();
	ULONG RecordSize();
	ULONG StringSize();

	wchar_t* wname_;
	char* name_;
	char unicode_;
};

 // String with 2 byte length field
struct LargeString
{
	LargeString();
	~LargeString();
	LargeString(const LargeString& s);

	LargeString& operator=(const LargeString& s);
	const LargeString& operator=(const char* str);
	const LargeString& operator=(const wchar_t* str);

	void Reset();
	ULONG Read(const char* data);
	ULONG ContinueRead(const char* data, int size);
	ULONG Write(char* data);
	ULONG DataSize();
	ULONG RecordSize();
	ULONG StringSize();

	vector<wchar_t> wname_;
	vector<char> name_;
	char unicode_;
	USHORT richtext_;
	ULONG phonetic_;
};


	//MF string conversion functions

inline std::string narrow_string(const vector<wchar_t>& wstr)
{
	return ::narrow_string(wstring(&*wstr.begin(), wstr.size()));
}

inline std::wstring widen_string(const vector<char>& wstr)
{
	return ::widen_string(string(&*wstr.begin(), wstr.size()));
}

inline string stringFromSmallString(const SmallString& ss)
{
	if (ss.unicode_)
		return ::narrow_string(ss.wname_);
	else
		return ss.name_;
}

inline string stringFromLargeString(const LargeString& ls)
{
	if (ls.unicode_)
		return narrow_string(ls.wname_);
	else
		return string(&*ls.name_.begin(), ls.name_.size());
}


inline wstring wstringFromSmallString(const SmallString& ss)
{
	if (ss.unicode_)
		return ss.wname_;
	else
		return ::widen_string(ss.name_);
}

inline wstring wstringFromLargeString(const LargeString& ls)
{
	if (ls.unicode_)
		return wstring(&*ls.wname_.begin(), ls.wname_.size());
	else
		return widen_string(ls.name_);
}


class Workbook
{
public:
	Workbook();

public:
	struct FileProtection;
	struct CodePage;
	struct DSF;
	struct TabID;
	struct FnGroupCount;
	struct WorkbookProtection;
	struct Window1 : public Record
	{
		Window1();
		virtual ULONG Read(const char* data);
		virtual ULONG Write(char* data);
		USHORT horizontalPos_;
		USHORT verticalPos_;
		USHORT width_;
		USHORT height_;
		USHORT options_;
		USHORT activeWorksheetIndex_;
		USHORT firstVisibleTabIndex_;
		USHORT selectedWorksheetNo_;
		USHORT worksheetTabBarWidth_;
	};
	struct Backup;
	struct HideObj;
	struct DateMode;
	struct Precision;
	struct RefreshAll;
	struct BookBool;
	struct Font : public Record
	{
		Font();
		virtual ULONG Read(const char* data);
		virtual ULONG Write(char* data);
		virtual ULONG DataSize();
		virtual ULONG RecordSize();
		USHORT height_;
		USHORT options_;
		USHORT colourIndex_;
		USHORT weight_;
		USHORT escapementType_;
		BYTE underlineType_;
		BYTE family_;
		BYTE characterSet_;
		BYTE unused_;
		SmallString name_;
	};
	struct Format : public Record
	{
		//MF
		Format();

		virtual ULONG Read(const char* data);
		virtual ULONG Write(char* data);
		virtual ULONG DataSize();
		virtual ULONG RecordSize();

		USHORT index_;
		LargeString fmtstring_;
	};
	struct XF : public Record
	{
		XF();
		virtual ULONG Read(const char* data);
		virtual ULONG Write(char* data);
		USHORT fontRecordIndex_;
		USHORT formatRecordIndex_;
		USHORT protectionType_;
		BYTE alignment_;	// 0x08: 1 = Text is wrapped at right border
		BYTE rotation_;
		BYTE textProperties_;
		BYTE usedAttributes_;
		ULONG borderLines_;
		ULONG colour1_;
		USHORT colour2_;
	};
	struct Style : public Record
	{
		Style();
		virtual ULONG Read(const char* data);
		virtual ULONG Write(char* data);
		virtual ULONG DataSize();
		virtual ULONG RecordSize();
		USHORT XFRecordIndex_;
		BYTE identifier_;
		BYTE level_;
		LargeString name_;
	};
	struct Palette;
	struct UseSelfs;
	struct BoundSheet : public Record
	{
		BoundSheet();
		virtual ULONG Read(const char* data);
		virtual ULONG Write(char* data);
		virtual ULONG DataSize();
		virtual ULONG RecordSize();
		ULONG BOFpos_;
		BYTE visibility_;
		BYTE type_;
		SmallString name_;
	};
	struct Country;
	struct LinkTable;
	struct SharedStringTable : public Record
	{
		SharedStringTable();
		virtual ULONG Read(const char* data);
		virtual ULONG Write(char* data);
		virtual ULONG DataSize();
		virtual ULONG RecordSize();
		ULONG stringsTotal_;
		ULONG uniqueStringsTotal_;
		vector<LargeString> strings_;
	};
	struct ExtSST : public Record
	{
		ExtSST();
		virtual ULONG Read(const char* data);
		virtual ULONG Write(char* data);
		virtual ULONG DataSize();
		virtual ULONG RecordSize();
		USHORT stringsTotal_;
		vector<ULONG> streamPos_;
		vector<USHORT> firstStringPos_;
		vector<USHORT> unused_;
	};
	ULONG Read(const char* data);
	ULONG Write(char* data);
	ULONG DataSize();
	ULONG RecordSize();

	BOF bof_;
	Window1 window1_;
	vector<Font> fonts_;
	vector<XF> XFs_;
	vector<Style> styles_;
	vector<Format> formats_;	//MF
	vector<BoundSheet> boundSheets_;
	SharedStringTable sst_; // shared string table
	ExtSST extSST_;
	YEOF eof_;
};

//MF: exception to handle unexpected YEOF records
struct EXCEPTION_YEOF
{
	EXCEPTION_YEOF(ULONG bytesRead)
	 :	_bytesRead(bytesRead)
	{
	}

	ULONG _bytesRead;
};

//MF
using namespace RefCount;

class Worksheet
{
public:
	Worksheet();

public:
	struct Uncalced;
	struct Index : public Record
	{
		Index();
		virtual ULONG Read(const char* data);
		virtual ULONG Write(char* data);
		virtual ULONG DataSize();
		virtual ULONG RecordSize();
		ULONG unused1_;
		size_t firstUsedRowIndex_;
		size_t firstUnusedRowIndex_;
		ULONG unused2_;
		vector<ULONG> DBCellPos_;
	};
	struct CalculationSettings
	{
		struct CalcCount;
		struct CalcMode;
		struct RefMode;
		struct Delta;
		struct Iteration;
		struct SafeRecalc;
	};
	struct PrintHeaders;
	struct PrintGridlines;
	struct Gridset;
	struct Guts;
	struct DefaultRowHeight;
	struct WSBool;
	struct PageSettings
	{
		struct Header;
		struct Footer;
		struct HCenter;
		struct VCenter;
		struct LeftMargin;
		struct RightMargin;
		struct TopMargin;
		struct BottomMargin;
		struct PLS;
		struct Setup;
	};
	struct WorksheetProtection;
	struct DefColWidth;
	struct ColInfo : public Record
	{
		ColInfo();
		virtual ULONG Read(const char* data);
		virtual ULONG Write(char* data);
		USHORT firstColumnIndex_;
		USHORT lastColumnIndex_;
		USHORT columnWidth_;
		USHORT XFRecordIndex_;
		USHORT options_;
		USHORT unused_;
	};
	struct Sort;
	struct ColInfos : public Record
	{
		virtual ULONG Read(const char* data);
		virtual ULONG Write(char* data);
		virtual ULONG RecordSize();
		vector<ColInfo> colinfo_;
	};
	struct Dimensions : public Record
	{
		Dimensions();
		virtual ULONG Read(const char* data);
		virtual ULONG Write(char* data);
		ULONG firstUsedRowIndex_;
		ULONG lastUsedRowIndexPlusOne_;
		USHORT firstUsedColIndex_;
		USHORT lastUsedColIndexPlusOne_;
		USHORT unused_;
	};
	struct CellTable
	{
		struct RowBlock
		{
			struct Row : public Record
			{
				Row();
				virtual ULONG Read(const char* data);
				virtual ULONG Write(char* data);
				USHORT rowIndex_;
				USHORT firstCellColIndex_;
				USHORT lastCellColIndexPlusOne_;
				USHORT height_;
				USHORT unused1_;
				USHORT unused2_;
				ULONG options_;
			};

			struct CellBlock : public RefCnt
			{
				struct Blank : public Record
				{
					Blank();
					virtual ULONG Read(const char* data);
					virtual ULONG Write(char* data);
					USHORT rowIndex_;
					USHORT colIndex_;
					USHORT XFRecordIndex_;
				};
				struct BoolErr : public Record
				{
					BoolErr();
					virtual ULONG Read(const char* data);
					virtual ULONG Write(char* data);
					USHORT rowIndex_;
					USHORT colIndex_;
					USHORT XFRecordIndex_;
					BYTE value_;
					BYTE error_;
				};
				struct LabelSST : public Record
				{
					LabelSST();
					virtual ULONG Read(const char* data);
					virtual ULONG Write(char* data);
					USHORT rowIndex_;
					USHORT colIndex_;
					USHORT XFRecordIndex_;
					size_t SSTRecordIndex_; // shared string table index
				};
				struct MulBlank : public Record
				{
					MulBlank();
					virtual ULONG Read(const char* data);
					virtual ULONG Write(char* data);
					virtual ULONG DataSize();
					virtual ULONG RecordSize();
					USHORT rowIndex_;
					USHORT firstColIndex_;
					vector<USHORT> XFRecordIndices_;
					USHORT lastColIndex_;
				};
				struct MulRK : public Record
				{
					MulRK();
					virtual ULONG Read(const char* data);
					virtual ULONG Write(char* data);
					virtual ULONG DataSize();
					virtual ULONG RecordSize();
					struct XFRK
					{
						XFRK();
						void Read(const char* data);
						void Write(char* data);
						USHORT XFRecordIndex_;
						LONG RKValue_;
					};
					USHORT rowIndex_;
					USHORT firstColIndex_;
					vector<XFRK> XFRK_;
					USHORT lastColIndex_;
				};
				struct Number : public Record
				{
					Number();
					virtual ULONG Read(const char* data);
					virtual ULONG Write(char* data);
					USHORT rowIndex_;
					USHORT colIndex_;
					USHORT XFRecordIndex_;
					double value_;

				private:
					union
					{
						LONGINT intvalue_;
						double doublevalue_;
					} intdouble_;
				};
				struct RK : public Record
				{
					RK();
					virtual ULONG Read(const char* data);
					virtual ULONG Write(char* data);
					USHORT rowIndex_;
					USHORT colIndex_;
					USHORT XFRecordIndex_;
					ULONG value_;
				};

				struct Formula : public Record
				{
					struct Array : public Record
					{
						Array();
						virtual ULONG Read(const char* data);
						virtual ULONG Write(char* data);
						virtual ULONG DataSize();
						virtual ULONG RecordSize();
						USHORT firstRowIndex_;
						USHORT lastRowIndex_;
						BYTE firstColIndex_;
						BYTE lastColIndex_;
						USHORT options_;
						ULONG unused_;
						vector<char> formula_;
					};
					struct ShrFmla : public Record
					{
						ShrFmla();
						virtual ULONG Read(const char* data);
						virtual ULONG Write(char* data);
						virtual ULONG DataSize();
						virtual ULONG RecordSize();
						USHORT firstRowIndex_;
						USHORT lastRowIndex_;
						BYTE firstColIndex_;
						BYTE lastColIndex_;
						USHORT unused_;
						vector<char> formula_;
					};
					struct ShrFmla1 : public Record
					{
						ShrFmla1();
						virtual ULONG Read(const char* data);
						virtual ULONG Write(char* data);
						virtual ULONG DataSize();
						virtual ULONG RecordSize();
						USHORT firstRowIndex_;
						USHORT lastRowIndex_;
						BYTE firstColIndex_;
						BYTE lastColIndex_;
						USHORT unused_;
						vector<char> formula_;
					};
					struct Table : public Record
					{
						Table();
						virtual ULONG Read(const char* data);
						virtual ULONG Write(char* data);
						USHORT firstRowIndex_;
						USHORT lastRowIndex_;
						BYTE firstColIndex_;
						BYTE lastColIndex_;
						USHORT options_;
						USHORT inputCellRowIndex_;
						USHORT inputCellColIndex_;
						USHORT inputCellColumnInputRowIndex_;
						USHORT inputCellColumnInputColIndex_;
					};
					struct String : public Record
					{
						String();
						~String();
						virtual ULONG Read(const char* data);
						virtual ULONG Write(char* data);
						virtual ULONG DataSize();
						virtual ULONG RecordSize();
						bool empty() {return !wstr_;}
						void Reset();
						char flag_;				// 0 = compressed unicode string  1 = uncompressed unicode string
						// From BIFF8 on, strings are always stored using UTF-16LE text encoding, optionally compressed (see compressed field)
						wchar_t* wstr_;
					};

					Formula();
					virtual ULONG Read(const char* data);
					virtual ULONG Write(char* data);
					virtual ULONG DataSize();
					virtual ULONG RecordSize();
					USHORT rowIndex_;
					USHORT colIndex_;
					USHORT XFRecordIndex_;
					BYTE result_[8];			// formula result (IEEE 754 floating-point value, 64-bit double precision or other special values)
					USHORT options_; 			// 1 = Recalculate always  2 = Calculate on open  8 = Part of a shared formula
					ULONG unused_;				// chn field
					vector<char> RPNtoken_;		// 2 length bytes, followed by a variable length structure
					USHORT type_;

					Array array_;
					ShrFmla shrfmla_;
					ShrFmla1 shrfmla1_;
					Table table_;
					String string_;
				};

				CellBlock();
				~CellBlock();

				void Reset();
				void SetType(int type);

				ULONG Read(const char* data);
				ULONG Write(char* data);
				ULONG DataSize();
				ULONG RecordSize();
				USHORT RowIndex();
				USHORT ColIndex();
				USHORT LastColIndex();
				SHORT type_;

				//MF
				union CellBlockUnion {
					void* void_;
					Blank* blank_;
					MulBlank* mulblank_;
					BoolErr* boolerr_;
					LabelSST* labelsst_;
					MulRK* mulrk_;
					Number* number_;
					RK* rk_;
					Formula* formula_;
				} _union;
			};
			struct DBCell : public Record
			{
				DBCell();
				virtual ULONG Read(const char* data);
				virtual ULONG Write(char* data);
				virtual ULONG DataSize();
				virtual ULONG RecordSize();
				ULONG firstRowOffset_;
				vector<USHORT> offsets_;
			};

			ULONG Read(const char* data);
			ULONG Write(char* data);
			ULONG DataSize();
			ULONG RecordSize();

			vector<Row> rows_;
			vector<SmartPtr<CellBlock> > cellBlocks_;
			DBCell dbcell_;
		};
		ULONG Read(const char* data);
		ULONG Write(char* data);
		ULONG DataSize();
		ULONG RecordSize();

		vector<RowBlock> rowBlocks_;
	};
	struct Window2 : public Record
	{
		Window2();
		virtual ULONG Read(const char* data);
		virtual ULONG Write(char* data);
		USHORT options_;
		USHORT firstVisibleRowIndex_;
		USHORT firstVisibleColIndex_;
		USHORT gridLineColourIndex_;
		USHORT unused1_;
		USHORT magnificationFactorPageBreakPreview_;
		USHORT magnificationFactorNormalView_;
		ULONG unused2_;
	};
	struct SCL;
	struct Pane;
	struct Selection;
	struct MergedCells
	{
		struct MergedCell
		{
			MergedCell();
			virtual ULONG Read(const char* data);
			virtual ULONG Write(char* data);
			ULONG DataSize();
			ULONG RecordSize();
			USHORT firstRow_;
			USHORT lastRow_;
			USHORT firstColumn_;
			USHORT lastColumn_;
		};

		ULONG Read(const char* data);
		ULONG Write(char* data);
		ULONG DataSize();
		ULONG RecordSize();

		vector<MergedCell> mergedCellsVector_;
	};
	struct LabelRanges;
	struct ConditionalFormattingTable;
	struct HyperlinkTable;
	struct SheetLayout;
	struct SheetProtection;
	struct RangeProtection;

	ULONG Read(const char* data);
	ULONG Write(char* data);
	ULONG DataSize();
	ULONG RecordSize();

	BOF bof_;
	Index index_;
	ColInfos colinfos_;
	Dimensions dimensions_;
	CellTable cellTable_;
	Window2 window2_;
	MergedCells mergedCells_;
	YEOF eof_;
};

double GetDoubleFromRKValue(LONG rkValue);	///< Convert a rk value to a double.
int GetIntegerFromRKValue(LONG rkValue); 	///< Convert a rk value to an integer.
LONG GetRKValueFromDouble(double value); 	///< Convert a double to a rk value.
LONG GetRKValueFromInteger(int value);		///< Convert an integer to a rk value.
bool CanStoreAsRKValue(double value);		///< Returns true if the supplied double can be stored as a rk value.

// Forward declarations
class BasicExcel;
class BasicExcelWorksheet;
class BasicExcelCell;

/*******************************************************************************************************/
/*						   Actual classes to read and write to Excel files							   */
/*******************************************************************************************************/
class BasicExcel
{
public:
	BasicExcel();
	BasicExcel(const char* filename);
	~BasicExcel();

public: // File functions.
	void New(int sheets=3); ///< Create a new Excel workbook with a given number of spreadsheets (Minimum 1).
	bool Load(const char* filename);	///< Load an Excel workbook from a file.
	bool Load(const wchar_t* filename);	///< Load an Excel workbook from a file.
	bool Save();	///< Save current Excel workbook to opened file.
	bool SaveAs(const char* filename);	///< Save current Excel workbook to a file.
	bool SaveAs(const wchar_t* filename);	///< Save current Excel workbook to a file.
	void Close();

public: // Worksheet functions.
	int GetTotalWorkSheets();	///< Total number of Excel worksheets in current Excel workbook.

	BasicExcelWorksheet* GetWorksheet(int sheetIndex);	///< Get a pointer to an Excel worksheet at the given index. Index starts from 0. Returns 0 if index is invalid.
	BasicExcelWorksheet* GetWorksheet(const char* name);	///< Get a pointer to an Excel worksheet that has given ANSI name. Returns 0 if there is no Excel worksheet with the given name.
	BasicExcelWorksheet* GetWorksheet(const wchar_t* name); ///< Get a pointer to an Excel worksheet that has given Unicode name. Returns 0 if there is no Excel worksheet with the given name.

	BasicExcelWorksheet* AddWorksheet(int sheetIndex=-1);	///< Add a new Excel worksheet to the given index. Name given to worksheet is SheetX, where X is a number which starts from 1. Index starts from 0. Worksheet is added to the last position if sheetIndex == -1. Returns a pointer to the worksheet if successful, 0 if otherwise.
	BasicExcelWorksheet* AddWorksheet(const char* name, int sheetIndex=-1); ///< Add a new Excel worksheet with given ANSI name to the given index. Index starts from 0. Worksheet is added to the last position if sheetIndex == -1. Returns a pointer to the worksheet if successful, 0 if otherwise.
	BasicExcelWorksheet* AddWorksheet(const wchar_t* name, int sheetIndex=-1);	///< Add a new Excel worksheet with given Unicode name to the given index. Index starts from 0. Worksheet is added to the last position if sheetIndex == -1. Returns a pointer to the worksheet if successful, 0 if otherwise.

	bool DeleteWorksheet(int sheetIndex);		///< Delete an Excel worksheet at the given index. Index starts from 0. Returns true if successful, false if otherwise.
	bool DeleteWorksheet(const char* name); 	///< Delete an Excel worksheet that has given ANSI name. Returns true if successful, false if otherwise.
	bool DeleteWorksheet(const wchar_t* name);	///< Delete an Excel worksheet that has given Unicode name. Returns true if successful, false if otherwise.

	char* GetAnsiSheetName(int sheetIndex);			///< Get the worksheet name at the given index. Index starts from 0. Returns 0 if name is in Unicode format.
	wchar_t* GetUnicodeSheetName(int sheetIndex);	///< Get the worksheet name at the given index. Index starts from 0. Returns 0 if name is in Ansi format.
	bool GetSheetName(int sheetIndex, char* name);		///< Get the worksheet name at the given index. Index starts from 0. Returns false if name is in Unicode format.
	bool GetSheetName(int sheetIndex, wchar_t* name);	///< Get the worksheet name at the given index. Index starts from 0. Returns false if name is in Ansi format.

	bool RenameWorksheet(int sheetIndex, const char* to);			///< Rename an Excel worksheet at the given index to the given ANSI name. Index starts from 0. Returns true if successful, false if otherwise.
	bool RenameWorksheet(int sheetIndex, const wchar_t* to);	 	///< Rename an Excel worksheet at the given index to the given Unicode name. Index starts from 0. Returns true if successful, false if otherwise.
	bool RenameWorksheet(const char* from, const char* to); 		///< Rename an Excel worksheet that has given ANSI name to another ANSI name. Returns true if successful, false if otherwise.
	bool RenameWorksheet(const wchar_t* from, const wchar_t* to);	///< Rename an Excel worksheet that has given Unicode name to another Unicode name. Returns true if successful, false if otherwise.

private: // Functions to read and write raw Excel format.
	size_t Read(const char* data, size_t dataSize);
	size_t Write(char* data);
	void AdjustStreamPositions();
	void AdjustBoundSheetBOFPositions();
	void AdjustDBCellPositions();
	void AdjustExtSSTPositions();

	enum {WORKBOOK_GLOBALS=0x0005, VISUAL_BASIC_MODULE=0x0006,
		  WORKSHEET=0x0010, CHART=0x0020};

private: // Internal functions
	void UpdateYExcelWorksheet();	///< Update yesheets_ using information from worksheets_.
	void UpdateWorksheets();		///< Update worksheets_ using information from yesheets_.

public:
	CompoundFile file_; 					///< Compound file handler.
	Workbook workbook_; 					///< Raw Workbook.
	vector<Worksheet> worksheets_;			///< Raw Worksheets.
	vector<SmartPtr<BasicExcelWorksheet> > yesheets_;	///< Parsed Worksheets.
};

class BasicExcelWorksheet : public RefCount::RefCnt
{
	friend class BasicExcel;

public:
	BasicExcelWorksheet(BasicExcel* excel, int sheetIndex);

public: // Worksheet functions
	char* GetAnsiSheetName();			///< Get the current worksheet name. Returns 0 if name is in Unicode format.
	wchar_t* GetUnicodeSheetName();		///< Get the current worksheet name. Returns 0 if name is in Ansi format.
	bool GetSheetName(char* name);		///< Get the current worksheet name. Returns false if name is in Unicode format.
	bool GetSheetName(wchar_t* name);	///< Get the current worksheet name. Returns false if name is in Ansi format.
	bool Rename(const char* to);		///< Rename current Excel worksheet to another ANSI name. Returns true if successful, false if otherwise.
	bool Rename(const wchar_t* to);		///< Rename current Excel worksheet to another Unicode name. Returns true if successful, false if otherwise.
	void Print(ostream& os, char delimiter=',', char textQualifier='\0')  const; ///< Print entire worksheet to an output stream, separating each column with the defined delimiter and enclosing text using the defined textQualifier. Leave out the textQualifier argument if do not wish to have any text qualifiers.

public: // Cell functions
	int GetTotalRows() const;		///< Total number of rows in current Excel worksheet.
	int GetTotalCols() const;		///< Total number of columns in current Excel worksheet.

	BasicExcelCell* Cell(int row, int col); ///< Return a pointer to an Excel cell. row and col starts from 0. Returns 0 if row exceeds 65535 or col exceeds 255.
	const BasicExcelCell* Cell(int row, int col) const; ///< Return a pointer to an Excel cell. row and col starts from 0. Returns 0 if row exceeds 65535 or col exceeds 255.
	bool EraseCell(int row, int col);		///< Erase content of a cell. row and col starts from 0. Returns true if successful, false if row or col exceeds range.

	void SetColWidth(const int colindex , const USHORT colwidth);
	USHORT GetColWidth(const int colindex);

	void MergeCells(int row, int col, USHORT rowRange, USHORT colRange);

private: // Internal functions
	void UpdateCells(); ///< Update cells using information from BasicExcel.worksheets_.

private:
	BasicExcel* excel_; 				///< Pointer to instance of BasicExcel.
	int sheetIndex_; 					///< Index of worksheet in workbook.
	int	maxRows_;						///< Total number of rows in worksheet.
	int	maxCols_;						///< Total number of columns in worksheet.
	vector<vector<BasicExcelCell> > cells_; ///< Cells matrix.
	Worksheet::ColInfos colInfos_;		/// used to record column info
};

class BasicExcelCell
{
public:
	BasicExcelCell();

	enum {UNDEFINED, INT, DOUBLE, STRING, WSTRING, FORMULA/*MF*/};
	int Type() const;					///< Get type of value stored in current Excel cell. Returns one of the above enums.

	bool Get(int& val) const;			///< Get an integer value. Returns false if cell does not contain an integer or a double.
	bool Get(double& val) const;		///< Get a double value. Returns false if cell does not contain a double or an integer.
	size_t GetStringLength() const; 	///< Return length of ANSI or Unicode string (excluding null character).

	int GetInteger() const; 			///< Get an integer value. Returns 0 if cell does not contain an integer.
	double GetDouble() const;			///< Get a double value. Returns 0.0 if cell does not contain a double.
	const char* GetString() const;		///< Get an ANSI string. Returns 0 if cell does not contain an ANSI string.
	const wchar_t* GetWString() const;	///< Get an Unicode string. Returns 0 if cell does not contain an Unicode string.

	friend ostream& operator<<(ostream& os, const BasicExcelCell& cell);	///< Print cell to output stream. Print a null character if cell is undefined.

	void Set(int val);					///< Set content of current Excel cell to an integer.
	void Set(double val);				///< Set content of current Excel cell to a double.
	void Set(const char* str);			///< Set content of current Excel cell to an ANSI string.
	void Set(const wchar_t* str);		///< Set content of current Excel cell to an Unicode string.

	void SetInteger(int val);			///< Set content of current Excel cell to an integer.
	void SetDouble(double val); 		///< Set content of current Excel cell to a double.
	void SetRKValue(int rkValue);		///< Set content of current Excel cell to a double or integer value.
	void SetString(const char* str);	///< Set content of current Excel cell to an ANSI string.
	void SetWString(const wchar_t* str);///< Set content of current Excel cell to an Unicode string.

	void SetFormula(const Worksheet::CellTable::RowBlock::CellBlock::Formula& f);//MF

	int GetXFormatIdx() const				{return _xf_idx;}	//MF
	void SetXFormatIdx(int xf_idx)			{_xf_idx = xf_idx;} //MF
	void SetFormat(const ExcelFormat::CellFormat& fmt); //MF

	void EraseContents();	///< Erase the content of current Excel cell. Set type to UNDEFINED.

	USHORT GetMergedRows() const				{return mergedRows_;}
	USHORT GetMergedColumns() const				{return mergedColumns_;}

	void SetMergedRows(USHORT mergedRows)		{mergedRows_ = mergedRows;}
	void SetMergedColumns(USHORT mergedColumns)	{mergedColumns_ = mergedColumns;}

private:
	int type_;							///< Type of value stored in current Excel cell. Contains one of the above enums.
	int ival_;							///< Integer value stored in current Excel cell.
	double dval_;						///< Double value stored in current Excel cell.
	vector<char> str_;					///< ANSI string stored in current Excel cell. Include null character.
	vector<wchar_t> wstr_;				///< Unicode string stored in current Excel cell. Include null character.
										
	USHORT mergedRows_;					///< Number of rows merged to this cell. 1 means only itself.
	USHORT mergedColumns_;				///< Number of columns merged to this cell. 1 means only itself.

	bool Get(char* str) const;			///< Get an ANSI string. Returns false if cell does not contain an ANSI string.
	bool Get(wchar_t* str) const;		///< Get an Unicode string. Returns false if cell does not contain an Unicode string.

	//MF extensions for formating and formulas ...

	int _xf_idx;			//MF

	friend class BasicExcel;
	friend class BasicExcelWorksheet; // for Get(w/char*);

	struct Formula : public RefCount::RefCnt
	{
		Formula(const Worksheet::CellTable::RowBlock::CellBlock::Formula& f);

		int _formula_type;
		vector<char> _formula;
		unsigned char _result[8];	// formula result (IEEE 754 floating-point value, 64-bit double precision or other special values)
		std::wstring wstr_;			// formula result in case of strings, stored as UTF-16LE string
		std::string str_;			// formula result in case of strings, stored as ANSI string

		//shrfmla1_
		USHORT	firstRowIndex_;
		USHORT	lastRowIndex_;
		char	firstColIndex_;
		char	lastColIndex_;
		USHORT	unused_;
		vector<char> shrformula_;
	};

	SmartPtr<Formula> _pFormula;

	bool get_formula(Worksheet::CellTable::RowBlock::CellBlock* pCell) const;
};


} // namespace YExcel

#endif
