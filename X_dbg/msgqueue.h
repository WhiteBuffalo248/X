#ifndef _MSGQUEUE_H
#define _MSGQUEUE_H
#include "_global.h"
#include <agents.h>

// Message structure
struct MESSAGE
{
	int msg;
	duint param1;
	duint param2;
};

// Message stack structure
class MESSAGE_STACK
{
public:
	Concurrency::unbounded_buffer<MESSAGE> msgs;

	int WaitingCalls;   // Number of threads waiting
	bool Destroy;       // Destroy stack as soon as possible
};

// Function definitions
MESSAGE_STACK* MsgAllocStack();
void MsgFreeStack(MESSAGE_STACK* Stack);
void MsgWait(MESSAGE_STACK* Stack, MESSAGE* Msg);
bool MsgSend(MESSAGE_STACK* Stack, int Msg, duint Param1, duint Param2);

#endif // _MSGQUEUE_H