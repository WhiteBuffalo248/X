#include "stdafx.h"
#include <mmsyscom.h>
#include "TraceFileReaderInternal.h"
#include "cJSON/cJSON.h"


#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

CTraceFileReader::CTraceFileReader(CInstructTrace* parent) : parent(parent)
{
	length = 0;
	progress = 0;
	error = true;
	parser = nullptr;
	lastAccessedPage = nullptr;
	lastAccessedIndexOffset = 0;
	hashValue = 0;
}


CTraceFileReader::~CTraceFileReader()
{
	
}

bool CTraceFileReader::Open(const CString & fileName)
{

	DWORD code;
	BOOL res = FALSE;
	if (parser)
		res = GetExitCodeThread(parser->m_hThread, &code);
	if (parser != NULL && !res && code == STILL_ACTIVE)//线程还活着
		parser->bExit = true;
	
	error = true;
	//traceFile.Rename(traceFile.GetFilePath(), fileName);
	BOOL s = traceFile.Open(fileName, CFile::shareDenyNone);
	int err = GetLastError();
	if (s)
	{
		parser = new CTraceFileParser(this);
		//connect(parser, SIGNAL(finished()), this, SLOT(parseFinishedSlot()));
		progress.store(0);
		//traceFile.moveToThread(parser);
		//parser->start();
		parser->CreateThread();
		//CWinThread* pParseThread = AfxBeginThread(RUNTIME_CLASS(CTraceFileParser));
		return true;
	}
	else
	{
		progress.store(0);
		//emit parseFinished();
		return false;
	}
}

void CTraceFileReader::Close()
{
	if (parser != NULL)
	{
		parser->bExit = true;
	}
	traceFile.Close();
	progress.store(0);
	length = 0;
	fileIndex.clear();
	hashValue = 0;
	EXEPath.ReleaseBuffer();
	error = false;
}

void CTraceFileReader::Delete()
{
	if (parser != NULL)
	{
		parser->bExit = true;
	}
	CString csfile = traceFile.GetFilePath();
	traceFile.Close();
	DeleteFile(csfile);
	progress.store(0);
	length = 0;
	fileIndex.clear();
	hashValue = 0;
	EXEPath.ReleaseBuffer();
	error = false;
}

void CTraceFileReader::parseFinished()
{
	if (!error)
		progress.store(100);
	else
		progress.store(0);
	//delete parser;
	parser = nullptr;
	SendMessage(parent->GetSafeHwnd(), WM_USER + TraceFileParser_Finish, 0, 0);

	//for(auto i : fileIndex)
	//GuiAddLogMessage(QString("%1;%2;%3\r\n").arg(i.first).arg(i.second.first).arg(i.second.second).toUtf8().constData());
}

bool CTraceFileReader::isError() const
{
	return error;
}

int CTraceFileReader::Progress() const
{
	return progress.load();
}

unsigned long long CTraceFileReader::Length() const
{
	return length;
}

duint CTraceFileReader::HashValue() const
{
	return hashValue;
}

CString CTraceFileReader::ExePath() const
{
	return EXEPath;
}

REGDUMP CTraceFileReader::Registers(unsigned long long index)
{
	unsigned long long base;
	CTraceFilePage* page = getPage(index, &base);
	if (page == nullptr)
	{
		REGDUMP registers;
		memset(&registers, 0, sizeof(registers));
		return registers;
	}
	else
		return page->Registers(index - base);
}

void CTraceFileReader::OpCode(unsigned long long index, unsigned char* buffer, int* opcodeSize)
{
	unsigned long long base;
	CTraceFilePage* page = getPage(index, &base);
	if (page == nullptr)
	{
		memset(buffer, 0, 16);
		return;
	}
	else
		page->OpCode(index - base, buffer, opcodeSize);
}

DWORD CTraceFileReader::ThreadId(unsigned long long index)
{
	unsigned long long base;
	CTraceFilePage* page = getPage(index, &base);
	if (page == nullptr)
		return 0;
	else
		return page->ThreadId(index - base);
}

void CTraceFileParser::readFileHeader(CTraceFileReader* that)
{
	
	LARGE_INTEGER header;
	if (that->traceFile.Read((char*)&header, 8) != 8)
		throw std::wstring(L"Unspecified");
	if (header.LowPart != MAKEFOURCC('T', 'R', 'A', 'C'))
		throw std::wstring(L"File type mismatch");
	if (header.HighPart > 16384)
		throw std::wstring(L"Header info is too big");
	char *data;
	data = (char*)malloc(header.HighPart + 1);
	
	UINT size = that->traceFile.Read(data, header.HighPart);
	
	if (size != header.HighPart)
		throw std::wstring(L"Unspecified");
	cJSON *jsonDoc = cJSON_Parse(data);
	if (!jsonDoc)
		throw std::wstring(L"Unspecified");
	cJSON *item;
	size = cJSON_GetArraySize(jsonDoc);
	item = cJSON_GetObjectItem(jsonDoc, "ver");
	if (!item)
		throw std::wstring(L"Unspecified");	
	if (item->valueint != 1)
		throw std::wstring(L"Version not supported");
	/*
	checkKey(jsonRoot, "arch", ArchValue("x86", "x64"));
	checkKey(jsonRoot, "compression", "");
	*/

	CString csName;
	item = cJSON_GetObjectItem(jsonDoc, "hashAlgorithm");
	if (item)
	{
		csName = item->valuestring;
		if ("murmurhash" == csName)
		{
			item = cJSON_GetObjectItem(jsonDoc, "hash");
			if (item)
			{
				CString hashVal(item->valuestring);
				if (hashVal.Find(_T("0x") == 0))
				{
					hashVal = hashVal.Mid(2);
#ifdef _WIN64
					that->hashValue = _tcstoull(hashVal, NULL, 16);
#else //x86
					that->hashValue = _tcstoul(hashVal, NULL, 16);
#endif //_WIN64
				}
			}				
		}
	}
	
	item = cJSON_GetObjectItem(jsonDoc, "path");
	if (item)
	{
		that->EXEPath = item->valuestring;
	}
		
	cJSON_Delete(jsonDoc);
	delete[] data;
}

static bool readBlock(CFile & traceFile)
{
	CFileStatus status;
	if (traceFile.GetStatus(status))
	{
		if (status.m_attribute != CFile::archive)
			throw std::wstring(L"File is not readable");
	}
	
		
	unsigned char blockType;
	unsigned char changedCountFlags[3]; //reg changed count, mem accessed count, flags
	if (traceFile.Read((char*)&blockType, 1) != 1)
		throw std::wstring(L"Read block type failed");
	if (blockType == 0)
	{

		if (traceFile.Read((char*)&changedCountFlags, 3) != 3)
			throw std::wstring(L"Read flags failed");
		//skipping: thread id, registers
		if (traceFile.Seek(traceFile.GetPosition() + ((changedCountFlags[2] & 0x80) ? 4 : 0) + (changedCountFlags[2] & 0x0F) + changedCountFlags[0] * (1 + sizeof(duint)), CFile::begin) == false)
			throw std::wstring(L"Unspecified");
		UINT size = changedCountFlags[1];
		char *memflags = new char[size];
		size = traceFile.Read(memflags, changedCountFlags[1]);
		if (size < changedCountFlags[1])
			throw std::wstring(L"Read memory flags failed");
		unsigned int skipOffset = 0;
		for (unsigned char i = 0; i < changedCountFlags[1]; i++)
			skipOffset += ((memflags[i] & 1) == 1) ? 2 : 3;
		delete[] memflags;
		if (traceFile.Seek(traceFile.GetPosition() + skipOffset * sizeof(duint), CFile::begin) == false)
			throw std::wstring(L"Unspecified");
		//Gathered information, build index
		if (changedCountFlags[0] == (FIELD_OFFSET(REGDUMP, lastError) + sizeof(DWORD)) / sizeof(duint))
			return true;
		else
			return false;
	}
	else
		throw std::wstring(L"Unsupported block type");
	return false;
}

int CTraceFileParser::mainWork()
{
	CTraceFileReader* that = parent;
	
	unsigned long long index = 0;
	unsigned long long lastIndex = 0;
	if (that == NULL)
	{
		return 0; //Error
	}
	try
	{
		if (that->traceFile.GetLength() == 0)
			throw std::wstring(L"File is empty");
		//Process file header
		readFileHeader(that);
		//Update progress
		that->progress.store(that->traceFile.GetPosition() * 100 / that->traceFile.GetLength());
		//Process file content
	
		while (that->traceFile.GetPosition() < that->traceFile.GetLength())
		{
			uint64 blockStart = that->traceFile.GetPosition();
			bool isPageBoundary = readBlock(that->traceFile);
			if (isPageBoundary)
			{
				if (lastIndex != 0)
					that->fileIndex.back().second.second = index - (lastIndex - 1);
				that->fileIndex.push_back(std::make_pair(index, CTraceFileReader::Range(blockStart, 0)));
				lastIndex = index + 1;
				//Update progress
				that->progress.store(that->traceFile.GetPosition() * 100 / that->traceFile.GetLength());
				if (this->bExit && that->traceFile.GetPosition() != that->traceFile.GetLength()) //Cancel loading
					throw std::wstring(L"Canceled");
			}
			index++;
		}
		if (index > 0)
			that->fileIndex.back().second.second = index - (lastIndex - 1);
		that->error = false;
		that->length = index;
	}
	catch (const std::wstring & errReason)
	{
		//MessageBox(0, errReason.c_str(), L"debug", MB_ICONERROR);
		that->error = true;
	}
	catch (std::bad_alloc &)
	{
		that->error = true;
	}

	//that->traceFile.moveToThread(that->thread());

	PostMessage(that->parent->GetSafeHwnd(), WM_USER + TraceFileParserThread_Exit, 0, 0);
	return 1;
}

BOOL CTraceFileParser::InitInstance()
{
	mainWork();
	return FALSE;
}

void CTraceFileReader::purgeLastPage()
{
    unsigned long long index = 0;
    unsigned long long lastIndex = 0;
    bool isBlockExist = false;
    if(length > 0)
    {
        index = fileIndex.back().first;
        const auto lastpage = pages.find(Range(index, index));
        if(lastpage != pages.cend())
        {
            //Purge last accessed page
            if(index == lastAccessedIndexOffset)
                lastAccessedPage = nullptr;
            //Remove last page from page cache
            pages.erase(lastpage);
        }
        //Seek start of last page
        traceFile.Seek(fileIndex.back().second.first, CFile::begin);
        //Remove last page from file index cache
        fileIndex.pop_back();
    }
    try
    {
		uint64 pos = traceFile.GetPosition();
		uint64 len = traceFile.GetLength();
        while(traceFile.GetPosition() < traceFile.GetLength())
        {
            uint64 blockStart = traceFile.GetPosition();
            bool isPageBoundary = readBlock(traceFile);
            if(isPageBoundary)
            {
                if(lastIndex != 0)
                    fileIndex.back().second.second = index - (lastIndex - 1);
                fileIndex.push_back(std::make_pair(index, CTraceFileReader::Range(blockStart, 0)));
                lastIndex = index + 1;
                isBlockExist = true;
            }
            index++;
        }
        if(isBlockExist)
            fileIndex.back().second.second = index - (lastIndex - 1);
        error = false;
        length = index;
    }
    catch(std::wstring & errReason)
    {
        error = true;
    }
}

CTraceFilePage* CTraceFileReader::getPage(unsigned long long index, unsigned long long* base)
{
	if (lastAccessedPage)
	{
		if (index >= lastAccessedIndexOffset && index < lastAccessedIndexOffset + lastAccessedPage->Length())
		{
			*base = lastAccessedIndexOffset;
			return lastAccessedPage;
		}
	}
	const auto cache = pages.find(Range(index, index));
	if (cache != pages.cend())
	{
		if (cache->first.first >= index && cache->first.second <= index)
		{
			if (lastAccessedPage)
				GetSystemTimes(nullptr, nullptr, &lastAccessedPage->lastAccessed);
			lastAccessedPage = &cache->second;
			lastAccessedIndexOffset = cache->first.first;
			GetSystemTimes(nullptr, nullptr, &lastAccessedPage->lastAccessed);
			*base = lastAccessedIndexOffset;
			return lastAccessedPage;
		}
	}
	else if (index >= Length()) //Out of bound
		return nullptr;
	//page in
	if (pages.size() >= 2048) //TODO: trim resident pages based on system memory usage, instead of a hard limit.
	{
		FILETIME pageOutTime = pages.begin()->second.lastAccessed;
		Range pageOutIndex = pages.begin()->first;
		for (auto i : pages)
		{
			if (pageOutTime.dwHighDateTime < i.second.lastAccessed.dwHighDateTime || (pageOutTime.dwHighDateTime == i.second.lastAccessed.dwHighDateTime && pageOutTime.dwLowDateTime < i.second.lastAccessed.dwLowDateTime))
			{
				pageOutTime = i.second.lastAccessed;
				pageOutIndex = i.first;
			}
		}
		pages.erase(pageOutIndex);
	}
	//binary search fileIndex to get file offset, push a TraceFilePage into cache and return it.
	size_t start = 0;
	size_t end = fileIndex.size() - 1;
	size_t middle = (start + end) / 2;
	std::pair<unsigned long long, Range>* fileOffset;
	while (true)
	{
		if (start == end || start == end - 1)
		{
			if (fileIndex[end].first <= index)
				fileOffset = &fileIndex[end];
			else
				fileOffset = &fileIndex[start];
			break;
		}
		if (fileIndex[middle].first > index)
			end = middle;
		else if (fileIndex[middle].first == index)
		{
			fileOffset = &fileIndex[middle];
			break;
		}
		else
			start = middle;
		middle = (start + end) / 2;
	}

	if (fileOffset->second.second + fileOffset->first >= index && fileOffset->first <= index)
	{
		Range r(fileOffset->first, fileOffset->first + fileOffset->second.second - 1);
		pages.insert(std::make_pair(Range(fileOffset->first, fileOffset->first + fileOffset->second.second - 1), CTraceFilePage(this, fileOffset->second.first, fileOffset->second.second)));
		const auto newPage = pages.find(Range(index, index));
		if (newPage != pages.cend())
		{
			if (lastAccessedPage)
				GetSystemTimes(nullptr, nullptr, &lastAccessedPage->lastAccessed);
			lastAccessedPage = &newPage->second;
			lastAccessedIndexOffset = newPage->first.first;
			GetSystemTimes(nullptr, nullptr, &lastAccessedPage->lastAccessed);
			*base = lastAccessedIndexOffset;
			return lastAccessedPage;
		}
		else
		{
			GuiAddLogMessage("PAGEFAULT2\r\n"); //debug
			return nullptr; //???
		}
	}
	else
	{
		GuiAddLogMessage("PAGEFAULT1\r\n"); //debug
		return nullptr; //???
	}
}


//CTraceFilePage
CTraceFilePage::CTraceFilePage(CTraceFileReader* parent, unsigned long long fileOffset, unsigned long long maxLength)
{
	DWORD lastThreadId = 0;
	union
	{
		REGDUMP registers;
		duint regwords[(FIELD_OFFSET(REGDUMP, lastError) + sizeof(DWORD)) / sizeof(duint)];
	};
	unsigned char changed[_countof(regwords)];
	duint regContent[_countof(regwords)];
	duint memAddress[MAX_MEMORY_OPERANDS];
	duint memOldContent[MAX_MEMORY_OPERANDS];
	duint memNewContent[MAX_MEMORY_OPERANDS];
	size_t memOperandOffset = 0;
	mParent = parent;
	length = 0;
	GetSystemTimes(nullptr, nullptr, &lastAccessed); //system user time, no GetTickCount64() for XP compatibility.
	memset(&registers, 0, sizeof(registers));
	try
	{
		if (mParent->traceFile.Seek(fileOffset, CFile::begin) != fileOffset)
			throw std::exception();
		//Process file content
		while (mParent->traceFile.GetPosition() != mParent->traceFile.GetLength() && length < maxLength)
		{
			unsigned char blockType;
			unsigned char changedCountFlags[3]; //reg changed count, mem accessed count, flags
			mParent->traceFile.Read((char*)&blockType, 1);
			if (blockType == 0)
			{
				if (mParent->traceFile.Read((char*)&changedCountFlags, 3) != 3)
					throw std::exception();
				if (changedCountFlags[2] & 0x80) //Thread Id
					mParent->traceFile.Read((char*)&lastThreadId, 4);
				threadId.push_back(lastThreadId);
				int size = changedCountFlags[2] & 0x0F;
				if ((changedCountFlags[2] & 0x0F) > 0) //Opcode
				{
					/*
					CByteArray opcode;
					opcode.SetSize(size);
					char* buffer = new char[size];
					mParent->traceFile.Read(buffer, changedCountFlags[2] & 0x0F);
					for (int i = 0; i < size; i++)
					{
						opcode[i] = buffer[i];
					}
					*/

					CByteArray opcode;
					opcode.SetSize(size);
					mParent->traceFile.Read(opcode.GetData(), changedCountFlags[2] & 0x0F);
					
					if (opcode.IsEmpty())
						throw std::exception();
					opcodeOffset.push_back(opcodes.GetSize());
					opcodeSize.push_back(opcode.GetSize());
					opcodes.Append(opcode);
				}
				else
					throw std::exception();
				if (changedCountFlags[0] > 0) //registers
				{
					int lastPosition = -1;
					if (changedCountFlags[0] > _countof(regwords)) //Bad count?
						throw std::exception();
					if (mParent->traceFile.Read((char*)changed, changedCountFlags[0]) != changedCountFlags[0])
						throw std::exception();
					if (mParent->traceFile.Read((char*)regContent, changedCountFlags[0] * sizeof(duint)) != changedCountFlags[0] * sizeof(duint))
					{
						throw std::exception();
					}
					for (int i = 0; i < changedCountFlags[0]; i++)
					{
						lastPosition = lastPosition + changed[i] + 1;
						if (lastPosition < _countof(regwords) && lastPosition >= 0)
							regwords[lastPosition] = regContent[i];
						else //out of bounds?
						{
							throw std::exception();
						}
					}
					mRegisters.push_back(registers);
				}
				if (changedCountFlags[1] > 0) //memory
				{
					
					if (changedCountFlags[1] > _countof(memAddress)) //too many memory operands?
						throw std::exception();
					
					CByteArray memflags;
					memflags.SetSize(changedCountFlags[1]);
					
					mParent->traceFile.Read(memflags.GetData(), changedCountFlags[1] & 0x0F);
					
					if (memflags.GetSize() < changedCountFlags[1])
						throw std::exception();
					memoryOperandOffset.push_back(memOperandOffset);
					memOperandOffset += changedCountFlags[1];
					if (mParent->traceFile.Read((char*)memAddress, sizeof(duint) * changedCountFlags[1]) != sizeof(duint) * changedCountFlags[1])
						throw std::exception();
					if (mParent->traceFile.Read((char*)memOldContent, sizeof(duint) * changedCountFlags[1]) != sizeof(duint) * changedCountFlags[1])
						throw std::exception();
					for (unsigned char i = 0; i < changedCountFlags[1]; i++)
					{
						if ((memflags[i] & 1) == 0)
						{
							if (mParent->traceFile.Read((char*)&memNewContent[i], sizeof(duint)) != sizeof(duint))
								throw std::exception();
						}
						else
							memNewContent[i] = memOldContent[i];
					}
					for (unsigned char i = 0; i < changedCountFlags[1]; i++)
					{
						memoryFlags.push_back(memflags[i]);
						memoryAddress.push_back(memAddress[i]);
						oldMemory.push_back(memOldContent[i]);
						newMemory.push_back(memNewContent[i]);
					}
				}
				else
					memoryOperandOffset.push_back(memOperandOffset);
				length++;
			}
			else
				throw std::exception();
		}

	}
	catch (const std::exception &)
	{
		mParent->error = true;
	}
}

unsigned long long CTraceFilePage::Length() const
{
	return length;
}

REGDUMP CTraceFilePage::Registers(unsigned long long index) const
{
	return mRegisters.at(index);
}

void CTraceFilePage::OpCode(unsigned long long index, unsigned char* buffer, int* opcodeSize) const
{
	*opcodeSize = this->opcodeSize.at(index);
	memcpy(buffer, opcodes.GetData() + opcodeOffset.at(index), *opcodeSize);
}

DWORD CTraceFilePage::ThreadId(unsigned long long index) const
{
	return threadId.at(index);
}
