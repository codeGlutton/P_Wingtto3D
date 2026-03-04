#pragma once

#include "Graphics/DXDefaultVertexData.h"

/**
 * 정사각형을 만드는 함수
 * \param geometry 결과를 담을 Geometry
 * \param color 색
 */
void CreateQuad(OUT std::vector<UIVertexData>& vertices, OUT std::vector<uint32>& indices, float left = -0.5f, float right = 0.5f, float up = 0.5f, float down = -0.5f, Color color = Color::White);
void PushQuad(OUT std::vector<UIVertexData>& vertices, OUT std::vector<uint32>& indices, float left = -0.5f, float right = 0.5f, float up = 0.5f, float down = -0.5f, Color color = Color::White);
void PushQuad(
	OUT std::vector<UIVertexData>& vertices, 
	OUT std::vector<uint32>& indices, 
	float left = -0.5f, 
	float right = 0.5f, 
	float up = 0.5f, 
	float down = -0.5f,
	Vec2 uvStart = Vec2(0.f),
	Vec2 uvSize = Vec2(1.f),
	Color color = Color::White
);

/**
 * 정사각형을 만드는 함수
 * \param geometry 결과를 담을 Geometry
 */
void CreateQuad(OUT std::vector<StaticVertexData>& vertices, OUT std::vector<uint32>& indices);
/**
 * 큐브를 만드는 함수
 * \param geometry 결과를 담을 Geometry
 */
void CreateCube(OUT std::vector<StaticVertexData>& vertices, OUT std::vector<uint32>& indices);
/**
 * 구체를 만드는 함수
 * \param geometry 결과를 담을 Geometry
 * \param stackCount 가로선 디테일 갯수
 * \param sliceCount 세로선 디테일 갯수
 */
void CreateSphere(OUT std::vector<StaticVertexData>& vertices, OUT std::vector<uint32>& indices, uint32 stackCount = 20, uint32 sliceCount = 20);
/**
 * 그리드를 만드는 함수
 * \param geometry 결과를 담을 Geometry
 * \param columnCount 가로선 갯수
 * \param rowCount 세로선 갯수
 */
void CreateGrid(OUT std::vector<StaticVertexData>& vertices, OUT std::vector<uint32>& indices, uint32 columnCount = 256, uint32 rowCount = 256);

