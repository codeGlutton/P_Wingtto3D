#include "pch.h"
#include "WidgetGeometry.h"

#include "Graphics/Widget/Widget.h"
#include "Graphics/Widget/Type/WidgetBrush.h"

WidgetGeometry::WidgetGeometry() :
	mIsRenderCalculated(false)
{
}

WidgetGeometry::WidgetGeometry(const Vec2& boxSize, const Transform2D& layoutTransform) :
	mBoxSize(boxSize),
	mLocalPos(layoutTransform.GetLocalPosition()),
	mLocalSize(layoutTransform.GetLocalScale()),
	mMatRenderLocalToRoot(layoutTransform.MakeLocalMatrix()),
	mIsRenderCalculated(true)
{
	mMatRenderLocalToRoot.Decompose(mRootSize, mRootPos);
}

WidgetGeometry::WidgetGeometry(
	const Vec2& boxSize,
	const Transform2D& layoutTransform,
	const Transform2D& renderTransform,
	const Vec2& renderTransformPivot,
	const Matrix2D& parentAccLayoutMat,
	const Matrix2D& parentAccRenderMat) :
	mBoxSize(boxSize),
	mLocalPos(layoutTransform.GetLocalPosition()),
	mLocalSize(layoutTransform.GetLocalScale()),
	mIsRenderCalculated(true)
{
	Matrix2D matLocalLayout = layoutTransform.MakeLocalSTMatrix();

	mMatRenderLocalToRoot =
		Matrix2D::CreateTranslation(boxSize * -renderTransformPivot) *
		renderTransform.MakeLocalMatrix() *
		Matrix2D::CreateTranslation(boxSize * renderTransformPivot) *
		matLocalLayout *
		parentAccRenderMat;

	Matrix2D matAccLayout = matLocalLayout * parentAccLayoutMat;
	matAccLayout.Decompose(mRootSize, mRootPos);
}

WidgetGeometry WidgetGeometry::MakeRoot(const Vec2& boxSize, const Transform2D& layoutTransform)
{
	return WidgetGeometry(boxSize, layoutTransform);
}

WidgetGeometry WidgetGeometry::MakeChild(const Vec2& boxSize, const Transform2D& layoutTransform) const
{
	return WidgetGeometry(boxSize, layoutTransform, Transform2D(), Vec2(0.5f, 0.5f), GetAccLayoutMatrix2D(), GetAccRenderMatrix2D());
}

WidgetGeometry WidgetGeometry::MakeChild(const Vec2& boxSize, const Transform2D& layoutTransform, const Transform2D& renderTransform, const Vec2& renderTransformPivot) const
{
	return WidgetGeometry(boxSize, layoutTransform, renderTransform, renderTransformPivot, GetAccLayoutMatrix2D(), GetAccRenderMatrix2D());
}

WidgetGeometry WidgetGeometry::MakeChild(std::shared_ptr<const Widget> childWidget, const Vec2& boxSize, const Transform2D& layoutTransform) const
{
	return MakeChild(boxSize, layoutTransform, childWidget->GetRenderTransform(), childWidget->GetRenderTransformPivot());
}

void WidgetGeometry::AppendTransform(const Transform2D& layoutTransform)
{
	Matrix2D localLayoutMat = layoutTransform.MakeLocalSTMatrix();

	mMatRenderLocalToRoot = mMatRenderLocalToRoot * localLayoutMat;
	localLayoutMat = GetAccLayoutMatrix2D() * localLayoutMat;
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
	return BoundingAABB2D(
		static_cast<long>(mRootPos.x), 
		static_cast<long>(mRootPos.y), 
		static_cast<long>(mRootSize.x * mBoxSize.x),
		static_cast<long>(mRootSize.y * mBoxSize.y)
	);
}

BoundingAABB2D WidgetGeometry::GetRenderBoundingBox() const
{
	Matrix mat = mMatRenderLocalToRoot;
	Vec3 corners[4] =
	{
		{0, 0, 0},
		{mBoxSize.x, 0, 0},
		{mBoxSize.x, mBoxSize.y, 0},
		{0, mBoxSize.y, 0}
	};
	
	long minX = LONG_MAX, maxX = LONG_MIN, minY = LONG_MAX, maxY = LONG_MIN;
	for (auto& corner : corners)
	{
		Vec3 transformed = Vec3::Transform(corner, mMatRenderLocalToRoot);

		minX = std::min<long>(minX, static_cast<long>(transformed.x));
		maxX = std::max<long>(maxX, static_cast<long>(transformed.x));
		minY = std::min<long>(minY, static_cast<long>(transformed.y));
		maxY = std::max<long>(maxY, static_cast<long>(transformed.y));
	}
	return BoundingAABB2D(minX, minY, maxX - minX, maxY - minY);
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
