#ifndef CM3D_FILE_SYSTEM
#define CM3D_FILE_SYSTEM

#include "cmcommon.h"
#include <string>

// @TODO: delete extra code

/*
#if __cplusplus >= 201703L
#  include <filesystem>
#else
#  include <experimental/filesystem>
#endif
*/

NAMESPACE_BEGIN(cm3d)
NAMESPACE_BEGIN(FileSystem)

/*
using namespace std
#if __cplusplus < 201703L
	::experimental
#endif
	::filesystem;
*/

bool isCorrectName(const char *name);

bool isCorrectPath(const char *path);

cmLen getSize(const char *path);
/*
cmLen readBinary(const char *path, cmByte *buffer, size_t len, cmLen pos = 0LL);

cmLen readText(const char *path, char *buffer, size_t len, cmLen pos = 0LL);
*/

NAMESPACE_END(FileSystem)
NAMESPACE_END(cm3d)

#endif
