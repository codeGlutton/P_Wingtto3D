#pragma once

#include "Core/Resource/Resource.h"
#include "Core/GUID.h"

struct LevelActorDesc
{
	ObjectGUID mGUID;
	std::weak_ptr<TypeInfo> mTypeInfo;
};

/**
 * 월드 내 공간 저장 단위
 */
class Level : public Resource
{
	GEN_REFLECTION(Level)

	// 레벨은 저장단위, 
	// 이러이러한 액터 인스턴스들이 이 클래스로 존재하고, 이 순서대로 저장할께
	// GUID와 클래스는 실시간으로 업데이트되고, 저장시에는 월드 액터를 참고할께
	// 로드시에는 map 파일로 가져올건데, 월드가 열어서 레벨 데이터에 재배치해줄꺼야.

private:
	std::vector<LevelActorDesc> _mActorDescs;
};

