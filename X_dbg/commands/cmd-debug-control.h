#pragma once

#include "command.h"

bool cbDebugRunInternal(int argc, char* argv[]);
bool cbDebugInit(int argc, char* argv[]);
bool cbDebugStop(int argc, char* argv[]);
bool cbDebugRun(int argc, char* argv[]);
bool cbDebugSkip(int argc, char* argv[]);
bool cbDebugStepOver(int argc, char* argv[]);
bool cbDebugStepInto(int argc, char* argv[]);
bool cbDebugPause(int argc, char* argv[]);