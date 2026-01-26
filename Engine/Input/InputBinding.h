#pragma once

#include "Input/InputInclude.h"
#include "Input/InputValue.h"

namespace KeyState
{
	enum Type : uint8
	{
		None = 0xFF,
		Press = 0,
		Hold,
		Release,
		Trigger,
		Count
	};
}

DECLARE_DELEGATE_2_PARAMS(OnCallInputBinding, const InputValue*, KeyState::Type);

struct InputBinding
{
	GEN_STRUCT_REFLECTION(InputBinding)

public:
	bool operator==(const InputBinding& other) const
	{
		return mKeyType == other.mKeyType && 
			mCtrl == other.mCtrl && 
			mAlt == other.mAlt && 
			mShift == other.mShift;
	}

public:
	uint8 GetPriority(bool ctrl, bool alt, bool shift) const
	{
		return (ctrl == mCtrl) + (alt == mAlt) + (shift == mShift);
	}

public:
	PROPERTY(mKeyType)
	KeyType::Type mKeyType = KeyType::None;
	PROPERTY(mCtrl)
	bool mCtrl = false;
	PROPERTY(mAlt)
	bool mAlt = false;
	PROPERTY(mShift)
	bool mShift = false;

public:
	OnCallInputBinding mOnCallInputBinding;
};

// 매니저는 키 타입 배열에 등록된 대상 및 컨트롤, 알트, 쉬프트에 한해서 정기적 업데이트 실시
// 게임패드는 XInput 검사, 다른 부분은 DInput 혹은 Win Input 검사
// 해당 키를 사용하는 대상에 찾아가서 상황이 적합한 경우 콜백 실행			-> 일반적 엔진 키 바인딩에 쓸 예정
// 액션의 경우는 바인딩되어 중간 과정에서 이벤트를 필터링하고 콜백 실행		-> 게임 내
// 매핑 컨텍스트의 경우, 등록 및 제거 시 최상단 대상을 검사하고 바인딩 맵과 액션 맵 수정

