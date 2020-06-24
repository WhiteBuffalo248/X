
#include "bookmark.h"


struct BookmarkSerializer : AddrInfoSerializer<BOOKMARKSINFO>
{
};

struct Bookmarks : AddrInfoHashMap<LockBookmarks, BOOKMARKSINFO, BookmarkSerializer>
{
	const char* jsonKey() const override
	{
		return "bookmarks";
	}
};

static Bookmarks bookmarks;

bool BookmarkSet(duint Address, bool Manual)
{
	BOOKMARKSINFO bookmark;
	if (!bookmarks.PrepareValue(bookmark, Address, Manual))
		return false;
	auto key = Bookmarks::VaKey(Address);
	if (bookmarks.Contains(key))
		return bookmarks.Delete(key);
	return bookmarks.Add(bookmark);
}

bool BookmarkGet(duint Address)
{
	return bookmarks.Contains(Bookmarks::VaKey(Address));
}

void BookmarkCacheSave(JSON Root)
{
	bookmarks.CacheSave(Root);
}

void BookmarkCacheLoad(JSON Root)
{
	bookmarks.CacheLoad(Root);
	bookmarks.CacheLoad(Root, "auto"); //legacy support
}

void BookmarkClear()
{
	bookmarks.Clear();
}

void BookmarkDelRange(duint Start, duint End, bool Manual)
{
	bookmarks.DeleteRange(Start, End, Manual);
}