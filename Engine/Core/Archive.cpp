#include "pch.h"
#include "Archive.h"

Archive::Archive(std::string filePath) :
	_mStream(filePath, std::ios::in | std::ios::out | std::ios::binary)
{
	_mStream.open(filePath);
	ASSERT_MSG(_mStream.is_open() == true, "File path is wrong");
}

ObjectGUID Archive::ConvertGUID(const Object* object) const
{
	return _mGUIDFinder(object);
}

