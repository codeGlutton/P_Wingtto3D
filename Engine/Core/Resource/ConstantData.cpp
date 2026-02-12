#include "pch.h"
#include "ConstantData.h"

#include "Manager/ResourceManager.h"

ConstantDataBase::ConstantDataBase()
{
	_mBulkData = std::make_shared<ConstantBufferBulkData>();
}

ConstantDataBase::~ConstantDataBase()
{
}

