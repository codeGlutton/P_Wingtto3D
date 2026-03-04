#include "pch.h"
#include "DXDefaultVertexData.h"

#include "Manager/DXGraphicSystem.h"

#include "Graphics/Resource/DXInputLayout.h"

const DXInputElementList& NullVertexData::MakeLayout()
{
	static DXInputElementList inputElements = DXInputElementList::Make();

	return inputElements;
}

const DXInputElementList& UIVertexData::MakeLayout()
{
	static DXInputElementList inputElements = DXInputElementList::Make()
		.Push3Float("Position", 0)
		.Push2Float("UV", 0)
		.Push4Float("Tint", 0);

	return inputElements;
}

const DXInputElementList& StaticVertexData::MakeLayout()
{
	static DXInputElementList inputElements = DXInputElementList::Make()
		.Push3Float("Position", 0)
		.Push2Float("UV", 0)
		.Push3Float("Normal", 0)
		.Push3Float("Tangent", 0);

	return inputElements;
}

const DXInputElementList& SkeletalVertexData::MakeLayout()
{
	static DXInputElementList inputElements = DXInputElementList::Make()
		.Push3Float("Position", 0)
		.Push2Float("UV", 0)
		.Push3Float("Normal", 0)
		.Push3Float("Tangent", 0)
		.Push4Float("BlendIndices", 0)
		.Push4Float("BlendWeights", 0);

	return inputElements;
}
