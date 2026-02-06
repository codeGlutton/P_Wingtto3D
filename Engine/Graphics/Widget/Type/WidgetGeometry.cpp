#include "pch.h"
#include "WidgetGeometry.h"

#include "Graphics/Widget/Widget.h"
#include "Graphics/Widget/Type/WidgetBrush.h"

WidgetGeometry::WidgetGeometry() :
	mIsRenderCalculated(false)
{
}

WidgetGeometry::WidgetGeometry(const Vec2& boxSize, const Transform2D& layoutTransform, const Transform2D& renderTransform) :
	mBoxSize(boxSize),
	mLocalPos(layoutTransform.GetLocalPosition()),
	mRootPos(mLocalPos),
	mLocalSize(layoutTransform.GetLocalScale()),
	mRootSize(mLocalSize),
	mMatRenderLocalToRoot(renderTransform.MakeLocalMatrix()),
	mIsRenderCalculated(true)
{
}

WidgetGeometry::WidgetGeometry(
	const Vec2& boxSize,
	const Transform2D& layoutTransform,
	const Transform2D& renderTransform,
	const Matrix2D& parentAccLayoutMat,
	const Matrix2D& parentAccRenderMat) :
	mBoxSize(boxSize),
	mLocalPos(layoutTransform.GetLocalPosition()),
	mLocalSize(layoutTransform.GetLocalScale()),
	mMatRenderLocalToRoot(renderTransform.MakeLocalMatrix() * parentAccRenderMat),
	mIsRenderCalculated(true)
{
	Matrix2D matLayoutLocalToRoot = Transform2D(mLocalPos, 0.f, mLocalSize).MakeLocalSTMatrix() * parentAccLayoutMat;
	matLayoutLocalToRoot.Decompose(mRootSize, mRootPos);
}

WidgetGeometry WidgetGeometry::MakeRoot(const Vec2& boxSize, const Transform2D& layoutTransform)
{
	return WidgetGeometry(boxSize, layoutTransform, Transform2D());
}

WidgetGeometry WidgetGeometry::MakeChild(const Vec2& boxSize, const Transform2D& layoutTransform, const Transform2D& renderTransform) const
{
	return WidgetGeometry(boxSize, layoutTransform, renderTransform, GetAccLayoutMatrix2D(), GetAccRenderMatrix2D());
}

WidgetGeometry WidgetGeometry::MakeChild(std::shared_ptr<const Widget> childWidget, const Vec2& boxSize, const Transform2D& layoutTransform) const
{
	Matrix2D localRenderMat = childWidget->GetRenderTransform().MakeLocalMatrix();
	return MakeChild(boxSize, layoutTransform, localRenderMat);
}

void WidgetGeometry::AppendTransform(const Transform2D& layoutTransform)
{
	Matrix2D localLayoutMat = layoutTransform.MakeLocalSTMatrix();

	mMatRenderLocalToRoot = localLayoutMat * mMatRenderLocalToRoot;
	localLayoutMat = localLayoutMat * GetAccLayoutMatrix2D();
	localLayoutMat.Decompose(mRootSize, mRootPos);
}

Matrix2D WidgetGeometry::GetAccLayoutMatrix2D() const
{
	return Transform2D(mRootPos, 0.f, mRootSize).MakeLocalSTMatrix();
}

const Matrix2D& WidgetGeometry::GetAccRenderMatrix2D() const
{
	return mMatRenderLocalToRoot;
}

BoundingAABB2D WidgetGeometry::GetLayoutBoundingBox() const
{
	return BoundingAABB2D(static_cast<long>(mRootPos.x), static_cast<long>(mRootPos.y), static_cast<long>(mRootSize.x * mBoxSize.x), static_cast<long>(mRootSize.y * mBoxSize.y));
}

BoundingAABB2D WidgetGeometry::GetRenderBoundingBox() const
{
	return BoundingOBB2D(
		static_cast<long>(mRootPos.x), static_cast<long>(mRootPos.y),
		mMatRenderLocalToRoot.Right(), mMatRenderLocalToRoot.Up(),
		static_cast<long>(mRootSize.x * mBoxSize.x / 2.f), static_cast<long>(mRootSize.y * mBoxSize.y / 2.f)
	).ToBoundingAABB2D();
}

BoundingAABB2D WidgetGeometry::GetRenderBoundingBox(const Margin& extentMargin) const
{
	BoundingAABB2D aabb = GetRenderBoundingBox();
	return BoundingAABB2D(
		aabb.x - static_cast<long>(extentMargin.mLeft),
		aabb.y - static_cast<long>(extentMargin.mUp),
		aabb.width + static_cast<long>(extentMargin.mRight + extentMargin.mLeft),
		aabb.height + static_cast<long>(extentMargin.mDown + extentMargin.mUp)
	);
}
