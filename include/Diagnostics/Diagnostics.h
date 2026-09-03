#pragma once

#ifdef WIN32
#include <DbgHelp.h>
#pragma comment(lib, "dbghelp.lib")
#endif

#include "Process.h"
#include "ThisProcess.h"
