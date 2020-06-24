#include "argument.h"

struct ArgumentSerializer : JSONWrapper<ARGUMENTSINFO>
{
	bool Save(const ARGUMENTSINFO & value) override
	{
		setString("module", value.mod());
		setHex("start", value.start);
		setHex("end", value.end);
		setHex("icount", value.instructioncount);
		setBool("manual", value.manual);
		return true;
	}

	bool Load(ARGUMENTSINFO & value) override
	{
		std::string mod;
		if (!getString("module", mod))
			return false;
		value.modhash = ModHashFromName(mod.c_str());
		return getHex("start", value.start) &&
			getHex("end", value.end) &&
			getBool("manual", value.manual) &&
			getHex("icount", value.instructioncount) &&
			value.end >= value.start;
	}
};

struct Arguments : SerializableModuleRangeMap<LockArguments, ARGUMENTSINFO, ArgumentSerializer>
{
	void AdjustValue(ARGUMENTSINFO & value) const override
	{
		auto base = ModBaseFromName(value.mod().c_str());
		value.start += base;
		value.end += base;
	}

protected:
	const char* jsonKey() const override
	{
		return "arguments";
	}

	ModuleRange makeKey(const ARGUMENTSINFO & value) const override
	{
		return ModuleRange(value.modhash, Range(value.start, value.end));
	}
};

static Arguments arguments;

bool ArgumentGet(duint Address, duint* Start, duint* End, duint* InstrCount)
{
	ARGUMENTSINFO argument;
	if (!arguments.Get(Arguments::VaKey(Address, Address), argument))
		return false;
	arguments.AdjustValue(argument);
	if (Start)
		*Start = argument.start;
	if (End)
		*End = argument.end;
	if (InstrCount)
		*InstrCount = argument.instructioncount;
	return true;
}

void ArgumentCacheSave(JSON Root)
{
	arguments.CacheSave(Root);
}

void ArgumentCacheLoad(JSON Root)
{
	arguments.CacheLoad(Root);
}

void ArgumentClear()
{
	arguments.Clear();
}

bool ArgumentOverlaps(duint Start, duint End)
{
	// A argument can't end before it begins
	if (Start > End)
		return false;
	return arguments.Contains(Arguments::VaKey(Start, End));
}

bool ArgumentDelete(duint Address)
{
	return arguments.Delete(Arguments::VaKey(Address, Address));
}

bool ArgumentAdd(duint Start, duint End, bool Manual, duint InstructionCount)
{
	// Make sure memory is readable
	if (!MemIsValidReadPtr(Start))
		return false;

	// Fail if boundary exceeds module size
	auto moduleBase = ModBaseFromAddr(Start);

	if (moduleBase != ModBaseFromAddr(End))
		return false;

	// Fail if 'Start' and 'End' are incompatible
	if (Start > End || ArgumentOverlaps(Start, End))
		return false;

	ARGUMENTSINFO argument;
	argument.modhash = ModHashFromAddr(moduleBase);
	argument.start = Start - moduleBase;
	argument.end = End - moduleBase;
	argument.manual = Manual;
	argument.instructioncount = InstructionCount;

	return arguments.Add(argument);
}