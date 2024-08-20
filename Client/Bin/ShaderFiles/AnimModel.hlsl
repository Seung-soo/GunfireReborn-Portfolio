
#include "Shader_Defines.hpp" 
//
//cbuffer LightDesc
//{
//	float4		g_vLightDir;
//	float4		g_vLightPos;
//	float4		g_vLightDiffuse;
//	float4		g_vLightAmbient = (float4)1.f;
//	float4		g_vLightSpecular = (float4)1.f;
//};
//
//cbuffer	MtrlDesc
//{
//	float4		g_vMtrlAmbient = float4(0.8f, 0.8f, 0.8f, 1.f);
//	float4		g_vMtrlSpecular = float4(0.2f, 0.2f, 0.2f, 0.2f);
//};

struct BoneMatrixArray
{
	matrix				BoneMatrices[256];
};

cbuffer Matrices
{
	BoneMatrixArray		g_BoneMatrices;
};

texture2D			g_DiffuseTexture;
texture2D			g_SourTexture;
texture2D			g_NormalTexture;
texture2D			g_ThirdTexture;

cbuffer NoiseAndMoveUV
{
	float				g_fMoveUV;
	float				g_fMoveNoise;
};

cbuffer Alpha
{
	float				g_fDissolveAlpha;
};

//
//cbuffer CameraDesc
//{
//	float4		g_vCamPosition;
//};


struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float3		vTangent : TANGENT;
	uint4		vBlendIndex : BLENDINDEX;
	float4		vBlendWeight : BLENDWEIGHT;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
};

struct VS_OUT_NORMAL
{
	float4		vPosition : SV_POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
	float4		vTangent : TANGENT;
	float4		vBinormal : BINORMAL;
};

VS_OUT VS_MAIN_DEFAULT(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	float		fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

	matrix		BoneMatrix = g_BoneMatrices.BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
		g_BoneMatrices.BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
		g_BoneMatrices.BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
		g_BoneMatrices.BoneMatrices[In.vBlendIndex.w] * fWeightW;

	vector		vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
	vector		vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vNormal = normalize(mul(vNormal, g_WorldMatrix).xyz);
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(vPosition, g_WorldMatrix);
	Out.vProjPos = Out.vPosition;
	return Out;
}

VS_OUT VS_MAIN_COLLIDER(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

VS_OUT VS_MAIN_MOVEUV(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	float		fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

	matrix		BoneMatrix = g_BoneMatrices.BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
		g_BoneMatrices.BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
		g_BoneMatrices.BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
		g_BoneMatrices.BoneMatrices[In.vBlendIndex.w] * fWeightW;

	vector		vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
	vector		vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vNormal = normalize(mul(vNormal, g_WorldMatrix).xyz);
	Out.vTexUV = In.vTexUV + float2(0.f, g_fMoveUV);
	Out.vWorldPos = mul(vPosition, g_WorldMatrix);
	Out.vProjPos = Out.vPosition;
	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
};

struct PS_OUT
{
	vector		vDiffuse : SV_TARGET0;
	vector		vNormal : SV_TARGET1;
	vector		vDepth : SV_TARGET2;
	vector		vMask : SV_TARGET3;
};

PS_OUT PS_MAIN_DEFAULT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	
	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, 0.8f, 0.22f);
	Out.vMask = 0.f;
	if (Out.vDiffuse.a < 0.1f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_COLLIDER(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = vector(0.f, 1.f, 0.f, 1.f);
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	return Out;
}

PS_OUT PS_MAIN_NOTLIGHT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	//if (vMtrlDiffuse.a < 0.05f)
	//	discard;

	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, 0.8f, 0.22f);

	if (Out.vDiffuse.a < 0.05f)
		discard;

	//Out.vColor = vMtrlDiffuse;



	return Out;
}

PS_OUT PS_MAIN_MOVENOISE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, 0.8f, 0.22f);
	Out.vMask = 1.f;
	if (Out.vDiffuse.a < 0.1f)
		discard;

	return Out;
}


PS_OUT PS_MAIN_DISSOLVE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector	vDissolve = g_ThirdTexture.Sample(DefaultSampler, In.vTexUV);
	float	fDissolveRed = vDissolve.r;
	fDissolveRed += g_fDissolveAlpha;
	if (vDiffuse.r < fDissolveRed)
		discard;

	Out.vDiffuse = vDiffuse;
	if (vDiffuse.r < fDissolveRed + 0.05f)
		Out.vDiffuse = vector(0.f, 0.f, 1.f, 1.f);
	if (vDiffuse.r < fDissolveRed + 0.03f)
		Out.vDiffuse = vector(0.f, 1.f, 1.f, 1.f);
	if (vDiffuse.r <fDissolveRed + 0.02f)
		Out.vDiffuse = vector(1.f, 1.f, 1.f, 1.f);


	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, 0.8f, 0.22f);
	Out.vMask = 1.f;
	if (Out.vDiffuse.a < 0.1f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_BLUR(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, 0.8f, 0.22f);
	Out.vMask = 1.f;
	if (Out.vDiffuse.a < 0.1f)
		discard;

	return Out;
}
PS_OUT PS_MAIN_MASKBLUR(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector	vMask = g_SourTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, 0.8f, 0.22f);
	if (0.f < vMask.b)
		vMask.b = 1.f;
		Out.vMask = vMask.bbbb;
	if (Out.vDiffuse.a < 0.1f)
		discard;

	return Out;
}

technique11		DefaultTechnique
{
	pass Rect //0
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEFAULT();
	}

	pass Collider	// 1
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(FillMode_Wireframe);

		VertexShader = compile vs_5_0 VS_MAIN_COLLIDER();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_COLLIDER();
	}

	pass NotLight	// 2
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NOTLIGHT();
	}

	pass MoveUV	// 3
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_MOVEUV();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEFAULT();
	}

	pass Dissolve //4
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DISSOLVE();
	}
	pass Test //5
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DISSOLVE();
	}

	pass Blur //6
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLUR();
	}
	pass MaskBlur //7
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MASKBLUR();
	}
}