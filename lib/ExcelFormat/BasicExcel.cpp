#include "ExcelFormat.h"

#ifdef _MSC_VER
#include <malloc.h>	// for alloca()
#endif


#ifdef _WIN32

namespace WinCompFiles
{

CompoundFile::CompoundFile()
{
	_pStg = NULL;
}

CompoundFile::~CompoundFile()
{
	Close();
}

// Compound File functions
bool CompoundFile::Create(const wchar_t* filename)
{
	HRESULT hr = StgCreateDocfile(filename, STGM_READWRITE|STGM_CREATE|STGM_SHARE_EXCLUSIVE, 0, &_pStg);

	return SUCCEEDED(hr);
}

bool CompoundFile::Open(const wchar_t* filename, ios_base::openmode mode/*=ios_base::in|ios_base::out*/)
{
	int stgm_mode;

	if ((mode & (ios_base::in|ios_base::out)) == (ios_base::in|ios_base::out))
		stgm_mode = STGM_READWRITE | STGM_SHARE_EXCLUSIVE;
	else if (mode & ios_base::out)
		stgm_mode = STGM_WRITE | STGM_SHARE_EXCLUSIVE;
	else
		stgm_mode = STGM_READ | STGM_SHARE_EXCLUSIVE;

	HRESULT hr = StgOpenStorage(filename, NULL, stgm_mode, NULL, 0, &_pStg);

	return SUCCEEDED(hr);
}

bool CompoundFile::Close()
{
	if (_pStg) {
		_pStg->Release();
		_pStg = NULL;
		return true;
	} else
		return false;
}

bool CompoundFile::IsOpen()
{
	return _pStg != NULL;
}

// File functions
CF_RESULT CompoundFile::MakeFile(const wchar_t* path)
{
	IStream* pStream = NULL;

	HRESULT hr = _pStg->CreateStream(path, STGM_READWRITE|STGM_CREATE|STGM_SHARE_EXCLUSIVE, 0, 0, &pStream);

	if (pStream)
		pStream->Release();

	return SUCCEEDED(hr)? SUCCESS: INVALID_PATH;
}

CF_RESULT CompoundFile::FileSize(const wchar_t* path, ULONGLONG& size)
{
	IStream* pStream = NULL;

	// needs STGM_READWRITE in the StgCreateDocfile() call
	if (FAILED(_pStg->OpenStream(path, NULL, STGM_READ|STGM_SHARE_EXCLUSIVE, 0, &pStream)))
		return INVALID_PATH;

	STATSTG stat;

	HRESULT hr = pStream->Stat(&stat, STATFLAG_NONAME);

	if (pStream)
		pStream->Release();

	if (SUCCEEDED(hr)) {
		size = stat.cbSize.QuadPart;
		return SUCCESS;
	} else
		return INVALID_PATH;
}

CF_RESULT CompoundFile::ReadFile(const wchar_t* path, char* data, ULONG size)
{
	IStream* pStream = NULL;

	if (FAILED(_pStg->OpenStream(path, NULL, STGM_READ|STGM_SHARE_EXCLUSIVE, 0, &pStream)))
		return INVALID_PATH;

	ULONG read;
	HRESULT hr = pStream->Read(data, size, &read);

	if (pStream)
		pStream->Release();

	return SUCCEEDED(hr)? SUCCESS: INVALID_PATH;
}

CF_RESULT CompoundFile::ReadFile(const wchar_t* path, vector<char>&data)
{
	data.clear();

	ULONGLONG dataSize;

	CF_RESULT ret = FileSize(path, dataSize);

	if (ret == SUCCESS) {
		if (dataSize) {
			if (dataSize == (ULONG)dataSize) {
				data.resize((size_t)dataSize);
				ret = ReadFile(path, &*(data.begin()), (ULONG)dataSize);
			} else
				ret = INVALID_SIZE;
		} else
			ret = SUCCESS;
	}

	return ret;
}

CF_RESULT CompoundFile::WriteFile(const wchar_t* path, const char* data, ULONG size)
{
	IStream* pStream = NULL;

	if (FAILED(_pStg->OpenStream(path, NULL, STGM_READWRITE|STGM_SHARE_EXCLUSIVE, 0, &pStream)))
		return INVALID_PATH;

	ULONG written;

	HRESULT hr = pStream->Write(data, size, &written);

	if (pStream)
		pStream->Release();

	return SUCCEEDED(hr)? SUCCESS: INVALID_PATH;
}

CF_RESULT CompoundFile::WriteFile(const wchar_t* path, const vector<char>&data, ULONG size)
{
	IStream* pStream = NULL;

	if (FAILED(_pStg->OpenStream(path, NULL, STGM_READWRITE|STGM_SHARE_EXCLUSIVE, 0, &pStream)))
		return INVALID_PATH;

	ULONG written;

	HRESULT hr = pStream->Write(&*(data.begin()), size, &written);

	if (pStream)
		pStream->Release();

	return SUCCEEDED(hr)? SUCCESS: INVALID_PATH;
}


// ANSI char functions
bool CompoundFile::Create(const char* filename)
{
	return Create(widen_string(filename).c_str());
}

bool CompoundFile::Open(const char* filename, ios_base::openmode mode/*=ios_base::in|ios_base::out*/)
{
	return Open(widen_string(filename).c_str(), mode);
}

CF_RESULT CompoundFile::MakeFile(const char* path)
{
	return MakeFile(widen_string(path).c_str());
}

CF_RESULT CompoundFile::FileSize(const char* path, ULONGLONG& size)
{
	return FileSize(widen_string(path).c_str(), size);
}

CF_RESULT CompoundFile::ReadFile(const char* path, char* data, ULONG size)
{
	return ReadFile(widen_string(path).c_str(), data, size);
}

CF_RESULT CompoundFile::ReadFile(const char* path, vector<char>& data)
{
	return ReadFile(widen_string(path).c_str(), data);
}

CF_RESULT CompoundFile::WriteFile(const char* path, const char* data, ULONG size)
{
	return WriteFile(widen_string(path).c_str(), data, size);
}

CF_RESULT CompoundFile::WriteFile(const char* path, const vector<char>& data, ULONG size)
{
	return WriteFile(widen_string(path).c_str(), data, size);
}

} // namespace WinCompFiles

#else // _WIN32

#if _MSC_VER>=1400	// VS 2005
#include <share.h>	// _SH_DENYRW
#endif

namespace YCompoundFiles
{
/********************************** Start of Class Block *************************************/
// PURPOSE: Manage a file by treating it as blocks of data of a certain size.
Block::Block() :
	blockSize_(512), fileSize_(0), indexEnd_(0),
	filename_(0) {}

bool Block::Create(const wchar_t* filename)
// PURPOSE: Create a new block file and open it.
// PURPOSE: If file is present, truncate it and then open it.
// PROMISE: Return true if file is successfully created and opened, false if otherwise.
{
	// Create new file
	size_t filenameLength = wcslen(filename);
	char* name = new char[filenameLength+1];
	wcstombs(name, filename, filenameLength);
	name[filenameLength] = 0;

	// Open the file while truncating any existing file
	bool ret = this->Open(filename, ios_base::in | ios_base::out | ios_base::trunc);

	delete[] name;

	return ret;
}

bool Block::Open(const wchar_t* filename, ios_base::openmode mode)
// PURPOSE: Open an existing block file.
// PROMISE: Return true if file is successfully opened, false if otherwise.
{
	// Open existing file for reading or writing or both
	size_t filenameLength = wcslen(filename);
	filename_.resize(filenameLength+1, 0);
	wcstombs(&*(filename_.begin()), filename, filenameLength);

#if _MSC_VER>=1400	// VS 2005
	file_.open(&*(filename_.begin()), mode | ios_base::binary, _SH_DENYRW);
#else
	file_.open(&*(filename_.begin()), mode | ios_base::binary);
#endif

	if (!file_.is_open())
		return false;

	mode_ = mode;

	// Calculate filesize
	if (mode & ios_base::in) {
		file_.seekg(0, ios_base::end);
		fileSize_ = (ULONG) file_.tellg();
	} else if (mode & ios_base::out) {
		file_.seekp(0, ios_base::end);
		fileSize_ = (ULONG) file_.tellp();
	} else {
		this->Close();
		return false;
	}

	// Calculate last index + 1
	indexEnd_ = fileSize_/blockSize_ + (fileSize_ % blockSize_ ? 1 : 0);
	return true;
}

bool Block::Close()
// PURPOSE: Close the opened block file.
// PROMISE: Return true if file is successfully closed, false if otherwise.
{
	file_.close();
	file_.clear();
	filename_.clear();
	fileSize_ = 0;
	indexEnd_ = 0;
	blockSize_ = 512;

	return !file_.is_open();
}

bool Block::IsOpen()
// PURPOSE: Check if the block file is still opened.
// PROMISE: Return true if file is still opened, false if otherwise.
{
	return file_.is_open();
}

bool Block::Read(SECT index, char* block)
// PURPOSE: Read a block of data from the opened file at the index position.
// EXPLAIN: index is from [0..].
// PROMISE: Return true if data are successfully read, false if otherwise.
{
	if (!(mode_ & ios_base::in))
		return false;

	if (index < indexEnd_) {
		file_.seekg(index * blockSize_);
		file_.read(block, blockSize_);
		return !file_.fail();
	} else
		return false;
}

bool Block::Write(SECT index, const char* block)
// PURPOSE: Write a block of data to the opened file at the index position.
// EXPLAIN: index is from [0..].
// PROMISE: Return true if data are successfully written, false if otherwise.
{
	if (!(mode_ & ios_base::out))
		return false;

	file_.seekp(index * blockSize_);
	file_.write(block, blockSize_);

	if (indexEnd_ <= index) {
		indexEnd_ = index + 1;
		fileSize_ += blockSize_;
	}

	// flush the file immediatelly
	file_.flush();

	// return false on error
	return file_.good();
}

bool Block::Swap(SECT index1, SECT index2)
// PURPOSE: Swap two blocks of data in the opened file at the index positions.
// EXPLAIN: index1 and index2 are from [0..].
// PROMISE: Return true if data are successfully swapped, false if otherwise.
{
	if (!(mode_ & ios_base::out))
		return false;

	if (index1 < indexEnd_ && index2 < indexEnd_) {
		if (index1 == index2)
			return true;

		char* block1 = new char[blockSize_];
		if (!this->Read(index1, block1))
			return false;

		char* block2 = new char[blockSize_];
		if (!this->Read(index2, block2))
			return false;

		if (!this->Write(index1, block2))
			return false;

		if (!this->Write(index2, block1))
			return false;

		delete[] block1;
		delete[] block2;
		return true;
	} else
		return false;
}

bool Block::Move(SECT from, SECT to)
// PURPOSE: Move a block of data in the opened file from an index position to another index position.
// EXPLAIN: from and to are from [0..].
// PROMISE: Return true if data are successfully moved, false if otherwise.
{
	if (!(mode_ & ios_base::out))
		return false;

	if (from<indexEnd_ && to<indexEnd_) {
		if (to > from) {
			for(SECT i=from; i!=to; ++i) {
				if (!this->Swap(i, i+1))
					return false;
			}
		} else {
			for(SECT i=from; i!=to; --i) {
				if (!this->Swap(i, i-1))
					return false;
			}
		}

		return true;
	} else
		return false;
}

bool Block::Insert(SECT index, const char* block)
// PURPOSE: Insert a new block of data in the opened file at the index position.
// EXPLAIN: index is from [0..].
// PROMISE: Return true if data are successfully inserted, false if otherwise.
{
	if (!(mode_ & ios_base::out))
		return false;

	if (index <= indexEnd_) {
		// Write block to end of file
		if (!this->Write(indexEnd_, block))
			return false;

		// Move block to index if necessary
		if (index < indexEnd_-1)
			return this->Move(indexEnd_-1, index);
		else
			return true;
	} else {
		// Write block to index after end of file
		return this->Write(index, block);
	}
}

bool Block::Erase(SECT index)
// PURPOSE: Erase a block of data in the opened file at the index position.
// EXPLAIN: index is from [0..].
// PROMISE: Return true if data are successfully erased, false if otherwise.
{
	if (!(mode_ & ios_base::out))
		return false;

	if (index < indexEnd_) {
		fileSize_ -= blockSize_;
		indexEnd_ -= 1;

		// Read entire file except the block to be deleted into memory.
		char* buffer = new char[fileSize_];
		for(SECT i=0, j=0; i!=indexEnd_+1; ++i) {
			file_.seekg(i*blockSize_);
			if (i != index) {
				file_.read(buffer+j*blockSize_, blockSize_);
				++j;
			}
		}
// Close / Clear / Open may lead to bugs when another process accesss the file (ie.: Anti-Virus)
//		file_.close();
//		file_.open(&*(filename_.begin()), ios_base::out | ios_base::trunc | ios_base::binary);
		file_.write(buffer, fileSize_); // Write the new file.

		// flush the file immediatelly
		file_.flush();
//		file_.close();
//		file_.open(&*(filename_.begin()), mode_ | ios_base::binary);

		delete[] buffer;
		return true;
	} else
		return false;
}

bool Block::Erase(vector<SECT>& indices)
// PURPOSE: Erase blocks of data in the opened file at the index positions.
// EXPLAIN: Each index in indices is from [0..].
// PROMISE: Return true if data are successfully erased, false if otherwise.
{
	if (!(mode_ & ios_base::out))
		return false;

	// Read entire file except the blocks to be deleted into memory.
	ULONG maxIndices = (ULONG) indices.size();
	fileSize_ -= maxIndices*blockSize_;
	char* buffer = new char[fileSize_];
	for(SECT i=0, k=0; i!=indexEnd_; ++i) {
		file_.seekg(i*blockSize_);

		bool toDelete = false;
		for(size_t j=0; j<maxIndices; ++j) {
			if (i == indices[j]) {
				toDelete = true;
				break;
			}
		}

		if (!toDelete) {
			file_.read(buffer+k*blockSize_, blockSize_);
			++k;
		}
	}
	indexEnd_ -= maxIndices;

	file_.write(buffer, fileSize_); // Write the new file.
	// flush the file immediatelly
	file_.flush();

	delete[] buffer;
	return true;
}
/********************************** End of Class Block ***************************************/

/********************************** Start of Class Header ************************************/
// PURPOSE: Read and write data to a compound file header.
CompoundFile::Header::Header()
{
	_abSig = LONGINT_CONST(0xE11AB1A1E011CFD0);
	memset(&_clid, 0, sizeof(_clid));
	_ulMinorVersion = 0x3B;
	_uDllVersion = 3;
	_uByteOrder = 0xFFFE;
	_uSectorShift = 9;
	_uMiniSectorShift = 6;
	_ulReserved1 = 0;
	_ulReserved2 = 0;
	_csectFat = 1;
	_sectDirStat = 1;
	_signature = 0;
	_ulMiniSectorCutOff = 0x1000;
	_usReserved = 0;
	_sectMiniFatStart = ENDOFCHAIN;
	_csectMiniFat = 0;
	_sectDifStart = ENDOFCHAIN;
	_csectDif = 0;

	_sectFat[0] = 0;	// Initial BAT indices at block 0 (=block 1 in Block)
	fill(_sectFat+1, _sectFat+109, FREESECT);	// Rest of the BATArray is empty

	Initialize();
}

void CompoundFile::Header::Write(char* block)
// PURPOSE: Write header information into a block of data.
// REQUIRE: Block of data must be at least 512 bytes in size.
{
	LittleEndian::Write(block, _abSig, 0x0000, 8);
//	LittleEndian::Write(block, _clid, 0x0008, 16);
	LittleEndian::Write(block, *(const LONGINT*)&_clid.Data1, 0x0008, 8);
	LittleEndian::Write(block, *(const LONGINT*)&_clid.Data4, 0x0010, 8);
	LittleEndian::Write(block, _ulMinorVersion, 0x0018, 2);
	LittleEndian::Write(block, _uDllVersion, 0x001A, 2);
	LittleEndian::Write(block, _uByteOrder, 0x001C, 2);
	LittleEndian::Write(block, _uSectorShift, 0x001E, 2);
	LittleEndian::Write(block, _uMiniSectorShift, 0x0020, 2);
	LittleEndian::Write(block, _usReserved, 0x0022, 2);
	LittleEndian::Write(block, _ulReserved1, 0x0024, 4);
	LittleEndian::Write(block, _ulReserved2, 0x0028, 4);
	LittleEndian::Write(block, _csectFat, 0x002C, 4);
	LittleEndian::Write(block, _sectDirStat, 0x0030, 4);
	LittleEndian::Write(block, _signature, 0x0034, 4);
	LittleEndian::Write(block, _ulMiniSectorCutOff, 0x0038, 4);
	LittleEndian::Write(block, _sectMiniFatStart, 0x003C, 4);
	LittleEndian::Write(block, _csectMiniFat, 0x0040, 4);
	LittleEndian::Write(block, _sectDifStart, 0x0044, 4);
	LittleEndian::Write(block, _csectDif, 0x0048, 4);

	for(int i=0; i<109; ++i)
		LittleEndian::Write(block, _sectFat[i], 0x004C+i*4, 4);
}

void CompoundFile::Header::Read(char* block)
// PURPOSE: Read header information from a block of data.
// REQUIRE: Block of data must be at least 512 bytes in size.
{
	LittleEndian::Read(block, _abSig, 0x0000, 8);
//	LittleEndian::Read(block, _clid, 0x0008, 16);
	LittleEndian::Read(block, *(LONGINT*)&_clid.Data1, 0x0008, 8);
	LittleEndian::Read(block, *(LONGINT*)&_clid.Data4, 0x0010, 8);
	LittleEndian::Read(block, _ulMinorVersion, 0x0018, 2);
	LittleEndian::Read(block, _uDllVersion, 0x001A, 2);
	LittleEndian::Read(block, _uByteOrder, 0x001C, 2);
	LittleEndian::Read(block, _uSectorShift, 0x001E, 2);
	LittleEndian::Read(block, _uMiniSectorShift, 0x0020, 2);
	LittleEndian::Read(block, _usReserved, 0x0022, 2);
	LittleEndian::Read(block, _ulReserved1, 0x0024, 4);
	LittleEndian::Read(block, _ulReserved2, 0x0028, 4);
	LittleEndian::Read(block, _csectFat, 0x002C, 4);
	LittleEndian::Read(block, _sectDirStat, 0x0030, 4);
	LittleEndian::Read(block, _signature, 0x0034, 4);
	LittleEndian::Read(block, _ulMiniSectorCutOff, 0x0038, 4);
	LittleEndian::Read(block, _sectMiniFatStart, 0x003C, 4);
	LittleEndian::Read(block, _csectMiniFat, 0x0040, 4);
	LittleEndian::Read(block, _sectDifStart, 0x0044, 4);
	LittleEndian::Read(block, _csectDif, 0x0048, 4);

	for(int i=0; i<109; ++i)
		LittleEndian::Read(block, _sectFat[i], 0x004C+i*4, 4);

	Initialize();
}

void CompoundFile::Header::Initialize()
{
	bigBlockSize_ = 1 << _uSectorShift;			// Calculate each big block size.
	smallBlockSize_ = 1 << _uMiniSectorShift;	// Calculate each small block size.
}
/********************************** End of Class Header **************************************/

/********************************** Start of Class DirectoryEntry **********************************/
// PURPOSE: Read and write data to a compound file property.
CompoundFile::DirectoryEntry::DirectoryEntry()
{
	fill(name_, name_+32, 0);
	_cb_namesize = 0;
	_mse = STGTY_STORAGE;
	_bflags = DE_BLACK;
	_sidLeftSib = -1;
	_sidRightSib = -1;
	_sidChild = -1;
	memset(&_clsId, 0, sizeof(_clsId));
	_dwUserFlags = 0;
	_time[0].dwHighDateTime = 0;
	_time[0].dwLowDateTime = 0;
	_time[1].dwHighDateTime = 0;
	_time[1].dwLowDateTime = 0;
	_sectStart = ENDOFCHAIN;
	_ulSize = 0;
	_dptPropType = 0;
}

void CompoundFile::DirectoryEntry::Write(char* block)
// PURPOSE: Write property information from a block of data.
// REQUIRE: Block of data must be at least 128 bytes in size.
{
	LittleEndian::WriteString(block, name_, 0x00, 32);
	LittleEndian::Write(block, _cb_namesize, 0x40, 2);
	LittleEndian::Write(block, _mse, 0x42, 1);
	LittleEndian::Write(block, _bflags, 0x43, 1);
	LittleEndian::Write(block, _sidLeftSib, 0x44, 4);
	LittleEndian::Write(block, _sidRightSib, 0x48, 4);
	LittleEndian::Write(block, _sidChild, 0x4C, 4);
//	LittleEndian::Write(block, _clsId, 0x50, 16);
	LittleEndian::Write(block, *(const LONGINT*)&_clsId.Data1, 0x50, 16);
	LittleEndian::Write(block, *(const LONGINT*)&_clsId.Data4, 0x58, 16);
	LittleEndian::Write(block, _dwUserFlags, 0x60, 4);
	LittleEndian::Write(block, _time[0].dwLowDateTime, 0x64, 4);
	LittleEndian::Write(block, _time[0].dwHighDateTime, 0x68, 4);
	LittleEndian::Write(block, _time[1].dwLowDateTime, 0x6C, 4);
	LittleEndian::Write(block, _time[1].dwHighDateTime, 0x70, 4);
	LittleEndian::Write(block, _sectStart, 0x74, 4);
	LittleEndian::Write(block, _ulSize, 0x78, 4);
	LittleEndian::Write(block, _dptPropType, 0x7C, 2);
}

void CompoundFile::DirectoryEntry::Read(char* block)
// PURPOSE: Read property information from a block of data.
// REQUIRE: Block of data must be at least 128 bytes in size.
{
	LittleEndian::ReadString(block, name_, 0x00, 32);
	LittleEndian::Read(block, _cb_namesize, 0x40, 2);
	LittleEndian::Read(block, _mse, 0x42, 1);
	LittleEndian::Read(block, _bflags, 0x43, 1);
	LittleEndian::Read(block, _sidLeftSib, 0x44, 4);
	LittleEndian::Read(block, _sidRightSib, 0x48, 4);
	LittleEndian::Read(block, _sidChild, 0x4C, 4);
//	LittleEndian::Read(block, _clsId, 0x50, 16);
	LittleEndian::Read(block, *(LONGINT*)&_clsId.Data1, 0x50, 16);
	LittleEndian::Read(block, *(LONGINT*)&_clsId.Data4, 0x58, 16);
	LittleEndian::Read(block, _dwUserFlags, 0x60, 4);
	LittleEndian::Read(block, _time[0].dwLowDateTime, 0x64, 4);
	LittleEndian::Read(block, _time[0].dwHighDateTime, 0x68, 4);
	LittleEndian::Read(block, _time[1].dwLowDateTime, 0x6C, 4);
	LittleEndian::Read(block, _time[1].dwHighDateTime, 0x70, 4);
	LittleEndian::Read(block, _sectStart, 0x74, 4);
	LittleEndian::Read(block, _ulSize, 0x78, 4);
	LittleEndian::Read(block, _dptPropType, 0x7C, 2);
}
/********************************** End of Class DirectoryEntry ************************************/

/********************************** Start of Class PropertyTree **********************************/
CompoundFile::PropertyTree::PropertyTree() {}

CompoundFile::PropertyTree::~PropertyTree()
{
	size_t maxChildren = children_.size();

	for(size_t i=0; i<maxChildren; ++i)
		delete children_[i];
}
/********************************** End of Class PropertyTree ************************************/

/********************************** Start of Class CompoundFile ******************************/
// PURPOSE: Manage a compound file.
CompoundFile::CompoundFile() :
	block_(512), dirEntries_(0), propertyTrees_(0),
	blocksIndices_(0), sblocksIndices_(0) {}

CompoundFile::~CompoundFile() {this->Close();}

/************************* Compound File Functions ***************************/
bool CompoundFile::Create(const wchar_t* filename)
// PURPOSE: Create a new compound file and open it.
// PURPOSE: If file is present, truncate it and then open it.
// PROMISE: Return true if file is successfully created and opened, false if otherwise.
{
	Close();

	if (!file_.Create(filename))
		return false;

	// Write compound file header
	header_ = Header();
	SaveHeader();

	// Save BAT
	blocksIndices_.clear();
	blocksIndices_.resize(128, -1);
	blocksIndices_[0] = FATSECT;
	blocksIndices_[1] = ENDOFCHAIN;
	SaveBAT();

	// Save properties
	DirectoryEntry* root = new DirectoryEntry;
	wcscpy(root->name_, L"Root Entry");
	root->_mse = STGTY_ROOT;
	dirEntries_.push_back(root);
	SaveProperties();

	// Set property tree
	propertyTrees_ = new PropertyTree;
	propertyTrees_->parent_ = 0;
	propertyTrees_->self_ = dirEntries_[0];
	propertyTrees_->index_ = 0;
	currentDirectory_ = propertyTrees_;

	return true;
}

bool CompoundFile::Open(const wchar_t* filename, ios_base::openmode mode)
// PURPOSE: Open an existing compound file.
// PROMISE: Return true if file is successfully opened, false if otherwise.
{
	Close();
	if (!file_.Open(filename, mode)) return false;

	// Load header
	if (!LoadHeader()) return false;

	// Load BAT information
	LoadBAT();

	// Load properties
	propertyTrees_ = new PropertyTree;
	LoadProperties();
	currentDirectory_ = propertyTrees_;

	return true;
}

bool CompoundFile::Close()
// PURPOSE: Close the opened compound file.
// PURPOSE: Reset BAT indices, SBAT indices, properties and properties tree information.
// PROMISE: Return true if file is successfully closed, false if otherwise.
{
	blocksIndices_.clear();
	sblocksIndices_.clear();

	ULONG maxProperties = (ULONG) dirEntries_.size();
	for(size_t i=0; i<maxProperties; ++i) {
		if (dirEntries_[i])
			delete dirEntries_[i];
	}
	dirEntries_.clear();

	if (propertyTrees_) {
		delete propertyTrees_;
		propertyTrees_ = 0;
	}

	previousDirectories_.clear();
	currentDirectory_ = 0;

	if (file_.IsOpen())
		file_.Close();

	return true;
}

bool CompoundFile::IsOpen()
// PURPOSE: Check if the compound file is still opened.
// PROMISE: Return true if file is still opened, false if otherwise.
{
	return file_.IsOpen();
}

/************************* Directory Functions ***************************/
int CompoundFile::ChangeDirectory(const wchar_t* path)
// PURPOSE: Change to a different directory in the compound file.
// PROMISE: Current directory will not be changed if directory is not present.
{
	previousDirectories_.push_back(currentDirectory_);

	// Handle special cases
	if (wcscmp(path, L".") == 0)
	{
		// Current directory
		previousDirectories_.pop_back();
		return SUCCESS;
	}
	if (wcscmp(path, L"..") == 0)
	{
		// Go up 1 directory
		if (currentDirectory_->parent_ != 0)
		{
			currentDirectory_ = currentDirectory_->parent_;
		}
		previousDirectories_.pop_back();
		return SUCCESS;
	}
	if (wcscmp(path, L"\\") == 0)
	{
		// Go to root directory
		currentDirectory_ = propertyTrees_;
		previousDirectories_.pop_back();
		return SUCCESS;
	}

	// Handle normal cases
	ULONG ipos = 0;
	ULONG npos = 0;
	size_t pathLength = wcslen(path);

	if (pathLength > 0 && path[0] == L'\\') {
		// Start from root directory
		currentDirectory_ = propertyTrees_;
		++ipos;
		++npos;
	}

	do {
		for(; npos<pathLength; ++npos) {
			if (path[npos] == L'\\')
				break;
		}

		wchar_t* directory = new wchar_t[npos-ipos+1];
		copy(path+ipos, path+npos, directory);
		directory[npos-ipos] = 0;
		currentDirectory_ = FindProperty(currentDirectory_, directory);
		delete[] directory;
		ipos = npos + 1;
		npos = ipos;
		if (currentDirectory_ == 0)
		{
			// Directory not found
			currentDirectory_ = previousDirectories_.back();
			previousDirectories_.pop_back();
			return DIRECTORY_NOT_FOUND;
		}
	} while(npos < pathLength);
	previousDirectories_.pop_back();
	return SUCCESS;
}

int CompoundFile::MakeDirectory(const wchar_t* path)
// PURPOSE: Create a new directory in the compound file.
// PROMISE: Directory will not be created if it is already present or
// PROMISE: a file with the same name is present.
{
	previousDirectories_.push_back(currentDirectory_);
	DirectoryEntry* property = new DirectoryEntry;
	property->_mse = STGTY_STORAGE;
	int ret = MakeProperty(path, property);
	currentDirectory_ = previousDirectories_.back();
	previousDirectories_.pop_back();
	SaveHeader();
	SaveBAT();
	SaveProperties();
	return ret;
}

/************************* File Functions ***************************/
int CompoundFile::MakeFile(const wchar_t* path)
// PURPOSE: Create a new file in the compound file.
// PROMISE: File will not be created if it is already present or
// PROMISE: a directory with the same name is present.
{
	previousDirectories_.push_back(currentDirectory_);
	DirectoryEntry* property = new DirectoryEntry;
	property->_mse = STGTY_STREAM;
	int ret = MakeProperty(path, property);
	currentDirectory_ = previousDirectories_.back();
	previousDirectories_.pop_back();
	SaveHeader();
	SaveBAT();
	SaveProperties();
	return ret;
}
	
int CompoundFile::FileSize(const wchar_t* path, ULONG& size)
// PURPOSE: Get the size of a file in the compound file.
// PROMISE: Return the data size stored in the Root Entry if path = "\".
// PROMISE: size will not be set if file is not present in the compound file.
{
	// Special case of reading root entry
	if (wcscmp(path, L"\\") == 0)
	{
		size = propertyTrees_->self_->_ulSize;
		return SUCCESS;
	}

	// Check to see if file is present in the specified directory.
	PropertyTree* property = FindProperty(path);
	if (!property)
		return FILE_NOT_FOUND;
	else {
		size = property->self_->_ulSize;
		return SUCCESS;
	}
}

int CompoundFile::ReadFile(const wchar_t* path, char* data)
// PURPOSE: Read a file's data in the compound file.
// REQUIRE: data must be large enough to receive the file's data.
// REQUIRE: The required data size can be obtained by using FileSize().
// PROMISE: Returns the small blocks of data stored by the Root Entry if path = "\".
// PROMISE: data will not be set if file is not present in the compound file.
 {
	// Special case of reading root entry
	char* buffer;
	if (wcscmp(path, L"\\") == 0)
	{
		buffer = new char[DataSize(propertyTrees_->self_->_sectStart, true)];
		ReadData(propertyTrees_->self_->_sectStart, buffer, true);
		copy(buffer, buffer+propertyTrees_->self_->_ulSize, data);
		delete[] buffer;
		return SUCCESS;
	}

	// Check to see if file is present in the specified directory.
	PropertyTree* property = FindProperty(path);
	if (!property)
		return FILE_NOT_FOUND;

	if (property->self_->_ulSize >= 4096)
	{
		// Data stored in normal big blocks
		buffer = new char[DataSize(property->self_->_sectStart, true)];
		ReadData(property->self_->_sectStart, buffer, true);
	}
	else
	{
		// Data stored in small blocks
		buffer = new char[DataSize(property->self_->_sectStart, false)];
		ReadData(property->self_->_sectStart, buffer, false);
	}
	// Truncated the retrieved data to the actual file size.
	copy(buffer, buffer+property->self_->_ulSize, data);
	delete[] buffer;
	return SUCCESS;
}

int CompoundFile::ReadFile(const wchar_t* path, vector<char>& data)
// PURPOSE: Read a file's data in the compound file.
// PROMISE: Returns the small blocks of data stored by the Root Entry if path = "\".
// PROMISE: data will not be set if file is not present in the compound file.
{
	data.clear();

	ULONG dataSize;

	int ret = FileSize(path, dataSize);
	if (ret != SUCCESS)
		return ret;

	data.resize(dataSize);

	return ReadFile(path, &*(data.begin()));
}

int CompoundFile::WriteFile(const wchar_t* path, const char* data, ULONG size)
// PURPOSE: Write data to a file in the compound file.
// PROMISE: The file's original data will be replaced by the new data.
{
	PropertyTree* property = FindProperty(path);
	if (!property)
		return FILE_NOT_FOUND;

	if (property->self_->_ulSize >= 4096) {
		if (size >= 4096)
			property->self_->_sectStart = WriteData(data, size, property->self_->_sectStart, true);
		else {
			property->self_->_sectStart = WriteData(0, 0, property->self_->_sectStart, true);
			property->self_->_sectStart = WriteData(data, size, property->self_->_sectStart, false);
		}
	} else {
		if (size < 4096)
			property->self_->_sectStart = WriteData(data, size, property->self_->_sectStart, false);
		else {
			property->self_->_sectStart = WriteData(0, 0, property->self_->_sectStart, false);
			property->self_->_sectStart = WriteData(data, size, property->self_->_sectStart, true);
		}
	}
	property->self_->_ulSize = size;

	SaveHeader();
	SaveBAT();
	SaveProperties();

	return SUCCESS;
}

int CompoundFile::WriteFile(const wchar_t* path, const vector<char>& data, ULONG size)
// PURPOSE: Write data to a file in the compound file.
// PROMISE: The file's original data will be replaced by the new data.
{
	return WriteFile(path, &*(data.begin()), size);
}

/*************ANSI char compound file, directory and file functions******************/
bool CompoundFile::Create(const char* filename)
{
	size_t filenameLength = strlen(filename);
	wchar_t* wname = new wchar_t[filenameLength+1];
	mbstowcs(wname, filename, filenameLength);
	wname[filenameLength] = 0;
	bool ret = Create(wname);
	delete[] wname;
	return ret;
}

bool CompoundFile::Open(const char* filename, ios_base::openmode mode)
{
	size_t filenameLength = strlen(filename);
	wchar_t* wname = new wchar_t[filenameLength+1];
	mbstowcs(wname, filename, filenameLength);
	wname[filenameLength] = 0;
	bool ret = Open(wname, mode);
	delete[] wname;
	return ret;
}

int CompoundFile::ChangeDirectory(const char* path)
{
	size_t pathLength = strlen(path);
	wchar_t* wpath = new wchar_t[pathLength+1];
	mbstowcs(wpath, path, pathLength);
	wpath[pathLength] = 0;
	int ret = ChangeDirectory(wpath);
	delete[] wpath;
	return ret;
}

int CompoundFile::MakeDirectory(const char* path)
{
	size_t pathLength = strlen(path);
	wchar_t* wpath = new wchar_t[pathLength+1];
	mbstowcs(wpath, path, pathLength);
	wpath[pathLength] = 0;
	int ret = MakeDirectory(wpath);
	delete[] wpath;
	return ret;
}

int CompoundFile::MakeFile(const char* path)
{
	size_t pathLength = strlen(path);
	wchar_t* wpath = new wchar_t[pathLength+1];
	mbstowcs(wpath, path, pathLength);
	wpath[pathLength] = 0;
	int ret = MakeFile(wpath);
	delete[] wpath;
	return ret;
}

int CompoundFile::FileSize(const char* path, ULONG& size)
{
	size_t pathLength = strlen(path);
	wchar_t* wpath = new wchar_t[pathLength+1];
	mbstowcs(wpath, path, pathLength);
	wpath[pathLength] = 0;

	int ret = FileSize(wpath, size);

	delete[] wpath;
	return ret;
}
int CompoundFile::ReadFile(const char* path, char* data)
{
	size_t pathLength = strlen(path);
	wchar_t* wpath = new wchar_t[pathLength+1];
	mbstowcs(wpath, path, pathLength);
	wpath[pathLength] = 0;
	int ret = ReadFile(wpath, data);
	delete[] wpath;
	return ret;
}
int CompoundFile::ReadFile(const char* path, vector<char>& data)
{
	size_t pathLength = strlen(path);
	wchar_t* wpath = new wchar_t[pathLength+1];
	mbstowcs(wpath, path, pathLength);
	wpath[pathLength] = 0;

	int ret = ReadFile(wpath, data);

	delete[] wpath;
	return ret;
}
int CompoundFile::WriteFile(const char* path, const char* data, ULONG size)
{
	size_t pathLength = strlen(path);
	wchar_t* wpath = new wchar_t[pathLength+1];
	mbstowcs(wpath, path, pathLength);
	wpath[pathLength] = 0;

	int ret = WriteFile(wpath, data, size);

	delete[] wpath;
	return ret;
}
int CompoundFile::WriteFile(const char* path, const vector<char>& data, ULONG size)
{
	size_t pathLength = strlen(path);
	wchar_t* wpath = new wchar_t[pathLength+1];
	mbstowcs(wpath, path, pathLength);
	wpath[pathLength] = 0;

	int ret = WriteFile(wpath, data, size);

	delete[] wpath;
	return ret;
}

/*********************** Inaccessible General Functions ***************************/
void CompoundFile::IncreaseLocationReferences(vector<SECT> indices)
// PURPOSE: Increase block location references in header, BAT indices and properties,
// PURPOSE: which will be affected by the insertion of new indices contained in indices.
// PROMISE: Block location references which are smaller than all the new indices
// PROMISE: will not be affected.
// PROMISE: SBAT location references will not be affected.
// PROMISE: Changes will not be written to compound file.
{
	ULONG maxIndices = (ULONG) indices.size();

	// Change BAT Array references
	{for(int i=0; i<109 && header_._sectFat[i]!=FREESECT; ++i) {
		ULONG count = 0;

		for(size_t j=0; j<maxIndices; ++j) {
			if (header_._sectFat[i] >= indices[j] &&
				header_._sectFat[i] != FREESECT)
				++count;
		}
		header_._sectFat[i] += count;
	}}

	// Change XBAT start block if any
	if (header_._csectDif) {
		ULONG count = 0;
		for(size_t j=0; j<maxIndices; ++j) {
			if (header_._sectDifStart >= indices[j] &&
				header_._sectDifStart != ENDOFCHAIN) ++count;
		}

		header_._sectDifStart += count;
	}

	// Change SBAT start block if any
	if (header_._csectMiniFat) {
		ULONG count = 0;
		for(size_t j=0; j<maxIndices; ++j) {
			if (header_._sectMiniFatStart >= indices[j] &&
				header_._sectMiniFatStart != ENDOFCHAIN) ++count;
		}

		header_._sectMiniFatStart += count;
	}

	// Change BAT block indices
	size_t maxBATindices = blocksIndices_.size();
	{for(size_t i=0; i<maxBATindices && blocksIndices_[i]!=FREESECT; ++i)
	{
		ULONG count = 0;
		for(size_t j=0; j<maxIndices; ++j) {
			if (blocksIndices_[i] > indices[j] &&
				blocksIndices_[i] != ENDOFCHAIN &&
				blocksIndices_[i] != FATSECT)
				++count;
		}

		blocksIndices_[i] += count;
	}}

	// Change properties start block
	ULONG count = 0;
	{for(size_t i=0; i<maxIndices; ++i) {
		if (header_._sectDirStat >= indices[i] &&
			header_._sectDirStat != ENDOFCHAIN)
			++count;
	}}
	header_._sectDirStat += count;

	// Change individual properties start block if their size is more than 4096
	ULONG maxProperties = (ULONG) dirEntries_.size();

	if (!dirEntries_.empty()) {
		ULONG count = 0;
		for(size_t j=0; j<maxIndices; ++j) {
			if (dirEntries_[0]->_sectStart >= indices[j] &&
				dirEntries_[0]->_sectStart != ENDOFCHAIN)
				++count;
		}

		dirEntries_[0]->_sectStart += count;
	}

	{for(size_t i=1; i<maxProperties; ++i) {
		if (dirEntries_[i]->_ulSize >= 4096) {
			ULONG count = 0;
			for(size_t j=0; j<maxIndices; ++j) {
				if (dirEntries_[i]->_sectStart >= indices[j] &&
					dirEntries_[i]->_sectStart != ENDOFCHAIN)
					++count;
			}

			dirEntries_[i]->_sectStart += count;
		}
	}}
}

void CompoundFile::DecreaseLocationReferences(vector<SECT> indices)
// PURPOSE: Decrease block location references in header, BAT indices and properties,
// PURPOSE: which will be affected by the deletion of indices contained in indices.
// PROMISE: BAT indices pointing to a deleted index will be redirected to point to
// PROMISE: the location where the deleted index original points to.
// PROMISE: Block location references which are smaller than all the new indices
// PROMISE: will not be affected.
// PROMISE: SBAT location references will not be affected.
// PROMISE: Changes will not be written to compound file.
{
	ULONG maxIndices = (ULONG) indices.size();

	// Change BAT Array references
	{for(int i=0; i<109 && header_._sectFat[i]!=FREESECT; ++i) {
		ULONG count = 0;
		for(size_t j=0; j<maxIndices; ++j) {
			if (header_._sectFat[i] > indices[j] &&
				header_._sectFat[i] != FREESECT) ++count;
		}
		header_._sectFat[i] -= count;
	}}

	// Change XBAT start block if any
	if (header_._csectDif) {
		ULONG count = 0;
		for(size_t j=0; j<maxIndices; ++j) {
			if (header_._sectDifStart > indices[j] &&
				header_._sectDifStart != ENDOFCHAIN) ++count;
		}

		header_._sectDifStart -= count;
	}

	// Change SBAT start block if any
	if (header_._csectMiniFat) {
		ULONG count = 0;
		for(size_t j=0; j<maxIndices; ++j) {
			if (header_._sectMiniFatStart > indices[j] &&
				header_._sectMiniFatStart != ENDOFCHAIN) ++count;
		}

		header_._sectMiniFatStart -= count;
	}

	// Change BAT block indices
	// Redirect BAT indices pointing to a deleted index to point to
	// the location where the deleted index original points to.
	size_t maxBATindices = blocksIndices_.size();
	{for(size_t i=0; i<maxBATindices && blocksIndices_[i]!=FREESECT; ++i)
	{
		bool end;
		do
		{
			end = true;
			for(size_t j=0; j<maxIndices; ++j)
			{
				if (blocksIndices_[i] == indices[j])
				{
					blocksIndices_[i] = blocksIndices_[indices[j]];
					end = false;
					break;
				}
			}
		} while(!end);
	}}
	// Erase indices to be deleted from the block indices
	sort (indices.begin(), indices.end(), greater<size_t>());
	{for(size_t i=0; i<maxIndices; ++i)
	{
		blocksIndices_.erase(blocksIndices_.begin()+indices[i]);
		blocksIndices_.push_back(-1);
	}}

	// Decrease block location references for affected block indices.
	{for(size_t i=0; i<maxBATindices && blocksIndices_[i]!=FREESECT; ++i)
	{
		ULONG count = 0;
		for(size_t j=0; j<maxIndices; ++j)
		{
			if (blocksIndices_[i] > indices[j] &&
				blocksIndices_[i] != ENDOFCHAIN &&
				blocksIndices_[i] != FATSECT)
				++count;
		}
		blocksIndices_[i] -= count;
	}}

	// Change properties start block
	ULONG count = 0;
	{for(size_t i=0; i<maxIndices; ++i) {
		if (header_._sectDirStat > indices[i] &&
			header_._sectDirStat != ENDOFCHAIN)
			++count;
	}}
	header_._sectDirStat -= count;

	ULONG maxProperties = (ULONG) dirEntries_.size();
	// Change Root Entry start block
	if (!dirEntries_.empty()) {
		ULONG count = 0;
		for(size_t j=0; j<maxIndices; ++j) {
			if (dirEntries_[0]->_sectStart > indices[j] &&
				dirEntries_[0]->_sectStart != ENDOFCHAIN)
				++count;
		}

		dirEntries_[0]->_sectStart -= count;
	}
	{for(size_t i=1; i<maxProperties; ++i) {
		if (dirEntries_[i]->_ulSize >= 4096) {
			// Change individual properties start block if their size is more than 4096
			ULONG count = 0;
			for(size_t j=0; j<maxIndices; ++j) {
				if (dirEntries_[i]->_sectStart > indices[j] &&
					dirEntries_[i]->_sectStart != ENDOFCHAIN)
					++count;
			}

			dirEntries_[i]->_sectStart -= count;
		}
	}}
}

void CompoundFile::SplitPath(const wchar_t* path,
							 wchar_t*& parentpath,
							 wchar_t*& propertyname)
// PURPOSE: Get a path's parent path and its name.
// EXPLAIN: E.g. path = "\\Abc\\def\\ghi => parentpath = "\\Abc\\def", propertyname = "ghi".
// REQUIRE: Calling function is responsible for deleting the memory created for
// REQUIRE: parentpath and propertyname.
{
	int pathLength = (int) wcslen(path);

	int npos;
	for(npos=pathLength-1; npos>0; --npos) {
		if (path[npos] == L'\\')
			break;
	}

	if (npos != 0) {
		// Get parent path if available
		parentpath = new wchar_t[npos+1];
		copy(path, path+npos, parentpath);
		parentpath[npos] = 0;
		++npos;
	}

	// Get property name (ignore initial "\" if present)
	if (npos==0 && pathLength>0 && path[0]==L'\\')
		++npos;

	propertyname = new wchar_t[pathLength-npos+1];
	copy(path+npos, path+pathLength, propertyname);
	propertyname[pathLength-npos] = 0;
}

/*********************** Inaccessible Header Functions ***************************/
bool CompoundFile::LoadHeader()
// PURPOSE: Load header information for compound file.
// PROMISE: Return true if file header contain magic number, false if otherwise.
{
	file_.Read(0, &*(block_.begin()));
	header_.Read(&*(block_.begin()));

	// Check magic number to see if it is a compound file
	if (header_._abSig != LONGINT_CONST(0xE11AB1A1E011CFD0))
		return false;

	block_.resize(header_.bigBlockSize_);		// Resize buffer block
	file_.SetBlockSize(header_.bigBlockSize_);	// Resize block array block size

	return true;
}

void CompoundFile::SaveHeader()
// PURPOSE: Save header information for compound file.
{
	header_.Write(&*(block_.begin()));
	file_.Write(0, &*(block_.begin()));
}

/*********************** Inaccessible BAT Functions ***************************/
void CompoundFile::LoadBAT()
// PURPOSE: Load all block allocation table information for compound file.
{
	// Read BAT indices
	{for(size_t i=0; i<header_._csectFat; ++i) {
		// Load blocksIndices_
		blocksIndices_.resize(blocksIndices_.size()+128, -1);
		file_.Read(header_._sectFat[i]+1, &*(block_.begin()));

		for(int j=0; j<128; ++j)
			LittleEndian::Read(&*(block_.begin()), blocksIndices_[j+i*128], j*4, 4);
	}}

	// Read XBAT indices
	{for(FSINDEX i=0; i<header_._csectDif; ++i) {
		blocksIndices_.resize(blocksIndices_.size()+128, -1);
		file_.Read(header_._sectDifStart+i+1, &*(block_.begin()));

		for(int j=0; j<128; ++j)
			LittleEndian::Read(&*(block_.begin()), blocksIndices_[j+((i+109)*128)], j*4, 4);
	}}

	// Read SBAT indices
	{for(FSINDEX i=0; i<header_._csectMiniFat; ++i) {
		sblocksIndices_.resize(sblocksIndices_.size()+128, -1);
		file_.Read(header_._sectMiniFatStart+i+1, &*(block_.begin()));

		for(int j=0; j<128; ++j)
			LittleEndian::Read(&*(block_.begin()), sblocksIndices_[j+i*128], j*4, 4);
	}}
}

void CompoundFile::SaveBAT()
// PURPOSE: Save all block allocation table information for compound file.
{
	// Write BAT indices
	{for(FSINDEX i=0; i<header_._csectFat; ++i) {
		for(int j=0; j<128; ++j)
			LittleEndian::Write(&*(block_.begin()), blocksIndices_[j+i*128], j*4, 4);
		file_.Write(header_._sectFat[i]+1, &*(block_.begin()));
	}}

	// Write XBAT indices
	{for(FSINDEX i=0; i<header_._csectDif; ++i) {
		for(int j=0; j<128; ++j)
			LittleEndian::Write(&*(block_.begin()), blocksIndices_[j+((i+109)*128)], j*4, 4);
		file_.Write(header_._sectDifStart+i+1, &*(block_.begin()));
	}}

	// Write SBAT indices
	{for(FSINDEX i=0; i<header_._csectMiniFat; ++i) {
		for(int j=0; j<128; ++j)
			LittleEndian::Write(&*(block_.begin()), sblocksIndices_[j+i*128], j*4, 4);
		file_.Write(header_._sectMiniFatStart+i+1, &*(block_.begin()));
	}}
}

ULONG CompoundFile::DataSize(SECT startIndex, bool isBig)
// PURPOSE: Gets the total size occupied by a property, starting from startIndex.
// EXPLAIN: isBig is true if property uses big blocks, false if it uses small blocks.
// PROMISE: Returns the total size occupied by the property which is the total
// PROMISE: number of blocks occupied multiply by the block size.
{
	vector<SECT> indices;

	if (isBig) {
		GetBlockIndices(startIndex, indices, true);
		return (ULONG)indices.size() * header_.bigBlockSize_;
	} else {
		GetBlockIndices(startIndex, indices, false);
		return (ULONG)indices.size() * header_.smallBlockSize_;
	}
}

ULONG CompoundFile::ReadData(SECT startIndex, char* data, bool isBig)
// PURPOSE: Read a property's data, starting from startIndex.
// REQUIRE: data must be large enough to receive the property's data
// REQUIRE: The required data size can be obtained by using DataSize().
// EXPLAIN: isBig is true if property uses big blocks, false if it uses small blocks.
// PROMISE: Returns the total size occupied by the property which is the total
// PROMISE: number of blocks occupied multiply by the block size.
{
	vector<SECT> indices;

	if (isBig) {
		GetBlockIndices(startIndex, indices, true);
		ULONG maxIndices = (ULONG) indices.size();

		for(size_t i=0; i<maxIndices; ++i)
			file_.Read(indices[i]+1, data+i*header_.bigBlockSize_);

		return maxIndices*header_.bigBlockSize_;
	} else {
		GetBlockIndices(startIndex, indices, false);
		size_t minIndex = *min_element(indices.begin(), indices.end());
//		size_t maxIndex = *max_element(indices.begin(), indices.end());
		size_t smallBlocksPerBigBlock = header_.bigBlockSize_ / header_.smallBlockSize_;
		size_t minBlock = minIndex / smallBlocksPerBigBlock;
//		size_t maxBlock = maxIndex / smallBlocksPerBigBlock +
//						  (maxIndex % smallBlocksPerBigBlock ? 1 : 0);
//		size_t totalBlocks = maxBlock - minBlock;
		char* buffer = new char[DataSize(dirEntries_[0]->_sectStart, true)];
		ReadData(dirEntries_[0]->_sectStart, buffer, true);

		ULONG maxIndices = (ULONG) indices.size();
		for(size_t i=0; i<maxIndices; ++i) {
			size_t start = (indices[i] - minBlock*smallBlocksPerBigBlock)*header_.smallBlockSize_;
			copy(buffer+start,
				  buffer+start+header_.smallBlockSize_,
				  data+i*header_.smallBlockSize_);
		}

		delete[] buffer;
		return maxIndices*header_.smallBlockSize_;
	}
}

SECT CompoundFile::WriteData(const char* data, ULONG size, SECT startIndex, bool isBig)
// PURPOSE: Write data to a property, starting from startIndex.
// EXPLAIN: startIndex can be ENDOFCHAIN if property initially has no data.
// EXPLAIN: isBig is true if property uses big blocks, false if it uses small blocks.
// PROMISE: The file's original data will be replaced by the new data.
// PROMISE: Returns the startIndex of new data for the property.
{
	if (isBig) {
		if (size==0 && startIndex==ENDOFCHAIN)
			return startIndex;

		// Get present indices
		vector<SECT> indices;
		GetBlockIndices(startIndex, indices, true);
		ULONG maxPresentBlocks = (ULONG) indices.size();

		// Calculate how many blocks does the data need
		ULONG extraSize = size % header_.bigBlockSize_;
		ULONG maxNewBlocks = size / header_.bigBlockSize_ + (extraSize ? 1 : 0);

		// Readjust indices and remove blocks if new data size is smaller than original
		int extraBlocks = maxPresentBlocks - maxNewBlocks;
		if (extraBlocks > 0) {
			// Place new end marker
			if (maxNewBlocks != 0)
				blocksIndices_[indices[maxNewBlocks]-1] = ENDOFCHAIN;
			else
				startIndex = ENDOFCHAIN;

			// Get indices of blocks to delete
			vector<SECT> indicesToRemove(extraBlocks);
			copy(indices.begin()+maxNewBlocks, indices.end(), indicesToRemove.begin());
			indices.erase(indices.begin()+maxNewBlocks, indices.end());

			// Remove extra blocks and readjust indices
			FreeBlocks(indicesToRemove, true);
		}

		// Write blocks into available space
		size_t remainingFullBlocks = size / header_.bigBlockSize_;
		size_t curIndex=0;
		if (maxPresentBlocks != 0) {
			for(; remainingFullBlocks && curIndex<maxPresentBlocks; --remainingFullBlocks, ++curIndex)
				file_.Write(indices[curIndex]+1, data+curIndex*header_.bigBlockSize_);
		}

		// Check if all blocks have been written
		SECT index;

		if (indices.empty())
			index = 0;
		else if (curIndex == 0)
			index = indices[0];
		else
			index = (startIndex != ENDOFCHAIN) ? indices[curIndex-1] : 0;

		if (remainingFullBlocks != 0) {
			// Require extra blocks to write data (i.e. new data is larger than original data
			do {
				SECT newIndex = GetFreeBlockIndex(true); // Get new free block to write data

				if (startIndex == ENDOFCHAIN)
					startIndex = newIndex; // Get start index
				else
					LinkBlocks(index, newIndex, true); // Link last index to new index

				file_.Write(newIndex+1, data+curIndex*header_.bigBlockSize_);
				++curIndex;
				index = newIndex;
			} while(--remainingFullBlocks);
		}

		if (extraSize != 0) {
			SECT newIndex;

			if (curIndex >= maxPresentBlocks) {
				// No more free blocks to write extra block data
				newIndex = GetFreeBlockIndex(true); // Get new free block to write data

				if (startIndex == ENDOFCHAIN)
					startIndex = newIndex;
				else
					LinkBlocks(index, newIndex,true);
			} else
				newIndex = indices[curIndex];

			// Write extra block after increasing its size to the minimum block size
			vector<char> tempdata(header_.bigBlockSize_, 0);
			copy(data+curIndex*header_.bigBlockSize_, data+curIndex*header_.bigBlockSize_+extraSize, tempdata.begin());
			file_.Write(newIndex+1, &*(tempdata.begin()));
		}
		return startIndex;
	}
	else
	{
		if (size==0 && startIndex==ENDOFCHAIN)
			return startIndex;

		if (size != 0 && dirEntries_[0]->_sectStart == ENDOFCHAIN) {
			SECT newIndex = GetFreeBlockIndex(true);
			fill (block_.begin(), block_.end(), 0);
			file_.Insert(newIndex, &*(block_.begin()));
			IncreaseLocationReferences(vector<SECT>(1, newIndex));
			dirEntries_[0]->_sectStart = newIndex;
			dirEntries_[0]->_ulSize = header_.bigBlockSize_;
		}

		// Get present indices
		vector<SECT> indices;
		GetBlockIndices(startIndex, indices, false);
		ULONG maxPresentBlocks = (ULONG) indices.size();

		// Calculate how many blocks does the data need
		ULONG extraSize = size % header_.smallBlockSize_;
		ULONG maxNewBlocks = size / header_.smallBlockSize_ + (extraSize ? 1 : 0);

		vector<char> smallBlocksData;
		int extraBlocks = maxPresentBlocks - maxNewBlocks;
		if (extraBlocks > 0) {
			// Readjust indices and remove blocks
			// Place new end marker
			if (maxNewBlocks != 0)
				sblocksIndices_[indices[maxNewBlocks]-1] = ENDOFCHAIN;
			else
				startIndex = ENDOFCHAIN;

			// Get indices of blocks to delete
			vector<SECT> indicesToRemove(extraBlocks);
			copy(indices.begin()+maxNewBlocks, indices.end(), indicesToRemove.begin());
			indices.erase(indices.begin()+maxNewBlocks, indices.end());

			// Remove extra blocks and readjust indices
			FreeBlocks(indicesToRemove, false);
		} else if (extraBlocks < 0) {
			ULONG maxBlocks = dirEntries_[0]->_ulSize / header_.bigBlockSize_ +
							   (dirEntries_[0]->_ulSize % header_.bigBlockSize_ ? 1 : 0);
			size_t actualSize = maxBlocks * header_.bigBlockSize_;
			smallBlocksData.resize(actualSize);
			ReadData(dirEntries_[0]->_sectStart, &*(smallBlocksData.begin()), true);
			smallBlocksData.resize(dirEntries_[0]->_ulSize);

			// Readjust indices and add blocks
			ULONG newBlocksNeeded = -extraBlocks;
			SECT index = maxPresentBlocks - 1;
			for(size_t i=0; i<newBlocksNeeded; ++i) {
				SECT newIndex = GetFreeBlockIndex(false); // Get new free block to write data

				if (startIndex == ENDOFCHAIN)
					startIndex = newIndex; // Get start index
				else
					LinkBlocks(index, newIndex, false);  // Link last index to new index

				smallBlocksData.insert(smallBlocksData.begin()+newIndex,
									   header_.smallBlockSize_, 0);
				index = newIndex;
			}

			dirEntries_[0]->_ulSize = newBlocksNeeded * header_.smallBlockSize_;
		}

		if (smallBlocksData.empty()) {
			ULONG maxBlocks = dirEntries_[0]->_ulSize / header_.bigBlockSize_ +
							   (dirEntries_[0]->_ulSize % header_.bigBlockSize_ ? 1 : 0);
			size_t actualSize = maxBlocks * header_.bigBlockSize_;
			smallBlocksData.resize(actualSize);
			ReadData(dirEntries_[0]->_sectStart, &*(smallBlocksData.begin()), true);
			smallBlocksData.resize(dirEntries_[0]->_ulSize);
		}

		// Write blocks
		GetBlockIndices(startIndex, indices, false);
		size_t fullBlocks = size / header_.smallBlockSize_;
		for(size_t i=0; i<fullBlocks; ++i) {
			copy(data+i*header_.smallBlockSize_,
				  data+i*header_.smallBlockSize_+header_.smallBlockSize_,
				  smallBlocksData.begin()+indices[i]*header_.smallBlockSize_);
		}

		if (extraSize != 0) {
			copy(data+fullBlocks*header_.smallBlockSize_,
				  data+fullBlocks*header_.smallBlockSize_+extraSize,
				  smallBlocksData.begin()+indices[fullBlocks]*header_.smallBlockSize_);
		}
		WriteData(&*(smallBlocksData.begin()), dirEntries_[0]->_ulSize, dirEntries_[0]->_sectStart, true);

		return startIndex;
	}
}

void CompoundFile::GetBlockIndices(SECT startIndex, vector<SECT>& indices, bool isBig)
// PURPOSE: Get the indices of blocks where data are stored, starting from startIndex.
// EXPLAIN: isBig is true if property uses big blocks, false if it uses small blocks.
{
	indices.clear();

	if (isBig) {
		for(SECT i=startIndex; i!=ENDOFCHAIN; i=blocksIndices_[i])
			indices.push_back(i);
	} else {
		for(SECT i=startIndex; i!=ENDOFCHAIN; i=sblocksIndices_[i])
			indices.push_back(i);
	}
}

SECT CompoundFile::GetFreeBlockIndex(bool isBig)
// PURPOSE: Get the index of a new block where data can be stored.
// EXPLAIN: isBig is true if property uses big blocks, false if it uses small blocks.
// PROMISE: It does not physically create a new block in the compound file.
// PROMISE: It only adjust BAT arrays and indices or SBAT arrays and indices so that
// PROMISE: it gives the index of a new block where data can be inserted.
{
	SECT index;

	if (isBig) {
		// Find first free location
		index = (SECT) distance(blocksIndices_.begin(),
						 find(blocksIndices_.begin(), blocksIndices_.end(), -1));

		if (index == blocksIndices_.size()) {
			ExpandBATArray(true);
			index = (SECT) distance(blocksIndices_.begin(),
							 find(blocksIndices_.begin(), blocksIndices_.end(), -1));
		}

		blocksIndices_[index] = ENDOFCHAIN;
	} else {
		// Find first free location
		index = (SECT) distance(sblocksIndices_.begin(),
						 find(sblocksIndices_.begin(), sblocksIndices_.end(), -1));

		if (index == sblocksIndices_.size()) {
			ExpandBATArray(false);
			index = (SECT) distance(sblocksIndices_.begin(),
							 find(sblocksIndices_.begin(), sblocksIndices_.end(), -1));
		}

		sblocksIndices_[index] = ENDOFCHAIN;
	}

	return index;
}

void CompoundFile::ExpandBATArray(bool isBig)
// PURPOSE: Create a new block of BAT or SBAT indices.
// EXPLAIN: isBig is true if property uses big blocks, false if it uses small blocks.
{
	SECT newIndex;

	fill(block_.begin(), block_.end(), -1);

	if (isBig) {
		size_t BATindex = distance(&header_._sectFat[0],
								   find(header_._sectFat, header_._sectFat+109, -1));
		if (BATindex < 109) {
			// Set new BAT index location
			newIndex = (SECT) blocksIndices_.size(); // New index location
			file_.Insert(newIndex+1, &*(block_.begin()));
			IncreaseLocationReferences(vector<SECT>(1, newIndex));

			// Update BAT array
			header_._sectFat[BATindex] = newIndex;
			++header_._csectFat;
		} else {
			// No free BAT indices. Increment using XBAT
			// Set new XBAT index location
			if (header_._csectDif != 0) {
				newIndex = header_._sectDifStart + header_._csectDif;
				file_.Insert(newIndex, &*(block_.begin()));
				IncreaseLocationReferences(vector<SECT>(1, newIndex));
			} else {
				newIndex = (SECT) blocksIndices_.size();
				file_.Insert(newIndex, &*(block_.begin()));
				IncreaseLocationReferences(vector<SECT>(1, newIndex));
				header_._sectDifStart = newIndex;
			}

			++header_._csectDif;
		}
		blocksIndices_.insert(blocksIndices_.begin()+newIndex, FATSECT);
		blocksIndices_.resize(blocksIndices_.size()+127, FREESECT);
	}
	else
	{
		// Set new SBAT index location
		if (header_._csectMiniFat != 0)
		{
			newIndex = header_._sectMiniFatStart + header_._csectMiniFat;
			file_.Insert(newIndex, &*(block_.begin()));
			IncreaseLocationReferences(vector<SECT>(1, newIndex));
		}
		else
		{
			newIndex = GetFreeBlockIndex(true);
			file_.Insert(newIndex, &*(block_.begin()));
			IncreaseLocationReferences(vector<SECT>(1, newIndex));
			header_._sectMiniFatStart = newIndex;
		}
		++header_._csectMiniFat;
		sblocksIndices_.resize(sblocksIndices_.size()+128, -1);
	}
}

void CompoundFile::LinkBlocks(SECT from, SECT to, bool isBig)
// PURPOSE: Link one BAT index to another.
// EXPLAIN: isBig is true if property uses big blocks, false if it uses small blocks.
{
	if (isBig)
		blocksIndices_[from] = to;
	else
		sblocksIndices_[from] = to;
}

void CompoundFile::FreeBlocks(vector<SECT>& indices, bool isBig)
// PURPOSE: Delete blocks of data from compound file.
// EXPLAIN: indices contains indices to blocks of data to be deleted.
// EXPLAIN: isBig is true if property uses big blocks, false if it uses small blocks.
{
	if (isBig)
	{
		// Decrease all location references before deleting blocks from file.
		DecreaseLocationReferences(indices);
		ULONG maxIndices = (ULONG) indices.size();

		{for(size_t i=0; i<maxIndices; ++i)
			++indices[i];} // Increase by 1 because block index 1 corresponds to index 0 here

		file_.Erase(indices);

		// Shrink BAT indices if necessary
		vector<SECT> indicesToRemove;
		while(distance(find(blocksIndices_.begin(),
							 blocksIndices_.end(),-1), blocksIndices_.end()) >= 128)
		{
			blocksIndices_.resize(blocksIndices_.size()-128);
			if (header_._csectDif != 0) {
				// Shrink XBAT first
				--header_._csectDif;
				indicesToRemove.push_back(header_._sectDifStart+header_._csectDif+1); // Add 1 because block index 1 corresponds to index 0 here
				if (header_._csectDif == 0)
					header_._sectDifStart = ENDOFCHAIN;
			} else {
				// No XBAT, delete last occupied BAT array element
				size_t BATindex = distance(&header_._sectFat[0],
										   find(header_._sectFat, header_._sectFat+109, -1));
				if (BATindex != 109) {
					--header_._csectFat;
					indicesToRemove.push_back(header_._sectFat[BATindex-1]+1); // Add 1 because block index 1 corresponds to index 0 here
					header_._sectFat[BATindex-1] = -1;
				}
			}
		}
		// Erase extra BAT indices if present
		if (!indicesToRemove.empty()) file_.Erase(indicesToRemove);
	}
	else
	{
		// Erase block
		ULONG maxIndices = (ULONG) indices.size();
		ULONG maxBlocks = dirEntries_[0]->_ulSize / header_.bigBlockSize_ +
						   (dirEntries_[0]->_ulSize % header_.bigBlockSize_ ? 1 : 0);
		size_t size = maxBlocks * header_.bigBlockSize_;
		char* data = new char[size];
		ReadData(dirEntries_[0]->_sectStart, data, true);
		size_t maxSmallBlocks = dirEntries_[0]->_ulSize / header_.smallBlockSize_;
		char* newdata = new char[dirEntries_[0]->_ulSize-maxIndices*header_.smallBlockSize_];
		{for(size_t i=0, j=0; i<maxSmallBlocks; ++i) {
			if (find(indices.begin(), indices.end(), i) == indices.end()) {
				copy(data+i*header_.smallBlockSize_,
					  data+i*header_.smallBlockSize_+header_.smallBlockSize_,
					  newdata+j*header_.smallBlockSize_);
				++j;
			}
		}}

		dirEntries_[0]->_sectStart = WriteData(newdata, dirEntries_[0]->_ulSize-maxIndices*header_.smallBlockSize_,
											   dirEntries_[0]->_sectStart, true);
		dirEntries_[0]->_ulSize -= maxIndices*header_.smallBlockSize_;
		delete[] data;
		delete[] newdata;

		// Change SBAT indices
		size_t maxSBATindices = sblocksIndices_.size();
		{for(size_t i=0; i<maxIndices; ++i)
		{
			for(size_t j=0; j<maxSBATindices; ++j)
			{
				if (j == indices[i]) continue;
				if (sblocksIndices_[j] == indices[i]) sblocksIndices_[j] = sblocksIndices_[indices[i]];
				if (sblocksIndices_[j] > indices[i] &&
					sblocksIndices_[j] != FREESECT &&
					sblocksIndices_[j] != ENDOFCHAIN) --sblocksIndices_[j];
			}
		}}
		sort (indices.begin(), indices.end(), greater<size_t>());
		{for(size_t i=0; i<maxIndices; ++i)
		{
			sblocksIndices_.erase(sblocksIndices_.begin()+indices[i]);
			sblocksIndices_.push_back(-1);
		}}
		vector<SECT> indicesToRemove;
		while(distance(find(sblocksIndices_.begin(), sblocksIndices_.end(), -1),
							 sblocksIndices_.end()) >= 128)
		{
			// Shrink SBAT indices if necessary
			sblocksIndices_.resize(sblocksIndices_.size()-128);
			--header_._csectMiniFat;
			indicesToRemove.push_back(header_._sectMiniFatStart+header_._csectMiniFat);
			if (header_._csectMiniFat == 0) header_._sectMiniFatStart = ENDOFCHAIN;
		}

		FreeBlocks(indicesToRemove, true);
	}
}

/*********************** Inaccessible Properties Functions ***************************/
void CompoundFile::LoadProperties()
// PURPOSE: Load properties information for compound file.
{
	// Read properties' data from compound file.
	ULONG propertiesSize = DataSize(header_._sectDirStat, true);
	char* buffer = new char[propertiesSize];
	ReadData(header_._sectDirStat, buffer, true);

	// Split properties' data into individual property.
	ULONG maxPropertiesBlock = propertiesSize / header_.bigBlockSize_;
	ULONG propertiesPerBlock = header_.bigBlockSize_ / 128;
	ULONG maxProperties = maxPropertiesBlock * propertiesPerBlock;
	ULONG maxBlocks = maxProperties / propertiesPerBlock +
					   (maxProperties % propertiesPerBlock ? 1 : 0);

	for(size_t i=0; i<maxBlocks; ++i)
	{
		for(size_t j=0; j<4; ++j)
		{
			// Read individual property
			DirectoryEntry* property = new DirectoryEntry;
			property->Read(buffer+i*512+j*128);
			if (wcslen(property->name_) == 0)
			{
				delete property;
				break;
			}
			dirEntries_.push_back(property);
		}
	}
	delete[] buffer;

	// Generate property trees
	propertyTrees_->parent_ = 0;
	propertyTrees_->self_ = dirEntries_[0];
	propertyTrees_->index_ = 0;

	InsertPropertyTree(propertyTrees_,
					   dirEntries_[dirEntries_[0]->_sidChild],
					   dirEntries_[0]->_sidChild);
}

void CompoundFile::SaveProperties()
// PURPOSE: Save properties information for compound file.
{
	// Calculate total size required by properties
	ULONG maxProperties = (ULONG) dirEntries_.size();
	ULONG propertiesPerBlock = header_.bigBlockSize_ / 128;
	ULONG maxBlocks = maxProperties / propertiesPerBlock +
					   (maxProperties % propertiesPerBlock ? 1 : 0);
	ULONG propertiesSize = maxBlocks*header_.bigBlockSize_;

	char* buffer = new char[propertiesSize];
	{for(size_t i=0; i<propertiesSize; ++i) buffer[i] = 0;}
	{for(size_t i=0; i<maxProperties; ++i)
	{
		// Save individual property
		dirEntries_[i]->Write(buffer+i*128);
	}}

	// Write properties' data to compound file.
	WriteData(buffer, propertiesSize, header_._sectDirStat, true);
	delete[] buffer;
}

int CompoundFile::MakeProperty(const wchar_t* path, CompoundFile::DirectoryEntry* property)
// PURPOSE: Create a new property in the compound file.
// EXPLAIN: path is the full path name for the property.
// EXPLAIN: property contains information on the type of property to be created.
{
	wchar_t* parentpath = 0;
	wchar_t* propertyname = 0;

	// Change to the specified directory. If specified directory is not present,
	// create it.
	if (wcslen(path) != 0)
	{
		if (path[0] == L'\\') currentDirectory_ = propertyTrees_;
	}
	SplitPath(path, parentpath, propertyname);

	if (propertyname != 0)
	{
		if (parentpath != 0)
		{
			if (ChangeDirectory(parentpath) != SUCCESS)
			{
				int ret = MakeDirectory(parentpath);
				if (ret != SUCCESS)
				{
					delete[] parentpath;
					delete[] propertyname;
					return ret;
				}
				else ChangeDirectory(parentpath);
			}
			delete[] parentpath;
		}

		// Insert property into specified directory
		size_t propertynameLength = wcslen(propertyname);
		if (propertynameLength >= 32) {
			delete[] propertyname;
			return NAME_TOO_LONG;
		}
		wcscpy(property->name_, propertyname);
		delete[] propertyname;
		property->_cb_namesize = (WORD) (propertynameLength*2+2);

		if (FindProperty(currentDirectory_, property->name_) == 0) {
			// Find location to insert property
			ULONG maxProperties = (ULONG) dirEntries_.size();
			SECT index;
			for(index=1; index<maxProperties; ++index) {
				if (*(dirEntries_[index]) > *property)
					break;
			}

			if (index != maxProperties) {
				// Change references for all properties affected by the new property
				IncreasePropertyReferences(propertyTrees_, index);
			}
			dirEntries_.insert(dirEntries_.begin()+index, property);
			InsertPropertyTree(currentDirectory_, property, index);
			return SUCCESS;
		} else
			return DUPLICATE_PROPERTY;
	}
	else
	{
		if (parentpath != 0) delete[] parentpath;
		return INVALID_PATH;
	}
}

CompoundFile::PropertyTree* CompoundFile::FindProperty(SECT index)
// PURPOSE: Find property in the compound file, given the index of the property.
// PROMISE: Returns a pointer to the property tree of the property if property
// PROMISE: is present, 0 if otherwise.
{
	if (previousDirectories_.empty()) previousDirectories_.push_back(propertyTrees_);
	PropertyTree* currentTree = previousDirectories_.back();
	if (currentTree->index_ != index)
	{
		size_t maxChildren = currentTree->children_.size();
		for(size_t i=0; i<maxChildren; ++i)
		{
			previousDirectories_.push_back(currentTree->children_[i]);
			PropertyTree* child = FindProperty(index);
			if (child != 0)
			{
				previousDirectories_.pop_back();
				return child;
			}
		}
	}
	else
	{
		previousDirectories_.pop_back();
		return currentTree;
	}
	previousDirectories_.pop_back();
	return NULL;
}

CompoundFile::PropertyTree* CompoundFile::FindProperty(const wchar_t* path)
// PURPOSE: Find property in the compound file, given the path of the property.
// PROMISE: Returns a pointer to the property tree of the property if property
// PROMISE: is present, 0 if otherwise.
{
	previousDirectories_.push_back(currentDirectory_);

	// Change to specified directory
	wchar_t* parentpath = 0;
	wchar_t* filename = 0;

	if (wcslen(path) != 0)
	{
		if (path[0] == L'\\') currentDirectory_ = propertyTrees_;
	}

	SplitPath(path, parentpath, filename);
	if (parentpath != 0)
	{
		int ret = ChangeDirectory(parentpath);
		delete[] parentpath;
		if (ret != SUCCESS)
		{
			// Cannot change to specified directory
			if (filename != 0) delete[] filename;
			currentDirectory_ = previousDirectories_.back();
			previousDirectories_.pop_back();
			PropertyTree* property = 0;
			return property;
		}
	}

	// Check to see if file is present in the specified directory.
	PropertyTree* property = 0;
	if (filename != 0)
	{
		property = FindProperty(currentDirectory_, filename);
		delete[] filename;
	}
	currentDirectory_ = previousDirectories_.back();
	previousDirectories_.pop_back();
	return property;
}

CompoundFile::PropertyTree*
CompoundFile::FindProperty(CompoundFile::PropertyTree* parentTree,
						   wchar_t* name)
// PURPOSE: Find property in the compound file, given the parent property tree and its name.
// PROMISE: Returns a pointer to the property tree of the property if property
// PROMISE: is present, 0 if otherwise.
{
	if (parentTree->self_->_sidChild != (CBF_SID)-1) {
		size_t maxChildren = parentTree->children_.size();

		for(size_t i=0; i<maxChildren; ++i) {
			if (wcscmp(parentTree->children_[i]->self_->name_, name) == 0)
				return parentTree->children_[i];
		}
	}

	return NULL;
}

void CompoundFile::InsertPropertyTree(CompoundFile::PropertyTree* parentTree,
									  CompoundFile::DirectoryEntry* property,
									  SECT index)
// PURPOSE: Insert a property and all its siblings and children into the property tree.
// REQUIRE: If the property is a new property and its index is already occupied by
// REQUIRE: another property, the calling function has to call IncreasePropertyReferences()
// REQUIRE: first before calling this function.
// EXPLAIN: This function is used by LoadProperty() to initialize the property trees
// EXPLAIN: and MakeProperty() thus resulting in the above requirements.
// EXPLAIN: parentTree is the parent of the new property.
// EXPLAIN: property is the property to be added.
// EXPLAIN: index is the index of the new property.
// PROMISE: The property will be added as the parent tree's child and the parent's
// PROMISE: child property and all the its children previous property and next property
// PROMISE: will be readjusted to accomodate the next property.
{
	PropertyTree* tree = new PropertyTree;
	tree->parent_ = parentTree;
	tree->self_ = property;
	tree->index_ = index;

	if (property->_sidLeftSib != (CBF_SID)-1)
	{
		InsertPropertyTree(parentTree,
						   dirEntries_[property->_sidLeftSib],
						   property->_sidLeftSib);
	}

	if (property->_sidRightSib != (CBF_SID)-1)
	{
		InsertPropertyTree(parentTree,
						   dirEntries_[property->_sidRightSib],
						   property->_sidRightSib);
	}

	if (property->_sidChild != (CBF_SID)-1)
	{
		InsertPropertyTree(tree,
						   dirEntries_[property->_sidChild],
						   property->_sidChild);
	}

	// Sort children
	size_t maxChildren = parentTree->children_.size();
	size_t i;
	for(i=0; i<maxChildren; ++i)
	{
		if (index < parentTree->children_[i]->index_) break;
	}
	parentTree->children_.insert(parentTree->children_.begin()+i, tree);

	// Update children indices
	UpdateChildrenIndices(parentTree);
}

void CompoundFile::DeletePropertyTree(CompoundFile::PropertyTree* tree)
// PURPOSE: Delete a property from properties.
// EXPLAIN: tree is the property tree to be deleted.
// PROMISE: The tree's parent's child property and all the its children previous property
// PROMISE: and next property will be readjusted to accomodate the deleted property.
{
	// Decrease all property references
	DecreasePropertyReferences(propertyTrees_, tree->index_);

	// Remove property
	if (dirEntries_[tree->index_]) delete dirEntries_[tree->index_];
	dirEntries_.erase(dirEntries_.begin()+tree->index_);

	// Remove property from property trees
	size_t maxChildren = tree->parent_->children_.size();
	size_t i;

	for(i=0; i<maxChildren; ++i) {
		if (tree->parent_->children_[i]->index_ == tree->index_)
			break;
	}

	tree->parent_->children_.erase(tree->parent_->children_.begin()+i);

	// Update children indices
	UpdateChildrenIndices(tree->parent_);
}

void CompoundFile::UpdateChildrenIndices(CompoundFile::PropertyTree* parentTree)
{
	// Update indices for 1st to middle child
	size_t maxChildren = parentTree->children_.size();
	if (maxChildren != 0)
	{
		vector<PropertyTree*>& children = parentTree->children_;
		size_t prevChild = 0;
		children[0]->self_->_sidLeftSib = -1;
		children[0]->self_->_sidRightSib = -1;
		size_t curChild;
		for(curChild=1; curChild<=maxChildren/2; ++curChild)
		{
			children[curChild]->self_->_sidLeftSib = children[prevChild]->index_;
			children[curChild]->self_->_sidRightSib = -1;
			prevChild = curChild;
		}

		// Update middle child
		--curChild;
		children[curChild]->parent_->self_->_sidChild = children[curChild]->index_;

		// Update from middle to last child
		size_t nextChild = curChild + 1;
		if (nextChild < maxChildren) {
			children[curChild]->self_->_sidRightSib = children[nextChild]->index_;
			for(++curChild, ++nextChild;
				 nextChild<maxChildren;
				 ++curChild, ++nextChild) {
				children[curChild]->self_->_sidLeftSib = -1;
				children[curChild]->self_->_sidRightSib = children[nextChild]->index_;
			}

			children[curChild]->self_->_sidLeftSib = -1;
			children[curChild]->self_->_sidRightSib = -1;
		}
	} else {
		parentTree->self_->_sidChild = -1;
	}
}

void CompoundFile::IncreasePropertyReferences(CompoundFile::PropertyTree* parentTree, SECT index)
// PURPOSE: Increase all property references (previous property, next property
// PURPOSE: and child property) which will be affected by the insertion of the new index.
// EXPLAIN: The recursive method of going through each property tree is used instead of
// EXPLAIN: using the iterative method of going through each property in dirEntries_ is
// EXPLAIN: because the index in property tree needs to be updated also.
{
	if (parentTree->index_ >= index)
		++parentTree->index_;

	if (parentTree->self_->_sidLeftSib != (CBF_SID)-1)	{
		if (parentTree->self_->_sidLeftSib >= index)
			++parentTree->self_->_sidLeftSib;
	}

	if (parentTree->self_->_sidRightSib != (CBF_SID)-1) {
		if (parentTree->self_->_sidRightSib >= index)
			++parentTree->self_->_sidRightSib;
	}

	if (parentTree->self_->_sidChild != (CBF_SID)-1) {
		if (parentTree->self_->_sidChild >= index)
			++parentTree->self_->_sidChild;
	}

	size_t maxChildren = parentTree->children_.size();
	for(size_t i=0; i<maxChildren; ++i)
		IncreasePropertyReferences(parentTree->children_[i], index);
}

void CompoundFile::DecreasePropertyReferences(CompoundFile::PropertyTree* parentTree, SECT index)
// PURPOSE: Decrease all property references (previous property, next property
// PURPOSE: and child property) which will be affected by the deletion of the index.
// EXPLAIN: The recursive method of going through each property tree is used instead of
// EXPLAIN: using the iterative method of going through each property in dirEntries_ is
// EXPLAIN: because the index in property tree needs to be updated also.
{
	if (parentTree->index_ > index)
		--parentTree->index_;

	if (parentTree->self_->_sidLeftSib != (CBF_SID)-1) {
		if (parentTree->self_->_sidLeftSib > index)
			--parentTree->self_->_sidLeftSib;
	}

	if (parentTree->self_->_sidRightSib != (CBF_SID)-1) {
		if (parentTree->self_->_sidRightSib > index)
			--parentTree->self_->_sidRightSib;
	}

	if (parentTree->self_->_sidChild != (CBF_SID)-1) {
		if (parentTree->self_->_sidChild > index)
			--parentTree->self_->_sidChild;
	}

	size_t maxChildren = parentTree->children_.size();
	for(size_t i=0; i<maxChildren; ++i)
		DecreasePropertyReferences(parentTree->children_[i], index);
}

} // YCompoundFiles namespace end

#endif // _WIN32


namespace YExcel
{
using namespace YCompoundFiles;

#ifdef _WIN32
using namespace WinCompFiles;
#endif

/************************************************************************************************************/
Record::Record() : dataSize_(0), recordSize_(4) {}
Record::~Record() {}

ULONG Record::Read(const char* data)
{
	LittleEndian::Read(data, code_, 0, 2);		// Read operation code.
	LittleEndian::Read(data, dataSize_, 2, 2);	// Read size of record.
	data_.assign(data+4, data+4+dataSize_);

	recordSize_ = 4 + dataSize_;

	// Check if next record is a continue record
	continueIndices_.clear();
	short code;
	LittleEndian::Read(data, code, dataSize_+4, 2);
	while(code == CODE::CONTINUE) {
		continueIndices_.push_back(dataSize_);

		ULONG size;
		LittleEndian::Read(data, size, recordSize_+2, 2);
		data_.insert(data_.end(), data+recordSize_+4, data+recordSize_+4+size);
		dataSize_ += size;
		recordSize_ += 4 + size;

		LittleEndian::Read(data, code, recordSize_, 2);
	}

	return recordSize_;
}
ULONG Record::Write(char* data)
{
	LittleEndian::Write(data, code_, 0, 2); 	// Write operation code.
	ULONG npos = 2;

	if (continueIndices_.empty()) {
		ULONG size = dataSize_;
		ULONG i=0;

		while(size > 8224) {
			LittleEndian::Write(data, 8224, npos, 2);	// Write size of record.
			npos += 2;
			size -= 8224;
			copy(data_.begin()+i*8224, data_.begin()+(i+1)*8224, data+npos);
			npos += 8224;

			if (size != 0) {
				++i;
				LittleEndian::Write(data, 0x3C, npos, 2);	// Write CONTINUE code.
				npos += 2;
			}
		}

		LittleEndian::Write(data, size, npos, 2);	// Write size of record.
		npos += 2;
		copy(data_.begin()+i*8224, data_.begin()+i*8224+size, data+npos);
		npos += size;
	} else {
		size_t maxContinue = continueIndices_.size();
		ULONG size = continueIndices_[0];

		LittleEndian::Write(data, size, npos, 2); // Write size of record
		npos += 2;
		copy(data_.begin(), data_.begin()+size, data+npos);
		npos += size;

		size_t c = 0;
		for(c=1; c<maxContinue; ++c) {
			LittleEndian::Write(data, 0x3C, npos, 2);	// Write CONTINUE code.
			npos += 2;
			size = continueIndices_[c] - continueIndices_[c-1];
			LittleEndian::Write(data, size, npos, 2);
			npos += 2;
			copy(data_.begin()+continueIndices_[c-1],
				  data_.begin()+continueIndices_[c],
				  data+npos);
			npos += size;
		}

		LittleEndian::Write(data, 0x3C, npos, 2);	// Write CONTINUE code.
		npos += 2;
		size = (ULONG)data_.size() - continueIndices_[c-1];
		LittleEndian::Write(data, size, npos, 2);
		npos += 2;
		copy(data_.begin()+continueIndices_[c-1],
			  data_.end(),
			  data+npos);
		npos += size;
	}

	return npos;
}

ULONG Record::DataSize() {return dataSize_;}
ULONG Record::RecordSize() {return recordSize_;}

/************************************************************************************************************/

/************************************************************************************************************/
BOF::BOF() : Record() {code_ = CODE::BOF; dataSize_ = 16; recordSize_ = 20;}
ULONG BOF::Read(const char* data)
{
	Record::Read(data);
	LittleEndian::Read(data_, version_, 0, 2);
	LittleEndian::Read(data_, type_, 2, 2);
	LittleEndian::Read(data_, buildIdentifier_, 4, 2);
	LittleEndian::Read(data_, buildYear_, 6, 2);
	LittleEndian::Read(data_, fileHistoryFlags_, 8, 4);
	LittleEndian::Read(data_, lowestExcelVersion_, 12, 4);
	return RecordSize();
}
ULONG BOF::Write(char* data)
{
	data_.resize(dataSize_);
	LittleEndian::Write(data_, version_, 0, 2);
	LittleEndian::Write(data_, type_, 2, 2);
	LittleEndian::Write(data_, buildIdentifier_, 4, 2);
	LittleEndian::Write(data_, buildYear_, 6, 2);
	LittleEndian::Write(data_, fileHistoryFlags_, 8, 4);
	LittleEndian::Write(data_, lowestExcelVersion_, 12, 4);
	return Record::Write(data);
}
/************************************************************************************************************/

/************************************************************************************************************/
YEOF::YEOF() : Record() {code_ = CODE::YEOF; dataSize_ = 0; recordSize_ = 4;}
/************************************************************************************************************/

/************************************************************************************************************/
SmallString::SmallString() : name_(0), wname_(0) {}

SmallString::~SmallString() {Reset();}

SmallString::SmallString(const SmallString& s) :
	name_(0), wname_(0), unicode_(s.unicode_)
{
	if (s.name_)
	{
		size_t len = strlen(s.name_);
		name_ = new char[len+1];
		strcpy(name_, s.name_);
	}
	if (s.wname_)
	{
		size_t len = wcslen(s.wname_);
		wname_ = new wchar_t[len+1];
		wcscpy(wname_, s.wname_);
	}
}
SmallString& SmallString::operator=(const SmallString& s)
{
	Reset();
	unicode_ = s.unicode_;
	if (s.name_) {
		size_t len = strlen(s.name_);
		name_ = new char[len+1];
		strcpy(name_, s.name_);
	}

	if (s.wname_) {
		size_t len = wcslen(s.wname_);
		wname_ = new wchar_t[len+1];
		wcscpy(wname_, s.wname_);
	}

	return *this;
}
const SmallString& SmallString::operator=(const char* str)
{
	unicode_ = 0;
	Reset();
	size_t len = strlen(str);
	name_ = new char[len+1];
	strcpy(name_, str);
	return *this;
}
const SmallString& SmallString::operator=(const wchar_t* str)
{
	unicode_ = 1;
	Reset();
	size_t len = wcslen(str);
	wname_ = new wchar_t[len+1];
	wcscpy(wname_, str);
	return *this;
}
void SmallString::Reset()
{
	if (name_) {delete[] name_; name_ = NULL;}
	if (wname_) {delete[] wname_; wname_ = NULL;}
}
ULONG SmallString::Read(const char* data)
{
	Reset();

	char stringSize;
	LittleEndian::Read(data, stringSize, 0, 1);
	LittleEndian::Read(data, unicode_, 1, 1);
	ULONG bytesRead = 2;

	if (!(unicode_ & 0x01)) { //MF compressed format of UTF16LE string?
		// ANSI string
		name_ = new char[stringSize+1];
		LittleEndian::ReadString(data, name_, 2, stringSize);
		name_[stringSize] = 0;
		bytesRead += stringSize;
	} else {
		// UNICODE
		wname_ = new wchar_t[stringSize+1];
		LittleEndian::ReadString(data, wname_, 2, stringSize);
		wname_[stringSize] = 0;
		bytesRead += stringSize*2;
	}

	return bytesRead;
}
ULONG SmallString::Write(char* data)
{
	ULONG stringSize = 0;
	ULONG bytesWrite = 0;

	if (!(unicode_ & 0x01)) { //MF
		// ANSI string
		if (name_) {
			stringSize = (ULONG) strlen(name_);
			LittleEndian::Write(data, stringSize, 0, 1);
			LittleEndian::Write(data, unicode_, 1, 1);
			LittleEndian::WriteString(data, name_, 2, (int)stringSize);
			bytesWrite = 2 + stringSize;
		} else {
			LittleEndian::Write(data, stringSize, 0, 1);
			LittleEndian::Write(data, unicode_, 1, 1);
			bytesWrite = 2;
		}
	} else {
		// UNICODE
		if (wname_) {
			stringSize = (ULONG) wcslen(wname_);
			LittleEndian::Write(data, stringSize, 0, 1);
			LittleEndian::Write(data, unicode_, 1, 1);
			LittleEndian::WriteString(data, wname_, 2, (int)stringSize);
			bytesWrite = 2 + stringSize*2;
		} else {
			LittleEndian::Write(data, stringSize, 0, 1);
			LittleEndian::Write(data, unicode_, 1, 1);
			bytesWrite = 2;
		}
	}

	return bytesWrite;
}
ULONG SmallString::DataSize()
{
	return (!(unicode_ & 0x01)) ? StringSize()+2 : StringSize()*2+2; //MF
}

ULONG SmallString::RecordSize() {return DataSize();}
ULONG SmallString::StringSize()
{
	if (!(unicode_ & 0x01)) //MF
	{
		if (name_)
			return (ULONG) strlen(name_);
	} else {
		if (wname_)
			return (ULONG) wcslen(wname_);
	}

	return 0;
}
/************************************************************************************************************/

/************************************************************************************************************/
LargeString::LargeString() : unicode_(-1), richtext_(0), phonetic_(0) {}
LargeString::~LargeString() {}

LargeString::LargeString(const LargeString& s) :
	name_(s.name_), wname_(s.wname_),
	unicode_(s.unicode_), richtext_(s.richtext_), phonetic_(s.phonetic_) {}

LargeString& LargeString::operator=(const LargeString& s)
{
	unicode_ = s.unicode_;
	richtext_ = s.richtext_;
	phonetic_ = s.phonetic_;
	name_ = s.name_;
	wname_ = s.wname_;
	return *this;
}

const LargeString& LargeString::operator=(const char* str)
{
	unicode_ = 0;
	richtext_ = 0;
	phonetic_ = 0;
	wname_.clear();
	size_t len = strlen(str);
	name_.resize(len);	//MF: don't add an extra 0 byte
	memcpy(&*(name_.begin()), str, len);
	return *this;
}

const LargeString& LargeString::operator=(const wchar_t* str)
{
	unicode_ = 1;
	richtext_ = 0;
	phonetic_ = 0;
	name_.clear();
	size_t len = wcslen(str);
	wname_.resize(len); //MF: don't add an extra 0 byte
	memcpy(&*(wname_.begin()), str, len*sizeof(wchar_t));
	return *this;
}

ULONG LargeString::Read(const char* data)
{
	short stringSize;
	LittleEndian::Read(data, stringSize, 0, 2);
	LittleEndian::Read(data, unicode_, 2, 1);
	ULONG npos = 3;

	if (unicode_ & 8) {
		LittleEndian::Read(data, richtext_, npos, 2);
		npos += 2;
	}

	if (unicode_ & 4)
		LittleEndian::Read(data, phonetic_, npos, 4);

	name_.clear();
	wname_.clear();
	int bytesRead = 2;

	if (stringSize > 0)
		bytesRead += ContinueRead(data+2, stringSize);
	else
		bytesRead = 3;

	return bytesRead;
}

ULONG LargeString::ContinueRead(const char* data, int size)
{
	if (size == 0)
		return 0;

	char unicode;
	LittleEndian::Read(data, unicode, 0, 1);

	if (unicode_ == -1)
		unicode_ = unicode;

	if (unicode_ & 1) {
		// Present stored string is uncompressed (16 bit)
		ULONG npos = 1;
		if (richtext_) npos += 2;
		if (phonetic_) npos += 4;

		size_t strpos = wname_.size();
		wname_.resize(strpos+size, 0);
		if (unicode & 1) {
			LittleEndian::ReadString(data, &*(wname_.begin())+strpos, npos, size);
			npos += size * SIZEOFWCHAR_T;
		} else {
			// String to be read is in ANSI
			vector<char> name(size);
			LittleEndian::ReadString(data, &*(name.begin()), npos, size);
			mbstowcs(&*(wname_.begin())+strpos, &*(name.begin()), size);
			npos += size;
		}

		if (richtext_)
			npos += 4*richtext_;

		if (phonetic_)
			npos += phonetic_;

		return npos;
	} else {
		// Present stored string has character compression (8 bit)
		ULONG npos = 1;

		if (richtext_)
			npos += 2;

		if (phonetic_)
			npos += 4;

		size_t strpos = name_.size();
		name_.resize(strpos+size, 0);
		if (unicode & 1) {
			// String to be read is in unicode
			vector<wchar_t> name(size);
			LittleEndian::ReadString(data, &*(name.begin()), npos, size);
			wcstombs(&*(name_.begin())+strpos, &*(name.begin()), size);
			npos += size * SIZEOFWCHAR_T;
		} else {
			LittleEndian::ReadString(data, &*(name_.begin())+strpos, npos, size);
			npos += size;
		}

		if (richtext_)
			npos += 4*richtext_;

		if (phonetic_)
			npos += phonetic_;

		return npos;
	}
}

ULONG LargeString::Write(char* data)
{
	short stringSize = 0;
	int bytesWrite = 0;

	if (unicode_ & 1) {
		// UNICODE
		unicode_ = 1; // Don't handle richtext or phonetic for now.
		if (!wname_.empty()) {
			stringSize = (short) wname_.size();
			LittleEndian::Write(data, stringSize, 0, 2);
			LittleEndian::Write(data, unicode_, 2, 1);
			LittleEndian::WriteString(data, &*(wname_.begin()), 3, stringSize);
			bytesWrite = 3 + stringSize * SIZEOFWCHAR_T;
		} else {
			LittleEndian::Write(data, stringSize, 0, 2);
			LittleEndian::Write(data, unicode_, 2, 1);
			bytesWrite = 3;
		}
	} else {
		// ANSI string
		unicode_ = 0; // Don't handle richtext or phonetic for now.
		if (!name_.empty()) {
			stringSize = (short) name_.size();
			LittleEndian::Write(data, stringSize, 0, 2);
			LittleEndian::Write(data, unicode_, 2, 1);
			LittleEndian::WriteString(data, &*(name_.begin()), 3, stringSize);
			bytesWrite = 3 + stringSize;
		} else {
			LittleEndian::Write(data, stringSize, 0, 2);
			LittleEndian::Write(data, unicode_, 2, 1);
			bytesWrite = 3;
		}
	}

	return bytesWrite;
}

ULONG LargeString::DataSize()
{
	ULONG dataSize = StringSize() + 3;

	if (richtext_)
		dataSize += 2 + 4*richtext_;

	if (phonetic_)
		dataSize += 4 + phonetic_;

	return dataSize;
}

ULONG LargeString::RecordSize()
{
	return DataSize();
}

ULONG LargeString::StringSize()
{
	if (unicode_ & 1)
		return (ULONG) wname_.size() * SIZEOFWCHAR_T;
	else
		return (ULONG) name_.size();
}
/************************************************************************************************************/


/************************************************************************************************************/
Workbook::Workbook()
{
	bof_.version_ = 1536;
	bof_.type_ = 5;
	bof_.buildIdentifier_ = 6560;
	bof_.buildYear_ = 1997;
	bof_.fileHistoryFlags_ = 49353;
	bof_.lowestExcelVersion_ = 774;
}

ULONG Workbook::Read(const char* data)
{
	ULONG bytesRead = 0;

	short code;
	LittleEndian::Read(data, code, 0, 2);

	while(code != CODE::YEOF) {
		switch(code) {
			case CODE::BOF:
				bytesRead += bof_.Read(data+bytesRead);
				break;

			case CODE::WINDOW1:
				bytesRead += window1_.Read(data+bytesRead);
				break;

			case CODE::FONT:
				fonts_.push_back(Font());
				bytesRead += fonts_.back().Read(data+bytesRead);
				break;

			//MF
			case CODE::FORMAT:
				formats_.push_back(Format());
				bytesRead += formats_.back().Read(data+bytesRead);
				break;

			case CODE::XF:
				XFs_.push_back(XF());
				bytesRead += XFs_.back().Read(data+bytesRead);
				break;

			case CODE::STYLE:
				styles_.push_back(Style());
				bytesRead += styles_.back().Read(data+bytesRead);
				break;

			case CODE::BOUNDSHEET:
				boundSheets_.push_back(BoundSheet());
				bytesRead += boundSheets_.back().Read(data+bytesRead);
				break;

			case CODE::SST:
				bytesRead += sst_.Read(data+bytesRead);
				break;

//			case CODE::EXTSST:
//				bytesRead += extSST_.Read(data+bytesRead);
//				break;

			default:
			{
				Record rec;
				bytesRead += rec.Read(data+bytesRead);
			}
		}

		LittleEndian::Read(data, code, bytesRead, 2);
	}

	bytesRead += eof_.RecordSize();
	return bytesRead;
}
ULONG Workbook::Write(char* data)
{
	ULONG bytesWritten = bof_.Write(data);

	bytesWritten += window1_.Write(data+bytesWritten);

	size_t maxFonts = fonts_.size();
	{for(size_t i=0; i<maxFonts; ++i) {bytesWritten += fonts_[i].Write(data+bytesWritten);}}

	//MF
	size_t maxFormats = formats_.size();
	{for(size_t i=0; i<maxFormats; ++i) {
		if (formats_[i].index_ >= FIRST_USER_FORMAT_IDX)	// only write user defined formats
			bytesWritten += formats_[i].Write(data+bytesWritten);
	}}

	size_t maxXFs = XFs_.size();
	{for(size_t i=0; i<maxXFs; ++i) {bytesWritten += XFs_[i].Write(data+bytesWritten);}}

	size_t maxStyles = styles_.size();
	{for(size_t i=0; i<maxStyles; ++i) {bytesWritten += styles_[i].Write(data+bytesWritten);}}

	size_t maxBoundSheets = boundSheets_.size();
	{for(size_t i=0; i<maxBoundSheets; ++i) {bytesWritten += boundSheets_[i].Write(data+bytesWritten);}}

	bytesWritten += sst_.Write(data+bytesWritten);
//	bytesWritten += extSST_.Write(data+bytesWritten);

	bytesWritten += eof_.Write(data+bytesWritten);

	return bytesWritten;
}
ULONG Workbook::DataSize()
{
	ULONG size = bof_.RecordSize();
	size += window1_.RecordSize();

	size_t maxFonts = fonts_.size();
	{for(size_t i=0; i<maxFonts; ++i) {size += fonts_[i].RecordSize();}}

	//MF
	size_t maxFormats = formats_.size();
	{for(size_t i=0; i<maxFormats; ++i) {
		if (formats_[i].index_ >= FIRST_USER_FORMAT_IDX)	// only write user defined formats
			size += formats_[i].RecordSize();
	}}

	size_t maxXFs = XFs_.size();
	{for(size_t i=0; i<maxXFs; ++i) {size += XFs_[i].RecordSize();}}

	size_t maxStyles = styles_.size();
	{for(size_t i=0; i<maxStyles; ++i) {size += styles_[i].RecordSize();}}

	size_t maxBoundSheets = boundSheets_.size();
	{for(size_t i=0; i<maxBoundSheets; ++i)
		size += boundSheets_[i].RecordSize();}

	size += sst_.RecordSize();
//	size += extSST_.RecordSize();
	size += eof_.RecordSize();

	return size;
}
ULONG Workbook::RecordSize() {return DataSize();}
/************************************************************************************************************/

/************************************************************************************************************/
Workbook::Window1::Window1() : Record(),
	horizontalPos_(0x78), verticalPos_(0x78), width_(0x3B1F), height_(0x2454),
	options_(0x38), activeWorksheetIndex_(0), firstVisibleTabIndex_(0), selectedWorksheetNo_(1),
	worksheetTabBarWidth_(0x258)
{
	code_ = CODE::WINDOW1;
	dataSize_ = 18; recordSize_ = 22;
}

ULONG Workbook::Window1::Read(const char* data)
{
	Record::Read(data);
	LittleEndian::Read(data_, horizontalPos_, 0, 2);
	LittleEndian::Read(data_, verticalPos_, 2, 2);
	LittleEndian::Read(data_, width_, 4, 2);
	LittleEndian::Read(data_, height_, 6, 2);
	LittleEndian::Read(data_, options_, 8, 2);
	LittleEndian::Read(data_, activeWorksheetIndex_, 10, 2);
	LittleEndian::Read(data_, firstVisibleTabIndex_, 12, 2);
	LittleEndian::Read(data_, selectedWorksheetNo_, 14, 2);
	LittleEndian::Read(data_, worksheetTabBarWidth_, 16, 2);
	return RecordSize();
}
ULONG Workbook::Window1::Write(char* data)
{
	data_.resize(dataSize_);
	LittleEndian::Write(data_, horizontalPos_, 0, 2);
	LittleEndian::Write(data_, verticalPos_, 2, 2);
	LittleEndian::Write(data_, width_, 4, 2);
	LittleEndian::Write(data_, height_, 6, 2);
	LittleEndian::Write(data_, options_, 8, 2);
	LittleEndian::Write(data_, activeWorksheetIndex_, 10, 2);
	LittleEndian::Write(data_, firstVisibleTabIndex_, 12, 2);
	LittleEndian::Write(data_, selectedWorksheetNo_, 14, 2);
	LittleEndian::Write(data_, worksheetTabBarWidth_, 16, 2);
	return Record::Write(data);
}
/************************************************************************************************************/

/************************************************************************************************************/
Workbook::Font::Font() : Record(),
	height_(200), options_(0), colourIndex_(0x7FFF), weight_(400), escapementType_(0),
	underlineType_(0), family_(0), characterSet_(0), unused_(0)
{
	code_ = CODE::FONT;
	dataSize_ = 14;
	recordSize_ = 18;
	name_ = L"Arial";
	name_.unicode_ = 1;
}
ULONG Workbook::Font::Read(const char* data)
{
	Record::Read(data);
	LittleEndian::Read(data_, height_, 0, 2);
	LittleEndian::Read(data_, options_, 2, 2);
	LittleEndian::Read(data_, colourIndex_, 4, 2);
	LittleEndian::Read(data_, weight_, 6, 2);
	LittleEndian::Read(data_, escapementType_, 8, 2);
	LittleEndian::Read(data_, underlineType_, 10, 1);
	LittleEndian::Read(data_, family_, 11, 1);
	LittleEndian::Read(data_, characterSet_, 12, 1);
	LittleEndian::Read(data_, unused_, 13, 1);
	name_.Read(&*(data_.begin())+14);
	return RecordSize();
}
ULONG Workbook::Font::Write(char* data)
{
	data_.resize(DataSize());
	LittleEndian::Write(data_, height_, 0, 2);
	LittleEndian::Write(data_, options_, 2, 2);
	LittleEndian::Write(data_, colourIndex_, 4, 2);
	LittleEndian::Write(data_, weight_, 6, 2);
	LittleEndian::Write(data_, escapementType_, 8, 2);
	LittleEndian::Write(data_, underlineType_, 10, 1);
	LittleEndian::Write(data_, family_, 11, 1);
	LittleEndian::Write(data_, characterSet_, 12, 1);
	LittleEndian::Write(data_, unused_, 13, 1);
	name_.Write(&*(data_.begin())+14);
	return Record::Write(data);
}
ULONG Workbook::Font::DataSize() {return dataSize_ = 14 + name_.RecordSize();}
ULONG Workbook::Font::RecordSize() {return recordSize_ = DataSize()+4;}
/************************************************************************************************************/

/************************************************************************************************************/
//MF
Workbook::Format::Format() : Record(),
	index_(0)
{
	code_ = CODE::FORMAT;
	dataSize_ = 2;
	recordSize_ = 6;
	fmtstring_ = XLS_FORMAT_GENERAL;
}
ULONG Workbook::Format::Read(const char* data)
{
	Record::Read(data);
	LittleEndian::Read(data_, index_, 0, 2);
	fmtstring_.Read(&*(data_.begin())+2);
	return RecordSize();
}
ULONG Workbook::Format::Write(char* data)
{
	data_.resize(DataSize());
	LittleEndian::Write(data_, index_, 0, 2);
	fmtstring_.Write(&*(data_.begin())+2);
	return Record::Write(data);
}
ULONG Workbook::Format::DataSize() {return dataSize_ = 2 + fmtstring_.RecordSize();}
ULONG Workbook::Format::RecordSize() {return recordSize_ = DataSize()+4;}
/************************************************************************************************************/

/************************************************************************************************************/
Workbook::XF::XF() : Record(),
	fontRecordIndex_(0), formatRecordIndex_(0), protectionType_((short)0xFFF5),
	alignment_(0x20), // EXCEL_HALIGN_GENERAL|EXCEL_VALIGN_BOTTOM
	rotation_(0x00),
	textProperties_(0x00), usedAttributes_(0x00), borderLines_(0x0000),
	colour1_(0x0000),
	colour2_(0x20C0)//MAKE_COLOR2(64, 65)
{
	code_ = CODE::XF; dataSize_ = 20; recordSize_ = 24;
}
ULONG Workbook::XF::Read(const char* data)
{
	Record::Read(data); // XF record in BIFF8 format
	LittleEndian::Read(data_, fontRecordIndex_, 0, 2);
	LittleEndian::Read(data_, formatRecordIndex_, 2, 2);
	LittleEndian::Read(data_, protectionType_, 4, 2);
	LittleEndian::Read(data_, alignment_, 6, 1);
	LittleEndian::Read(data_, rotation_, 7, 1);
	LittleEndian::Read(data_, textProperties_, 8, 1);
	LittleEndian::Read(data_, usedAttributes_, 9, 1);
	LittleEndian::Read(data_, borderLines_, 10, 4);
	LittleEndian::Read(data_, colour1_, 14, 4);
	LittleEndian::Read(data_, colour2_, 18, 2);
	return RecordSize();
}
ULONG Workbook::XF::Write(char* data)
{
	data_.resize(dataSize_);
	LittleEndian::Write(data_, fontRecordIndex_, 0, 2);
	LittleEndian::Write(data_, formatRecordIndex_, 2, 2);
	LittleEndian::Write(data_, protectionType_, 4, 2);
	LittleEndian::Write(data_, alignment_, 6, 1);
	LittleEndian::Write(data_, rotation_, 7, 1);
	LittleEndian::Write(data_, textProperties_, 8, 1);
	LittleEndian::Write(data_, usedAttributes_, 9, 1);
	LittleEndian::Write(data_, borderLines_, 10, 4);
	LittleEndian::Write(data_, colour1_, 14, 4);
	LittleEndian::Write(data_, colour2_, 18, 2);
	return Record::Write(data);
}
/************************************************************************************************************/

/************************************************************************************************************/
Workbook::Style::Style() : Record(),
	XFRecordIndex_((short)0x8000), identifier_(0), level_((char)0xFF)
{
	code_ = CODE::STYLE;
	dataSize_ = 2; recordSize_ = 6;
}

ULONG Workbook::Style::Read(const char* data)
{
	Record::Read(data);
	LittleEndian::Read(data_, XFRecordIndex_, 0, 2);
	if (XFRecordIndex_ & 0x8000)
	{
		// Built-in styles
		LittleEndian::Read(data_, identifier_, 2, 1);
		LittleEndian::Read(data_, level_, 3, 1);
	}
	else
	{
		// User-defined styles
		name_.Read(&*(data_.begin())+2);
	}
	return RecordSize();
}
ULONG Workbook::Style::Write(char* data)
{
	data_.resize(DataSize());
	LittleEndian::Write(data_, XFRecordIndex_, 0, 2);
	if (XFRecordIndex_ & 0x8000)
	{
		// Built-in styles
		LittleEndian::Write(data_, identifier_, 2, 1);
		LittleEndian::Write(data_, level_, 3, 1);
	}
	else
	{
		// User-defined styles
		name_.Write(&*(data_.begin())+2);
	}
	return Record::Write(data);
}
ULONG Workbook::Style::DataSize() {return dataSize_ = (XFRecordIndex_ & 0x8000) ? 4 : 2+name_.RecordSize();}
ULONG Workbook::Style::RecordSize() {return recordSize_ = DataSize()+4;}
/************************************************************************************************************/

/************************************************************************************************************/
Workbook::BoundSheet::BoundSheet() : Record(),
	BOFpos_(0x0000), visibility_(0), type_(0)
{
	code_ = CODE::BOUNDSHEET;
	dataSize_ = 6;
	dataSize_ = 10;
	name_ = "Sheet1";
	name_.unicode_ = false;
}
ULONG Workbook::BoundSheet::Read(const char* data)
{
	Record::Read(data);
	LittleEndian::Read(data_, BOFpos_, 0, 4);
	LittleEndian::Read(data_, visibility_, 4, 1);
	LittleEndian::Read(data_, type_, 5, 1);
	name_.Read(&*(data_.begin())+6);
	return RecordSize();
}
ULONG Workbook::BoundSheet::Write(char* data)
{
	data_.resize(DataSize());
	LittleEndian::Write(data_, BOFpos_, 0, 4);
	LittleEndian::Write(data_, visibility_, 4, 1);
	LittleEndian::Write(data_, type_, 5, 1);
	name_.Write(&*(data_.begin())+6);
	return Record::Write(data);
}
ULONG Workbook::BoundSheet::DataSize() {return dataSize_ = 6+name_.RecordSize();}
ULONG Workbook::BoundSheet::RecordSize() {return recordSize_ = DataSize()+4;}
/************************************************************************************************************/

/************************************************************************************************************/
Workbook::SharedStringTable::SharedStringTable() : Record(),
	stringsTotal_(0), uniqueStringsTotal_(0)
{
	code_ = CODE::SST;
	dataSize_ = 8; recordSize_ = 12;
}

ULONG Workbook::SharedStringTable::Read(const char* data)
{
	Record::Read(data);
	LittleEndian::Read(data_, stringsTotal_, 0, 4);
	LittleEndian::Read(data_, uniqueStringsTotal_, 4, 4);
	strings_.clear();
	strings_.resize(uniqueStringsTotal_);

	ULONG npos = 8;
	if (continueIndices_.empty()) {
		for(ULONG i=0; i<uniqueStringsTotal_; ++i)
			npos += strings_[i].Read(&*(data_.begin())+npos);
	} else {
		// Require special handling since CONTINUE records are present
		ULONG maxContinue = (ULONG) continueIndices_.size();

		for(ULONG i=0, c=0; i<uniqueStringsTotal_; ++i) {
			char unicode;
			ULONG stringSize;
			LittleEndian::Read(data_, stringSize, npos, 2);
			LittleEndian::Read(data_, unicode, npos+2, 1);
			int multiplier = unicode & 1 ? 2 : 1;

			if (c >= maxContinue || npos+stringSize*multiplier+3 <= continueIndices_[c]) {
				// String to be read is not split into two records
				npos += strings_[i].Read(&*(data_.begin())+npos);
			} else {
				// String to be read is split into two or more records
				int bytesRead = 2;// Start from unicode field

				int size = continueIndices_[c] - npos - 1 - bytesRead;
				++c;

				if (size > 0) {
					size /= multiplier; // Number of characters available for string in current record.
					bytesRead += strings_[i].ContinueRead(&*(data_.begin())+npos+bytesRead, size);
					stringSize -= size;
					size = 0;
				}

				while(c<maxContinue && npos+stringSize+1>continueIndices_[c]) {
					ULONG dataSize = (continueIndices_[c] - continueIndices_[c-1] - 1) / multiplier;
					bytesRead += strings_[i].ContinueRead(&*(data_.begin())+npos+bytesRead, dataSize);
					stringSize -= dataSize + 1;
					++c;
				}

				if (stringSize > 0)
					bytesRead += strings_[i].ContinueRead(&*(data_.begin())+npos+bytesRead, stringSize);

				npos += bytesRead;
			}
		}
	}

	return npos + 4*(npos/8224 + 1);
}

ULONG Workbook::SharedStringTable::Write(char* data)
{
	data_.resize(DataSize());
	LittleEndian::Write(data_, stringsTotal_, 0, 4);
	LittleEndian::Write(data_, uniqueStringsTotal_, 4, 4);

	size_t maxContinue = continueIndices_.size();
	for(size_t i=0, c=0, npos=8; i<uniqueStringsTotal_; ++i) {
		npos += strings_[i].Write(&*(data_.begin())+npos);

		if (c<maxContinue && npos==continueIndices_[c])
			++c;
		else if (c<maxContinue && npos>continueIndices_[c]) {
			// Insert unicode flag where appropriate for CONTINUE records.
			data_.insert(data_.begin()+continueIndices_[c], strings_[i].unicode_);
			data_.pop_back();
			++c;
			++npos;
		}
	}

	return Record::Write(data);
}
ULONG Workbook::SharedStringTable::DataSize()
{
	dataSize_ = 8;
	continueIndices_.clear();
	SECT curMax = 8224;

	for(ULONG i=0; i<uniqueStringsTotal_; ++i) {
		ULONG stringSize = strings_[i].StringSize();

		if (dataSize_+stringSize+3 <= curMax)
			dataSize_ += stringSize + 3;
		else {
			// If have >= 12 bytes (2 for size, 1 for unicode and >=9 for data, can split string
			// otherwise, end record and start continue record.
			bool unicode = strings_[i].unicode_ & 1;
			if (curMax - dataSize_ >= 12) {
				if (unicode && !((curMax-dataSize_)%2))
					--curMax;	// Make sure space reserved for unicode strings is even.

				continueIndices_.push_back(curMax);
				stringSize -= (curMax - dataSize_ - 3);
				dataSize_ = curMax;
				curMax += 8224;

				size_t additionalContinueRecords = unicode ? stringSize/8222 : stringSize/8223; // 8222 or 8223 because the first byte is for unicode identifier
				for(size_t j=0; j<additionalContinueRecords; ++j) {
					if (unicode) {
						--curMax;
						continueIndices_.push_back(curMax);
						curMax += 8223;
						dataSize_ += 8223;
						stringSize -= 8222;
					} else {
						continueIndices_.push_back(curMax);
						curMax += 8224;
						dataSize_ += 8224;
						stringSize -= 8223;
					}
				}

				dataSize_ += stringSize + 1;
			} else {
				continueIndices_.push_back(dataSize_);
				curMax = dataSize_ + 8224;
				if (dataSize_+stringSize+3 < curMax)
					dataSize_ += stringSize + 3;
				else {
					// If have >= 12 bytes (2 for size, 1 for unicode and >=9 for data, can split string
					// otherwise, end record and start continue record.
					if (curMax - dataSize_ >= 12) {
						if (unicode && !((curMax-dataSize_)%2)) --curMax;	// Make sure space reserved for unicode strings is even.
						continueIndices_.push_back(curMax);
						stringSize -= (curMax - dataSize_ - 3);
						dataSize_ = curMax;
						curMax += 8224;

						size_t additionalContinueRecords = unicode ? stringSize/8222 : stringSize/8223; // 8222 or 8223 because the first byte is for unicode identifier
						for(size_t j=0; j<additionalContinueRecords; ++j) {
							if (unicode) {
								--curMax;
								continueIndices_.push_back(curMax);
								curMax += 8223;
								dataSize_ += 8223;
								stringSize -= 8222;
							} else {
								continueIndices_.push_back(curMax);
								curMax += 8224;
								dataSize_ += 8224;
								stringSize -= 8223;
							}
						}

						dataSize_ += stringSize + 1;
					}
				}
			}
		}
	}

	return dataSize_;
}
ULONG Workbook::SharedStringTable::RecordSize()
{
	ULONG dataSize = DataSize();
	return recordSize_ = dataSize + 4*(dataSize/8224 + 1);
}

/************************************************************************************************************/
Workbook::ExtSST::ExtSST() : Record(),
	stringsTotal_(0), streamPos_(0), firstStringPos_(0), unused_(0)
{
	code_ = CODE::EXTSST;
	dataSize_ = 2;
	recordSize_ = 6;
}

ULONG Workbook::ExtSST::Read(const char* data)
{
	Record::Read(data);
	LittleEndian::Read(data_, stringsTotal_, 0, 2);

	ULONG maxPortions = (dataSize_-2) / 8;
	streamPos_.clear();
	streamPos_.resize(maxPortions);
	firstStringPos_.clear();
	firstStringPos_.resize(maxPortions);
	unused_.clear();
	unused_.resize(maxPortions);

	for(ULONG i=0, npos=2; i<maxPortions; ++i) {
		LittleEndian::Read(data_, streamPos_[i], npos, 4);
		LittleEndian::Read(data_, firstStringPos_[i], npos+4, 2);
		LittleEndian::Read(data_, unused_[i], npos+6, 2);
		npos += 8;
	}
	return RecordSize();
}

ULONG Workbook::ExtSST::Write(char* data)
{
	data_.resize(DataSize());
	LittleEndian::Write(data_, stringsTotal_, 0, 2);

	ULONG maxPortions = (ULONG) streamPos_.size();
	for(ULONG i=0, npos=2; i<maxPortions; ++i) {
		LittleEndian::Write(data_, streamPos_[i], npos, 4);
		LittleEndian::Write(data_, firstStringPos_[i], npos+4, 2);
		LittleEndian::Write(data_, unused_[i], npos+6, 2);
		npos += 8;
	}
	return Record::Write(data);
}

ULONG Workbook::ExtSST::DataSize()
{
	dataSize_ = 2 + (ULONG)streamPos_.size()*8;
	dataSize_ += (int)(dataSize_/8224)*4;
	return dataSize_;
}

ULONG Workbook::ExtSST::RecordSize()
{
	return (recordSize_ = DataSize()+(int)((2+(ULONG)streamPos_.size()*8)/8224)*4)+4;
}
/************************************************************************************************************/



/************************************************************************************************************/
Worksheet::Worksheet()
{
	bof_.version_ = 1536;
	bof_.type_ = 16;
	bof_.buildIdentifier_ = 6560;
	bof_.buildYear_ = 1997;
	bof_.fileHistoryFlags_ = 49353;
	bof_.lowestExcelVersion_ = 774;
}

ULONG Worksheet::Read(const char* data)
{
	ULONG bytesRead = 0;

	try {
		short code;
		LittleEndian::Read(data, code, 0, 2);

		while(code != CODE::YEOF)
		{
			switch(code)
			{
				case CODE::BOF:
					bytesRead += bof_.Read(data+bytesRead);
					break;

				case CODE::INDEX:
					bytesRead += index_.Read(data+bytesRead);
					break;

				case CODE::COLINFO:
					bytesRead += colinfos_.Read(data+bytesRead);
					break;

				case CODE::DIMENSIONS:
					bytesRead += dimensions_.Read(data+bytesRead);
					break;

				case CODE::ROW:
					bytesRead += cellTable_.Read(data+bytesRead);
					break;

				case CODE::WINDOW2:
					bytesRead += window2_.Read(data+bytesRead);
					break;

				case CODE::MERGECELLS:
					bytesRead += mergedCells_.Read(data+bytesRead);
					break;

//				case CODE::SXFORMULA:
//					bytesRead += 4;	// skip SXFORMULA record
//					break;
#ifdef _DEBUG
				case 0:
				case (short)0xcdcd:
				case (short)0xfdfd:
//					assert(0);//@@
					break;
#endif

				default:
					Record rec;
					bytesRead += rec.Read(data+bytesRead);
			}

			LittleEndian::Read(data, code, bytesRead, 2);
		}

		bytesRead += eof_.RecordSize();
	} catch(EXCEPTION_YEOF& e) {
		bytesRead += e._bytesRead;
	}

	return bytesRead;
}

ULONG Worksheet::Write(char* data)
{
	ULONG bytesWritten = bof_.Write(data);

	bytesWritten += index_.Write(data+bytesWritten);

	bytesWritten += colinfos_.Write(data+bytesWritten);

	bytesWritten += dimensions_.Write(data+bytesWritten);

	bytesWritten += cellTable_.Write(data+bytesWritten);

	bytesWritten += window2_.Write(data+bytesWritten);

	bytesWritten += mergedCells_.Write(data+bytesWritten);

	bytesWritten += eof_.Write(data+bytesWritten);

	return bytesWritten;
}

ULONG Worksheet::DataSize()
{
	ULONG dataSize = bof_.RecordSize();
	dataSize += index_.RecordSize();
	dataSize += colinfos_.RecordSize();
	dataSize += dimensions_.RecordSize();
	dataSize += cellTable_.RecordSize();
	dataSize += window2_.RecordSize();
	dataSize += mergedCells_.RecordSize();
	dataSize += eof_.RecordSize();
	return dataSize;
}
ULONG Worksheet::RecordSize() {return DataSize();}
/************************************************************************************************************/

/************************************************************************************************************/
Worksheet::Index::Index() : Record(),
	unused1_(0), firstUsedRowIndex_(0), firstUnusedRowIndex_(0), unused2_(0)
{
	code_ = CODE::INDEX;
	dataSize_ = 16; recordSize_ = 20; DBCellPos_.resize(1);
}
ULONG Worksheet::Index::Read(const char* data)
{
	Record::Read(data);
	LittleEndian::Read(data_, unused1_, 0, 4);
	LittleEndian::Read(data_, firstUsedRowIndex_, 4, 4);
	LittleEndian::Read(data_, firstUnusedRowIndex_, 8, 4);
	LittleEndian::Read(data_, unused2_, 12, 4);
	size_t nm = int(firstUnusedRowIndex_ - firstUsedRowIndex_ - 1) / 32 + 1;
	DBCellPos_.clear();
	DBCellPos_.resize(nm);
	if (dataSize_>16)
	{
		for(size_t i=0; i<nm; ++i)
		{
			LittleEndian::Read(data_, DBCellPos_[i], 16+i*4, 4);
		}
	}
	return RecordSize();
}
ULONG Worksheet::Index::Write(char* data)
{
	data_.resize(DataSize());
	LittleEndian::Write(data_, unused1_, 0, 4);
	LittleEndian::Write(data_, firstUsedRowIndex_, 4, 4);
	LittleEndian::Write(data_, firstUnusedRowIndex_, 8, 4);
	LittleEndian::Write(data_, unused2_, 12, 4);
	size_t nm = DBCellPos_.size();
	for(size_t i=0; i<nm; ++i)
	{
		LittleEndian::Write(data_, DBCellPos_[i], 16+i*4, 4);
	}
	return Record::Write(data);
}
ULONG Worksheet::Index::DataSize() {return dataSize_ = 16 + (ULONG)DBCellPos_.size()*4;}
ULONG Worksheet::Index::RecordSize()
{
	ULONG dataSize = DataSize();
	return recordSize_ = dataSize + 4*(dataSize/8224 + 1);
}

/************************************************************************************************************/

/************************************************************************************************************/
Worksheet::ColInfo::ColInfo() : Record(),
	firstColumnIndex_(0),lastColumnIndex_(255),
	columnWidth_(256*10),XFRecordIndex_(0),
	options_(0),unused_(0)
{
	code_ = CODE::COLINFO; dataSize_ = 12; recordSize_ = 16;
}
ULONG Worksheet::ColInfo::Read(const char* data)
{
	Record::Read(data);
	LittleEndian::Read(data_, firstColumnIndex_, 0, 2);
	LittleEndian::Read(data_, lastColumnIndex_, 2, 2);
	LittleEndian::Read(data_, columnWidth_, 4, 2);
	LittleEndian::Read(data_, XFRecordIndex_, 6, 2);
	LittleEndian::Read(data_, options_, 8, 2);
	LittleEndian::Read(data_, unused_, 10, 2);
	return RecordSize();
}
ULONG Worksheet::ColInfo::Write(char* data)
{
	data_.resize(DataSize());
	LittleEndian::Write(data_, firstColumnIndex_, 0, 2);
	LittleEndian::Write(data_, lastColumnIndex_, 2, 2);
	LittleEndian::Write(data_, columnWidth_, 4, 2);
	LittleEndian::Write(data_, XFRecordIndex_, 6, 2);
	LittleEndian::Write(data_, options_, 8, 2);
	LittleEndian::Write(data_, unused_, 10, 2);
	return Record::Write(data);
}
/************************************************************************************************************/

ULONG Worksheet::ColInfos::Read(const char* data)
{
	ColInfo ci;
	ULONG bytesRead = ci.Read(data);
	colinfo_.push_back(ci);
	return bytesRead;
}
ULONG Worksheet::ColInfos::Write(char* data)
{
	ULONG bytesWritten = 0;

	for(size_t i=0; i<colinfo_.size(); ++i)
		bytesWritten += colinfo_[i].Write(data+bytesWritten);

	return bytesWritten;
}
ULONG Worksheet::ColInfos::RecordSize()
{
	ULONG dataSize = 0;
	for(size_t i=0; i<colinfo_.size(); ++i)
		dataSize += colinfo_[i].RecordSize();
	return dataSize;
}

/************************************************************************************************************/
Worksheet::Dimensions::Dimensions() : Record(),
	firstUsedRowIndex_(0), lastUsedRowIndexPlusOne_(0),
	firstUsedColIndex_(0), lastUsedColIndexPlusOne_(0),
	unused_(0)
{
	code_ = CODE::DIMENSIONS;
	dataSize_ = 14; recordSize_ = 18;
}
ULONG Worksheet::Dimensions::Read(const char* data)
{
	Record::Read(data);
	LittleEndian::Read(data_, firstUsedRowIndex_, 0, 4);
	LittleEndian::Read(data_, lastUsedRowIndexPlusOne_, 4, 4);
	LittleEndian::Read(data_, firstUsedColIndex_, 8, 2);
	LittleEndian::Read(data_, lastUsedColIndexPlusOne_, 10, 2);
	LittleEndian::Read(data_, unused_, 12, 2);
	return RecordSize();
}
ULONG Worksheet::Dimensions::Write(char* data)
{
	data_.resize(DataSize());
	LittleEndian::Write(data_, firstUsedRowIndex_, 0, 4);
	LittleEndian::Write(data_, lastUsedRowIndexPlusOne_, 4, 4);
	LittleEndian::Write(data_, firstUsedColIndex_, 8, 2);
	LittleEndian::Write(data_, lastUsedColIndexPlusOne_, 10, 2);
	LittleEndian::Write(data_, unused_, 12, 2);
	return Record::Write(data);
}
/************************************************************************************************************/

/************************************************************************************************************/
Worksheet::CellTable::RowBlock::CellBlock::Blank::Blank() : Record(),
	rowIndex_(0), colIndex_(0), XFRecordIndex_(0)
{
	code_ = CODE::BLANK;
	dataSize_ = 6; recordSize_ = 10;
}
ULONG Worksheet::CellTable::RowBlock::CellBlock::Blank::Read(const char* data)
{
	Record::Read(data);
	LittleEndian::Read(data_, rowIndex_, 0, 2);
	LittleEndian::Read(data_, colIndex_, 2, 2);
	LittleEndian::Read(data_, XFRecordIndex_, 4, 2);
	return RecordSize();
}
ULONG Worksheet::CellTable::RowBlock::CellBlock::Blank::Write(char* data)
{
	data_.resize(DataSize());
	LittleEndian::Write(data_, rowIndex_, 0, 2);
	LittleEndian::Write(data_, colIndex_, 2, 2);
	LittleEndian::Write(data_, XFRecordIndex_, 4, 2);
	return Record::Write(data);
}

Worksheet::CellTable::RowBlock::CellBlock::BoolErr::BoolErr() : Record(),
	rowIndex_(0), colIndex_(0), XFRecordIndex_(0), value_(0), error_(0)
{
	code_ = CODE::BOOLERR;
	dataSize_ = 8; recordSize_ = 12;
}
ULONG Worksheet::CellTable::RowBlock::CellBlock::BoolErr::Read(const char* data)
{
	Record::Read(data);
	LittleEndian::Read(data_, rowIndex_, 0, 2);
	LittleEndian::Read(data_, colIndex_, 2, 2);
	LittleEndian::Read(data_, XFRecordIndex_, 4, 2);
	LittleEndian::Read(data_, value_, 6, 1);
	LittleEndian::Read(data_, error_, 7, 1);
	return RecordSize();
}
ULONG Worksheet::CellTable::RowBlock::CellBlock::BoolErr::Write(char* data)
{
	data_.resize(DataSize());
	LittleEndian::Write(data_, rowIndex_, 0, 2);
	LittleEndian::Write(data_, colIndex_, 2, 2);
	LittleEndian::Write(data_, XFRecordIndex_, 4, 2);
	LittleEndian::Write(data_, value_, 6, 1);
	LittleEndian::Write(data_, error_, 7, 1);
	return Record::Write(data);
}

Worksheet::CellTable::RowBlock::CellBlock::LabelSST::LabelSST() : Record(),
	rowIndex_(0), colIndex_(0), XFRecordIndex_(0), SSTRecordIndex_(0)
{
	code_ = CODE::LABELSST;
	dataSize_ = 10; recordSize_ = 14;
}
ULONG Worksheet::CellTable::RowBlock::CellBlock::LabelSST::Read(const char* data)
{
	Record::Read(data);
	LittleEndian::Read(data_, rowIndex_, 0, 2);
	LittleEndian::Read(data_, colIndex_, 2, 2);
	LittleEndian::Read(data_, XFRecordIndex_, 4, 2);
	LittleEndian::Read(data_, SSTRecordIndex_, 6, 4);
	return RecordSize();
}
ULONG Worksheet::CellTable::RowBlock::CellBlock::LabelSST::Write(char* data)
{
	data_.resize(DataSize());
	LittleEndian::Write(data_, rowIndex_, 0, 2);
	LittleEndian::Write(data_, colIndex_, 2, 2);
	LittleEndian::Write(data_, XFRecordIndex_, 4, 2);
	LittleEndian::Write(data_, SSTRecordIndex_, 6, 4);
	return Record::Write(data);
}

Worksheet::CellTable::RowBlock::CellBlock::MulBlank::MulBlank() : Record(),
	rowIndex_(0), firstColIndex_(0), lastColIndex_(0)
{
	code_ = CODE::MULBLANK;
	dataSize_ = 10; recordSize_ = 14;
}
ULONG Worksheet::CellTable::RowBlock::CellBlock::MulBlank::Read(const char* data)
{
	Record::Read(data);
	LittleEndian::Read(data_, rowIndex_, 0, 2);
	LittleEndian::Read(data_, firstColIndex_, 2, 2);
	LittleEndian::Read(data_, lastColIndex_, dataSize_-2, 2);
	size_t nc = lastColIndex_ - firstColIndex_ + 1;
	XFRecordIndices_.clear();
	XFRecordIndices_.resize(nc);

	for(size_t i=0; i<nc; ++i)
		LittleEndian::Read(data_, XFRecordIndices_[i], 4+i*2, 2);

	return RecordSize();
}
ULONG Worksheet::CellTable::RowBlock::CellBlock::MulBlank::Write(char* data)
{
	data_.resize(DataSize());
	LittleEndian::Write(data_, rowIndex_, 0, 2);
	LittleEndian::Write(data_, firstColIndex_, 2, 2);
	LittleEndian::Write(data_, lastColIndex_, dataSize_-2, 2);

	size_t nc = XFRecordIndices_.size();
	for(size_t i=0; i<nc; ++i)
		LittleEndian::Write(data_, XFRecordIndices_[i], 4+i*2, 2);

	return Record::Write(data);
}
ULONG Worksheet::CellTable::RowBlock::CellBlock::MulBlank::DataSize()
{
	return dataSize_ = 6 + (ULONG)XFRecordIndices_.size()*2;
}
ULONG Worksheet::CellTable::RowBlock::CellBlock::MulBlank::RecordSize()
{
	ULONG dataSize = DataSize();
	return recordSize_ = dataSize + 4*(dataSize/8224 + 1);
}

Worksheet::CellTable::RowBlock::CellBlock::MulRK::XFRK::XFRK() :
	XFRecordIndex_(0), RKValue_(0) {}
void Worksheet::CellTable::RowBlock::CellBlock::MulRK::XFRK::Read(const char* data)
{
	LittleEndian::Read(data, XFRecordIndex_, 0, 2);
	LittleEndian::Read(data, RKValue_, 2, 4);
}
void Worksheet::CellTable::RowBlock::CellBlock::MulRK::XFRK::Write(char* data)
{
	LittleEndian::Write(data, XFRecordIndex_, 0, 2);
	LittleEndian::Write(data, RKValue_, 2, 4);
}

Worksheet::CellTable::RowBlock::CellBlock::MulRK::MulRK() : Record(),
	rowIndex_(0), firstColIndex_(0), lastColIndex_(0)
{
	code_ = CODE::MULRK;
	dataSize_ = 10; recordSize_ = 14;
}
ULONG Worksheet::CellTable::RowBlock::CellBlock::MulRK::Read(const char* data)
{
	Record::Read(data);
	LittleEndian::Read(data_, rowIndex_, 0, 2);
	LittleEndian::Read(data_, firstColIndex_, 2, 2);
	LittleEndian::Read(data_, lastColIndex_, dataSize_-2, 2);
	size_t nc = lastColIndex_ - firstColIndex_ + 1;
	XFRK_.clear();
	XFRK_.resize(nc);
	for(size_t i=0; i<nc; ++i)
	{
		XFRK_[i].Read(&*(data_.begin())+4+i*6);
	}
	return RecordSize();
}
ULONG Worksheet::CellTable::RowBlock::CellBlock::MulRK::Write(char* data)
{
	data_.resize(DataSize());
	LittleEndian::Write(data_, rowIndex_, 0, 2);
	LittleEndian::Write(data_, firstColIndex_, 2, 2);
	LittleEndian::Write(data_, lastColIndex_, dataSize_-2, 2);
	size_t nc = XFRK_.size();
	for(size_t i=0; i<nc; ++i)
	{
		XFRK_[i].Write(&*(data_.begin())+4+i*6);
	}
	return Record::Write(data);
}
ULONG Worksheet::CellTable::RowBlock::CellBlock::MulRK::DataSize()
{
	return dataSize_ = 6 + (ULONG)XFRK_.size()*6;
}
ULONG Worksheet::CellTable::RowBlock::CellBlock::MulRK::RecordSize()
{
	ULONG dataSize = DataSize();
	return recordSize_ = dataSize + 4*(dataSize/8224 + 1);
}

Worksheet::CellTable::RowBlock::CellBlock::Number::Number() : Record(),
	rowIndex_(0), colIndex_(0), XFRecordIndex_(0), value_(0)
{
	code_ = CODE::NUMBER;
	dataSize_ = 14; recordSize_ = 18;
}
ULONG Worksheet::CellTable::RowBlock::CellBlock::Number::Read(const char* data)
{
	Record::Read(data);
	LittleEndian::Read(data_, rowIndex_, 0, 2);
	LittleEndian::Read(data_, colIndex_, 2, 2);
	LittleEndian::Read(data_, XFRecordIndex_, 4, 2);
	LONGINT value;
	LittleEndian::Read(data_, value, 6, 8);
	intdouble_.intvalue_ = value;
	value_ = intdouble_.doublevalue_;
	return RecordSize();
}
ULONG Worksheet::CellTable::RowBlock::CellBlock::Number::Write(char* data)
{
	data_.resize(DataSize());
	LittleEndian::Write(data_, rowIndex_, 0, 2);
	LittleEndian::Write(data_, colIndex_, 2, 2);
	LittleEndian::Write(data_, XFRecordIndex_, 4, 2);
	intdouble_.doublevalue_ = value_;
	LONGINT value = intdouble_.intvalue_;
	LittleEndian::Write(data_, value, 6, 8);
	return Record::Write(data);
}

Worksheet::CellTable::RowBlock::CellBlock::RK::RK() : Record(),
	rowIndex_(0), colIndex_(0), XFRecordIndex_(0), value_(0) {code_ = CODE::RK; dataSize_ = 10; recordSize_ = 14;}
ULONG Worksheet::CellTable::RowBlock::CellBlock::RK::Read(const char* data)
{
	Record::Read(data);
	LittleEndian::Read(data_, rowIndex_, 0, 2);
	LittleEndian::Read(data_, colIndex_, 2, 2);
	LittleEndian::Read(data_, XFRecordIndex_, 4, 2);
	LittleEndian::Read(data_, value_, 6, 4);
	return RecordSize();
}
ULONG Worksheet::CellTable::RowBlock::CellBlock::RK::Write(char* data)
{
	data_.resize(DataSize());
	LittleEndian::Write(data_, rowIndex_, 0, 2);
	LittleEndian::Write(data_, colIndex_, 2, 2);
	LittleEndian::Write(data_, XFRecordIndex_, 4, 2);
	LittleEndian::Write(data_, value_, 6, 4);
	return Record::Write(data);
}

Worksheet::CellTable::RowBlock::CellBlock::Formula::Formula() : Record(),
	rowIndex_(0), colIndex_(0), XFRecordIndex_(0), options_(0), unused_(0), type_(-1)
{
	code_ = CODE::FORMULA;
	dataSize_ = 20; recordSize_ = 24;
}	// was 18/22 instead of 20/24

ULONG Worksheet::CellTable::RowBlock::CellBlock::Formula::Read(const char* data)
{
	Record::Read(data);
	LittleEndian::Read(data_, rowIndex_, 0, 2);
	LittleEndian::Read(data_, colIndex_, 2, 2);
	LittleEndian::Read(data_, XFRecordIndex_, 4, 2);
	LittleEndian::ReadString(data_, result_, 6, 8);
	LittleEndian::Read(data_, options_, 14, 2);
	LittleEndian::Read(data_, unused_, 16, 4);	//MF was ", 16, 2"
	RPNtoken_.clear();
	RPNtoken_.resize(dataSize_-20); //MF was "-18"
	LittleEndian::ReadString(data_, &*(RPNtoken_.begin()), 20, dataSize_-20);	// was 18 instead of 20

	ULONG offset = dataSize_ + 4;
	short code;
	LittleEndian::Read(data, code, offset, 2);

	switch(code) {
		case CODE::ARRAY:
			type_ = code;
			array_.Read(data+offset);
			offset += array_.RecordSize();
			break;

		case CODE::SHRFMLA:
			type_ = code;
			shrfmla_.Read(data+offset);
			offset += shrfmla_.RecordSize();
			break;

		case CODE::SHRFMLA1:
			type_ = code;
			shrfmla1_.Read(data+offset);
			offset += shrfmla1_.RecordSize();
			break;

		case CODE::TABLE:
			type_ = code;
			table_.Read(data+offset);
			offset += table_.RecordSize();
			break;
	}
	LittleEndian::Read(data, code, offset, 2);

	if (code == CODE::STRING)
		string_.Read(data+offset);

	return RecordSize();
}
ULONG Worksheet::CellTable::RowBlock::CellBlock::Formula::Write(char* data)
{
	data_.resize(DataSize());
	LittleEndian::Write(data_, rowIndex_, 0, 2);
	LittleEndian::Write(data_, colIndex_, 2, 2);
	LittleEndian::Write(data_, XFRecordIndex_, 4, 2);
	LittleEndian::WriteString(data_, result_, 6, 8);
	LittleEndian::Write(data_, options_, 14, 2);
	unused_ = 0;	// The chn field should be ignored when you read the BIFF file. If you write a BIFF file, the chn field must be 00000000h.
	LittleEndian::Write(data_, unused_, 16, 4); //MF was ", 16, 2"
	LittleEndian::WriteString(data_, &*(RPNtoken_.begin()), 20, (ULONG)RPNtoken_.size());	// was 18 instead of 20
	Record::Write(data);

	ULONG offset = dataSize_ + 4;
	switch(type_) {
		case CODE::ARRAY:
			array_.Write(data+offset);
			offset += array_.RecordSize();
			break;

		case CODE::SHRFMLA:
			shrfmla_.Write(data+offset);
			offset += shrfmla_.RecordSize();
			break;

		case CODE::SHRFMLA1:
			shrfmla1_.Write(data+offset);
			offset += shrfmla1_.RecordSize();
			break;

		case CODE::TABLE:
			table_.Write(data+offset);
			offset += table_.RecordSize();
			break;
	}

	if (!string_.empty())
		string_.Write(data+offset);

	return RecordSize();
}
ULONG Worksheet::CellTable::RowBlock::CellBlock::Formula::DataSize()
{
	return dataSize_ = 20 + (ULONG)RPNtoken_.size(); // was 18 instead of 20
}
ULONG Worksheet::CellTable::RowBlock::CellBlock::Formula::RecordSize()
{
	ULONG dataSize = DataSize();
	recordSize_ = dataSize + 4*(dataSize/8224 + 1);

	switch(type_) {
		case CODE::ARRAY:
			recordSize_ += array_.RecordSize();
			break;

		case CODE::SHRFMLA:
			recordSize_ += shrfmla_.RecordSize();
			break;

		case CODE::SHRFMLA1:
			recordSize_ += shrfmla1_.RecordSize();
			break;

		case CODE::TABLE:
			recordSize_ += table_.RecordSize();
			break;
	}

	if (!string_.empty())
		recordSize_ += string_.RecordSize();

	return recordSize_;
}

Worksheet::CellTable::RowBlock::CellBlock::Formula::Array::Array() : Record(),
	firstRowIndex_(0), lastRowIndex_(0), firstColIndex_(0), lastColIndex_(0),
	options_(0), unused_(0)
	{code_ = CODE::ARRAY; dataSize_ = 12; recordSize_ = 16;}
ULONG Worksheet::CellTable::RowBlock::CellBlock::Formula::Array::Read(const char* data)
{
	Record::Read(data);
	LittleEndian::Read(data_, firstRowIndex_, 0, 2);
	LittleEndian::Read(data_, lastRowIndex_, 2, 2);
	LittleEndian::Read(data_, firstColIndex_, 4, 1);
	LittleEndian::Read(data_, lastColIndex_, 5, 1);
	LittleEndian::Read(data_, options_, 6, 2);
	LittleEndian::Read(data_, unused_, 8, 4);
	formula_.clear();
	formula_.resize(dataSize_-12);
	LittleEndian::ReadString(data_, &*(formula_.begin()), 12, dataSize_-12);
	return RecordSize();
}
ULONG Worksheet::CellTable::RowBlock::CellBlock::Formula::Array::Write(char* data)
{
	data_.resize(DataSize());
	LittleEndian::Write(data_, firstRowIndex_, 0, 2);
	LittleEndian::Write(data_, lastRowIndex_, 2, 2);
	LittleEndian::Write(data_, firstColIndex_, 4, 1);
	LittleEndian::Write(data_, lastColIndex_, 5, 1);
	LittleEndian::Write(data_, options_, 6, 2);
	LittleEndian::Write(data_, unused_, 8, 4);
	LittleEndian::WriteString(data_, &*(formula_.begin()), 12, (ULONG)formula_.size());
	return Record::Write(data);
}
ULONG Worksheet::CellTable::RowBlock::CellBlock::Formula::Array::DataSize()
{
	return dataSize_ = 12 + (ULONG)formula_.size();
}
ULONG Worksheet::CellTable::RowBlock::CellBlock::Formula::Array::RecordSize()
{
	ULONG dataSize = DataSize();
	return recordSize_ = dataSize + 4*(dataSize/8224 + 1);
}

Worksheet::CellTable::RowBlock::CellBlock::Formula::ShrFmla::ShrFmla() : Record(),
	firstRowIndex_(0), lastRowIndex_(0), firstColIndex_(0), lastColIndex_(0), unused_(0)
{
	code_ = CODE::SHRFMLA; dataSize_ = 8; recordSize_ = 12;
}
ULONG Worksheet::CellTable::RowBlock::CellBlock::Formula::ShrFmla::Read(const char* data)
{
	Record::Read(data);
	LittleEndian::Read(data_, firstRowIndex_, 0, 2);
	LittleEndian::Read(data_, lastRowIndex_, 2, 2);
	LittleEndian::Read(data_, firstColIndex_, 4, 1);
	LittleEndian::Read(data_, lastColIndex_, 5, 1);
	LittleEndian::Read(data_, unused_, 6, 2);
	formula_.clear();
	formula_.resize(dataSize_-8);
	LittleEndian::ReadString(data_, &*(formula_.begin()), 8, dataSize_-8);
	return RecordSize();
}
ULONG Worksheet::CellTable::RowBlock::CellBlock::Formula::ShrFmla::Write(char* data)
{
	data_.resize(DataSize());
	LittleEndian::Write(data_, firstRowIndex_, 0, 2);
	LittleEndian::Write(data_, lastRowIndex_, 2, 2);
	LittleEndian::Write(data_, firstColIndex_, 4, 1);
	LittleEndian::Write(data_, lastColIndex_, 5, 1);
	LittleEndian::Write(data_, unused_, 6, 2);
	LittleEndian::WriteString(data_, &*(formula_.begin()), 8, (ULONG)formula_.size());
	return Record::Write(data);
}
ULONG Worksheet::CellTable::RowBlock::CellBlock::Formula::ShrFmla::DataSize()
{
	return dataSize_ = 8 + (ULONG)formula_.size();
}
ULONG Worksheet::CellTable::RowBlock::CellBlock::Formula::ShrFmla::RecordSize()
{
	ULONG dataSize = DataSize();
	return recordSize_ = dataSize + 4*(dataSize/8224 + 1);
}

Worksheet::CellTable::RowBlock::CellBlock::Formula::ShrFmla1::ShrFmla1() : Record(),
	firstRowIndex_(0), lastRowIndex_(0), firstColIndex_(0), lastColIndex_(0),
	unused_(0)
{
	code_ = CODE::SHRFMLA1; dataSize_ = 8; recordSize_ = 12;
}
ULONG Worksheet::CellTable::RowBlock::CellBlock::Formula::ShrFmla1::Read(const char* data)
{
	Record::Read(data);
	LittleEndian::Read(data_, firstRowIndex_, 0, 2);
	LittleEndian::Read(data_, lastRowIndex_, 2, 2);
	LittleEndian::Read(data_, firstColIndex_, 4, 1);
	LittleEndian::Read(data_, lastColIndex_, 5, 1);
	LittleEndian::Read(data_, unused_, 6, 2);
	formula_.clear();
	formula_.resize(dataSize_-8);
	LittleEndian::ReadString(data_, &*(formula_.begin()), 8, dataSize_-8);
	return RecordSize();
}
ULONG Worksheet::CellTable::RowBlock::CellBlock::Formula::ShrFmla1::Write(char* data)
{
	data_.resize(DataSize());
	LittleEndian::Write(data_, firstRowIndex_, 0, 2);
	LittleEndian::Write(data_, lastRowIndex_, 2, 2);
	LittleEndian::Write(data_, firstColIndex_, 4, 1);
	LittleEndian::Write(data_, lastColIndex_, 5, 1);
	LittleEndian::Write(data_, unused_, 6, 2);
	LittleEndian::WriteString(data_, &*(formula_.begin()), 8, (ULONG)formula_.size());
	return Record::Write(data);
}
ULONG Worksheet::CellTable::RowBlock::CellBlock::Formula::ShrFmla1::DataSize()
{
	return dataSize_ = 8 + (ULONG)formula_.size();
}
ULONG Worksheet::CellTable::RowBlock::CellBlock::Formula::ShrFmla1::RecordSize()
{
	ULONG dataSize = DataSize();
	return recordSize_ = dataSize + 4*(dataSize/8224 + 1);
}

Worksheet::CellTable::RowBlock::CellBlock::Formula::Table::Table() : Record(),
	firstRowIndex_(0), lastRowIndex_(0), firstColIndex_(0), lastColIndex_(0), options_(0),
	inputCellRowIndex_(0), inputCellColIndex_(0),
	inputCellColumnInputRowIndex_(0), inputCellColumnInputColIndex_(0)
	{code_ = CODE::TABLE; dataSize_ = 16; recordSize_ = 20;}
ULONG Worksheet::CellTable::RowBlock::CellBlock::Formula::Table::Read(const char* data)
{
	Record::Read(data);
	LittleEndian::Read(data_, firstRowIndex_, 0, 2);
	LittleEndian::Read(data_, lastRowIndex_, 2, 2);
	LittleEndian::Read(data_, firstColIndex_, 4, 1);
	LittleEndian::Read(data_, lastColIndex_, 5, 1);
	LittleEndian::Read(data_, options_, 6, 2);
	LittleEndian::Read(data_, inputCellRowIndex_, 8, 2);
	LittleEndian::Read(data_, inputCellColIndex_, 10, 2);
	LittleEndian::Read(data_, inputCellColumnInputRowIndex_, 12, 2);
	LittleEndian::Read(data_, inputCellColumnInputColIndex_, 14, 2);
	return RecordSize();
}
ULONG Worksheet::CellTable::RowBlock::CellBlock::Formula::Table::Write(char* data)
{
	data_.resize(DataSize());
	LittleEndian::Write(data_, firstRowIndex_, 0, 2);
	LittleEndian::Write(data_, lastRowIndex_, 2, 2);
	LittleEndian::Write(data_, firstColIndex_, 4, 1);
	LittleEndian::Write(data_, lastColIndex_, 5, 1);
	LittleEndian::Write(data_, options_, 6, 2);
	LittleEndian::Write(data_, inputCellRowIndex_, 8, 2);
	LittleEndian::Write(data_, inputCellColIndex_, 10, 2);
	LittleEndian::Write(data_, inputCellColumnInputRowIndex_, 12, 2);
	LittleEndian::Write(data_, inputCellColumnInputColIndex_, 14, 2);
	return Record::Write(data);
}

Worksheet::CellTable::RowBlock::CellBlock::Formula::String::String()
 : Record()
{
	code_ = CODE::STRING; dataSize_ = 0; recordSize_ = 4;
	flag_ = 0;
	wstr_ = NULL;
}
Worksheet::CellTable::RowBlock::CellBlock::Formula::String::~String()
{
	Reset();
}
void Worksheet::CellTable::RowBlock::CellBlock::Formula::String::Reset()
{
	if (wstr_) {delete[] wstr_; wstr_ = NULL;}
}
ULONG Worksheet::CellTable::RowBlock::CellBlock::Formula::String::Read(const char* data)
{
	Reset();

	Record::Read(data);

	//MF
	short stringSize;
	LittleEndian::Read(data_, stringSize, 0, 2);
	LittleEndian::Read(data_, flag_, 2, 1);

	wstr_ = new wchar_t[stringSize+1];
	ULONG bytesRead = 7;

	if (flag_ == 0) { // compressed UTF16LE string?
		char* str = (char*) alloca(stringSize+1);
		LittleEndian::ReadString(data_, str, 3, stringSize);
		str[stringSize] = 0;
		mbstowcs(wstr_, str, stringSize);
		wstr_[stringSize] = 0;
		bytesRead += stringSize;
	} else {
		LittleEndian::ReadString(data_, wstr_, 3, stringSize);
		wstr_[stringSize] = 0;
		bytesRead += stringSize*2;
	}

	return bytesRead;//RecordSize();
}
ULONG Worksheet::CellTable::RowBlock::CellBlock::Formula::String::Write(char* data)
{
	data_.resize(DataSize());

	short stringSize = (short) wcslen(wstr_);
	LittleEndian::Write(data_, stringSize, 0, 2);
	LittleEndian::Write(data_, flag_, 2, 1);

	if (flag_ == 0) { // compressed UTF16LE string?
		char* str = (char*) alloca(stringSize);
		wcstombs(str, wstr_, stringSize);
		LittleEndian::WriteString(data_, str, 3, stringSize);
	} else {
		LittleEndian::WriteString(data_, wstr_, 3, stringSize);
	}

	return Record::Write(data);
}
ULONG Worksheet::CellTable::RowBlock::CellBlock::Formula::String::DataSize()
{
	if (wstr_) {
		dataSize_ = 3;

		if (flag_ == 0) // compressed UTF16LE string?
			dataSize_ += (ULONG) wcslen(wstr_);
		else
			dataSize_ += (ULONG) wcslen(wstr_) * SIZEOFWCHAR_T;
	} else
		dataSize_ = 0;

	return dataSize_;
}
ULONG Worksheet::CellTable::RowBlock::CellBlock::Formula::String::RecordSize()
{
	ULONG dataSize = DataSize();
	return recordSize_ = dataSize + 4*(dataSize/8224 + 1);
}
/************************************************************************************************************/

/************************************************************************************************************/
Worksheet::CellTable::RowBlock::Row::Row() : Record(),
	rowIndex_(0), firstCellColIndex_(0), lastCellColIndexPlusOne_(0), height_(255),
	unused1_(0), unused2_(0), options_(0x100/*MF: documentation says "Always 1" for the 0x100 bit*/) {code_ = CODE::ROW; dataSize_ = 16; recordSize_ = 20;}
ULONG Worksheet::CellTable::RowBlock::Row::Read(const char* data)
{
	Record::Read(data);
	LittleEndian::Read(data_, rowIndex_, 0, 2);
	LittleEndian::Read(data_, firstCellColIndex_, 2, 2);
	LittleEndian::Read(data_, lastCellColIndexPlusOne_, 4, 2);
	LittleEndian::Read(data_, height_, 6, 2);
	LittleEndian::Read(data_, unused1_, 8, 2);
	LittleEndian::Read(data_, unused2_, 10, 2);
	LittleEndian::Read(data_, options_, 12, 4);
	return RecordSize();
}
ULONG Worksheet::CellTable::RowBlock::Row::Write(char* data)
{
	data_.resize(DataSize());
	LittleEndian::Write(data_, rowIndex_, 0, 2);
	LittleEndian::Write(data_, firstCellColIndex_, 2, 2);
	LittleEndian::Write(data_, lastCellColIndexPlusOne_, 4, 2);
	LittleEndian::Write(data_, height_, 6, 2);
	LittleEndian::Write(data_, unused1_, 8, 2);
	LittleEndian::Write(data_, unused2_, 10, 2);
	LittleEndian::Write(data_, options_, 12, 4);
	return Record::Write(data);
}
/************************************************************************************************************/

/************************************************************************************************************/
Worksheet::CellTable::RowBlock::CellBlock::CellBlock()
 :	type_(-1)
{
	_union.void_ = NULL;
}

Worksheet::CellTable::RowBlock::CellBlock::~CellBlock()
{
	if (_union.void_)
		Reset();
}

void Worksheet::CellTable::RowBlock::CellBlock::Reset()
{
	switch(type_) {
		case CODE::BLANK:
			delete _union.blank_;
			break;

		case CODE::MULBLANK:
			delete _union.mulblank_;
			break;

		case CODE::BOOLERR:
			delete _union.boolerr_;
			break;

		case CODE::LABELSST:
			delete _union.labelsst_;
			break;

		case CODE::MULRK:
			delete _union.mulrk_;
			break;

		case CODE::NUMBER:
			delete _union.number_;
			break;

		case CODE::RK:
			delete _union.rk_;
			break;

		case CODE::FORMULA:
			delete _union.formula_;
			break;
	}

	type_ = -1;
	_union.void_ = NULL;
}

void Worksheet::CellTable::RowBlock::CellBlock::SetType(int type)
{
	if (type_ == type)
		return;

	if (_union.void_)
		Reset();

	type_ = type;

	switch(type_) {
		case CODE::BLANK:
			_union.blank_ = new Blank;
			break;

		case CODE::MULBLANK:
			_union.mulblank_ = new MulBlank;
			break;

		case CODE::BOOLERR:
			_union.boolerr_ = new BoolErr;
			break;

		case CODE::LABELSST:
			_union.labelsst_ = new LabelSST;
			break;

		case CODE::MULRK:
			_union.mulrk_ = new MulRK;
			break;

		case CODE::NUMBER:
			_union.number_ = new Number;
			break;

		case CODE::RK:
			_union.rk_ = new RK;
			break;

		case CODE::FORMULA:
			_union.formula_ = new Formula;
			break;

		default:
			assert(0);
	}
}

ULONG Worksheet::CellTable::RowBlock::CellBlock::Read(const char* data)
{
	ULONG bytesRead = 0;

	int type;
	LittleEndian::Read(data, type, 0, 2);
	SetType(type);

	switch(type_) {
		case CODE::BLANK:
			_union.blank_ = new Blank;
			bytesRead += _union.blank_->Read(data);
			break;

		case CODE::MULBLANK:
			_union.mulblank_ = new MulBlank;
			bytesRead += _union.mulblank_->Read(data);
			break;

		case CODE::BOOLERR:
			_union.boolerr_ = new BoolErr;
			bytesRead += _union.boolerr_->Read(data);
			break;

		case CODE::LABELSST:
			_union.labelsst_ = new LabelSST;
			bytesRead += _union.labelsst_->Read(data);
			break;

		case CODE::MULRK:
			_union.mulrk_ = new MulRK;
			bytesRead +=_union. mulrk_->Read(data);
			break;

		case CODE::NUMBER:
			_union.number_ = new Number;
			bytesRead += _union.number_->Read(data);
			break;

		case CODE::RK:
			_union.rk_ = new RK;
			bytesRead += _union.rk_->Read(data);
			break;

		case CODE::FORMULA:
			_union.formula_ = new Formula;
			bytesRead += _union.formula_->Read(data);
			break;
	}

	return bytesRead;
}
ULONG Worksheet::CellTable::RowBlock::CellBlock::Write(char* data)
{
	ULONG bytesWritten = 0;

	switch(type_) {
		case CODE::BLANK:
			bytesWritten += _union.blank_->Write(data);
			break;

		case CODE::MULBLANK:
			bytesWritten += _union.mulblank_->Write(data);
			break;

		case CODE::BOOLERR:
			bytesWritten += _union.boolerr_->Write(data);
			break;

		case CODE::LABELSST:
			bytesWritten += _union.labelsst_->Write(data);
			break;

		case CODE::MULRK:
			bytesWritten += _union.mulrk_->Write(data);
			break;

		case CODE::NUMBER:
			bytesWritten += _union.number_->Write(data);
			break;

		case CODE::RK:
			bytesWritten += _union.rk_->Write(data);
			break;

		case CODE::FORMULA:
			bytesWritten += _union.formula_->Write(data);
			break;
	}

	return bytesWritten;
}
ULONG Worksheet::CellTable::RowBlock::CellBlock::DataSize()
{
	switch(type_) {
		case CODE::BLANK:
			return _union.blank_->DataSize();

		case CODE::MULBLANK:
			return _union.mulblank_->DataSize();

		case CODE::BOOLERR:
			return _union.boolerr_->DataSize();

		case CODE::LABELSST:
			return _union.labelsst_->DataSize();

		case CODE::MULRK:
			return _union.mulrk_->DataSize();

		case CODE::NUMBER:
			return _union.number_->DataSize();

		case CODE::RK:
			return _union.rk_->DataSize();

		case CODE::FORMULA:
			return _union.formula_->DataSize();
	}
	abort();
	return 0;
}
ULONG Worksheet::CellTable::RowBlock::CellBlock::RecordSize()
{
	switch(type_) {
		case CODE::BLANK:
			return _union.blank_->RecordSize();

		case CODE::MULBLANK:
			return _union.mulblank_->RecordSize();

		case CODE::BOOLERR:
			return _union.boolerr_->RecordSize();

		case CODE::LABELSST:
			return _union.labelsst_->RecordSize();

		case CODE::MULRK:
			return _union.mulrk_->RecordSize();

		case CODE::NUMBER:
			return _union.number_->RecordSize();

		case CODE::RK:
			return _union.rk_->RecordSize();

		case CODE::FORMULA:
			return _union.formula_->RecordSize();
	}
	abort();
	return 0;
}
USHORT Worksheet::CellTable::RowBlock::CellBlock::RowIndex()
{
	switch(type_) {
		case CODE::BLANK:
			return _union.blank_->rowIndex_;

		case CODE::MULBLANK:
			return _union.mulblank_->rowIndex_;

		case CODE::BOOLERR:
			return _union.boolerr_->rowIndex_;

		case CODE::LABELSST:
			return _union.labelsst_->rowIndex_;

		case CODE::MULRK:
			return _union.mulrk_->rowIndex_;

		case CODE::NUMBER:
			return _union.number_->rowIndex_;

		case CODE::RK:
			return _union.rk_->rowIndex_;

		case CODE::FORMULA:
			return _union.formula_->rowIndex_;
	}
	abort();
	return 0;
}
USHORT Worksheet::CellTable::RowBlock::CellBlock::ColIndex()
{
	switch(type_) {
		case CODE::BLANK:
			return _union.blank_->colIndex_;

		case CODE::MULBLANK:
			return _union.mulblank_->firstColIndex_;

		case CODE::BOOLERR:
			return _union.boolerr_->colIndex_;

		case CODE::LABELSST:
			return _union.labelsst_->colIndex_;

		case CODE::MULRK:
			return _union.mulrk_->firstColIndex_;

		case CODE::NUMBER:
			return _union.number_->colIndex_;

		case CODE::RK:
			return _union.rk_->colIndex_;

		case CODE::FORMULA:
			return _union.formula_->colIndex_;
	}
	abort();
	return 0;
}

/************************************************************************************************************/

/************************************************************************************************************/
Worksheet::CellTable::RowBlock::DBCell::DBCell() : Record(),
	firstRowOffset_(0)
{
	code_ = CODE::DBCELL;
	dataSize_ = 4; recordSize_ = 8;
}
ULONG Worksheet::CellTable::RowBlock::DBCell::Read(const char* data)
{
	Record::Read(data);
	LittleEndian::Read(data_, firstRowOffset_, 0, 4);
	size_t nm = (dataSize_-4) / 2;
	offsets_.clear();
	offsets_.resize(nm);

	for(size_t i=0; i<nm; ++i)
		LittleEndian::Read(data_, offsets_[i], 4+i*2, 2);

	return RecordSize();
}
ULONG Worksheet::CellTable::RowBlock::DBCell::Write(char* data)
{
	data_.resize(DataSize());
	LittleEndian::Write(data_, firstRowOffset_, 0, 4);
	size_t nm = offsets_.size();

	for(size_t i=0; i<nm; ++i)
		LittleEndian::Write(data_, offsets_[i], 4+i*2, 2);

	return Record::Write(data);
}
ULONG Worksheet::CellTable::RowBlock::DBCell::DataSize()
{
	return dataSize_ = 4+(ULONG)offsets_.size()*2;
}
ULONG Worksheet::CellTable::RowBlock::DBCell::RecordSize()
{
	ULONG dataSize = DataSize();
	return recordSize_ = dataSize + 4*(dataSize/8224 + 1);
}
/************************************************************************************************************/

/************************************************************************************************************/
ULONG Worksheet::CellTable::RowBlock::Read(const char* data)
{
	ULONG bytesRead = 0;

	short code;
	LittleEndian::Read(data, code, 0, 2);

	Row row;
	cellBlocks_.reserve(1000);
	while(code != CODE::DBCELL) {
		switch(code) {
			case CODE::ROW:
				rows_.push_back(row);
				bytesRead += rows_.back().Read(data+bytesRead);
				break;

			case CODE::BLANK:
			case CODE::MULBLANK:
			case CODE::BOOLERR:
			case CODE::LABELSST:
			case CODE::MULRK:
			case CODE::NUMBER:
			case CODE::RK:
			case CODE::FORMULA:
				cellBlocks_.push_back(new CellBlock);
				if (cellBlocks_.size()%1000==0) cellBlocks_.reserve(cellBlocks_.size()+1000);
				bytesRead += cellBlocks_[cellBlocks_.size()-1]->Read(data+bytesRead);
				break;

			default:
				Record rec;
				bytesRead += rec.Read(data+bytesRead);
		}

		//MF: finish loop and skip reading the following expected records, if there is a EOF marker before DBCELL
		// (written by the XLS export of MacOS Numbers.app)
		if (code == CODE::YEOF)
			throw EXCEPTION_YEOF(bytesRead);

		LittleEndian::Read(data, code, bytesRead, 2);
	}
	bytesRead += dbcell_.Read(data+bytesRead);
	return bytesRead;
}
ULONG Worksheet::CellTable::RowBlock::Write(char* data)
{
	ULONG bytesWritten = 0;

	size_t maxRows = rows_.size();
	{for(size_t i=0; i<maxRows; ++i)
	{
		bytesWritten += rows_[i].Write(data+bytesWritten);
	}}

	size_t maxCellBlocks = cellBlocks_.size();
	{for(size_t i=0; i<maxCellBlocks; ++i)
	{
		bytesWritten += cellBlocks_[i]->Write(data+bytesWritten);
	}}

	bytesWritten += dbcell_.Write(data+bytesWritten);
	return bytesWritten;
}
ULONG Worksheet::CellTable::RowBlock::DataSize()
{
	ULONG dataSize = 0;
	size_t maxRows = rows_.size();
	{for(size_t i=0; i<maxRows; ++i)
		dataSize += rows_[i].RecordSize();}

	size_t maxCellBlocks = cellBlocks_.size();
	{for(size_t i=0; i<maxCellBlocks; ++i)
		dataSize += cellBlocks_[i]->RecordSize();}

	dataSize += dbcell_.RecordSize();
	return dataSize;
}
ULONG Worksheet::CellTable::RowBlock::RecordSize() {return DataSize();}
/************************************************************************************************************/

/************************************************************************************************************/
ULONG Worksheet::CellTable::Read(const char* data)
{
	ULONG bytesRead = 0;

	short code;
	LittleEndian::Read(data, code, 0, 2);

	RowBlock rowBlock;
	rowBlocks_.reserve(1000);
	while(code == CODE::ROW) {
		rowBlocks_.push_back(rowBlock);
		bytesRead += rowBlocks_.back().Read(data+bytesRead);
		LittleEndian::Read(data, code, bytesRead, 2);
	}

	return bytesRead;
}
ULONG Worksheet::CellTable::Write(char* data)
{
	ULONG bytesWritten = 0;
	size_t maxRowBlocks_ = rowBlocks_.size();

	for(size_t i=0; i<maxRowBlocks_; ++i)
		bytesWritten += rowBlocks_[i].Write(data+bytesWritten);

	return bytesWritten;
}
ULONG Worksheet::CellTable::DataSize()
{
	ULONG dataSize = 0;
	size_t maxRowBlocks_ = rowBlocks_.size();

	for(size_t i=0; i<maxRowBlocks_; ++i)
		dataSize += rowBlocks_[i].RecordSize();

	return dataSize;
}
ULONG Worksheet::CellTable::RecordSize() {return DataSize();}
/************************************************************************************************************/

/************************************************************************************************************/
Worksheet::Window2::Window2() : Record(),
	options_(1718), firstVisibleRowIndex_(0), firstVisibleColIndex_(0), gridLineColourIndex_(64),
	unused1_(0), magnificationFactorPageBreakPreview_(0), magnificationFactorNormalView_(0), unused2_(0)
	{code_ = CODE::WINDOW2; dataSize_ = 18; recordSize_ = 22;}

ULONG Worksheet::Window2::Read(const char* data)
{
	Record::Read(data);
	LittleEndian::Read(data_, options_, 0, 2);
	LittleEndian::Read(data_, firstVisibleRowIndex_, 2, 2);
	LittleEndian::Read(data_, firstVisibleColIndex_, 4, 2);
	LittleEndian::Read(data_, gridLineColourIndex_, 6, 2);
	LittleEndian::Read(data_, unused1_, 8, 2);
	LittleEndian::Read(data_, magnificationFactorPageBreakPreview_, 10, 2);
	LittleEndian::Read(data_, magnificationFactorNormalView_, 12, 2);
	LittleEndian::Read(data_, unused2_, 14, 4);
	return RecordSize();
}
ULONG Worksheet::Window2::Write(char* data)
{
	data_.resize(DataSize());
	LittleEndian::Write(data_, options_, 0, 2);
	LittleEndian::Write(data_, firstVisibleRowIndex_, 2, 2);
	LittleEndian::Write(data_, firstVisibleColIndex_, 4, 2);
	LittleEndian::Write(data_, gridLineColourIndex_, 6, 2);
	LittleEndian::Write(data_, unused1_, 8, 2);
	LittleEndian::Write(data_, magnificationFactorPageBreakPreview_, 10, 2);
	LittleEndian::Write(data_, magnificationFactorNormalView_, 12, 2);
	LittleEndian::Write(data_, unused2_, 14, 4);
	return Record::Write(data);
}
/************************************************************************************************************/

/************************************************************************************************************/
ULONG Worksheet::MergedCells::Read(const char* data)
{
	ULONG bytesRead = 0;

	short code;
	LittleEndian::Read(data, code, 0, 2);

	short dataSize;
	size_t nbMergedCells;
	MergedCell mergedCell;
	mergedCellsVector_.reserve(1000);
	while(code == CODE::MERGECELLS)
	{
		bytesRead += 2;

		LittleEndian::Read(data, dataSize, bytesRead, 2);
		bytesRead += 2;

		LittleEndian::Read(data, nbMergedCells, bytesRead, 2);
		bytesRead += 2;

		for(size_t i = 0; i < nbMergedCells; i++)
		{
			mergedCellsVector_.push_back(mergedCell);
			bytesRead += mergedCellsVector_.back().Read(data+bytesRead);
		}

		LittleEndian::Read(data, code, bytesRead, 2);
	}

	return bytesRead;
}
ULONG Worksheet::MergedCells::Write(char* data)
{
	ULONG bytesWritten = 0;
	size_t mergedCellsLeft_ = mergedCellsVector_.size();
	short maxPackedMergedCells;

	while(mergedCellsLeft_) {
		if (mergedCellsLeft_ > 1027)
			maxPackedMergedCells = 1027;
		else
			maxPackedMergedCells = (short) mergedCellsLeft_;

		short code = CODE::MERGECELLS;
		LittleEndian::Write(data, code, bytesWritten, 2);
		bytesWritten += 2;
		LittleEndian::Write(data, maxPackedMergedCells * 8 + 2, bytesWritten, 2);
		bytesWritten += 2;
		LittleEndian::Write(data, maxPackedMergedCells, bytesWritten, 2);
		bytesWritten += 2;

		for(short i = 0; i < maxPackedMergedCells; ++i)
			bytesWritten += mergedCellsVector_[i].Write(data+bytesWritten);

		mergedCellsLeft_ -= maxPackedMergedCells;
	}

	return bytesWritten;
}
ULONG Worksheet::MergedCells::DataSize()
{
	ULONG dataSize = 0;
	size_t maxMergedCells_ = mergedCellsVector_.size();

	for(size_t i=0; i<maxMergedCells_; ++i)
		dataSize += mergedCellsVector_[i].RecordSize();

	return dataSize;
}
ULONG Worksheet::MergedCells::RecordSize()
{
	ULONG dataSize = DataSize();
	size_t nbMergedCellsPack = 1;
	size_t mergedCellsCount = mergedCellsVector_.size();

	while(mergedCellsCount > 1027) {
		mergedCellsCount -= 1027;
		nbMergedCellsPack++;
	}

	if (dataSize == 0)
		return 0;

	return dataSize + (ULONG)nbMergedCellsPack * 6;
}
/************************************************************************************************************/

/************************************************************************************************************/
Worksheet::MergedCells::MergedCell::MergedCell()
 : firstRow_(0), lastRow_(0), firstColumn_(0), lastColumn_(0)
{
}

ULONG Worksheet::MergedCells::MergedCell::Read(const char* data)
{
	vector<char> data_;
	data_.assign(data, data+DataSize());

	// read REF record
	LittleEndian::Read(data_, firstRow_, 0, 2);
	LittleEndian::Read(data_, lastRow_, 2, 2);
	LittleEndian::Read(data_, firstColumn_, 4, 2);
	LittleEndian::Read(data_, lastColumn_, 6, 2);
	return RecordSize();
}
ULONG Worksheet::MergedCells::MergedCell::Write(char* data)
{
	// write REF record
	LittleEndian::Write(data, firstRow_, 0, 2);
	LittleEndian::Write(data, lastRow_, 2, 2);
	LittleEndian::Write(data, firstColumn_, 4, 2);
	LittleEndian::Write(data, lastColumn_, 6, 2);

	return DataSize();
}
ULONG Worksheet::MergedCells::MergedCell::DataSize() {return 8;}
ULONG Worksheet::MergedCells::MergedCell::RecordSize() {return DataSize();}
/************************************************************************************************************/

//MF union to work with a RK value (encoded integer or floating point value)
union RKValueUnion {
	LONGINT intvalue_;
	double doublevalue_;
};

// Convert a double to a RK value.
LONG GetRKValueFromDouble(double value)
{
	bool isMultiplied = false;
	LONG testVal1 = (LONG)value * 100;
	LONG testVal2 = (LONG)(value * 100);
	if (testVal1 != testVal2) {
		isMultiplied = true;
		value *= 100;
	}

	RKValueUnion intdouble;

	// CODE ADDDED 2010/09/08 by VaKa: missing initialization
	intdouble.intvalue_ = 0; //MF: should not be neccessary, as intvalue_ and doublevalue_ use the same bytes in memory

	intdouble.doublevalue_ = value;
	intdouble.intvalue_ >>= 34;

	LONG rkValue = (LONG) intdouble.intvalue_;
	rkValue <<= 2;
	rkValue |= (isMultiplied? 1: 0);
	return rkValue;
}

// Convert an integer to a RK value.
LONG GetRKValueFromInteger(int value)
{
	value <<= 2;
	value |= 2;

	return value;
}

// Returns true if the supplied double can be stored as a RK value.
bool CanStoreAsRKValue(double value)
{
	LONG testVal1 = 100 * (LONG)(value * 100);
	LONG testVal2 = LONG(value * 10000);

	if (testVal1 != testVal2)
		return false;
	else
		return true;
}

/************************************************************************************************************/

/************************************************************************************************************/
BasicExcel::BasicExcel() {}
BasicExcel::BasicExcel(const char* filename)
{
	Load(filename);
}

BasicExcel::~BasicExcel()
{
	Close();
}

void BasicExcel::Close()
{
	if (file_.IsOpen())
		file_.Close();
}

// Create a new Excel workbook with a given number of spreadsheets (Minimum 1)
void BasicExcel::New(int sheets)
{
	workbook_ = Workbook();
	worksheets_.clear();

	workbook_.fonts_.resize(1); //MF was 4, see XLSFormatManager::get_font_idx()
	workbook_.XFs_.resize(21);
	workbook_.styles_.resize(6);
	workbook_.boundSheets_.resize(1);
	worksheets_.resize(1);
	UpdateYExcelWorksheet();

	for(int i=0; i<sheets-1; ++i)
		AddWorksheet();
}

// Load an Excel workbook from a file.
bool BasicExcel::Load(const char* filename)
{
	if (file_.IsOpen())
		file_.Close();

	if (file_.Open(filename)) {
		workbook_ = Workbook();
		worksheets_.clear();

		vector<char> data;
		file_.ReadFile("Workbook", data);
		Read(&*(data.begin()), data.size());

		UpdateYExcelWorksheet();

		return true;
	} else
		return false;
}

// Load an Excel workbook from a file with Unicode filename.
bool BasicExcel::Load(const wchar_t* filename)
{
	if (file_.IsOpen())
		file_.Close();

	if (file_.Open(filename)) {
		workbook_ = Workbook();
		worksheets_.clear();

		vector<char> data;
		file_.ReadFile("Workbook", data);
		Read(&*(data.begin()), data.size());

		UpdateYExcelWorksheet();

		return true;
	} else
		return false;
}

// Save current Excel workbook to opened file.
bool BasicExcel::Save()
{
	if (file_.IsOpen()) {
		// Prepare Raw Worksheets for saving.
		UpdateWorksheets();

		AdjustStreamPositions();

		// Calculate bytes needed for a workbook.
		size_t minBytes = workbook_.RecordSize();
		size_t maxWorkSheets = worksheets_.size();

		for(size_t i=0; i<maxWorkSheets; ++i)
			minBytes += worksheets_[i].RecordSize();

		// Create new workbook.
		vector<char> data(minBytes, 0);
		Write(&*(data).begin());

		if (file_.WriteFile("Workbook", data, (ULONG)data.size()) != SUCCESS)
			return false;
		else
			return true;
	} else
		return false;
}

// Save current Excel workbook to a file.
bool BasicExcel::SaveAs(const char* filename)
{
	if (file_.IsOpen())
		file_.Close();

	if (!file_.Create(filename))
		return false;

	if (file_.MakeFile("Workbook") != SUCCESS)
		return false;

	return Save();
}

// Save current Excel workbook to a file with Unicode filename
bool BasicExcel::SaveAs(const wchar_t* filename)
{
	if (file_.IsOpen())
		file_.Close();

	if (!file_.Create(filename))
		return false;

	if (file_.MakeFile("Workbook") != SUCCESS)
		return false;

	return Save();
}

// Total number of Excel worksheets in current Excel workbook.
int BasicExcel::GetTotalWorkSheets()
{
	return (int) worksheets_.size();
}

// Get a pointer to an Excel worksheet at the given index.
// Index starts from 0.
// Returns 0 if index is invalid.
BasicExcelWorksheet* BasicExcel::GetWorksheet(int sheetIndex)
{
	return &*(yesheets_[sheetIndex]);
}

// Get a pointer to an Excel worksheet that has given ANSI name.
// Returns 0 if there is no Excel worksheet with the given name.
BasicExcelWorksheet* BasicExcel::GetWorksheet(const char* name)
{
	int maxWorksheets = (int) yesheets_.size();

	for(int i=0; i<maxWorksheets; ++i) {
		if (workbook_.boundSheets_[i].name_.unicode_ & 1)
			continue;

		if (strcmp(name, workbook_.boundSheets_[i].name_.name_) == 0)
			return &*yesheets_[i];
	}

	return NULL;
}

// Get a pointer to an Excel worksheet that has given Unicode name.
// Returns 0 if there is no Excel worksheet with the given name.
BasicExcelWorksheet* BasicExcel::GetWorksheet(const wchar_t* name)
{
	int maxWorksheets = (int) yesheets_.size();

	for(int i=0; i<maxWorksheets; ++i) {
		if (!(workbook_.boundSheets_[i].name_.unicode_ & 1))
			continue;

		if (wcscmp(name, workbook_.boundSheets_[i].name_.wname_) == 0)
			return &*yesheets_[i];
	}

	return NULL;
}

// Add a new Excel worksheet to the given index.
// Name given to worksheet is SheetX, where X is a number which starts from 1.
// Index starts from 0.
// Worksheet is added to the last position if sheetIndex == -1.
// Returns a pointer to the worksheet if successful, 0 if otherwise.
BasicExcelWorksheet* BasicExcel::AddWorksheet(int sheetIndex)
{
	int sheetNo = (int)yesheets_.size() + 1;
	BasicExcelWorksheet* yesheet = 0;

	do {
		char sname[50];
		sprintf(sname, "Sheet%d", sheetNo++);
		yesheet = AddWorksheet(sname, sheetIndex);
	} while(!yesheet);

	return yesheet;
}

// Add a new Excel worksheet with given ANSI name to the given index.
// Index starts from 0.
// Worksheet is added to the last position if sheetIndex == -1.
// Returns a pointer to the worksheet if successful, 0 if otherwise.
BasicExcelWorksheet* BasicExcel::AddWorksheet(const char* name, int sheetIndex)
{
	int maxWorksheets = (int) yesheets_.size();

	for(int i=0; i<maxWorksheets; ++i) {
		if (workbook_.boundSheets_[i].name_.unicode_ & 1)
			continue;

		if (strcmp(name, workbook_.boundSheets_[i].name_.name_) == 0)
			return NULL;
	}

	Workbook::BoundSheet* boundSheet;
	Worksheet* worksheet;
	BasicExcelWorksheet* yesheet;

	if (sheetIndex == -1) {
		workbook_.boundSheets_.push_back(Workbook::BoundSheet());
		worksheets_.push_back(Worksheet());
		yesheets_.push_back(new BasicExcelWorksheet(this, (int)worksheets_.size()-1));
		boundSheet = &(workbook_.boundSheets_.back());
		worksheet = &(worksheets_.back());
		yesheet = &*yesheets_.back();
	} else {
		boundSheet = &*(workbook_.boundSheets_.insert(workbook_.boundSheets_.begin()+sheetIndex, Workbook::BoundSheet()));
		worksheet = &*(worksheets_.insert(worksheets_.begin()+sheetIndex, Worksheet()));
		yesheet = &**(yesheets_.insert(yesheets_.begin()+sheetIndex, new BasicExcelWorksheet(this, sheetIndex)));
		int maxSheets = (int) worksheets_.size();

		for(int i=sheetIndex+1; i<maxSheets; ++i)
			yesheets_[i]->sheetIndex_ = i;
	}

	boundSheet->name_ = name;
	worksheet->window2_.options_ &= ~0x200;
	return yesheet;
}

// Add a new Excel worksheet with given Unicode name to the given index.
// Index starts from 0.
// Worksheet is added to the last position if sheetIndex == -1.
// Returns a pointer to the worksheet if successful, 0 if otherwise.
BasicExcelWorksheet* BasicExcel::AddWorksheet(const wchar_t* name, int sheetIndex)
{
	int maxWorksheets = (int)yesheets_.size();

	for(int i=0; i<maxWorksheets; ++i) {
		if (!(workbook_.boundSheets_[i].name_.unicode_ & 1))
			continue;

		if (wcscmp(name, workbook_.boundSheets_[i].name_.wname_) == 0)
			return NULL;
	}

	Workbook::BoundSheet* boundSheet;
	Worksheet* worksheet;
	BasicExcelWorksheet* yesheet;

	if (sheetIndex == -1) {
		workbook_.boundSheets_.push_back(Workbook::BoundSheet());
		worksheets_.push_back(Worksheet());
		yesheets_.push_back(new BasicExcelWorksheet(this, (int)worksheets_.size()-1));
		boundSheet = &(workbook_.boundSheets_.back());
		worksheet = &(worksheets_.back());
		yesheet = &*yesheets_.back();
	} else {
		boundSheet = &*(workbook_.boundSheets_.insert(workbook_.boundSheets_.begin()+sheetIndex, Workbook::BoundSheet()));
		worksheet = &*(worksheets_.insert(worksheets_.begin()+sheetIndex, Worksheet()));
		yesheet = &**(yesheets_.insert(yesheets_.begin()+sheetIndex, new BasicExcelWorksheet(this, sheetIndex)));
		int maxSheets = (int) worksheets_.size();

		for(int i=sheetIndex+1; i<maxSheets; ++i)
			yesheets_[i]->sheetIndex_ = i;
	}
	boundSheet->name_ = name;
	worksheet->window2_.options_ &= ~0x200;
	return yesheet;
}

// Delete an Excel worksheet at the given index.
// Index starts from 0.
// Returns true if successful, false if otherwise.
bool BasicExcel::DeleteWorksheet(int sheetIndex)
{
	if (sheetIndex < (int)workbook_.boundSheets_.size()) {
		workbook_.boundSheets_.erase(workbook_.boundSheets_.begin()+sheetIndex);
		worksheets_.erase(worksheets_.begin()+sheetIndex);
		yesheets_.erase(yesheets_.begin()+sheetIndex);
		return true;
	} else
		return false;
}

// Delete an Excel worksheet that has given ANSI name.
// Returns true if successful, false if otherwise.
bool BasicExcel::DeleteWorksheet(const char* name)
{
	int maxWorksheets = (int)yesheets_.size();

	for(int i=0; i<maxWorksheets; ++i) {
		if (workbook_.boundSheets_[i].name_.unicode_ & 1)
			continue;

		if (strcmp(name, workbook_.boundSheets_[i].name_.name_) == 0)
			return DeleteWorksheet(i);
	}
	return false;
}

// Delete an Excel worksheet that has given Unicode name.
// Returns true if successful, false if otherwise.
bool BasicExcel::DeleteWorksheet(const wchar_t* name)
{
	int maxWorksheets = (int) worksheets_.size();

	for(int i=0; i<maxWorksheets; ++i) {
		if (!(workbook_.boundSheets_[i].name_.unicode_ & 1))
			continue;

		if (wcscmp(name, workbook_.boundSheets_[i].name_.wname_) == 0)
			return DeleteWorksheet(i);
	}

	return false;
}

// Get the worksheet name at the given index.
// Index starts from 0.
// Returns 0 if name is in Unicode format.
char* BasicExcel::GetAnsiSheetName(int sheetIndex)
{
	if (!(workbook_.boundSheets_[sheetIndex].name_.unicode_ & 1))
		return workbook_.boundSheets_[sheetIndex].name_.name_;
	else
		return NULL;
}

// Get the worksheet name at the given index.
// Index starts from 0.
// Returns 0 if name is in Ansi format.
wchar_t* BasicExcel::GetUnicodeSheetName(int sheetIndex)
{
	if (workbook_.boundSheets_[sheetIndex].name_.unicode_ & 1)
		return workbook_.boundSheets_[sheetIndex].name_.wname_;
	else
		return NULL;
}

// Get the worksheet name at the given index.
// Index starts from 0.
// Returns false if name is in Unicode format.
bool BasicExcel::GetSheetName(int sheetIndex, char* name)
{
	if (!(workbook_.boundSheets_[sheetIndex].name_.unicode_ & 1))
	{
		strcpy(name, workbook_.boundSheets_[sheetIndex].name_.name_);
		return true;
	}
	else return false;
}

// Get the worksheet name at the given index.
// Index starts from 0.
// Returns false if name is in Ansi format.
bool BasicExcel::GetSheetName(int sheetIndex, wchar_t* name)
{
	if (workbook_.boundSheets_[sheetIndex].name_.unicode_ & 1)
	{
		wcscpy(name, workbook_.boundSheets_[sheetIndex].name_.wname_);
		return true;
	}
	else return false;
}

// Rename an Excel worksheet at the given index to the given ANSI name.
// Index starts from 0.
// Returns true if successful, false if otherwise.
bool BasicExcel::RenameWorksheet(int sheetIndex, const char* to)
{
	int maxWorksheets = (int)yesheets_.size();

	if (sheetIndex < maxWorksheets) {
		for(int i=0; i<maxWorksheets; ++i) {
			if (workbook_.boundSheets_[i].name_.unicode_ & 1)
				continue;

			if (strcmp(to, workbook_.boundSheets_[i].name_.name_) == 0)
				return false;
		}

		workbook_.boundSheets_[sheetIndex].name_ = to;
		return true;
	}
	else return false;
}

// Rename an Excel worksheet at the given index to the given Unicode name.
// Index starts from 0.
// Returns true if successful, false if otherwise.
bool BasicExcel::RenameWorksheet(int sheetIndex, const wchar_t* to)
{
	int maxWorksheets = (int)yesheets_.size();
	if (sheetIndex < maxWorksheets) {
		for(int i=0; i<maxWorksheets; ++i) {
			if (!(workbook_.boundSheets_[i].name_.unicode_ & 1)) continue;
			if (wcscmp(to, workbook_.boundSheets_[i].name_.wname_) == 0) return false;
		}

		workbook_.boundSheets_[sheetIndex].name_ = to;
		return true;
	}
	else return false;
}

// Rename an Excel worksheet that has given ANSI name to another ANSI name.
// Returns true if successful, false if otherwise.
bool BasicExcel::RenameWorksheet(const char* from, const char* to)
{
	int maxWorksheets = (int)yesheets_.size();
	for(int i=0; i<maxWorksheets; ++i) {
		if (workbook_.boundSheets_[i].name_.unicode_ & 1)
			continue;

		if (strcmp(from, workbook_.boundSheets_[i].name_.name_) == 0) {
			for(int j=0; j<maxWorksheets; ++j) {
				if (workbook_.boundSheets_[j].name_.unicode_ & 1)
					continue;

				if (strcmp(to, workbook_.boundSheets_[j].name_.name_) == 0)
					return false;
			}

			workbook_.boundSheets_[i].name_ = to;
			return true;
		}
	}

	return false;
}

// Rename an Excel worksheet that has given Unicode name to another Unicode name.
// Returns true if successful, false if otherwise.
bool BasicExcel::RenameWorksheet(const wchar_t* from, const wchar_t* to)
{
	int maxWorksheets = (int) worksheets_.size();

	for(int i=0; i<maxWorksheets; ++i) {
		if (!(workbook_.boundSheets_[i].name_.unicode_ & 1))
			continue;

		if (wcscmp(from, workbook_.boundSheets_[i].name_.wname_) == 0) {
			for(int j=0; j<maxWorksheets; ++j) {
				if (!(workbook_.boundSheets_[j].name_.unicode_ & 1))
					continue;

				if (wcscmp(to, workbook_.boundSheets_[j].name_.wname_) == 0)
					return false;
			}

			workbook_.boundSheets_[i].name_ = to;
			return true;
		}
	}

	return false;
}

size_t BasicExcel::Read(const char* data, size_t dataSize)
{
	size_t bytesRead = 0;

	short code;
	LittleEndian::Read(data, code, 0, 2);

	BOF bof;
	Record rec;
	while(code == CODE::BOF)
	{
		bof.Read(data+bytesRead);
		switch(bof.type_)
		{
			case WORKBOOK_GLOBALS:
				bytesRead += workbook_.Read(data+bytesRead);
				break;

			case VISUAL_BASIC_MODULE:
				bytesRead += rec.Read(data+bytesRead);
				break;

			case WORKSHEET:
				worksheets_.push_back(Worksheet());
				bytesRead += worksheets_.back().Read(data+bytesRead);
				break;

			case CHART:
				bytesRead += rec.Read(data+bytesRead);
				break;

			default:
				bytesRead += rec.Read(data+bytesRead);
				break;
		}

		if (bytesRead < dataSize)
			LittleEndian::Read(data, code, bytesRead, 2);
		else
			break;
	}
	return bytesRead;
}

size_t BasicExcel::Write(char* data)
{
	size_t bytesWritten = 0;
	bytesWritten += workbook_.Write(data+bytesWritten);

	size_t maxWorkSheets = worksheets_.size();

	for(size_t i=0; i<maxWorkSheets; ++i)
		bytesWritten += worksheets_[i].Write(data+bytesWritten);

	return bytesWritten;
}

void BasicExcel::AdjustStreamPositions()
{
//	AdjustExtSSTPositions();
	AdjustBoundSheetBOFPositions();
	AdjustDBCellPositions();
}

void BasicExcel::AdjustBoundSheetBOFPositions()
{
	ULONG offset = workbook_.RecordSize();
	size_t maxBoundSheets = workbook_.boundSheets_.size();

	for(size_t i=0; i<maxBoundSheets; ++i) {
		workbook_.boundSheets_[i].BOFpos_ = offset;
		offset += worksheets_[i].RecordSize();
	}
}

void BasicExcel::AdjustDBCellPositions()
{
	int offset = workbook_.RecordSize();
	int maxSheets = (int) worksheets_.size();

	for(int i=0; i<maxSheets; ++i) {
		offset += worksheets_[i].bof_.RecordSize();
		offset += worksheets_[i].index_.RecordSize();
		offset += worksheets_[i].colinfos_.RecordSize();
		offset += worksheets_[i].dimensions_.RecordSize();

		size_t maxRowBlocks_ = worksheets_[i].cellTable_.rowBlocks_.size();
		for(size_t j=0; j<maxRowBlocks_; ++j) {
			ULONG firstRowOffset = 0;

			size_t maxRows = worksheets_[i].cellTable_.rowBlocks_[j].rows_.size();
			{for(size_t k=0; k<maxRows; ++k)
			{
				offset += worksheets_[i].cellTable_.rowBlocks_[j].rows_[k].RecordSize();
				firstRowOffset += worksheets_[i].cellTable_.rowBlocks_[j].rows_[k].RecordSize();
			}}
			USHORT cellOffset = (USHORT)firstRowOffset - 20; // a ROW record is 20 bytes long

			size_t maxCellBlocks = worksheets_[i].cellTable_.rowBlocks_[j].cellBlocks_.size();
			{for(size_t k=0; k<maxCellBlocks; ++k)
			{
				offset += worksheets_[i].cellTable_.rowBlocks_[j].cellBlocks_[k]->RecordSize();
				firstRowOffset += worksheets_[i].cellTable_.rowBlocks_[j].cellBlocks_[k]->RecordSize();
			}}

			// Adjust Index DBCellPos_ absolute offset
			worksheets_[i].index_.DBCellPos_[j] = offset;

			offset += worksheets_[i].cellTable_.rowBlocks_[j].dbcell_.RecordSize();

			// Adjust DBCell first row offsets
			worksheets_[i].cellTable_.rowBlocks_[j].dbcell_.firstRowOffset_ = firstRowOffset;

			// Adjust DBCell offsets
			size_t l=0;
			{for(size_t k=0; k<maxRows; ++k) {
				for(; l<maxCellBlocks; ++l) {
					if (worksheets_[i].cellTable_.rowBlocks_[j].rows_[k].rowIndex_ <=
						worksheets_[i].cellTable_.rowBlocks_[j].cellBlocks_[l]->RowIndex()) {
						worksheets_[i].cellTable_.rowBlocks_[j].dbcell_.offsets_[k] = cellOffset;
						break;
					}

					cellOffset += (USHORT)worksheets_[i].cellTable_.rowBlocks_[j].cellBlocks_[l]->RecordSize();
				}
				cellOffset = 0;
			}}
		}

		offset += worksheets_[i].cellTable_.RecordSize();
		offset += worksheets_[i].window2_.RecordSize();
		offset += worksheets_[i].eof_.RecordSize();
	}
}

void BasicExcel::AdjustExtSSTPositions()
{
	ULONG offset = workbook_.bof_.RecordSize();
	offset += workbook_.bof_.RecordSize();
	offset += workbook_.window1_.RecordSize();

	size_t maxFonts = workbook_.fonts_.size();
	{for(size_t i=0; i<maxFonts; ++i) {offset += workbook_.fonts_[i].RecordSize();}}

	//MF
	size_t maxFormats = workbook_.formats_.size();
	{for(size_t i=0; i<maxFormats; ++i) {
		if (workbook_.formats_[i].index_ >= FIRST_USER_FORMAT_IDX)	// only write user defined formats
			offset += workbook_.formats_[i].RecordSize();
	}}

	size_t maxXFs = workbook_.XFs_.size();
	{for(size_t i=0; i<maxXFs; ++i) {offset += workbook_.XFs_[i].RecordSize();}}

	size_t maxStyles = workbook_.styles_.size();
	{for(size_t i=0; i<maxStyles; ++i) {offset += workbook_.styles_[i].RecordSize();}}

	size_t maxBoundSheets = workbook_.boundSheets_.size();
	{for(size_t i=0; i<maxBoundSheets; ++i) {offset += workbook_.boundSheets_[i].RecordSize();}}

	workbook_.extSST_.stringsTotal_ = 10;
	ULONG maxPortions = workbook_.sst_.uniqueStringsTotal_ / workbook_.extSST_.stringsTotal_ +
						(workbook_.sst_.uniqueStringsTotal_%workbook_.extSST_.stringsTotal_ ? 1 : 0);
	workbook_.extSST_.streamPos_.resize(maxPortions);
	workbook_.extSST_.firstStringPos_.resize(maxPortions);
	workbook_.extSST_.unused_.resize(maxPortions);

	ULONG relativeOffset = 8;
	for(size_t i=0; i<maxPortions; ++i) {
		workbook_.extSST_.streamPos_[i] = offset + 4 + relativeOffset;
		workbook_.extSST_.firstStringPos_[i] = 4 + (USHORT)relativeOffset;
		workbook_.extSST_.unused_[i] = 0;

		for(size_t j=0; (int)j<workbook_.extSST_.stringsTotal_; ++j) {
			if (i*workbook_.extSST_.stringsTotal_+j >= workbook_.sst_.strings_.size())
				break;

			ULONG stringSize = workbook_.sst_.strings_[i*workbook_.extSST_.stringsTotal_+j].StringSize();

			if (relativeOffset+stringSize+3 < 8224)
				relativeOffset += stringSize + 3;
			else {
				// If have >= 12 bytes (2 for size, 1 for unicode and >=9 for data, can split string
				// otherwise, end record and start continue record.
				if (8224 - relativeOffset >= 12) {
					stringSize -= (8224 - relativeOffset - 3);
					offset += 12 + relativeOffset;
					relativeOffset = 0;

					size_t additionalContinueRecords = stringSize / 8223; // 8223 because the first byte is for unicode
					for(size_t k=0; k<additionalContinueRecords; ++k)
						stringSize -= 8223;

					relativeOffset += stringSize + 1;
				} else {
					if (relativeOffset+stringSize+3 < 8224)
						relativeOffset += stringSize + 3;
					else {
						// If have >= 12 bytes (2 for size, 1 for unicode and >=9 for data, can split string
						// otherwise, end record and start continue record.
						if (8224 - relativeOffset >= 12) {
							stringSize -= (8224 - relativeOffset - 3);
							offset += 12 + relativeOffset;
							relativeOffset = 0;

							size_t additionalContinueRecords = stringSize / 8223; // 8223 because the first byte is for unicode
							for(size_t k=0; k<additionalContinueRecords; ++k)
								stringSize -= 8223;

							relativeOffset += stringSize + 1;
						}
					}
				}
			}
		}
	}
}

// Update yesheets_ using information from worksheets_.
void BasicExcel::UpdateYExcelWorksheet()
{
	int maxWorksheets = (int) worksheets_.size();
	yesheets_.clear();
	yesheets_.reserve(maxWorksheets);

	for(int i=0; i<maxWorksheets; ++i) {
		yesheets_.push_back(new BasicExcelWorksheet(this, i));

		for(size_t j=0; j<worksheets_[i].colinfos_.colinfo_.size(); ++j)
			yesheets_[i]->colInfos_.colinfo_.push_back(worksheets_[i].colinfos_.colinfo_[j]);
	}
}

// Update worksheets_ using information from yesheets_.
void BasicExcel::UpdateWorksheets()
{
	// Constants.
	const int maxWorksheets = (int) yesheets_.size();
	Worksheet::CellTable::RowBlock rowBlock;
	Worksheet::CellTable::RowBlock::Row row;
	Worksheet::CellTable::RowBlock::CellBlock::MulRK::XFRK xfrk;
	LargeString largeString;
	Worksheet::ColInfo oneCol;

	map<vector<char>, size_t> stringMap;
	map<vector<char>, size_t>::iterator stringMapIt;
	map<vector<wchar_t>, size_t> wstringMap;
	map<vector<wchar_t>, size_t>::iterator wstringMapIt;

	// Reset worksheets and string table.
	worksheets_.clear();
	worksheets_.resize(maxWorksheets);

	workbook_.sst_.stringsTotal_ = 0;
	workbook_.sst_.uniqueStringsTotal_ = 0;
	workbook_.sst_.strings_.clear();

	for(int s=0; s<maxWorksheets; ++s) {
		const BasicExcelWorksheet& sheet = *yesheets_[s];
		Worksheet& rawSheet = worksheets_[s];

		int maxRows = sheet.GetTotalRows();
		int maxCols = sheet.GetTotalCols();

		// Modify Index
		rawSheet.index_.firstUsedRowIndex_ = 100000;	// Use 100000 to indicate that firstUsedRowIndex is not set yet since maximum allowed rows in Excel is 65535.
		rawSheet.index_.firstUnusedRowIndex_ = maxRows;

		//Modify ColInfo
		rawSheet.colinfos_ = sheet.colInfos_;

		// Modify Dimensions
		rawSheet.dimensions_.firstUsedRowIndex_ = 100000; // Use 100000 to indicate that firstUsedRowIndex is not set yet since maximum allowed rows in Excel is 65535.
		rawSheet.dimensions_.firstUsedColIndex_ = 1000;	// Use 1000 to indicate that firstUsedColIndex is not set yet since maximum allowed columns in Excel is 255.
		rawSheet.dimensions_.lastUsedRowIndexPlusOne_ = maxRows;
		rawSheet.dimensions_.lastUsedColIndexPlusOne_ = maxCols;

		// Make first sheet selected and other sheets unselected
		if (s > 0) rawSheet.window2_.options_ &= ~0x200;

		// References and pointers to shorten code
		vector<Worksheet::CellTable::RowBlock>& rRowBlocks = rawSheet.cellTable_.rowBlocks_;
		vector<SmartPtr<Worksheet::CellTable::RowBlock::CellBlock> >* pCellBlocks;
		Worksheet::CellTable::RowBlock::CellBlock* pCell;
		rRowBlocks.resize(maxRows/32 + (maxRows%32 ? 1 : 0));
		for(int r=0, curRowBlock=0; r<maxRows; ++r) {
			if (r % 32 == 0) {
				// New row block for every 32 rows.
				pCellBlocks = &(rRowBlocks[curRowBlock++].cellBlocks_);
			}
			bool newRow = true; // Keep track whether current row contains data.
			pCellBlocks->reserve(1000);
			for(int c=0; c<maxCols; ++c) {
				const BasicExcelCell* cell = sheet.Cell(r, c);

				int cellType = cell->Type();
//				if (cellType != BasicExcelCell::UNDEFINED)	// Current cell contains some data
				// Write cell content, even if blank in order to keep format
				{
					if (rawSheet.index_.firstUsedRowIndex_ == 100000) {
						// Set firstUsedRowIndex.
						rawSheet.index_.firstUsedRowIndex_ = r;
						rawSheet.dimensions_.firstUsedRowIndex_ = r;

						// Resize DBCellPos.
						size_t nm = int(rawSheet.index_.firstUnusedRowIndex_ - rawSheet.index_.firstUsedRowIndex_ - 1) / 32 + 1;
						rawSheet.index_.DBCellPos_.resize(nm);
					}

					if (rawSheet.dimensions_.firstUsedColIndex_ == 1000) {
						// Set firstUsedColIndex.
						rawSheet.dimensions_.firstUsedColIndex_ = c;
					}

					if (newRow) {
						// Prepare Row and DBCell for new row with data.
						Worksheet::CellTable::RowBlock& rRowBlock = rRowBlocks[curRowBlock-1];
						rRowBlock.rows_.push_back(row);
						rRowBlock.rows_.back().rowIndex_ = r;
						rRowBlock.rows_.back().lastCellColIndexPlusOne_ = maxCols;
						rRowBlock.dbcell_.offsets_.push_back(0);
						newRow = false;
					}

					// Create new cellblock to store cell.
					pCellBlocks->push_back(new Worksheet::CellTable::RowBlock::CellBlock);
					if (pCellBlocks->size()%1000==0) pCellBlocks->reserve(pCellBlocks->size()+1000);
					pCell = &*(pCellBlocks->back());

					// Store cell.
					switch(cellType) {
						case BasicExcelCell::INT:
						{
							// Check whether it is a single cell or range of cells.
							int cl = c + 1;
							for(; cl<maxCols; ++cl) {
								const BasicExcelCell* cellNext = sheet.Cell(r, cl);
								if (cellNext->Type()==BasicExcelCell::UNDEFINED ||
									cellNext->Type()!=cell->Type()) break;
							}

							if (cl > c+1) {
								// MULRK cells
								pCell->SetType(CODE::MULRK);
								pCell->_union.mulrk_->rowIndex_ = r;
								pCell->_union.mulrk_->firstColIndex_ = c;
								pCell->_union.mulrk_->lastColIndex_ = cl - 1;
								pCell->_union.mulrk_->XFRK_.resize(cl-c);

								for(size_t i=0; c<cl; ++c, ++i) {
									cell = sheet.Cell(r, c);
									pCell->_union.mulrk_->XFRK_[i].RKValue_ = GetRKValueFromInteger(cell->GetInteger());
									pCell->_union.mulrk_->XFRK_[i].XFRecordIndex_ = cell->GetXFormatIdx();	//MF set format index
								}

								--c;
							} else {
								// Single cell
								pCell->SetType(CODE::RK);
								pCell->_union.rk_->rowIndex_ = r;
								pCell->_union.rk_->colIndex_ = c;
								pCell->_union.rk_->value_ = GetRKValueFromInteger(cell->GetInteger());
								pCell->_union.rk_->XFRecordIndex_ = cell->GetXFormatIdx();	//MF set format index
							}
							break;
						}

						case BasicExcelCell::DOUBLE:
						{
							// Check whether it is a single cell or range of cells.
							// Double values which cannot be stored as RK values will be stored as single cells.
							bool canStoreAsRKValue = CanStoreAsRKValue(cell->GetDouble());
							int cl = c + 1;
							for(; cl<maxCols; ++cl) {
								const BasicExcelCell* cellNext = sheet.Cell(r, cl);

								if (cellNext->Type()==BasicExcelCell::UNDEFINED ||
									cellNext->Type()!=cell->Type() ||
									canStoreAsRKValue!=CanStoreAsRKValue(cellNext->GetDouble())) break;
							}

							if (cl > c+1 && canStoreAsRKValue) {
								// MULRK cells
								pCell->SetType(CODE::MULRK);
								pCell->_union.mulrk_->rowIndex_ = r;
								pCell->_union.mulrk_->firstColIndex_ = c;
								pCell->_union.mulrk_->lastColIndex_ = cl - 1;
								pCell->_union.mulrk_->XFRK_.resize(cl-c);

								for(size_t i=0; c<cl; ++c, ++i) {
									cell = sheet.Cell(r, c);
									pCell->_union.mulrk_->XFRK_[i].RKValue_ = GetRKValueFromDouble(cell->GetDouble());
									pCell->_union.mulrk_->XFRK_[i].XFRecordIndex_ = cell->GetXFormatIdx();	//MF set format index
								}
								--c;
							} else {
								// Single cell
								if (canStoreAsRKValue) {
									pCell->SetType(CODE::RK);
									pCell->_union.rk_->rowIndex_ = r;
									pCell->_union.rk_->colIndex_ = c;
									pCell->_union.rk_->value_ = GetRKValueFromDouble(cell->GetDouble());
									pCell->_union.rk_->XFRecordIndex_ = cell->GetXFormatIdx();	//MF set format index
								} else {
									pCell->SetType(CODE::NUMBER);
									pCell->_union.number_->rowIndex_ = r;
									pCell->_union.number_->colIndex_ = c;
									pCell->_union.number_->value_ = cell->GetDouble();
									pCell->_union.number_->XFRecordIndex_ = cell->GetXFormatIdx();	//MF set format index
								}
							}
							break;
						}

						case BasicExcelCell::STRING:
						{
							// Fill cell information
							pCell->SetType(CODE::LABELSST);
							pCell->_union.labelsst_->rowIndex_ = r;
							pCell->_union.labelsst_->colIndex_ = c;

							// Get cell string
							vector<char> str(cell->GetStringLength()+1);
							cell->Get(&*(str.begin()));
							str.pop_back(); // Remove null character because LargeString does not store null character.

							// Check if string is present in Shared string table.
							++workbook_.sst_.stringsTotal_;
							ULONG maxUniqueStrings = workbook_.sst_.uniqueStringsTotal_;
							size_t strIndex = 0;
							stringMapIt = stringMap.find(str);
							if (stringMapIt != stringMap.end()) strIndex = stringMapIt->second;
							else strIndex = maxUniqueStrings;

							if (strIndex < maxUniqueStrings) {
								// String is present in Shared string table.
								pCell->_union.labelsst_->SSTRecordIndex_ = strIndex;
							} else {
								// New unique string.
								stringMap[str] = maxUniqueStrings;
								workbook_.sst_.strings_.push_back(largeString);
								workbook_.sst_.strings_[maxUniqueStrings].name_ = str;
								workbook_.sst_.strings_[maxUniqueStrings].unicode_ = 0;
								pCell->_union.labelsst_->SSTRecordIndex_ = maxUniqueStrings;
								++workbook_.sst_.uniqueStringsTotal_;
							}

							pCell->_union.labelsst_->XFRecordIndex_ = cell->GetXFormatIdx();	//MF set format index
							break;
						}

						case BasicExcelCell::WSTRING:
						{
							// Fill cell information
							pCell->SetType(CODE::LABELSST);
							pCell->_union.labelsst_->rowIndex_ = r;
							pCell->_union.labelsst_->colIndex_ = c;

							// Get cell string
							vector<wchar_t> str(cell->GetStringLength()+1);
							cell->Get(&*(str.begin()));
							str.pop_back(); // Remove null character because LargeString does not store null character.

							// Check if string is present in Shared string table.
							++workbook_.sst_.stringsTotal_;
							size_t maxUniqueStrings = workbook_.sst_.strings_.size();
							size_t strIndex = 0;
							wstringMapIt = wstringMap.find(str);
							if (wstringMapIt != wstringMap.end()) strIndex = wstringMapIt->second;
							else strIndex = maxUniqueStrings;

							if (strIndex < maxUniqueStrings) {
								// String is present in Shared string table.
								pCell->_union.labelsst_->SSTRecordIndex_ = strIndex;
							} else {
								// New unique string
								wstringMap[str] = maxUniqueStrings;
								workbook_.sst_.strings_.push_back(largeString);
								workbook_.sst_.strings_[maxUniqueStrings].wname_ = str;
								workbook_.sst_.strings_[maxUniqueStrings].unicode_ = 1;
								pCell->_union.labelsst_->SSTRecordIndex_ = maxUniqueStrings;
								++workbook_.sst_.uniqueStringsTotal_;
							}

							pCell->_union.labelsst_->XFRecordIndex_ = cell->GetXFormatIdx();	//MF set format index
							break;
						}

						//MF: handle formulas
						case BasicExcelCell::FORMULA:
						{
							// Fill cell information
							pCell->SetType(CODE::FORMULA);

							pCell->_union.formula_->rowIndex_ = r;
							pCell->_union.formula_->colIndex_ = c;

							pCell->_union.formula_->XFRecordIndex_ = cell->GetXFormatIdx();

							cell->get_formula(pCell);
							break;
						}

						// handle blank case to keep formatting
						case BasicExcelCell::UNDEFINED:
						{
							// Fill cell information
							pCell->SetType(CODE::BLANK);

							pCell->_union.blank_->colIndex_ = c;
							pCell->_union.blank_->rowIndex_ = r;

							pCell->_union.blank_->XFRecordIndex_ = cell->GetXFormatIdx();
							break;
						}
					}
				}
			}
		}

		// assemble the MERGECELL records
		for(int mr=0; mr<maxRows; ++mr) {
			for(int c=0; c<maxCols; ++c) {
				const BasicExcelCell* cell = sheet.Cell(mr,c);

				// Merged cells
				if (cell->GetMergedRows() > 1 || cell->GetMergedColumns() > 1) {
					YExcel::Worksheet::MergedCells::MergedCell mergedCell;

					mergedCell.firstRow_ = mr;
					mergedCell.firstColumn_ = c;
					mergedCell.lastRow_ = mr + cell->GetMergedRows() - 1;
					mergedCell.lastColumn_ = c + cell->GetMergedColumns() - 1;

					rawSheet.mergedCells_.mergedCellsVector_.push_back(mergedCell);
				}
			}
		}

		// If worksheet has no data
		if (rawSheet.index_.firstUsedRowIndex_ == 100000) {
			// Set firstUsedRowIndex.
			rawSheet.index_.firstUsedRowIndex_ = 0;
			rawSheet.dimensions_.firstUsedRowIndex_ = 0;

			// Resize DBCellPos.
			size_t nm = int(rawSheet.index_.firstUnusedRowIndex_ - rawSheet.index_.firstUsedRowIndex_ - 1) / 32 + 1;
			rawSheet.index_.DBCellPos_.resize(nm);
		}

		if (rawSheet.dimensions_.firstUsedColIndex_ == 1000) {
			// Set firstUsedColIndex.
			rawSheet.dimensions_.firstUsedColIndex_ = 0;
		}
	}
}
/************************************************************************************************************/

/************************************************************************************************************/
BasicExcelWorksheet::BasicExcelWorksheet(BasicExcel* excel, int sheetIndex) :
	excel_(excel), sheetIndex_(sheetIndex)
{
	UpdateCells();
}

// Get the current worksheet name.
// Returns 0 if name is in Unicode format.
char* BasicExcelWorksheet::GetAnsiSheetName()
{
	if (!(excel_->workbook_.boundSheets_[sheetIndex_].name_.unicode_ & 1))
		return excel_->workbook_.boundSheets_[sheetIndex_].name_.name_;
	else
		return NULL;
}

// Get the current worksheet name.
// Returns 0 if name is in Ansi format.
wchar_t* BasicExcelWorksheet::GetUnicodeSheetName()
{
	if (excel_->workbook_.boundSheets_[sheetIndex_].name_.unicode_ & 1)
		return excel_->workbook_.boundSheets_[sheetIndex_].name_.wname_;
	else
		return NULL;
}

// Get the current worksheet name.
// Returns false if name is in Unicode format.
bool BasicExcelWorksheet::GetSheetName(char* name)
{
	if (!(excel_->workbook_.boundSheets_[sheetIndex_].name_.unicode_ & 1))
	{
		strcpy(name, excel_->workbook_.boundSheets_[sheetIndex_].name_.name_);
		return true;
	}
	else return false;
}

// Get the current worksheet name.
// Returns false if name is in Ansi format.
bool BasicExcelWorksheet::GetSheetName(wchar_t* name)
{
	if (excel_->workbook_.boundSheets_[sheetIndex_].name_.unicode_ & 1)
	{
		wcscpy(name, excel_->workbook_.boundSheets_[sheetIndex_].name_.wname_);
		return true;
	}
	else return false;
}

// Rename current Excel worksheet to another ANSI name.
// Returns true if successful, false if otherwise.
bool BasicExcelWorksheet::Rename(const char* to)
{
	int maxWorksheets = (int) excel_->workbook_.boundSheets_.size();
	for(int i=0; i<maxWorksheets; ++i) {
		if (excel_->workbook_.boundSheets_[i].name_.unicode_ & 1) continue;
		if (strcmp(to, excel_->workbook_.boundSheets_[i].name_.name_) == 0) return false;
	}

	excel_->workbook_.boundSheets_[sheetIndex_].name_ = to;
	return true;
}

// Rename current Excel worksheet to another Unicode name.
// Returns true if successful, false if otherwise.
bool BasicExcelWorksheet::Rename(const wchar_t* to)
{
	int maxWorksheets = (int) excel_->workbook_.boundSheets_.size();
	for(int i=0; i<maxWorksheets; ++i) {
		if (!(excel_->workbook_.boundSheets_[i].name_.unicode_ & 1)) continue;
		if (wcscmp(to, excel_->workbook_.boundSheets_[i].name_.wname_) == 0) return false;
	}

	excel_->workbook_.boundSheets_[sheetIndex_].name_ = to;
	return true;
}

///< Print entire worksheet to an output stream, separating each column with the defined delimiter and enclosing text using the defined textQualifier.
///< Leave out the textQualifier argument if do not wish to have any text qualifiers.
void BasicExcelWorksheet::Print(ostream& os, char delimiter, char textQualifier) const
{
	for(int r=0; r<maxRows_; ++r)
	{
		for(int c=0; c<maxCols_; ++c)
		{
			const BasicExcelCell* cell = Cell(r, c);
			switch(cell->Type())
			{
				case BasicExcelCell::UNDEFINED:
					break;

				case BasicExcelCell::INT:
					os << cell->GetInteger();
					break;

				case BasicExcelCell::DOUBLE:
					os << setprecision(15) << cell->GetDouble();
					break;

				case BasicExcelCell::STRING:
				{
					if (textQualifier != '\0')
						{
						// Get string.
						size_t maxLength = cell->GetStringLength();
						vector<char> cellString(maxLength+1);
						cell->Get(&*(cellString.begin()));

						// Duplicate textQualifier if found in string.
						ULONG npos = 0;
						vector<char>::iterator it;
						while((it=find(cellString.begin()+npos, cellString.end(), textQualifier)) != cellString.end())
							npos = (ULONG) distance(cellString.begin(), cellString.insert(it, textQualifier)) + 2;

						// Print out string enclosed with textQualifier.
						os << textQualifier << &*(cellString.begin()) << textQualifier;
					} else
						os << cell->GetString();
					break;
				}

				case BasicExcelCell::WSTRING:
				{
					// Print out string enclosed with textQualifier (does not work).
					//os << textQualifier << cell->GetWString() << textQualifier;
					break;
				}
			}
			if (c < maxCols_-1)
				os << delimiter;
		}
		os << endl;
	}
}

// Total number of rows in current Excel worksheet.
int BasicExcelWorksheet::GetTotalRows() const
{
	return maxRows_;
}

// Total number of columns in current Excel worksheet.
int BasicExcelWorksheet::GetTotalCols() const
{
	return maxCols_;
}

// Return a pointer to an Excel cell.
// row and col starts from 0.
// Returns NULL if row exceeds 65535 or col exceeds 255.
BasicExcelCell* BasicExcelWorksheet::Cell(int row, int col)
{
	// Check to ensure row and col do not exceed the maximum allowable range for an Excel worksheet.
	if (row>65535 || col>255) return NULL;

	// Increase size of the cell matrix if necessary
	if (col >= maxCols_) {
		// Increase the number of columns.
		maxCols_ = col + 1;
		for(int i=0; i<maxRows_; ++i)
			cells_[i].resize(maxCols_);
	}

	if (row >= maxRows_) {
		// Increase the number of rows.
		maxRows_ = row + 1;
		cells_.resize(maxRows_, vector<BasicExcelCell>(maxCols_));
	}

	return &(cells_[row][col]);
}

// Return a pointer to an Excel cell.
// row and col starts from 0.
// Returns NULL if row exceeds 65535 or col exceeds 255.
const BasicExcelCell* BasicExcelWorksheet::Cell(int row, int col) const
{
	// row and col do not exceed the current worksheet size

	if (row >= maxRows_)
		return NULL;

	if (col >= maxCols_)
		return NULL;

	return &(cells_[row][col]);
}

// Erase content of a cell. row and col starts from 0.
// Returns true if successful, false if row or col exceeds range.
bool BasicExcelWorksheet::EraseCell(int row, int col)
{
	if (row<maxRows_ && col<maxCols_)
	{
		cells_[row][col].EraseContents();
		return true;
	}
	else return false;
}

//MF: calculate sheet dimension from row blocks, only looking at non-empty cells
static void calculate_dimension(vector<Worksheet::CellTable::RowBlock>& rRowBlocks, int& maxRows_, int& maxCols_)
{
	int maxRow = 0;
	int maxCol = 0;

	for(size_t i=0; i<rRowBlocks.size(); ++i) {
		vector<SmartPtr<Worksheet::CellTable::RowBlock::CellBlock> >& rCellBlocks = rRowBlocks[i].cellBlocks_;

		for(size_t j=0; j<rCellBlocks.size(); ++j) {
			int row = rCellBlocks[j]->RowIndex();
			int col = rCellBlocks[j]->ColIndex();

			switch(rCellBlocks[j]->type_) {
			  case CODE::BLANK:
			  case CODE::MULBLANK:
				break;

			  case CODE::MULRK: {
				int maxCols = rCellBlocks[j]->_union.mulrk_->lastColIndex_ - rCellBlocks[j]->_union.mulrk_->firstColIndex_ + 1;
				col += maxCols;
				// fall through
				}

			  default:
				if (row > maxRow)
					maxRow = row;

				if (col > maxCol)
					maxCol = col;
			}
		}
	}

	maxRows_ = maxRow + 1;
	maxCols_ = maxCol + 1;
}

void BasicExcelWorksheet::SetColWidth(const int colindex, const USHORT colwidth)
{
	Worksheet::ColInfo tmpColInfo;
	tmpColInfo.firstColumnIndex_ = tmpColInfo.lastColumnIndex_ = colindex;
	tmpColInfo.columnWidth_ = colwidth;
	colInfos_.colinfo_.push_back(tmpColInfo);
}

// Get the colwidth for the given col
USHORT BasicExcelWorksheet::GetColWidth(const int colindex)
{
	for(size_t i=0; i<colInfos_.colinfo_.size(); ++i) {
		if (colindex == colInfos_.colinfo_[i].firstColumnIndex_)
			return colInfos_.colinfo_[i].columnWidth_;
	}

	return 0;
}

// Update cells using information from BasicExcel.worksheets_
void BasicExcelWorksheet::UpdateCells()
{
	// Define some references
	vector<Worksheet::CellTable::RowBlock>& rRowBlocks = excel_->worksheets_[sheetIndex_].cellTable_.rowBlocks_;
	const vector<YExcel::Worksheet::MergedCells::MergedCell>& mergedCells = excel_->worksheets_[sheetIndex_].mergedCells_.mergedCellsVector_;

	vector<wchar_t> wstr;
	vector<char> str;

	//MF calculate sheet dimension independent from the DIMENSIONS record
	calculate_dimension(rRowBlocks, maxRows_, maxCols_);

//	const Worksheet::Dimensions& dimension = excel_->worksheets_[sheetIndex_].dimensions_;
//	maxRows_ = dimension.lastUsedRowIndexPlusOne_;
//	maxCols_ = dimension.lastUsedColIndexPlusOne_;

	// Resize the cells to the size of the worksheet
	vector<BasicExcelCell> cellCol(maxCols_);
	cells_.resize(maxRows_, cellCol);

	size_t maxRowBlocks = rRowBlocks.size();
	for(size_t i=0; i<maxRowBlocks; ++i) {
		vector<SmartPtr<Worksheet::CellTable::RowBlock::CellBlock> >& rCellBlocks = rRowBlocks[i].cellBlocks_;
		size_t maxCells = rCellBlocks.size();

		for(size_t j=0; j<maxCells; ++j) {
			int row = rCellBlocks[j]->RowIndex();
			int col = rCellBlocks[j]->ColIndex();

			if (row >= maxRows_) {
				// skip empty rows a the bottom
				continue;
//				// resize on unexpected row values
//				maxRows_ = row + 1;
//				cells_.resize(maxRows_, cellCol);
			}

			if (col >= maxCols_) {
				// skip empty columns a the right sheet border
				continue;
//				// resize on unexpected column values
//				if (col >= (int)cells_[row].size())
//					cells_[row].resize(col+1);
			}

			switch(rCellBlocks[j]->type_) {
				case CODE::BLANK:
					cells_[row][col].SetXFormatIdx(rCellBlocks[j]->_union.blank_->XFRecordIndex_);	//MF read format index
					break;

				case CODE::MULBLANK: {
					size_t maxCols = rCellBlocks[j]->_union.mulblank_->lastColIndex_ - rCellBlocks[j]->_union.mulblank_->firstColIndex_ + 1;

					for(size_t k=0; k<maxCols; ++k,++col) {
						//MF resize on unexpected column values
						if (col >= maxCols_) {
							if (col >= (int)cells_[row].size())
								cells_[row].resize(col+1);
						}

						cells_[row][col].SetXFormatIdx(rCellBlocks[j]->_union.mulblank_->XFRecordIndices_[k]);	//MF read format index
					}
					break;}

				case CODE::BOOLERR:
					if (rCellBlocks[j]->_union.boolerr_->error_ == 0)
						cells_[row][col].Set(rCellBlocks[j]->_union.boolerr_->value_);	//MF was "boolerr_.code_" in VC6 version

					cells_[row][col].SetXFormatIdx(rCellBlocks[j]->_union.boolerr_->XFRecordIndex_); //MF read format index
					break;

				case CODE::LABELSST: {
					vector<LargeString>& ss = excel_->workbook_.sst_.strings_;
					if (ss[rCellBlocks[j]->_union.labelsst_->SSTRecordIndex_].unicode_ & 1) {
						wstr = ss[rCellBlocks[j]->_union.labelsst_->SSTRecordIndex_].wname_;
						wstr.resize(wstr.size()+1);
						wstr.back() = L'\0';
						cells_[row][col].Set(&*(wstr.begin()));
					} else {
						str = ss[rCellBlocks[j]->_union.labelsst_->SSTRecordIndex_].name_;
						str.resize(str.size()+1);
						str.back() = '\0';
						cells_[row][col].Set(&*(str.begin()));
					}

					cells_[row][col].SetXFormatIdx(rCellBlocks[j]->_union.labelsst_->XFRecordIndex_);	//MF read format index
					break;}

				case CODE::MULRK: {
					size_t maxCols = rCellBlocks[j]->_union.mulrk_->lastColIndex_ - rCellBlocks[j]->_union.mulrk_->firstColIndex_ + 1;
					for(size_t k=0; k<maxCols; ++k,++col) {
						//MF resize on unexpected column values
						if (col >= maxCols_) {
							if (col >= (int)cells_[row].size())
								cells_[row].resize(col+1);
							break; // skip invalid column values
						}

						// Get values of the whole range
						const Worksheet::CellTable::RowBlock::CellBlock::MulRK::XFRK& xfrk = rCellBlocks[j]->_union.mulrk_->XFRK_[k];

						cells_[row][col].SetRKValue(xfrk.RKValue_);
						cells_[row][col].SetXFormatIdx(xfrk.XFRecordIndex_);	//MF read format index
					}
					break;}

				case CODE::NUMBER:
					cells_[row][col].Set(rCellBlocks[j]->_union.number_->value_);
					cells_[row][col].SetXFormatIdx(rCellBlocks[j]->_union.number_->XFRecordIndex_);	//MF read format index
					break;

				case CODE::RK: {
					cells_[row][col].SetRKValue(rCellBlocks[j]->_union.rk_->value_);
					cells_[row][col].SetXFormatIdx(rCellBlocks[j]->_union.rk_->XFRecordIndex_);	//MF read format index
					break;}

				//MF: handle formulas
				case CODE::FORMULA: {
					const Worksheet::CellTable::RowBlock::CellBlock::Formula& formula = *rCellBlocks[j]->_union.formula_;

					cells_[row][col].SetFormula(formula);
					cells_[row][col].SetXFormatIdx(formula.XFRecordIndex_); //MF read format index
					break;}
			}
		}
	}

	// handle merged cells information
	for(size_t k=0; k<mergedCells.size(); k++) {
		int row = mergedCells[k].firstRow_;
		int col = mergedCells[k].firstColumn_;

		if (row<(int)cells_.size() && col<(int)cells_[row].size()) {
			cells_[row][col].SetMergedRows(mergedCells[k].lastRow_ - mergedCells[k].firstRow_ + 1);
			cells_[row][col].SetMergedColumns(mergedCells[k].lastColumn_ - mergedCells[k].firstColumn_ + 1);
		}
	}
}

void BasicExcelWorksheet::MergeCells(int row, int col, USHORT rowRange, USHORT colRange)
{
	BasicExcelCell* cell = Cell(row, col);

	cell->SetMergedRows(rowRange);
	cell->SetMergedColumns(colRange);

/* Assembling MERGECELL records is accomplished in BasicExcel::UpdateWorksheets()
	Worksheet::MergedCells::MergedCell mergedCell;
	mergedCell.firstRow_ = row;
	mergedCell.lastRow_ = row + rowRange - 1;
	mergedCell.firstColumn_ = col;
	mergedCell.lastColumn_ = col + colRange - 1;

	Worksheet& worksheet = excel_->worksheets_[sheetIndex_];
	worksheet.mergedCells_.mergedCellsVector_.push_back(mergedCell);
*/
}

/************************************************************************************************************/

/************************************************************************************************************/
BasicExcelCell::BasicExcelCell()
 :	type_(UNDEFINED),
	_xf_idx(0), //MF
	mergedRows_(1),
	mergedColumns_(1)
{
}

// Get type of value stored in current Excel cell.
// Returns one of the enums.
int BasicExcelCell::Type() const {return type_;}

// Get an integer value.
// Returns false if cell does not contain an integer or a double.
bool BasicExcelCell::Get(int& val) const
{
	if (type_ == INT) {
		val = ival_;
		return true;
	} else if (type_ == DOUBLE) {
		val = (int)dval_;
		return true;
	} else if (type_ == FORMULA) {
		const unsigned char* presult = _pFormula->_result;

		//MF: If the two most significant bytes of the result field are 0xFFFF, the formula evaluates to a string, a boolean or an error value.
		if (presult[6]==0xFF && presult[7]==0xFF)
			return false;

		double dresult;
		memcpy(&dresult, presult, 8);
		val = (int)dresult;
		return true;
	} else
		return false;
}

// Get a double value.
// Returns false if cell does not contain a double or an integer.
bool BasicExcelCell::Get(double& val) const
{
	if (type_ == DOUBLE) {
		val = dval_;
		return true;
	} else if (type_ == INT) {
		val = (double)ival_;
		return true;
	} else if (type_ == FORMULA) {
		const unsigned char* presult = _pFormula->_result;

		//MF: If the two most significant bytes of the result field are 0xFFFF, the formula evaluates to a string, a boolean or an error value.
		if (presult[6]==0xFF && presult[7]==0xFF)
			return false;

		double dresult;
		memcpy(&dresult, presult, 8);
		val = dresult;

		return true;
	} else
		return false;
}

// Get an ANSI string.
// Returns false if cell does not contain an ANSI string.
bool BasicExcelCell::Get(char* str) const
{
	if (type_ == STRING) {
		if (str_.empty()) *str = '\0';
		else strcpy(str, &*(str_.begin()));

		return true;
	} else {
		assert(type_==STRING);
		return false;
	}
}

// Get an Unicode string.
// Returns false if cell does not contain an Unicode string.
bool BasicExcelCell::Get(wchar_t* str) const
{
	if (type_ == WSTRING) {
		if (wstr_.empty())
			*str = L'\0';
		else
			wcscpy(str, &*(wstr_.begin()));

		return true;
	} else {
		assert(type_==WSTRING);
		return false;
	}
}

// Return length of ANSI or Unicode string (excluding null character).
size_t BasicExcelCell::GetStringLength() const
{
	if (type_ == STRING)
		return str_.size() - 1;
	else {
		assert(type_==WSTRING);
		return wstr_.size() - 1;
	}
}

// Get an integer value.
// Returns 0 if cell does not contain an integer.
int BasicExcelCell::GetInteger() const
{
	int val;

	if (Get(val))
		return val;
	else
		return 0;
}

// Get a double value.
// Returns 0.0 if cell does not contain a double.
double BasicExcelCell::GetDouble() const
{
	double val;

	if (Get(val))
		return val;
	else
		return 0.0;
}

// Get an ANSI string.
// Returns NULL if cell does not contain an ANSI string.
const char* BasicExcelCell::GetString() const
{
	vector<char> str(str_.size());

	if (type_ == STRING) {
		if (!str.empty() && Get(&*(str.begin())))
			return &*(str_.begin());
	} else if (type_ == FORMULA) {
		return _pFormula->str_.c_str();
	}

	return NULL;
}

// Get an Unicode string.
// Returns NULL if cell does not contain an Unicode string.
const wchar_t* BasicExcelCell::GetWString() const
{
	vector<wchar_t> wstr(wstr_.size());

	if (type_ == WSTRING) {
		if (!wstr.empty() && Get(&*(wstr.begin())))
			return &*(wstr_.begin());
	} else if (type_ == FORMULA) {
		return _pFormula->wstr_.c_str();
	}

	return NULL;
}

// Set content of current Excel cell to an integer.
void BasicExcelCell::Set(int val)
{
	SetInteger(val);
}

// Set content of current Excel cell to a double.
void BasicExcelCell::Set(double val)
{
	SetDouble(val);
}

// Set content of current Excel cell to an ANSI string.
void BasicExcelCell::Set(const char* str)
{
	SetString(str);
}

// Set content of current Excel cell to an Unicode string.
void BasicExcelCell::Set(const wchar_t* str)
{
	SetWString(str);
}

// Set content of current Excel cell to an integer.
void BasicExcelCell::SetInteger(int val)
{
	type_ = INT;
	ival_ = val;
}

// Set content of current Excel cell to a double.
void BasicExcelCell::SetDouble(double val)
{
	type_ = DOUBLE;
	dval_ = val;
}

//MF: Set content of current Excel cell to a double or integer value.
void BasicExcelCell::SetRKValue(int rkValue)
{
	bool isMultiplied = rkValue & 1;
	bool rkInteger = (rkValue & 2)? true: false;

	if (rkInteger) {
		rkValue >>= 2;

		if (isMultiplied) {
			if ((rkValue % 100) == 0) {
				type_ = INT;
				ival_ = rkValue / 100;
			} else {
				type_ = DOUBLE;	// OpenOffice Calc stores double values with less than 3 decimal places as "integer" RKValues (MS Office doesn't use this case).
				dval_ = rkValue / 100.;
			}
		} else {
			type_ = INT;
			ival_ = rkValue;
		}
	} else {
		RKValueUnion intdouble;

		intdouble.intvalue_ = rkValue >> 2; // only valid if the integer flag (rkValue & 2) is not set
		intdouble.intvalue_ <<= 34;

		if (isMultiplied)
			intdouble.doublevalue_ /= 100;

		type_ = DOUBLE;
		dval_ = intdouble.doublevalue_;
	}
}

// Set content of current Excel cell to an ANSI string.
void BasicExcelCell::SetString(const char* str)
{
	size_t length = strlen(str);
	if (length > 0)	{
		type_ = STRING;
		str_ = vector<char>(length+1);
		strcpy(&*(str_.begin()), str);
		wstr_.clear();
	} else
		EraseContents();
}

// Set content of current Excel cell to an Unicode string.
void BasicExcelCell::SetWString(const wchar_t* str)
{
	size_t length = wcslen(str);
	if (length > 0)	{
		type_ = WSTRING;
		wstr_ = vector<wchar_t>(length+1);
		wcscpy(&*(wstr_.begin()), str);
		str_.clear();
	} else
		EraseContents();
}

//MF
void BasicExcelCell::SetFormula(const Worksheet::CellTable::RowBlock::CellBlock::Formula& f)
{
	type_ = BasicExcelCell::FORMULA;
	_pFormula = new Formula(f);
}

BasicExcelCell::Formula::Formula(const Worksheet::CellTable::RowBlock::CellBlock::Formula& f)
 :	  _formula_type(0)
{
	_formula_type = f.type_;

	if (_formula_type == CODE::SHRFMLA1) {
		shrformula_ 	= f.shrfmla1_.formula_;
		firstRowIndex_	= f.shrfmla1_.firstRowIndex_;
		lastRowIndex_	= f.shrfmla1_.lastRowIndex_;
		firstColIndex_	= f.shrfmla1_.firstColIndex_;
		lastColIndex_	= f.shrfmla1_.lastColIndex_;
		unused_ 		= f.shrfmla1_.unused_;
	} else {
		firstRowIndex_	= 0;
		lastRowIndex_	= 0;
		firstColIndex_	= 0;
		lastColIndex_	= 0;
		unused_ 		= 0;
	}

	_formula = f.RPNtoken_;

	// store result values
	memcpy(_result, f.result_, 8);

	if (f.string_.wstr_) {
		wstr_ = f.string_.wstr_;
		str_ = ::narrow_string(wstr_);
	}
}

bool BasicExcelCell::get_formula(Worksheet::CellTable::RowBlock::CellBlock* pCell) const
{
	if (type_==FORMULA && _pFormula) {
		pCell->_union.formula_->type_ = _pFormula->_formula_type;
		memcpy(pCell->_union.formula_->result_, _pFormula->_result, 8);

		if (pCell->_union.formula_->type_ == CODE::SHRFMLA1) {
			pCell->_union.formula_->shrfmla1_.formula_			= _pFormula->shrformula_;
			pCell->_union.formula_->shrfmla1_.firstRowIndex_	= _pFormula->firstRowIndex_;
			pCell->_union.formula_->shrfmla1_.lastRowIndex_ 	= _pFormula->lastRowIndex_;
			pCell->_union.formula_->shrfmla1_.firstColIndex_	= _pFormula->firstColIndex_;
			pCell->_union.formula_->shrfmla1_.lastColIndex_ 	= _pFormula->lastColIndex_;
			pCell->_union.formula_->shrfmla1_.unused_			= _pFormula->unused_;
		} else {
			pCell->_union.formula_->shrfmla1_.firstRowIndex_	= 0;
			pCell->_union.formula_->shrfmla1_.lastRowIndex_		= 0;
			pCell->_union.formula_->shrfmla1_.firstColIndex_	= 0;
			pCell->_union.formula_->shrfmla1_.lastColIndex_		= 0;
			pCell->_union.formula_->shrfmla1_.unused_ 			= 0;
		}

		pCell->_union.formula_->RPNtoken_ = _pFormula->_formula;

		if (!_pFormula->wstr_.empty()) {
			size_t stringSize = _pFormula->wstr_.size();
			wchar_t* wstr = new wchar_t[stringSize];
			memcpy(wstr, &*_pFormula->wstr_.begin(), stringSize);
			pCell->_union.formula_->string_.wstr_ = wstr;
		} else
			pCell->_union.formula_->string_.wstr_ = NULL;

		return true;
	}

	return false;
}

// Erase the content of current Excel cell.
// Set type to UNDEFINED.
void BasicExcelCell::EraseContents()
{
	type_ = UNDEFINED;
	str_.clear();
	wstr_.clear();
}

///< Print cell to output stream.
///< Print a null character if cell is undefined.
ostream& operator<<(ostream& os, const BasicExcelCell& cell)
{
	switch(cell.Type())
	{
		case BasicExcelCell::UNDEFINED:
			os << '\0';
			break;

		case BasicExcelCell::INT:
			os << cell.GetInteger();
			break;

		case BasicExcelCell::DOUBLE:
			os << cell.GetDouble();
			break;

		case BasicExcelCell::STRING:
			os << cell.GetString();
			break;

		case BasicExcelCell::WSTRING:
			os << cell.GetWString();
			break;
	}

	return os;
}

} // namespace YExcel
