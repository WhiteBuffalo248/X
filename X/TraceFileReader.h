#pragma once
#include "Bridge.h"
#include "InstructTrace.h"
#include <atomic>
#include <map>


class CTraceFileParser;
class CTraceFilePage;

#define MAX_MEMORY_OPERANDS 32
#define TraceFileParserThread_Exit		101
#define TraceFileParser_Finish			102


class CTraceFileReader
{
public:
	CTraceFileReader(CInstructTrace* parent);
	~CTraceFileReader();

public:
	bool Open(const CString & fileName);
	
	void Close();
	void Delete();
	void parseFinished();
	bool isError() const;
	int Progress() const;

	unsigned long long Length() const;

	REGDUMP Registers(unsigned long long index);
	void OpCode(unsigned long long index, unsigned char* buffer, int* opcodeSize);
	DWORD ThreadId(unsigned long long index);
	//int MemoryAccessCount(unsigned long long index);
	//void MemoryAccessInfo(unsigned long long index, duint* address, duint* oldMemory, duint* newMemory, bool* isValid);
	duint HashValue() const;
	CString ExePath() const;

	void purgeLastPage();
	
	

private:
	typedef std::pair<unsigned long long, unsigned long long> Range;
	
	struct RangeCompare //from addrinfo.h
	{
		bool operator()(const Range & a, const Range & b) const //a before b?
		{
			return a.second < b.first;
		}
	};

	CInstructTrace* parent;

	CFile traceFile;
	unsigned long long length;
	duint hashValue;
	CString EXEPath;
	std::vector<std::pair<unsigned long long, Range>> fileIndex; //index;<file offset;length>
	std::atomic<int> progress;
	bool error;
	CTraceFilePage* lastAccessedPage;
	unsigned long long lastAccessedIndexOffset;
	friend class CTraceFileParser;
	friend class CTraceFilePage;

	CTraceFileParser* parser;
	std::map<Range, CTraceFilePage, RangeCompare> pages;

	CTraceFilePage* getPage(unsigned long long index, unsigned long long* base);
};

