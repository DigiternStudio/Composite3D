#include "FileSystem.h"

#include <fstream>

// @TODO: delete extra code


NAMESPACE_BEGIN(cm3d)
NAMESPACE_BEGIN(FileSystem)


bool isCorrectName(const char *name)
{
    for (char c; (c = *name); ++name)
    {
        switch(c)
        {
            case '\\':
            case '/':
            case '<':
            case '>':
            case '?':
            case '*':
            case '|':
            case '\"':
                return false;
        }
    }
    return true;
}

bool isCorrectPath(const char *path)
{
	for (char c; (c = *path); ++path)
    {
        switch(c)
        {
            case '<':
            case '>':
            case '?':
            case '*':
            case '|':
            case '\"':
                return false;
        }
    }
    return true;
}

cmLen getSize(const char *path)
{
	std::ifstream rs(path, std::ifstream::binary);
	if (rs.fail())
		return -1LL;
	rs.seekg(0LL, rs.end);
	return rs.tellg();
}

/*
cmLen readBinary(const char *path, cmByte *buffer, size_t len, cmLen pos)
{
	std::ifstream rs(path, std::ifstream::binary);
	if (rs.fail())
		return -1LL;
	rs.seekg(pos);
	auto state = rs.rdstate();
	if (state & std::ifstream::eofbit)
		return -2LL;
	if (state & std::ifstream::badbit)
		return -4LL;
	rs.read((char*)buffer, len);
	if (rs.bad())
		return -8LL;

	return rs.gcount();
}

cmLen readText(const char *path, char *buffer, size_t len, cmLen pos)
{
	std::ifstream rs(path, std::ifstream::in);
	if (rs.fail())
		return -1LL;
	rs.seekg(pos);
	auto state = rs.rdstate();
	if (state & std::ifstream::eofbit)
		return -2LL;
	if (state & std::ifstream::badbit)
		return -4LL;
	rs.read((char*)buffer, len);
	if (rs.bad())
		return -8LL;

	return rs.gcount();
}
*/

NAMESPACE_END(FileSystem)
NAMESPACE_END(cm3d)
