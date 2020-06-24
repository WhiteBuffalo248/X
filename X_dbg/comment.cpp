
#include "comment.h"

struct CommentSerializer : AddrInfoSerializer<COMMENTSINFO>
{
	bool Save(const COMMENTSINFO & value) override
	{
		AddrInfoSerializer::Save(value);
		setString("text", value.text);
		return true;
	}

	bool Load(COMMENTSINFO & value) override
	{
		return AddrInfoSerializer::Load(value) &&
			getString("text", value.text);
	}
};

struct Comments : AddrInfoHashMap<LockComments, COMMENTSINFO, CommentSerializer>
{
	const char* jsonKey() const override
	{
		return "comments";
	}
};

static Comments comments;

bool CommentGet(duint Address, char* Text)
{
	COMMENTSINFO comment;
	if (!comments.Get(Comments::VaKey(Address), comment))
		return false;
	if (comment.manual)
		strcpy_s(Text, MAX_COMMENT_SIZE, comment.text.c_str());
	else
		sprintf_s(Text, MAX_COMMENT_SIZE, "\1%s", comment.text.c_str());
	return true;
}

void CommentCacheSave(JSON Root)
{
	comments.CacheSave(Root);
}

void CommentCacheLoad(JSON Root)
{
	comments.CacheLoad(Root);
	comments.CacheLoad(Root, "auto"); //legacy support
}

void CommentClear()
{
	comments.Clear();
}

bool CommentSet(duint Address, const char* Text, bool Manual)
{
	// Make sure the string is supplied, within bounds, and not a special delimiter
	if (!Text || Text[0] == '\1' || strlen(Text) >= MAX_COMMENT_SIZE - 1)
		return false;
	// Delete the comment if no text was supplied
	if (Text[0] == '\0')
	{
		CommentDelete(Address);
		return true;
	}
	// Fill in the structure + add to database
	COMMENTSINFO comment;
	if (!comments.PrepareValue(comment, Address, Manual))
		return false;
	comment.text = Text;
	return comments.Add(comment);
}

bool CommentDelete(duint Address)
{
	return comments.Delete(Comments::VaKey(Address));
}

void CommentDelRange(duint Start, duint End, bool Manual)
{
	comments.DeleteRange(Start, End, Manual);
}