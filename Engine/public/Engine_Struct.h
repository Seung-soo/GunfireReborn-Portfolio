#pragma once

namespace Engine
{
	typedef struct tagKeyFrame
	{
		_float3		vScale;
		_float4		vRotation;
		_float3		vPosition;
		_double		Time;		// 적용되어야 하는 시간
	}KEYFRAME;

	typedef struct tagMeshMaterialDesc
	{
		class CTexture* pTexture[AI_TEXTURE_TYPE_MAX];
	}MESHMATERIALDESC;
	typedef struct tagLightDesc
	{
		enum TYPE { TYPE_DIRECTTIONAL, TYPE_POINT, TYPE_POINTCHASE, TYPE_END };

		TYPE		eLightType = TYPE_END;
		XMFLOAT4	vDiffuse;
		XMFLOAT4	vAmbient;
		XMFLOAT4	vSpecular;
		XMFLOAT4	vDirection;
		XMFLOAT4	vPosition;
		XMFLOAT4*	pPosition = nullptr;
		bool*		pDead = nullptr;
		float		fRange = 0.f;
	}LIGHTDESC;

	typedef struct tagVertex_Model
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexUV;
		XMFLOAT3		vTangent;
	}VTXMODEL;

	typedef struct tagVertex_Animation_Model
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexUV;
		XMFLOAT3		vTangent;
		XMUINT4			vBlendIndex;
		XMFLOAT4		vBlendWeight;
	}VTXANIMMODEL;

	typedef struct ENGINE_DLL tagVertex_AnimModel_Declaration
	{
		static const unsigned int iNumElements = 6;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXANIMMODEL_DECLARATION;

	typedef struct tagVertex_Point
	{
		XMFLOAT3		vPosition;
		float			fPSize;
	}VTXPOINT;

	typedef struct ENGINE_DLL tagVertex_Point_Matrix_Declaration
	{
		static const unsigned int iNumElements = 6;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXPOINT_MATRIX_DECLARATION;

	typedef struct tagVertex_Matrix
	{
		XMFLOAT4			vRight;
		XMFLOAT4			vUp;
		XMFLOAT4			vLook;
		XMFLOAT4			vPosition;
	}VTXMATRIX;

	typedef struct tagVertex_Texture
	{
		XMFLOAT3			vPosition;
		XMFLOAT2			vTexUV;
	}VTXTEX;

	typedef struct ENGINE_DLL tagVertex_Texture_Declaration
	{
		static const unsigned int iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXTEX_DECLARATION;

	typedef struct tagVertex_Texture_Color
	{
		XMFLOAT3			vPosition;
		DWORD				dwColor;
		XMFLOAT2			vTexUV;
	}VTXTEX_COLOR;

	typedef struct tagVertex_CubeTexture
	{
		XMFLOAT3			vPosition;
		XMFLOAT3			vTexUV;
	}VTXCUBETEX;

	typedef struct ENGINE_DLL tagVertex_Cube_Texture_Declaration
	{
		static const unsigned int iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXCUBETEX_DECLARATION;

	typedef struct tagVertex_Normal_Texture
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexUV;
	}VTXNORTEX;

	typedef struct ENGINE_DLL tagVertex_Normal_Texture_Declaration
	{
		static const unsigned int iNumElements = 3;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXNORMALTEX_DECLARATION;

	typedef struct tagVertex_NonAnimModel_Texture
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexUV;
		XMFLOAT3		vTangent;
	}VTXNONANIMMODELTEX;

	typedef struct ENGINE_DLL tagVertex_NonAnimModel_Texture_Declaration
	{
		static const unsigned int iNumElements = 4;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXNONANIMMODELTEX_DECLARATION;

	typedef struct tagVertex_Color
	{
		XMFLOAT3	vPosition;
		XMFLOAT4	vColor;
	}VTXCOL;

	typedef struct ENGINE_DLL tagVertex_Color_Declaration
	{
		static const unsigned int iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXCOL_DECLARATION;

	typedef struct tagLineIndices32
	{
		unsigned long	_0, _1;
	}LINEINDICES32;


	typedef struct tagLineIndices16
	{
		unsigned short	_0, _1;
	}LINEINDICES16;

	typedef struct tagFaceIndices32
	{
		unsigned long	_0, _1, _2;
	}FACEINDICES32;


	typedef struct tagFaceIndices16
	{
		unsigned short	_0, _1, _2;
	}FACEINDICES16;


// IMGUI ///////////////////////////////////////////////////
	typedef struct tagInfo_Terrain
	{
		char	szChildName[MAX_PATH] = "";
		_int	iTextureIndex = 0;
		_int	iNumVerticesX = 129, iNumVerticesZ = 129;
		_float	fTileNumX = 1.f, fTileNumZ = 1.f;
		_float4	vPosition = _float4(0.f, 0.f, 0.f, 1.f);
		_uint	iPass = 1;
		_bool	bDelete = false;
	}INFO_TERRAIN;
	typedef struct tagGroup_Terrain
	{
		char					szTerrainFileName[MAX_PATH] = "";
		vector<INFO_TERRAIN*>	vInfo_Terrain;
	}GROUPTERRAIN;
	typedef struct tagInfo_Mesh
	{
		char	szFileName[MAX_PATH] = "";
		char	szFullPath[MAX_PATH] = "";
		_int	iTextureIndex = 0;
		_float	vScale[3] = {1.f, 1.f, 1.f};
		_float	vRotation[3] = {};
		_float	vPosition[3] = {};
		_float4x4	vWorldMatrix = {};
		_uint	iPass = 0;
		_float	fFrustumRadius = 1.f;
	}INFO_MESH;
	typedef struct tagInfo_Collider
	{
		char		szBoneName[MAX_PATH] = "";
		_float		fRadius = 0.f;
	}INFO_COLLIDER;
	typedef struct tagInfo_UI
	{
		char	szChildName[MAX_PATH] = "";
		_int	iTextureIndex = 0;
		_float	fScaleX = 0.f, fScaleY = 0.f;
		_float	fPositionX = 0.f, fPositionY = 0.f;
		_float	fDepth = 0.f;
		_bool	bDelete = false;
	}INFO_UI;
	typedef struct tagGroup_UI
	{
		char				szUIFileName[MAX_PATH] = "";
		vector<INFO_UI*>	vInfo_UI;
	}GROUPUI;
///////////////////////////////////////////////////////////
}
