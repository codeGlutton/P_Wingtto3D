#pragma once

#include "Core/Resource/Resource.h"
#include "Graphics/Resource/DXTexture.h"

class Texture2D : public Resource
{
	GEN_REFLECTION(Texture2D)

public:
	Texture2D();
	~Texture2D();

protected:
	virtual void PostLoad() override;

private:
	PROPERTY(_mBulkData)
	std::shared_ptr<Texture2DBulkData> _mBulkData;
	std::shared_ptr<DXTexture2D> _mProxy;
};

