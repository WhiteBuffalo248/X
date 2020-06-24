#ifndef QBEAENGINE_H
#define QBEAENGINE_H

#include <vector>
#include "capstone_gui.h"
#include "MyByteArray.h"

class EncodeMap;
class CodeFoldingHelper;

struct Instruction_t
{
	enum BranchType
	{
		None,
		Conditional,
		Unconditional,
		Call
	};

	Instruction_t()
		: rva(0),
		length(0),
		branchDestination(0),
		branchType(None)
	{
	}

	~Instruction_t()
	{
	}

    CString instStr;
	CMyByteArray dump;
    uint8_t prefixSize, opcodeSize, group1Size, group2Size, group3Size;
    duint rva;
    int length;
    duint branchDestination;
    BranchType branchType;
    CapstoneTokenizer::InstructionToken tokens;
    std::vector<std::pair<const char*, uint8_t>> regsReferenced;
};


class QBeaEngine
{
public:
	explicit QBeaEngine(int maxModuleSize);
	~QBeaEngine();
	duint DisassembleBack(byte_t* data, duint base, duint size, duint ip, int n);
	duint DisassembleNext(byte_t* data, duint base, duint size, duint ip, int n);
	Instruction_t DisassembleAt(byte_t* data, duint size, duint origBase, duint origInstRVA, bool datainstr = true);
	Instruction_t DecodeDataAt(byte_t* data, duint size, duint origBase, duint origInstRVA, ENCODETYPE type);
	void setCodeFoldingManager(CodeFoldingHelper* CodeFoldingManager);
	void UpdateConfig();

	EncodeMap* getEncodeMap()
	{
		return mEncodeMap;
	}

private:
	struct DataInstructionInfo
	{
		CString shortName;
		CString longName;
		CString cName;
	};

	void UpdateDataInstructionMap();
	CapstoneTokenizer _tokenizer;
	//QHash<ENCODETYPE, DataInstructionInfo> dataInstMap;
	unordered_map<ENCODETYPE, DataInstructionInfo> dataInstMap;
	bool _bLongDataInst;
	EncodeMap* mEncodeMap;
	CodeFoldingHelper* mCodeFoldingManager;
	uint8_t reginfo[ZYDIS_REGISTER_MAX_VALUE + 1];
	uint8_t flaginfo[ZYDIS_CPUFLAG_MAX_VALUE + 1];
};

void formatOpcodeString(const Instruction_t & inst, RichTextPainter::List & list, std::vector<std::pair<size_t, bool>> & realBytes);
#endif // QBEAENGINE_H