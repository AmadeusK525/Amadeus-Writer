#ifndef _WX_MEMDBG_H_
#define _WX_MEMDBG_H_

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
#else
#define DEBUG_NEW new
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif 

#endif // _WX_MEMDBG_H_
