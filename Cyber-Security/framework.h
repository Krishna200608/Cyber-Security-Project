#pragma once

// --- Start of Fix ---
// This section DEFINITIVELY sets the target Windows version.
// It targets Windows 7 and later, which defines OFN_OVERWRITEREPROMPT.
#include <winsdkver.h>
#define _WIN32_WINNT 0x0601
#include <sdkddkver.h>
// --- End of Fix ---

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>