#include "pch.h"
#include "GeometryUtils.h"

void CreateQuad(OUT std::vector<UIVertexData>& vertices, OUT std::vector<uint32>& indices, float left, float right, float up, float down, Color color)
{
	{
		vertices.resize(4);

		/* 좌표는 ^>, UV는 v> */

		vertices[0].mPosition = Vec3(left, up, 0.f);
		vertices[0].mUV = { 0.f, 1.f };
		vertices[0].mTint = color;
		vertices[1].mPosition = Vec3(left, down, 0.f);
		vertices[1].mUV = { 0.f, 0.f };
		vertices[1].mTint = color;
		vertices[2].mPosition = Vec3(right, up, 0.f);
		vertices[2].mUV = { 1.f, 1.f };
		vertices[2].mTint = color;
		vertices[3].mPosition = Vec3(right, down, 0.f);
		vertices[3].mUV = { 1.f, 0.f };
		vertices[3].mTint = color;
	}

	{
		indices.resize(6);

		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;
		indices[3] = 2;
		indices[4] = 1;
		indices[5] = 3;
	}
}

void PushQuad(OUT std::vector<UIVertexData>& vertices, OUT std::vector<uint32>& indices, float left, float right, float up, float down, Color color)
{
	std::size_t preVBSize = vertices.size();

	{
		/* 좌표는 ^>, UV는 v> */

		vertices.push_back(UIVertexData{ Vec3(left, up, 0.f), { 0.f, 1.f }, color });
		vertices.push_back(UIVertexData{ Vec3(left, down, 0.f), { 0.f, 0.f }, color });
		vertices.push_back(UIVertexData{ Vec3(right, up, 0.f), { 1.f, 1.f }, color });
		vertices.push_back(UIVertexData{ Vec3(right, down, 0.f), { 1.f, 0.f }, color });
	}

	{
		indices.push_back(static_cast<uint32>(preVBSize + 0));
		indices.push_back(static_cast<uint32>(preVBSize + 1));
		indices.push_back(static_cast<uint32>(preVBSize + 2));
		indices.push_back(static_cast<uint32>(preVBSize + 2));
		indices.push_back(static_cast<uint32>(preVBSize + 1));
		indices.push_back(static_cast<uint32>(preVBSize + 3));
	}
}

void CreateQuad(OUT std::vector<StaticVertexData>& vertices, OUT std::vector<uint32>& indices)
{
	{
		vertices.resize(4);

		/* 좌표는 ^>, UV는 v> */

		vertices[0].mPosition = Vec3(-0.5f, -0.5f, 0.f);
		vertices[0].mUV = { 0.f, 1.f };
		vertices[1].mPosition = Vec3(-0.5f, 0.5f, 0.f);
		vertices[1].mUV = { 0.f, 0.f };
		vertices[2].mPosition = Vec3(0.5f, -0.5f, 0.f);
		vertices[2].mUV = { 1.f, 1.f };
		vertices[3].mPosition = Vec3(0.5f, 0.5f, 0.f);
		vertices[3].mUV = { 1.f, 0.f };
	}

	{
		indices.resize(6);

		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;
		indices[3] = 2;
		indices[4] = 1;
		indices[5] = 3;
	}
}

void CreateCube(OUT std::vector<StaticVertexData>& vertices, OUT std::vector<uint32>& indices)
{

}

void CreateSphere(OUT std::vector<StaticVertexData>& vertices, OUT std::vector<uint32>& indices, uint32 stackCount, uint32 sliceCount)
{

}

void CreateGrid(OUT std::vector<StaticVertexData>& vertices, OUT std::vector<uint32>& indices, uint32 columnCount, uint32 rowCount)
{

}
