#pragma once
#include "Imports.h"
#include "RichTextPainter.h"
#include <zydis_wrapper.h>
#include "Configuration.h"
#include <map>


class CapstoneTokenizer
{
public:
	enum class TokenType
	{
		//filling
		Comma = 0,
		Space,
		ArgumentSpace,
		MemoryOperatorSpace,
		//general instruction parts
		Prefix,
		Uncategorized,
		//mnemonics
		MnemonicNormal,
		MnemonicPushPop,
		MnemonicCall,
		MnemonicRet,
		MnemonicCondJump,
		MnemonicUncondJump,
		MnemonicNop,
		MnemonicFar,
		MnemonicInt3,
		MnemonicUnusual,
		//values
		Address, //jump/call destinations or displacements inside memory
		Value,
		//memory
		MemorySize,
		MemorySegment,
		MemoryBrackets,
		MemoryStackBrackets,
		MemoryBaseRegister,
		MemoryIndexRegister,
		MemoryScale,
		MemoryOperator, //'+', '-' and '*'
		//registers
		GeneralRegister,
		FpuRegister,
		MmxRegister,
		XmmRegister,
		YmmRegister,
		ZmmRegister,
		//last
		Last
	};

	struct TokenValue
	{
		int size; //value size (in bytes), zero means no value
		duint value; //actual value

		TokenValue(int size, duint value) :
			size(size),
			value(value)
		{
		}

		TokenValue() :
			size(0),
			value(0)
		{
		}

		bool operator == (const TokenValue & rhs) const
		{
			return /*size == rhs.size &&*/ value == rhs.value;
		}
	};

	struct SingleToken
	{
		TokenType type; //token type
		CString text; //token text
		TokenValue value; //token value (if applicable)

		SingleToken() :
			type(TokenType::Uncategorized)
		{
		}

		SingleToken(TokenType type, const CString & text, const TokenValue & value) :
			type(type),
			text(text),
			value(value)
		{
		}

		SingleToken(TokenType type, const CString & text) :
			SingleToken(type, text, TokenValue())
		{
		}
		
		bool operator == (const SingleToken & rhs) const
		{
			return type == rhs.type && text == rhs.text && value == rhs.value;
		}
		
	};

	struct InstructionToken
	{
		std::vector<SingleToken> tokens; //list of tokens that form the instruction
		int x; //x of the first character

		InstructionToken()
		{
			tokens.clear();
			x = 0;
		}
	};

	struct TokenColor
	{
		RichTextPainter::CustomRichTextFlags flags;
		Color color;
		Color backgroundColor;

		TokenColor(string strColor, string StrBackgroundColor)
		{
			if (strColor.length() && StrBackgroundColor.length())
			{
				this->flags = RichTextPainter::FlagAll;
				this->color = ConfigColor(strColor);
				this->backgroundColor = ConfigColor(StrBackgroundColor);
			}
			else if (strColor.length())
			{
				this->flags = RichTextPainter::FlagColor;
				this->color = ConfigColor(strColor);
			}
			else if (StrBackgroundColor.length())
			{
				this->flags = RichTextPainter::FlagBackground;
				this->backgroundColor = ConfigColor(StrBackgroundColor);
			}
			else
				this->flags = RichTextPainter::FlagNone;
		}

		TokenColor()
			: TokenColor("", "")
		{
		}
	};

	CapstoneTokenizer(int maxModuleLength);
	bool Tokenize(duint addr, const unsigned char* data, int datasize, InstructionToken & instruction);
	bool TokenizeData(const CString & datatype, const CString & data, InstructionToken & instruction);
	void UpdateConfig();
	void SetConfig(bool bUppercase, bool bTabbedMnemonic, bool bArgumentSpaces, bool bHidePointerSizes, bool bHideNormalSegments, bool bMemorySpaces, bool bNoHighlightOperands, bool bNoCurrentModuleText, bool b0xPrefixValues);
	int Size() const;
	const Zydis & GetCapstone() const;

	static void UpdateColors();
	static void UpdateStringPool();
	static void TokenToRichText(const InstructionToken & instr, RichTextPainter::List & richTextList, const SingleToken* highlightToken);
	static bool TokenFromX(const InstructionToken & instr, SingleToken & token, int x, LOGFONT* fontMetrics);//...LOGFONT
	static bool IsHighlightableToken(const SingleToken & token);
	static bool TokenEquals(const SingleToken* a, const SingleToken* b, bool ignoreSize = true);
	static void addColorName(TokenType type, string color, string backgroundColor);
	static void addStringsToPool(const string& regs);
	static bool tokenTextPoolEquals(const CString & a, const CString & b);

private:
	Zydis _cp;
	bool isNop;
	InstructionToken _inst;
	bool _success;
	int _maxModuleLength;
	bool _bUppercase;
	bool _bTabbedMnemonic;
	bool _bArgumentSpaces;
	bool _bHidePointerSizes;
	bool _bHideNormalSegments;
	bool _bMemorySpaces;
	bool _bNoHighlightOperands;
	bool _bNoCurrentModuleText;
	bool _b0xPrefixValues;
	TokenType _mnemonicType;

	void addToken(TokenType type, CString text, const TokenValue & value);
	void addToken(TokenType type, const CString & text);
	void addMemoryOperator(char operatorText);
	CString printValue(const TokenValue & value, bool expandModule, int maxModuleLength) const;

	static std::map<CString, int> stringPoolMap;
	static int poolId;

	bool tokenizePrefix();
	bool tokenizeMnemonic();
	bool tokenizeMnemonic(TokenType type, const CString & mnemonic);
	bool tokenizeOperand(const ZydisDecodedOperand & op);
	bool tokenizeRegOperand(const ZydisDecodedOperand & op);
	bool tokenizeImmOperand(const ZydisDecodedOperand & op);
	bool tokenizeMemOperand(const ZydisDecodedOperand & op);
	bool tokenizePtrOperand(const ZydisDecodedOperand & op);
	bool tokenizeInvalidOperand(const ZydisDecodedOperand & op);
};

