#include "Shader_Defines.hpp"


texture2D		g_DiffuseTexture;
texture2D		g_SourTexture;

texture2D		g_WindowTexture;

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

//cbuffer	MtrlDesc
//{
//	float4		g_vMtrlAmbient = float4(0.5f, 0.5f, 0.5f, 1.f);
//	float4		g_vMtrlSpecular = float4(0.2f, 0.2f, 0.2f, 0.2f);
//};

cbuffer SPRITE
{
	float		g_fSpriteIndex;
	int			g_iSpriteYIndex = 0;
	int			g_iSpriteMaxXIndex;
	int			g_iSpriteMaxYIndex;
};


cbuffer Move_UV
{
	float		g_fMoveUV_X;
	float		g_fMoveUV_Y;
	float		g_fMoveNoise_X;
	float		g_fMoveNoise_Y;
	float		g_fAlpha;

	float4		g_vColor;
};

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float3		vTangent : TANGENT;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
};


VS_OUT VS_MAIN_DEFAULT(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
	Out.vProjPos = Out.vPosition;

	return Out;
}

VS_OUT VS_MAIN_SPRITE(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);
	In.vTexUV.x = In.vTexUV.x / (float)g_iSpriteMaxXIndex + (float)g_fSpriteIndex / (float)g_iSpriteMaxXIndex;
	In.vTexUV.y = In.vTexUV.y / (float)g_iSpriteMaxYIndex + (float)g_iSpriteYIndex / (float)g_iSpriteMaxYIndex;

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
	Out.vProjPos = Out.vPosition;

	return Out;
}

VS_OUT VS_MAIN_MOVEUV(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	In.vTexUV.x += g_fMoveUV_X;
	In.vTexUV.y += g_fMoveUV_Y;

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
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
	vector		vMask : SV_TARGET3;
};

struct PS_FORWARD_OUT
{
	vector		vColor : SV_TARGET0;
	vector		vSecondColor : SV_TARGET1;
	vector		vMask : SV_TARGET2;
};

PS_OUT PS_MAIN_DEFAULT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);



	//float	fShade = max(dot(normalize(g_vLightDir) * -1.f, In.vNormal), 0.f);

	//vector	vReflect = reflect(normalize(g_vLightDir), In.vNormal);
	//vector	vLook = normalize(In.vWorldPos - g_vCamPosition);

	//float	fSpecular = pow(max(dot(normalize(vReflect) * -1.f, vLook), 0.f), 50.f);

	//vector vLook = normalize((vector)g_vCamPosition - (vector)In.vWorldPos);

	//float fLine = (1.f - saturate(dot(In.vNormal, -vLook)));
	//if (fLine > 0.95f)
	//	Out.vColor = (vector)0.f;
	//else
	//Out.vColor = (g_vLightDiffuse * vMtrlDiffuse) * saturate(/*ceil(int(*/fShade/* * 10.f)) / 10.f*/ + (g_vLightAmbient * g_vMtrlAmbient))/* + (g_vLightSpecular * g_vMtrlSpecular) * fSpecular*/;
	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, 0.f, 0.f);
	Out.vMask = 0.f;
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

PS_OUT PS_MAIN_WAVE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;
	vector vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	//float	fShade = max(dot(normalize(g_vLightDir) * -1.f, In.vNormal), 0.f);

	//vector	vReflect = reflect(normalize(g_vLightDir), In.vNormal);
	//vector	vLook = normalize(In.vWorldPos - g_vCamPosition);

	//float	fSpecular = pow(max(dot(normalize(vReflect) * -1.f, vLook), 0.f), 50.f);

	//Out.vColor = (g_vLightDiffuse * vMtrlDiffuse) * saturate(fShade + (g_vLightAmbient * g_vMtrlAmbient)) + (g_vLightSpecular * g_vMtrlSpecular) * fSpecular + vector(0.1f, 0.1f, 0.3f, 0.5f);

	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, 0.f, 0.f);
	Out.vMask = 0.f;
	if (Out.vDiffuse.a < 0.1f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_WireFrame(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	//Out.vColor = (vector)1.f;
	Out.vDiffuse = (vector)1.f;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, 0.f, 0.f);
	Out.vMask = 0.f;
	return Out;
}

PS_FORWARD_OUT PS_MAIN_AURA(PS_IN In)
{
	PS_FORWARD_OUT		Out = (PS_FORWARD_OUT)0;

	Out.vColor = vector(0.7f, 0.7f, 1.f, 0.3);
	Out.vSecondColor = Out.vColor;
	Out.vMask = (vector)0.f;
	if (Out.vColor.a == 0.f)
		discard;

	return Out;
}

PS_FORWARD_OUT PS_MAIN_NOTLIGHT(PS_IN In)
{
	PS_FORWARD_OUT		Out = (PS_FORWARD_OUT)0;

	vector	vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	//if (vMtrlDiffuse.a < 0.05f)
	//	discard;

	//Out.vColor = vMtrlDiffuse;

	Out.vColor = vMtrlDiffuse;
	Out.vSecondColor = vMtrlDiffuse;
	Out.vMask = (vector)0.f;
	if (Out.vColor.a < 0.05f)
		discard;

	return Out;
}

PS_FORWARD_OUT PS_MAIN_SWORDTRAIL(PS_IN In)
{
	if (In.vTexUV.x > 1.f || In.vTexUV.x < 0.f)
		discard;
	if (In.vTexUV.y > 1.f || In.vTexUV.y < 0.f)
		discard;

	PS_FORWARD_OUT		Out = (PS_FORWARD_OUT)0;


	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	Out.vSecondColor = Out.vColor;
	Out.vMask = (vector)0.f;
	if (Out.vColor.a == 0.f)
		discard;
	return Out;
}

PS_FORWARD_OUT PS_MAIN_NOISE(PS_IN In)
{
	PS_FORWARD_OUT		Out = (PS_FORWARD_OUT)0;
	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	vector		vNoiseColor = g_SourTexture.Sample(DefaultSampler, In.vTexUV + float2(0.f, g_fMoveUV_X));

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV + vNoiseColor.r * 0.25f);
	Out.vSecondColor = Out.vColor;
	Out.vMask = (vector)0.f;
	return Out;
}

PS_FORWARD_OUT PS_MAIN_ONLYNOISE(PS_IN In)
{
	PS_FORWARD_OUT		Out = (PS_FORWARD_OUT)0;
	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	vector		vNoiseColor = g_SourTexture.Sample(DefaultSampler, In.vTexUV + float2(0.f, g_fMoveUV_X));

	Out.vColor = g_SourTexture.Sample(DefaultSampler, In.vTexUV + vNoiseColor.r * 0.25f);
	Out.vColor += vector(0.1f, 0.1f, 0.3f, 0.f);
	Out.vSecondColor = Out.vColor;
	Out.vMask = (vector)0.f;
	return Out;
}

PS_OUT PS_MAIN_ALPHADOWN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vDiffuse.a = g_fMoveUV_X;
	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, 0.f, 0.f);
	Out.vMask = 0.f;
	if (Out.vDiffuse.a == 0.f)
		discard;

	return Out;
}

PS_FORWARD_OUT PS_MAIN_NOISE_X(PS_IN In)
{
	PS_FORWARD_OUT		Out = (PS_FORWARD_OUT)0;
	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	vector		vNoiseColor = g_SourTexture.Sample(DefaultSampler, In.vTexUV + float2(g_fMoveUV_X, 0.f));

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV + vNoiseColor.r * 0.25f);
	Out.vSecondColor = Out.vColor;
	Out.vMask = (vector)0.f;
	if (0 == Out.vColor.a)
		discard;

	return Out;
}

PS_FORWARD_OUT PS_MAIN_TRAIL_Y(PS_IN In)
{
	if (In.vTexUV.y > 1.f || In.vTexUV.y < 0.f)
		discard;

	PS_FORWARD_OUT		Out = (PS_FORWARD_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	Out.vSecondColor = Out.vColor;
	Out.vMask = (vector)0.f;
	if (Out.vColor.a == 0.f)
		discard;
	return Out;
}

PS_FORWARD_OUT PS_MAIN_EFFECT(PS_IN In)
{
	PS_FORWARD_OUT		Out = (PS_FORWARD_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	Out.vSecondColor = Out.vColor;
	Out.vMask = (vector)0.f;
	if (Out.vColor.a == 0.f)
		discard;
	return Out;
}

PS_FORWARD_OUT PS_MAIN_NOISE_X_REAL(PS_IN In)
{
	PS_FORWARD_OUT		Out = (PS_FORWARD_OUT)0;
	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	vector		vNoiseColor = g_SourTexture.Sample(DefaultSampler, In.vTexUV + float2(g_fMoveNoise_X, 0.f));

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV + vNoiseColor.r * 0.25f);
	Out.vSecondColor = Out.vColor;
	Out.vMask = (vector)0.f;
	if (0 == Out.vColor.a)
		discard;

	return Out;
}

PS_FORWARD_OUT PS_MAIN_NOISE_X_TRAIL(PS_IN In)
{
	if (In.vTexUV.x > 1.f || In.vTexUV.x < 0.f)
		discard;

	PS_FORWARD_OUT		Out = (PS_FORWARD_OUT)0;
	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	vector		vNoiseColor = g_SourTexture.Sample(DefaultSampler, In.vTexUV + float2(g_fMoveNoise_X, 0.f));

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV + vNoiseColor.r * 0.25f);
	Out.vSecondColor = Out.vColor;
	Out.vMask = (vector)0.f;
	if (0 == Out.vColor.a)
		discard;

	return Out;
}

PS_FORWARD_OUT PS_MAIN_DISTORTION(PS_IN In)
{
	PS_FORWARD_OUT		Out = (PS_FORWARD_OUT)0;

	vector	vNoiseColor = g_SourTexture.Sample(DefaultSampler, In.vTexUV + float2(g_fMoveNoise_X, g_fMoveNoise_Y));
	vector	vColor = vector(0.f, 0.f, 0.f, 0.f);
	vector	vWindow = g_WindowTexture.Sample(DefaultSampler, float2(In.vPosition.x / 1280.f, In.vPosition.y / 720.f) + vNoiseColor.r * 0.1f);

	Out.vColor = pow(abs(vColor.a * vColor + (1.f - vColor.a) * vWindow), 1.f/1.5f);
	Out.vSecondColor = Out.vColor;
	Out.vMask = (vector)0.f;
	return Out;
}

PS_FORWARD_OUT PS_MAIN_DISTORTION_TEXTURE(PS_IN In)
{
	PS_FORWARD_OUT		Out = (PS_FORWARD_OUT)0;

	vector	vNoiseColor = g_SourTexture.Sample(DefaultSampler, In.vTexUV + float2(g_fMoveNoise_X, g_fMoveNoise_Y));
	vector	vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV + vNoiseColor.r * 0.05f);
	vector	vWindow = g_WindowTexture.Sample(DefaultSampler, float2(In.vPosition.x / 1280.f, In.vPosition.y / 720.f) + vNoiseColor.r * 0.05f);

	Out.vColor = pow(abs(vColor.a * vColor + (1.f - vColor.a) * vWindow), 1.f / 1.1f);
	Out.vSecondColor = Out.vColor;
	Out.vMask = (vector)0.f;
	return Out;
}

PS_FORWARD_OUT PS_MAIN_ALPHADOWN_DEFFERED(PS_IN In)
{
	PS_FORWARD_OUT		Out = (PS_FORWARD_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vColor.a -= g_fAlpha;
	Out.vSecondColor = Out.vColor;
	Out.vMask = (vector)0.f;
	if (Out.vColor.a < 0.01f)
		discard;

	return Out;
}

PS_FORWARD_OUT PS_MAIN_DISTORTION_TEXTURE_CUT(PS_IN In)
{
	PS_FORWARD_OUT		Out = (PS_FORWARD_OUT)0;

	if (In.vTexUV.x > 1.f || In.vTexUV.x < 0.f)
		discard;
	if (In.vTexUV.y > 1.f || In.vTexUV.y < 0.f)
		discard;

	vector	vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	if (vColor.a < 0.01f)
		discard;

	vector	vNoiseColor = g_SourTexture.Sample(DefaultSampler, In.vTexUV + float2(g_fMoveNoise_X, g_fMoveNoise_Y));
	vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV + vNoiseColor.r * 0.05f);
	vector	vWindow = g_WindowTexture.Sample(DefaultSampler, float2(In.vPosition.x / 1280.f, In.vPosition.y / 720.f) + vNoiseColor.r * 0.05f);

	Out.vColor = pow(abs(vColor.a * vColor + (1.f - vColor.a) * vWindow), 1.f / 1.1f);
	Out.vSecondColor = Out.vColor;
	Out.vMask = (vector)0.f;
	return Out;
}

PS_FORWARD_OUT PS_MAIN_DISTORTION_TEXTURE_CUT_ALPHADOWN(PS_IN In)
{
	PS_FORWARD_OUT		Out = (PS_FORWARD_OUT)0;

	//if (In.vTexUV.x > 1.f || In.vTexUV.x < 0.f)
	//	discard;
	//if (In.vTexUV.y > 1.f || In.vTexUV.y < 0.f)
	//	discard;

	vector	vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	if (vColor.a < 0.01f)
		discard;

	vector	vNoiseColor = g_SourTexture.Sample(DefaultSampler, In.vTexUV + float2(g_fMoveNoise_X, g_fMoveNoise_Y));
	vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV + vNoiseColor.r * 0.05f);
	vector	vWindow = g_WindowTexture.Sample(DefaultSampler, float2(In.vPosition.x / 1280.f, In.vPosition.y / 720.f) + vNoiseColor.r * 0.05f);

	Out.vColor = pow(abs(vColor.a * vColor + (1.f - vColor.a) * vWindow), 1.f / 1.1f);
	Out.vColor.a -= g_fAlpha;

	if (Out.vColor.a < 0.01f)
		discard;
	Out.vSecondColor = Out.vColor;
	Out.vMask = (vector)0.f;
	return Out;
}

PS_OUT PS_MAIN_GRAY(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vDiffuse.gb = vDiffuse.r;
	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, 0.f, 0.f);
	Out.vMask = 0.f;
	if (Out.vDiffuse.a < 0.1f)
		discard;

	return Out;
}

PS_FORWARD_OUT PS_MAIN_EFFECT_LERPALPHA(PS_IN In)
{
	PS_FORWARD_OUT		Out = (PS_FORWARD_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	int iTexV = In.vTexUV.y;
	float fTexUV_Y = In.vTexUV.y - iTexV;
	Out.vColor.a *= fTexUV_Y;
	Out.vSecondColor = Out.vColor;
	Out.vMask = (vector)0.f;
	if (Out.vColor.a == 0.f)
		discard;
	return Out;
}

PS_FORWARD_OUT PS_MAIN_MULCOLOR(PS_IN In)
{
	PS_FORWARD_OUT		Out = (PS_FORWARD_OUT)0;

	if (In.vTexUV.y > 1.f)
		discard;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	Out.vColor.a = Out.vColor.r * 0.8f;
	if (Out.vColor.a < 0.001f)
		discard;

	Out.vColor *= g_vColor;
	Out.vSecondColor = Out.vColor;
	Out.vMask = (vector)0.f;
	return Out;
}


technique11		DefaultTechnique
{
	pass Mesh_NonBlend //0
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEFAULT();
	}

	pass Mesh_Blend //1
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEFAULT();
	}

	pass Mesh_WireFrame //2
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(FillMode_Wireframe);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_WireFrame();
	}

	pass Mesh_Aura //3
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_AURA();
	}

	pass Mesh_Wave //4
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_WAVE();
	}

	pass NotLight //5
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NOTLIGHT();
	}
	pass SwordEffect //6
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);

		VertexShader = compile vs_5_0 VS_MAIN_SPRITE();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SWORDTRAIL();
	}
	pass MoveUV //7
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);

		VertexShader = compile vs_5_0 VS_MAIN_MOVEUV();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SWORDTRAIL();
	}
	pass Noise //8
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NOISE();
	}
	pass OnlyNoise //9
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ONLYNOISE();
	}
	pass AlphaDown //10
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHADOWN();
	}
	pass Noise_X //11
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NOISE_X();
	}
	pass Trail_Y //12
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);

		VertexShader = compile vs_5_0 VS_MAIN_MOVEUV();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_TRAIL_Y();
	}
	pass Effect //13
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_EFFECT();
	}
	pass OnlyMoveUV //14
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);

		VertexShader = compile vs_5_0 VS_MAIN_MOVEUV();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_EFFECT();
	}
	pass MoveUV_AND_NOISE //15
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);

		VertexShader = compile vs_5_0 VS_MAIN_MOVEUV();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NOISE_X_REAL();
	}
	pass MoveUV_AND_NOISE_TRAIL //16
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);

		VertexShader = compile vs_5_0 VS_MAIN_MOVEUV();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NOISE_X_TRAIL();
	}
	pass Distortion //17
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DISTORTION();
	}
	pass MoveDistortion //18
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_MOVEUV();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DISTORTION();
	}
	pass MoveDistortionTexture //19
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_MOVEUV();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DISTORTION_TEXTURE();
	}
	pass DownAlpha //20
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHADOWN_DEFFERED();
	}
	pass MoveDistortionTextureCut //21
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_MOVEUV();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DISTORTION_TEXTURE_CUT();
	}
	pass MoveDistortionTextureCut_ALPHADOWN //22
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DISTORTION_TEXTURE_CUT_ALPHADOWN();
	}
	pass MoveDistortionTextureCut_CullNone //23
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);

		VertexShader = compile vs_5_0 VS_MAIN_MOVEUV();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DISTORTION_TEXTURE_CUT();
	}
	pass Effect_ccw //24
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_EFFECT();
	}

	pass Gray //25
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_GRAY();
	}

	pass OnlyMoveUV_LERPALPHA //26
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_MOVEUV();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_EFFECT_LERPALPHA();
	}

	pass MulColor //27
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_MOVEUV();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MULCOLOR();
	}
	pass Blur //28
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLUR();
	}
}