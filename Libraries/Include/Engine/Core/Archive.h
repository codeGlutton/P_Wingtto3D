#pragma once

#include <fstream>

#include "Core/GUID.h"

class Archive
{
public:
	Archive(std::string filePath);

public:
	template<typename T>
	Archive& operator<<(T&& inst);

	//Archive& Read(OUT void* object);
	//Archive& Read(OUT void* object);
	//Archive& Read(OUT void* object);

public:
	ObjectGUID ConvertGUID(const Object* object) const;

private:
	std::fstream _mStream;
	std::function<ObjectGUID(const Object*)> _mGUIDFinder;
};

template<typename T>
inline Archive& Archive::operator<<(T&& inst)
{
	_mStream.write(
		reinterpret_cast<const char*>(&inst), 
		sizeof(std::remove_reference_t<T>)
	);

	return *this;
}


