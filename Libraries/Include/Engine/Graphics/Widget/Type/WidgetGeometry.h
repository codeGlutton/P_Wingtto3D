#pragma once

#include "Core/Transform.h"

class Widget;
struct Margin;

class WidgetGeometry
{
public:
	WidgetGeometry();
	WidgetGeometry(const WidgetGeometry& other) = default;

private:
	WidgetGeometry(const Vec2& boxSize, const Transform2D& layoutTransform, const Transform2D& renderTransform);
	WidgetGeometry(const Vec2& boxSize, const Transform2D& layoutTransform, const Transform2D& renderTransform, const Matrix2D& parentAccLayoutMat, const Matrix2D& parentAccRenderMat);

public:
	static WidgetGeometry MakeRoot(const Vec2& boxSize, const Transform2D& layoutTransform);

public:
	WidgetGeometry MakeChild(const Vec2& boxSize, const Transform2D& layoutTransform, const Transform2D& renderTransform) const;
	WidgetGeometry MakeChild(std::shared_ptr<const Widget> childWidget, const Vec2& boxSize, const Transform2D& layoutTransform) const;

public:
	void AppendTransform(const Transform2D& layoutTransform);

public:
	Matrix2D GetAccLayoutMatrix2D() const;
	const Matrix2D& GetAccRenderMatrix2D() const;

	BoundingAABB2D GetLayoutBoundingBox() const;

	BoundingAABB2D GetRenderBoundingBox() const;
	BoundingAABB2D GetRenderBoundingBox(const Margin& extentMargin) const;

	/* 기하 정보 */
public:
	Vec2 mBoxSize;

	/* 레이아웃 데이터 */
public:
	Vec2 mLocalPos;
	Vec2 mRootPos;

	Vec2 mLocalSize;
	Vec2 mRootSize;

	/* 최종 렌더 과정에 의해 결정되는 랜더링 데이터 */
public:
	// 최상위 좌표계로 변환하는 레이아웃 및 렌더 최종 결과 행렬
	Matrix2D mMatRenderLocalToRoot;
	// Paint 과정을 거쳐 최종 랜더 행렬이 계산되었는가
	bool mIsRenderCalculated = false;
};