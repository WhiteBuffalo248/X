#include "stdafx.h"
#include "QBeaEngine.h"
#include "StringUtil.h"
#include "EncodeMap.h"
#include "CodeFolding.h"
#include "Configuration.h"

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

QBeaEngine::QBeaEngine(int maxModuleSize)
	: _tokenizer(maxModuleSize), mCodeFoldingManager(nullptr), _bLongDataInst(false)
{
	CapstoneTokenizer::UpdateColors();
	UpdateDataInstructionMap();
	this->mEncodeMap = new EncodeMap();
}

QBeaEngine::~QBeaEngine()
{
	delete this->mEncodeMap;
}

/**
* @brief       Return the address of the nth instruction before the instruction pointed by ip.                 @n
*              This function has been grabbed from OllyDbg ("Disassembleback" in asmserv.c)
*
* @param[in]   data    Address of the data to disassemble
* @param[in]   base    Original base address of the memory page (Required to disassemble destination addresses)
* @param[in]   size    Size of the data block pointed by data
* @param[in]   ip      RVA of the current instruction (Relative to data pointer)
* @param[in]   n       Number of instruction back
*
* @return      Return the RVA (Relative to the data pointer) of the nth instruction before the instruction pointed by ip
*/
duint QBeaEngine::DisassembleBack(byte_t* data, duint base, duint size, duint ip, int n)
{
	int i;
	UINT abuf[128], addr, back, cmdsize;
	memset(abuf, 0, 128);
	unsigned char* pdata;

	// Reset Disasm Structure
	Zydis cp;

	// Check if the pointer is not null
	if (data == NULL)
		return 0;

	// Round the number of back instructions to 127
	if (n < 0)
		n = 0;
	else if (n > 127)
		n = 127;

	// Check if the instruction pointer ip is not outside the memory range
	if (ip >= size)
		ip = size - 1;

	// Obvious answer
	if (n == 0)
		return ip;

	if (ip < (UINT)n)
		return ip;

	//TODO: buffer overflow due to unchecked "back" value
	back = MAX_DISASM_BUFFER * (n + 3); // Instruction length limited to 16

	if (ip < back)
		back = ip;

	addr = ip - back;
	if (mCodeFoldingManager && mCodeFoldingManager->isFolded(addr + base))
	{
		duint newback = mCodeFoldingManager->getFoldBegin(addr + base);
		if (newback >= base && newback < size + base)
			addr = newback - base;
	}

	pdata = data + addr;

	for (i = 0; addr < ip; i++)
	{
		abuf[i % 128] = addr;

		if (mCodeFoldingManager && mCodeFoldingManager->isFolded(addr + base))
		{
			duint newaddr = mCodeFoldingManager->getFoldBegin(addr + base);
			if (newaddr >= base)
			{
				addr = newaddr - base;
			}
			cmdsize = mCodeFoldingManager->getFoldEnd(addr + base) - (addr + base) + 1;
		}
		else
		{
			if (!cp.DisassembleSafe(addr + base, pdata, (int)size))
				cmdsize = 2; //heuristic for better output (FF FE or FE FF are usually part of an instruction)
			else
				cmdsize = cp.Size();

			cmdsize = mEncodeMap->getDataSize(base + addr, cmdsize);

		}


		pdata += cmdsize;
		addr += cmdsize;
		back -= cmdsize;
		size -= cmdsize;
	}

	if (i < n)
		return abuf[0];
	else
		return abuf[(i - n + 128) % 128];

}

/**
* @brief       Return the address of the nth instruction after the instruction pointed by ip.                 @n
*              This function has been grabbed from OllyDbg ("Disassembleforward" in asmserv.c)
*
* @param[in]   data    Address of the data to disassemble
* @param[in]   base    Original base address of the memory page (Required to disassemble destination addresses)
* @param[in]   size    Size of the data block pointed by data
* @param[in]   ip      RVA of the current instruction (Relative to data pointer)
* @param[in]   n       Number of instruction next
*
* @return      Return the RVA (Relative to the data pointer) of the nth instruction after the instruction pointed by ip
*/
duint QBeaEngine::DisassembleNext(byte_t* data, duint base, duint size, duint ip, int n)
{
	int i;
	UINT cmdsize;
	unsigned char* pdata;

	// Reset Disasm Structure
	Zydis cp;

	if (data == NULL)
		return 0;

	if (ip >= size)
		ip = size - 1;

	if (n <= 0)
		return ip;


	pdata = data + ip;
	size -= ip;

	for (i = 0; i < n && size > 0; i++)
	{
		if (mCodeFoldingManager && mCodeFoldingManager->isFolded(ip + base))
		{
			cmdsize = mCodeFoldingManager->getFoldEnd(ip + base) - (ip + base) + 1;
		}
		else
		{
			if (!cp.DisassembleSafe(ip + base, pdata, (int)size))
				cmdsize = 1;
			else
				cmdsize = cp.Size();

			cmdsize = mEncodeMap->getDataSize(base + ip, cmdsize);

		}

		pdata += cmdsize;
		ip += cmdsize;
		size -= cmdsize;
	}

	return ip;
}

/**
* @brief       Disassemble the instruction at the given ip RVA.
*
* @param[in]   data            Pointer to memory data (Can be either a buffer or the original data memory)
* @param[in]   size            Size of the memory pointed by data (Can be the memory page size if data points to the original memory page base address)
* @param[in]   origBase        Original base address of the memory page (Required to disassemble destination addresses)
* @param[in]   origInstRVA     Original Instruction RVA of the instruction to disassemble
*
* @return      Return the disassembled instruction
*/
Instruction_t QBeaEngine::DisassembleAt(byte_t* data, duint size, duint origBase, duint origInstRVA, bool datainstr)
{
	if (datainstr)
	{
		ENCODETYPE type = mEncodeMap->getDataType(origBase + origInstRVA);
		if (!mEncodeMap->isCode(type))
			return DecodeDataAt(data, size, origBase, origInstRVA, type);
	}
	//tokenize
	CapstoneTokenizer::InstructionToken cap;
	_tokenizer.Tokenize(origBase + origInstRVA, data, size, cap);
	int len = _tokenizer.Size();

	const auto & cp = _tokenizer.GetCapstone();
	bool success = cp.Success();


	auto branchType = Instruction_t::None;
	Instruction_t wInst;
	if (success && cp.IsBranchType(Zydis::BTJmp | Zydis::BTCall | Zydis::BTRet | Zydis::BTLoop))
	{
		wInst.branchDestination = DbgGetBranchDestination(origBase + origInstRVA);
		if (cp.IsBranchType(Zydis::BTUncondJmp))
			branchType = Instruction_t::Unconditional;
		else if (cp.IsBranchType(Zydis::BTCall))
			branchType = Instruction_t::Call;
		else if (cp.IsBranchType(Zydis::BTCondJmp))
			branchType = Instruction_t::Conditional;
	}
	else
		wInst.branchDestination = 0;

	//wInst.instStr = CString(cp.InstructionText().c_str());
	wInst.instStr = CA2W(cp.InstructionText().data());

	//wInst.dump = QByteArray((const char*)data, len);
	//CByteArray 不支持操作符=，故用指针，使用时初始化空间；
	wInst.dump.SetSize(len);	
	for (int i = 0; i < len; i++)
		wInst.dump.SetAt(i, data[i]);//wInst.dump[i] = data[i];

	wInst.rva = origInstRVA;
	if (mCodeFoldingManager && mCodeFoldingManager->isFolded(origInstRVA))
		wInst.length = mCodeFoldingManager->getFoldEnd(origInstRVA + origBase) - (origInstRVA + origBase) + 1;
	else
		wInst.length = len;
	wInst.branchType = branchType;
	wInst.tokens = cap;
	cp.BytesGroup(&wInst.prefixSize, &wInst.opcodeSize, &wInst.group1Size, &wInst.group2Size, &wInst.group3Size);

	if (!success)
		return wInst;

	auto instr = cp.GetInstr();
	cp.RegInfo(reginfo);

	for (size_t i = 0; i < _countof(instr->accessedFlags); ++i)
	{
		auto flagAction = instr->accessedFlags[i].action;
		if (flagAction == ZYDIS_CPUFLAG_ACTION_NONE)
			continue;

		Zydis::RegAccessInfo rai;
		switch (flagAction)
		{
		case ZYDIS_CPUFLAG_ACTION_MODIFIED:
		case ZYDIS_CPUFLAG_ACTION_SET_0:
		case ZYDIS_CPUFLAG_ACTION_SET_1:
			rai = Zydis::RAIWrite;
			break;
		case ZYDIS_CPUFLAG_ACTION_TESTED:
			rai = Zydis::RAIRead;
			break;
		default:
			rai = Zydis::RAINone;
			break;
		}

		reginfo[ZYDIS_REGISTER_RFLAGS] = Zydis::RAINone;
		reginfo[ZYDIS_REGISTER_EFLAGS] = Zydis::RAINone;
		reginfo[ZYDIS_REGISTER_FLAGS] = Zydis::RAINone;

		wInst.regsReferenced.emplace_back(cp.FlagName(ZydisCPUFlag(i)), rai);
	}

	reginfo[ArchValue(ZYDIS_REGISTER_EIP, ZYDIS_REGISTER_RIP)] = Zydis::RAINone;
	for (int i = ZYDIS_REGISTER_NONE; i <= ZYDIS_REGISTER_MAX_VALUE; ++i)
		if (reginfo[i])
			wInst.regsReferenced.emplace_back(cp.RegName(ZydisRegister(i)), reginfo[i]);

	return wInst;
}

Instruction_t QBeaEngine::DecodeDataAt(byte_t* data, duint size, duint origBase, duint origInstRVA, ENCODETYPE type)
{
	//tokenize
	CapstoneTokenizer::InstructionToken cap;

	auto infoIter = dataInstMap.find(type);
	if (infoIter == dataInstMap.end())
		infoIter = dataInstMap.find(enc_byte);

	int len = mEncodeMap->getDataSize(origBase + origInstRVA, 1);

	//CString mnemonic = _bLongDataInst ? infoIter.value().longName : infoIter.value().shortName;
	CString mnemonic = _bLongDataInst ? infoIter->second.longName: infoIter->second.shortName;

	len = min(len, (int)size);

	CString datastr = GetDataTypeString(data, len, type);

	_tokenizer.TokenizeData(mnemonic, datastr, cap);

	Instruction_t wInst;
	wInst.instStr = mnemonic + " " + datastr;

	//wInst.dump = QByteArray((const char*)data, len);
	wInst.dump.RemoveAll();
	wInst.dump.SetSize(len);
	for (int i = 0; i < len; i++)
		wInst.dump.SetAt(i, data[i]);//wInst.dump[i] = data[i];

	wInst.rva = origInstRVA;
	wInst.length = len;
	wInst.branchType = Instruction_t::None;
	wInst.branchDestination = 0;
	wInst.tokens = cap;

	return wInst;
}

void QBeaEngine::UpdateDataInstructionMap()
{
	dataInstMap.clear();

	dataInstMap.insert(pair<ENCODETYPE, DataInstructionInfo>(enc_byte, { _T("db"), _T("byte"), _T("int8") }));
	dataInstMap.insert(pair<ENCODETYPE, DataInstructionInfo>(enc_word, { _T("dw"), _T("word"), _T("short") }));
	dataInstMap.insert(pair<ENCODETYPE, DataInstructionInfo>(enc_dword, { _T("dd"), _T("dword"), _T("int") }));
	dataInstMap.insert(pair<ENCODETYPE, DataInstructionInfo>(enc_fword, { _T("df"), _T("fword"), _T("fword") }));
	dataInstMap.insert(pair<ENCODETYPE, DataInstructionInfo>(enc_qword, { _T("dq"), _T("qword"), _T("long") }));
	dataInstMap.insert(pair<ENCODETYPE, DataInstructionInfo>(enc_tbyte, { _T("tbyte"), _T("tbyte"), _T("tbyte") }));
	dataInstMap.insert(pair<ENCODETYPE, DataInstructionInfo>(enc_oword, { _T("oword"), _T("oword"), _T("oword") }));
	dataInstMap.insert(pair<ENCODETYPE, DataInstructionInfo>(enc_mmword, { _T("mmword"), _T("mmword"), _T("long long") }));
	dataInstMap.insert(pair<ENCODETYPE, DataInstructionInfo>(enc_xmmword, { _T("xmmword"), _T("xmmword"), _T("_m128") }));
	dataInstMap.insert(pair<ENCODETYPE, DataInstructionInfo>(enc_ymmword, { _T("ymmword"), _T("ymmword"), _T("_m256") }));
	dataInstMap.insert(pair<ENCODETYPE, DataInstructionInfo>(enc_real4, { _T("real4"), _T("real4"), _T("int") }));
	dataInstMap.insert(pair<ENCODETYPE, DataInstructionInfo>(enc_real8, { _T("real8"), _T("real8"), _T("double") }));
	dataInstMap.insert(pair<ENCODETYPE, DataInstructionInfo>(enc_real10, { _T("real10"), _T("real10"), _T("long double") }));
	dataInstMap.insert(pair<ENCODETYPE, DataInstructionInfo>(enc_ascii, { _T("ascii"), _T("ascii"), _T("string") }));
	dataInstMap.insert(pair<ENCODETYPE, DataInstructionInfo>(enc_unicode, { _T("unicode"), _T("unicode"), _T("wstring") }));
}

void QBeaEngine::setCodeFoldingManager(CodeFoldingHelper* CodeFoldingManager)
{
	mCodeFoldingManager = CodeFoldingManager;
}

void QBeaEngine::UpdateConfig()
{
	_bLongDataInst = ConfigBool("Disassembler", "LongDataInstruction");
	_tokenizer.UpdateConfig();
}

void formatOpcodeString(const Instruction_t & inst, RichTextPainter::List & list, std::vector<std::pair<size_t, bool>> & realBytes)
{
	RichTextPainter::CustomRichText_t curByte;
	size_t size = inst.dump.GetSize();
	assert(list.empty()); //List must be empty before use
	curByte.highlightWidth = 1;
	curByte.flags = RichTextPainter::FlagAll;
	curByte.highlight = false;
	list.reserve(size + 5);
	realBytes.reserve(size + 5);
	for (size_t i = 0; i < size; i++)
	{
		curByte.text = ToByteString((BYTE)inst.dump.GetAt(i));
		list.push_back(curByte);
		realBytes.push_back({ i, true });

		auto addCh = [&](char ch)
		{
			curByte.text = CString(ch);
			list.push_back(curByte);
			realBytes.push_back({ i, false });
		};

		if (inst.prefixSize && i + 1 == inst.prefixSize)
			addCh(':');
		else if (inst.opcodeSize && i + 1 == inst.prefixSize + inst.opcodeSize)
			addCh(' ');
		else if (inst.group1Size && i + 1 == inst.prefixSize + inst.opcodeSize + inst.group1Size)
			addCh(' ');
		else if (inst.group2Size && i + 1 == inst.prefixSize + inst.opcodeSize + inst.group1Size + inst.group2Size)
			addCh(' ');
		else if (inst.group3Size && i + 1 == inst.prefixSize + inst.opcodeSize + inst.group1Size + inst.group2Size + inst.group3Size)
			addCh(' ');

	}
}
