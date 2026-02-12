#pragma once

/**
 * 현재 랜더 스레드에서 참고하는 현재 공간 객체
 */
class Scene
{
	// 정점 및 메테리얼 조합 정보들 <- 환경 데이터, Actor의 정점 데이터 등등 -- 노!!! 컴포넌트만
	// 조명 정보들
	// 간접 조명 연산 정보들

public:
	float GetPreRemainTime() const
	{
		return _mPreRemainTime;
	}
	void SetPreRemainTime(float remainTime)
	{
		_mPreRemainTime = remainTime;
	}

public:
	float GetAccRenderDeltaTime() const
	{
		return _mAccRenderDeltaTime;
	}
	void SetAccRenderDeltaTime(float remainTime)
	{
		_mAccRenderDeltaTime = remainTime;
	}

private:
	// 이전 씬 캡처 드로잉 시, 보간 처리하고 남은 델타 타임
	float _mPreRemainTime = 0.f;
	// 그려지지 않은 랜더링 델타 누적 시간
	float _mAccRenderDeltaTime = 0.f;
};

