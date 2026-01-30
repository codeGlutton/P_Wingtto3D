#include "pch.h"
#include "Texture.h"

Texture2D::Texture2D()
{
	_mBulkData = std::make_shared<Texture2DBulkData>();
}

Texture2D::~Texture2D()
{
}

void Texture2D::PostLoad()
{
	Super::PostLoad();
	_mProxy = std::make_shared<DXTexture2D>();
	
	// 랜더 스레드에 넘겨주기 
	// _mProxy->Init(_mBulkData);
}

