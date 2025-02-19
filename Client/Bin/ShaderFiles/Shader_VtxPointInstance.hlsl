
#include "Shader_Defines.hpp"

cbuffer CameraDesc
{
	vector			g_vCamPosition;
};

texture2D			g_DiffuseTexture;

struct VS_IN
{
	float3		vPosition : POSITION;
	float		fPSize : PSIZE;

	float4		vRight : TEXCOORD1;
	float4		vUp : TEXCOORD2;
	float4		vLook : TEXCOORD3;
	float4		vTranslation : TEXCOORD4;
};

struct VS_OUT
{
	float4		vPosition : POSITION;
	float		fPSize : PSIZE;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			TransformMatrix = float4x4(normalize(In.vRight), normalize(In.vUp), normalize(In.vLook), In.vTranslation);

	vector			vPosition = mul(vector(In.vPosition, 1.f), TransformMatrix);

	Out.vPosition = mul(vPosition, g_WorldMatrix);
	Out.fPSize = In.fPSize;

	return Out;
}

VS_OUT VS_MAIN_EFFECT(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			TransformMatrix = float4x4(normalize(In.vRight), normalize(In.vUp), normalize(In.vLook), In.vTranslation);

	vector			vPosition = mul(vector(In.vPosition, 1.f), TransformMatrix);

	Out.vPosition = vPosition;
	Out.fPSize = length(In.vRight);

	return Out;
}

struct GS_IN
{	
	float4		vPosition : POSITION;
	float		fPSize : PSIZE;	
};

struct GS_OUT
{
	float4		vPosition : SV_POSITION;	
	float2		vTexUV : TEXCOORD0;
};

[maxvertexcount(6)]
void GS_MAIN(in point GS_IN In[1], inout TriangleStream<GS_OUT> Trianglestream)
{
	GS_OUT			Out[4];

	float3		vLook = (g_vCamPosition - In[0].vPosition).xyz;
	float3		vAxisY = float3(0.f, 1.f, 0.f);
	float3		vRight = cross(vAxisY, vLook);
	float3		vUp = cross(vLook, vRight);

	vLook = normalize(vLook);
	vRight = normalize(vRight);
	vUp = normalize(vUp);
	float	fHalfSize = In[0].fPSize * 0.5f;

	matrix		matVP;
	matVP = mul(g_ViewMatrix, g_ProjMatrix);

	float3		vPosition;

	vPosition = In[0].vPosition.xyz + vRight * fHalfSize + vUp * fHalfSize;
	Out[0].vPosition = mul(vector(vPosition, 1.f), matVP);
	Out[0].vTexUV = float2(0.f, 0.f);	

	vPosition = In[0].vPosition.xyz - vRight * fHalfSize + vUp * fHalfSize;
	Out[1].vPosition = mul(vector(vPosition, 1.f), matVP);
	Out[1].vTexUV = float2(1.f, 0.f);	

	vPosition = In[0].vPosition.xyz - vRight * fHalfSize - vUp * fHalfSize;
	Out[2].vPosition = mul(vector(vPosition, 1.f), matVP);
	Out[2].vTexUV = float2(1.f, 1.f);

	vPosition = In[0].vPosition.xyz + vRight * fHalfSize - vUp * fHalfSize;
	Out[3].vPosition = mul(vector(vPosition, 1.f), matVP);
	Out[3].vTexUV = float2(0.f, 1.f);

	Trianglestream.Append(Out[0]);
	Trianglestream.Append(Out[1]);
	Trianglestream.Append(Out[2]);
	Trianglestream.RestartStrip();

	Trianglestream.Append(Out[0]);
	Trianglestream.Append(Out[2]);
	Trianglestream.Append(Out[3]);
	Trianglestream.RestartStrip();
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	vector		vColor : SV_TARGET0;
	vector		vSecondColor : SV_TARGET1;
	vector		vMask : SV_TARGET2;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	Out.vSecondColor = Out.vColor;
	Out.vMask = (vector)1.f;
	if (Out.vColor.a < 0.1f)
		discard;


	return Out;
}

technique11		DefaultTechnique
{
	pass Point
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}	
	pass Effect
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_EFFECT();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}