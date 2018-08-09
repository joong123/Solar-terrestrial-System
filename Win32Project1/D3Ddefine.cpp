#include"stdafx.h"
#include"D3Ddefine.h"

void CreateSphere(ID3DXMesh ** obj, int finess, float radius, D3DCOLOR color, float height)
{
	//球形
	const int loops = 4;//球精细度（180度内的环数）
	float loopradian = (float)(MYPI / loops);
	int point = 0;
	const int pointcount = 2 + (loops - 1)*loops * 2;
	//顶点序列
	if (radius <= 0)
		radius = 1;
	float top = height + radius;
	CUSTOMVERTEX1 g_vertices[pointcount] =
	{
		{ D3DXVECTOR3(0, 0, 0),
		D3DXVECTOR3(0.0f, 0.0f, -1.0f), color }
	};
	//三角索引序列
	const int indexcount = (2 * loops + 2 * (loops - 2)*loops) * 3 * 2;
	WORD index[indexcount] =
	{
		0
	};


	//顶点和索引列表赋值
	g_vertices[0] = { D3DXVECTOR3(0.0f, 0.0f, -top),//顶点
		D3DXVECTOR3(0.0f, 0.0f, -1.0f), color };
	float hradian = 0.0f;
	float vradian = loopradian;
	int loop = 1;
	int face = 1;
	for (face = 1, point = 1; point <= 2 * loops; face++, point++)//第一环
	{
		hradian = loopradian*(point - 1);
		g_vertices[point].pos = D3DXVECTOR3(
			(float)(radius*cos(hradian)*sin(vradian))
			, (float)(radius*sin(hradian)*sin(vradian))
			, (float)(-height - radius*cos(vradian))
		);
		g_vertices[point].normal
			= D3DXVECTOR3((float)(cos(hradian)*sin(vradian)), (float)(sin(hradian)*sin(vradian)), (float)(-cos(vradian)));
		g_vertices[point].color = color;

		index[(point - 1) * 3] = 0;
		if (point == loops * 2)
			index[(point - 1) * 3 + 1] = 1;
		else
			index[(point - 1) * 3 + 1] = point + 1;
		index[(point - 1) * 3 + 2] = point;
	}
	for (loop = 2; loop < loops; loop++)//第二环开始
	{
		vradian = loop*loopradian;
		int startpoint = (loop - 1)*loops * 2 + 1;
		for (point = startpoint; point <= loop*loops * 2; point++)
		{
			hradian = loopradian*(point - startpoint);
			g_vertices[point].pos= D3DXVECTOR3(
				(float)(radius*cos(hradian)*sin(vradian))
				, (float)(radius*sin(hradian)*sin(vradian))
				, (float)(-height - radius*cos(vradian))
			);
			g_vertices[point].normal = D3DXVECTOR3(
				(float)(cos(hradian)*sin(vradian))
				, (float)(sin(hradian)*sin(vradian))
				, (float)(-cos(vradian))
			);
			g_vertices[point].color = color;

			index[(face - 1) * 3] = point - loops * 2;
			if (point == loop*loops * 2)
				index[(face - 1) * 3 + 1] = startpoint;
			else
				index[(face - 1) * 3 + 1] = point + 1;
			index[(face - 1) * 3 + 2] = point;
			face++;

			index[(face - 1) * 3] = point - loops * 2;
			if (point == loop*loops * 2)
			{
				index[(face - 1) * 3 + 1] = point - 2 * loops * 2 + 1;
				index[(face - 1) * 3 + 2] = startpoint;
			}
			else
			{
				index[(face - 1) * 3 + 1] = point - loops * 2 + 1;
				index[(face - 1) * 3 + 2] = point + 1;
			}
			face++;
		}
	}
	//最后一点
	g_vertices[point].pos = D3DXVECTOR3(0.0f, 0.0f, -height + radius);
	g_vertices[point].normal = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	int lastpoint = point - loops * 2;
	for (; face <= indexcount / 3; face++, lastpoint++)
	{
		index[(face - 1) * 3] = lastpoint;
		if (face == indexcount / 3)
		{
			index[(face - 1) * 3 + 1] = point - loops * 2;
		}
		else
		{
			index[(face - 1) * 3 + 1] = lastpoint + 1;
		}
		index[(face - 1) * 3 + 2] = point;
	}
	g_vertices[point].color = color;

	void* pVertices = NULL;
	void* pIndex = NULL;
	HRESULT hr;

	if (*obj)
		(*obj)->Release();
	//mesh方式
	hr = D3DXCreateMeshFVF(indexcount / 3, pointcount, D3DXMESH_MANAGED,
		FVF_CUSTOM1,
		device,
		obj);
	(*obj)->LockVertexBuffer(0, &pVertices);
	memcpy(pVertices, g_vertices, sizeof(g_vertices));
	(*obj)->UnlockVertexBuffer();

	(*obj)->LockIndexBuffer(0, &pIndex);
	memcpy(pIndex, index, sizeof(index));
	(*obj)->UnlockIndexBuffer();
}
