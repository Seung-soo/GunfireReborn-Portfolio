#include "Shader_Defines.hpp"

cbuffer Wave
{
	int			g_fWaveValue;
};

//cbuffer LightDesc
//{
//	float4		g_vLightDir;
//	float4		g_vLightPos;
//	float4		g_vLightDiffuse;
//	float4		g_vLightAmbient = (float4)1.f;
//	float4		g_vLightSpecular = (float4)1.f;
//};
//
//cbuffer CameraDesc
//{
//	float4		g_vCamPosition;
//};

cbuffer BrushDesc
{
	float4		g_vBrushPos = float4(10.f, 0.f, 10.f, 1.f);
	float		g_fRadius = 3.f;
};

texture2D		g_DiffuseTexture;
texture2D		g_SourDiffuseTexture;
texture2D		g_DestDiffuseTexture;
texture2D		g_FilterTexture;
texture2D		g_BrushTexture;

//cbuffer	MtrlDesc
//{
//	float4		g_vMtrlAmbient = float4(0.4f, 0.4f, 0.4f, 1.f);
//	float4		g_vMtrlSpecular = float4(1.f, 1.f, 1.f, 1.f);
//};

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
};

VS_OUT VS_MAIN_TERRAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);	
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
	Out.vProjPos = Out.vPosition;

	return Out;
}

VS_OUT VS_MAIN_OCEAN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	float fDegree = In.vPosition.z * 10.f + (float)g_fWaveValue;
	float fNextDegree = (In.vPosition.z + 1.f) * 10.f + (float)g_fWaveValue;

	In.vPosition.y = cos(radians(fDegree)) * 0.25f + sin(radians(fDegree) * 3.f) * 0.15f;
	vector vNextPos = vector(In.vPosition.x, cos(radians(fNextDegree)) * 0.5f 
		+ sin(radians(fNextDegree) * 3.f) * 0.3f, In.vPosition.z + 1.f, 1.f);
	vector vLook = normalize(vNextPos - vector(In.vPosition, 1.f));

	float3 vNormal = cross(vLook.xyz, float3(1.f, 0.f, 0.f));

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);

	Out.vNormal = mul(vector(vNormal, 0.f), g_WorldMatrix);
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
	Out.vProjPos = Out.vPosition;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
};

struct PS_OUT
{
	vector		vDiffuse : SV_TARGET0;
	vector		vNormal : SV_TARGET1;
	vector		vDepth : SV_TARGET2;
};

PS_OUT PS_MAIN_RECT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;
	//float4	vDistortColor = g_DistortTexture.Sample(DefaultSampler, In.vTexUV);

	vector	vBrushColor = (vector)0.f;
	vector	vFilterColor = g_FilterTexture.Sample(DefaultSampler, In.vTexUV);

	if (g_vBrushPos.x - g_fRadius < In.vWorldPos.x && In.vWorldPos.x <= g_vBrushPos.x + g_fRadius &&
		g_vBrushPos.z - g_fRadius < In.vWorldPos.z && In.vWorldPos.z <= g_vBrushPos.z + g_fRadius)
	{
		float2		vBrushUV;

		vBrushUV.x = (In.vWorldPos.x - (g_vBrushPos.x - g_fRadius)) / (2.f * g_fRadius);
		vBrushUV.y = ((g_vBrushPos.z + g_fRadius) - In.vWorldPos.z) / (2.f * g_fRadius);

		vBrushColor = g_BrushTexture.Sample(DefaultSampler, vBrushUV);
	}

	vector	vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV) + vBrushColor;

	//float	fShade = max(dot(normalize(g_vLightDir) * -1.f, In.vNormal), 0.f);

	//vector	vReflect = reflect(normalize(g_vLightDir), In.vNormal);
	//vector	vLook = normalize(In.vWorldPos - g_vCamPosition);

	//float	fSpecular = pow(max(dot(normalize(vReflect) * -1.f, vLook), 0.f), 30.f);

	//vector vLook = normalize((vector)g_vCamPosition - (vector)In.vWorldPos);

	//float fLine = (1.f - saturate(dot(In.vNormal, -vLook)));
	//if (fLine > 0.95f)
	//	Out.vColor = (vector)0.f;
	//else
		//Out.vColor = (g_vLightDiffuse * vMtrlDiffuse) * saturate(ceil(int(fShade * 8.f)) / 8.f + (g_vLightAmbient * g_vMtrlAmbient))/* + (g_vLightSpecular * g_vMtrlSpecular) * fSpecular*/;

	Out.vDiffuse = vMtrlDiffuse;
	Out.vDiffuse.a = 1.f;
	// -1~1사이의 값을 0~1로 치환
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / 300.0f, In.vProjPos.z / In.vProjPos.w, 0.f, 1.f);

	return Out;
}

PS_OUT PS_MAIN_OCEAN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vNoise = g_SourDiffuseTexture.Sample(DefaultSampler, In.vTexUV + float2(0.f, g_fWaveValue * 0.005f));
	vector	vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV + vNoise.r * 0.25f);
	
	//float	fShade = max(dot(normalize(g_vLightDir) * -1.f, In.vNormal), 0.f);

	//vector	vReflect = reflect(normalize(g_vLightDir), In.vNormal);
	//vector	vLook = normalize(In.vWorldPos - g_vCamPosition);

	//float	fSpecular = pow(max(dot(normalize(vReflect) * -1.f, vLook), 0.f), 30.f);


	//Out.vColor = (g_vLightDiffuse * vMtrlDiffuse) * saturate(/*ceil(int(*/fShade/* * 8.f)) / 8.f*/ + (g_vLightAmbient * g_vMtrlAmbient))/* + (g_vLightSpecular * g_vMtrlSpecular) * fSpecular*/;

	Out.vDiffuse = vMtrlDiffuse;

	// -1~1사이의 값을 0~1로 치환
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / 300.0f, In.vProjPos.z / In.vProjPos.w, 0.f, 0.f);

	return Out;
}

technique11		DefaultTechnique
{
	pass Terrain_Blend
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_TERRAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_RECT();
	}

	pass Terrain_NonBlend
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_TERRAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_RECT();
	}

	pass Terrain_WireFrame
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(FillMode_Wireframe);

		VertexShader = compile vs_5_0 VS_MAIN_TERRAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_RECT();
	}

	pass Ocean
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_OCEAN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_OCEAN();
	}
}