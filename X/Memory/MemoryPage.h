#ifndef MEMORYPAGE_H
#define MEMORYPAGE_H

#include "Imports.h"

class MemoryPage
{
public:
	explicit MemoryPage(duint parBase, duint parSize);

	bool read(void* parDest, dsint parRVA, duint parSize) const;
	bool read(byte_t* parDest, dsint parRVA, duint parSize) const;
	bool write(const void* parDest, dsint parRVA, duint parSize);
	bool write(const byte_t* parDest, dsint parRVA, duint parSize);
	duint getSize() const;
	duint getBase() const;
	duint va(dsint rva) const;
	void setAttributes(duint base, duint size);
	bool inRange(duint va) const;

private:
	duint mBase;
	duint mSize;
};

#endif // MEMORYPAGE_H