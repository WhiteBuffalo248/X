#pragma once
#include "TraceFileReader.h"
#include "MyByteArray.h"

class CTraceFileParser : public CWinThread
{	
public:
	bool bExit = false;
	int mExitCode = -1;
	friend class CTraceFileReader;
	CTraceFileParser(){}
	CTraceFileParser(CTraceFileReader* parent) : parent(parent){}
	static void readFileHeader(CTraceFileReader* that);
	int mainWork();
	virtual BOOL InitInstance();
private:
	CTraceFileReader *parent;
};

class CTraceFilePage
{
public:
	CTraceFilePage(CTraceFileReader* parent, unsigned long long fileOffset, unsigned long long maxLength);
	
	unsigned long long Length() const;
	REGDUMP Registers(unsigned long long index) const;
	void OpCode(unsigned long long index, unsigned char* buffer, int* opcodeSize) const;
	DWORD ThreadId(unsigned long long index) const;
	//int MemoryAccessCount(unsigned long long index) const;
	//void MemoryAccessInfo(unsigned long long index, duint* address, duint* oldMemory, duint* newMemory, bool* isValid) const;
	
	

	FILETIME lastAccessed; //system user time

private:
	friend class CTraceFileReader;
	CTraceFileReader* mParent;
	std::vector<REGDUMP> mRegisters;
	CMyByteArray opcodes;
	std::vector<size_t> opcodeOffset;
	std::vector<unsigned char> opcodeSize;
	std::vector<size_t> memoryOperandOffset;
	std::vector<char> memoryFlags;
	std::vector<duint> memoryAddress;
	std::vector<duint> oldMemory;
	std::vector<duint> newMemory;
	std::vector<DWORD> threadId;
	unsigned long long length;
};
