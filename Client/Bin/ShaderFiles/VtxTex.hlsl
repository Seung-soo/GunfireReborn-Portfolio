#include "Shader_Defines.hpp"
texture2D		g_DiffuseTexture;
texture2D		g_SourTexture;
texture2D		g_WindowTexture;

cbuffer NOISE
{
	float		g_AddNoiseUV;
	float		g_DistortionPower;
};

cbuffer HIT
{
	int			g_iHitAlpha;
};

cbuffer COLOR
{
	float3		g_vColor;
};

cbuffer HP
{
	float		g_CurrentHP_UV;
};

cbuffer SPRITE
{
	int			g_iSpriteIndex;
	int			g_iSpriteYIndex = 0;
	int			g_iSpriteMaxXIndex;
	int			g_iSpriteMaxYIndex;
};

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

VS_OUT VS_MAIN_RECT(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

VS_OUT VS_MAIN_SPRITE(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	In.vTexUV.x = In.vTexUV.x / 4.f + g_iSpriteIndex / 4.f;
	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

VS_OUT VS_MAIN_SPRITEXY(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	In.vTexUV.x = In.vTexUV.x / 2.f + g_iSpriteIndex / 2.f;
	In.vTexUV.y = In.vTexUV.y / 2.f + g_iSpriteYIndex / 2.f;
	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

VS_OUT VS_MAIN_SPRITE4X4(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	In.vTexUV.x = In.vTexUV.x / 4.f + g_iSpriteIndex / 4.f;
	In.vTexUV.y = In.vTexUV.y / 4.f + g_iSpriteYIndex / 4.f;
	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

VS_OUT VS_MAIN_SPRITE4X2(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	In.vTexUV.x = In.vTexUV.x / 4.f + g_iSpriteIndex / 4.f;
	In.vTexUV.y = In.vTexUV.y / 2.f + g_iSpriteYIndex / 2.f;
	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

VS_OUT VS_MAIN_FINAL_SPRITE(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	In.vTexUV.x = In.vTexUV.x / (float)g_iSpriteMaxXIndex + (float)g_iSpriteIndex / (float)g_iSpriteMaxXIndex;
	In.vTexUV.y = In.vTexUV.y / (float)g_iSpriteMaxYIndex + (float)g_iSpriteYIndex / (float)g_iSpriteMaxYIndex;
	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	vector		vColor : SV_TARGET0;

};

struct PS_OUT_DEFFERED
{
	vector		vColor : SV_TARGET0;
	vector		vSecondColor : SV_TARGET1;
	vector		vMask : SV_TARGET2;
};

PS_OUT PS_MAIN_RECT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	if (Out.vColor.a < 0.25f)
		discard;

	return Out;
}
PS_OUT PS_MAIN_RECTALPHA(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	if (Out.vColor.a < 0.1f)
		discard;

	return Out;
}

PS_OUT_DEFFERED PS_MAIN_RECT_DEFFERED(PS_IN In)
{
	PS_OUT_DEFFERED		Out = (PS_OUT_DEFFERED)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	Out.vSecondColor = Out.vColor;
	Out.vMask = (vector)0.f;

	if (Out.vColor.a < 0.25f)
		discard;

	return Out;
}

PS_OUT_DEFFERED PS_MAIN_BULLETTESLA(PS_IN In)
{
	PS_OUT_DEFFERED		Out = (PS_OUT_DEFFERED)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	Out.vSecondColor = Out.vColor;
	Out.vMask = (vector)0.f;
	if (Out.vColor.r < 0.3f && Out.vColor.g < 0.3f && Out.vColor.b < 0.3f)
		discard;



	return Out;
}

PS_OUT PS_MAIN_HP(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	if (Out.vColor.a < 0.1f)
		discard;

	if (In.vTexUV.x > g_CurrentHP_UV)
		Out.vColor.rgb = vector(0.5f, 0.5f, 0.5f, 0.f).rgb;


	return Out;
}

PS_OUT PS_MAIN_HIT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	if (Out.vColor.a < 0.01f)
		discard;
	
	Out.vColor.r = Out.vColor.a;
	Out.vColor.gb = 0.f;

	Out.vColor.a -= float(g_iHitAlpha) / 100.f;
	if (Out.vColor.a < 0.f)
		Out.vColor.a = 0.f;


	return Out;
}

PS_OUT PS_MAIN_VICTORY(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	if (Out.vColor.a < 0.01f)
		discard;

	Out.vColor.a = float(g_iHitAlpha) / 255.f;
	if (Out.vColor.a < 0.f)
		Out.vColor.a = 0.f;


	return Out;
}

PS_OUT_DEFFERED PS_MAIN_VICTORY_DEFFERED(PS_IN In)
{
	PS_OUT_DEFFERED		Out = (PS_OUT_DEFFERED)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	Out.vSecondColor = Out.vColor;
	Out.vMask = (vector)0.f;
	if (Out.vColor.a < 0.01f)
		discard;

	Out.vColor.a = float(g_iHitAlpha) / 255.f;
	if (Out.vColor.a < 0.f)
		Out.vColor.a = 0.f;


	return Out;
}

PS_OUT PS_MAIN_GRAY(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	if (Out.vColor.a < 0.01f)
		discard;

	if (Out.vColor.r > 0.3f || Out.vColor.g > 0.3f || Out.vColor.b > 0.3f)
		Out.vColor.gb = Out.vColor.r;


	return Out;
}

PS_OUT PS_MAIN_REALGRAY(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	if (Out.vColor.a < 0.01f)
		discard;

	Out.vColor.rg = Out.vColor.b;


	return Out;
}

PS_OUT PS_MAIN_GRAYBLUE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	if (Out.vColor.a < 0.01f)
		discard;

	if (Out.vColor.r > 0.3f || Out.vColor.g > 0.3f || Out.vColor.b > 0.3f)
		Out.vColor.rg = Out.vColor.b;

	
	return Out;
}

PS_OUT PS_MAIN_ALPHA(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	if (Out.vColor.a < 0.01f)
		discard;

	return Out;
}

PS_OUT_DEFFERED PS_MAIN_ALPHA_DEFFERED(PS_IN In)
{
	PS_OUT_DEFFERED		Out = (PS_OUT_DEFFERED)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	Out.vSecondColor = Out.vColor;
	Out.vMask = (vector)1.f;
	if (Out.vColor.a < 0.01f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_ALPHACOLOR(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	if (Out.vColor.a < 0.01f)
		discard;
	if (Out.vColor.r < 0.2f && Out.vColor.g < 0.2f && Out.vColor.b < 0.2f)
		discard;
	Out.vColor.a = float(g_iHitAlpha) / 255.f;
	Out.vColor.rgb = g_vColor;
	if (Out.vColor.a < 0.f)
		Out.vColor.a = 0.f;

	return Out;
}

PS_OUT_DEFFERED PS_MAIN_ALPHACOLOR_DEFFERED(PS_IN In)
{
	PS_OUT_DEFFERED		Out = (PS_OUT_DEFFERED)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	if (Out.vColor.a < 0.01f)
		discard;
	//if (Out.vColor.r < 0.2f && Out.vColor.g < 0.2f && Out.vColor.b < 0.2f)
	//	discard;
	Out.vColor.a = float(g_iHitAlpha) / 255.f;
	Out.vColor.rgb *= g_vColor;
	if (Out.vColor.a < 0.f)
		Out.vColor.a = 0.f;
	Out.vSecondColor = Out.vColor;
	Out.vMask = (vector)0.f;
	return Out;
}

PS_OUT PS_MAIN_COOLTIME(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	if (Out.vColor.a < 0.1f)
		discard;

	Out.vColor = vector(0.79f, 0.54f, 0.1f, 0.7f);

	if (In.vTexUV.y < g_CurrentHP_UV)
		discard;

	return Out;
}

PS_OUT PS_MAIN_ALPHADOWN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vColor.a -= float(g_iHitAlpha) / 255.f;

	if (Out.vColor.a < 0.01f)
		discard;

	return Out;
}

PS_OUT_DEFFERED PS_MAIN_ALPHADOWN_DEFFERED(PS_IN In)
{
	PS_OUT_DEFFERED		Out = (PS_OUT_DEFFERED)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vColor.a -= float(g_iHitAlpha) / 255.f;
	Out.vSecondColor = Out.vColor;
	Out.vMask = (vector)0.f;
	if (Out.vColor.a < 0.01f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_NOISE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;
	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	vector		vNoiseColor = g_SourTexture.Sample(DefaultSampler, In.vTexUV + float2(0.f, g_AddNoiseUV));
	
	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV + vNoiseColor.r * 0.25f);

	if (Out.vColor.a < 0.001f)
		discard;

	return Out;
}

PS_OUT_DEFFERED PS_MAIN_NOISE_DEFFERED(PS_IN In)
{
	PS_OUT_DEFFERED		Out = (PS_OUT_DEFFERED)0;
	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	vector		vNoiseColor = g_SourTexture.Sample(DefaultSampler, In.vTexUV + float2(0.f, g_AddNoiseUV));

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV + vNoiseColor.r * 0.25f);
	Out.vSecondColor = Out.vColor;
	if (Out.vColor.a == 0.f)
		discard;
	Out.vMask = (vector)1.f;
	return Out;
}

PS_OUT_DEFFERED PS_MAIN_ADDCOLOR_DEFFERED(PS_IN In)
{
	PS_OUT_DEFFERED		Out = (PS_OUT_DEFFERED)0;
	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vColor += vector(g_vColor, g_iHitAlpha / 255.f);
	Out.vSecondColor = Out.vColor;
	Out.vMask = (vector)0.f;
	return Out;
}

PS_OUT_DEFFERED PS_MAIN_MULCOLOR_DEFFERED(PS_IN In)
{
	PS_OUT_DEFFERED		Out = (PS_OUT_DEFFERED)0;
	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vColor *= vector(g_vColor, g_iHitAlpha / 255.f);
	Out.vSecondColor = Out.vColor;
	Out.vMask = (vector)0.f;
	return Out;
}

PS_OUT_DEFFERED PS_MAIN_NOISE_X_DEFFERED(PS_IN In)
{
	PS_OUT_DEFFERED		Out = (PS_OUT_DEFFERED)0;
	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	vector		vNoiseColor = g_SourTexture.Sample(DefaultSampler, In.vTexUV + float2(g_AddNoiseUV, 0.f));

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV + vNoiseColor.r * 0.25f);
	Out.vSecondColor = Out.vColor;
	Out.vMask = (vector)0.f;
	return Out;
}

PS_OUT_DEFFERED PS_MAIN_NOISE_ROTATION_DEFFERED(PS_IN In)
{
	PS_OUT_DEFFERED		Out = (PS_OUT_DEFFERED)0;

	//vector vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);


	float fRadian = radians(g_AddNoiseUV);

	float2x2 RotationMatrix = float2x2(cos(fRadian), -sin(fRadian), sin(fRadian), cos(fRadian));
	float2 uv = In.vTexUV - float2(0.5f, 0.5f);
	uv = mul(uv, RotationMatrix);
	uv += float2(0.5f, 0.5f);
	vector		vNoiseColor = g_SourTexture.Sample(DefaultSampler, uv);

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV + vNoiseColor.r * 0.25f);
	Out.vSecondColor = Out.vColor;
	if (Out.vColor.a < 0.1f)
		discard;
	Out.vMask = (vector)0.f;
	return Out;
}


PS_OUT_DEFFERED PS_MAIN_RECT_DISTORTION(PS_IN In)
{
	PS_OUT_DEFFERED		Out = (PS_OUT_DEFFERED)0;

	vector	vNoiseColor = g_SourTexture.Sample(DefaultSampler, In.vTexUV + float2(0.f, g_AddNoiseUV));
	vector	vColor = vector(0.f, 0.f, 0.f, 0.f);
	vector	vWindow = g_WindowTexture.Sample(DefaultSampler, float2(In.vPosition.x / 1280.f, In.vPosition.y / 720.f) + vNoiseColor.r * g_DistortionPower);

	Out.vColor = vColor.a * vColor + (1.f - vColor.a) * vWindow;
	Out.vSecondColor = Out.vColor;
	Out.vMask = (vector)0.f;
	return Out;
}

PS_OUT_DEFFERED PS_MAIN_RECT_MIXMASK_MOVEUV(PS_IN In)
{
	PS_OUT_DEFFERED		Out = (PS_OUT_DEFFERED)0;

	vector	vMixTex = g_SourTexture.Sample(DefaultSampler, In.vTexUV + float2(0.f, g_AddNoiseUV));
	vector	vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vColor = vColor.a * vColor + (1.f - vColor.a) * vMixTex;
	Out.vSecondColor = Out.vColor;
	Out.vMask = (vector)0.f;
	return Out;
}

PS_OUT_DEFFERED PS_MAIN_ADDTEXTURECOLOR_DEFFERED(PS_IN In)
{
	PS_OUT_DEFFERED		Out = (PS_OUT_DEFFERED)0;

	float fRadian = radians(g_AddNoiseUV);

	float2x2 RotationMatrix = float2x2(cos(fRadian), -sin(fRadian), sin(fRadian), cos(fRadian));
	float2 uv = In.vTexUV - float2(0.5f, 0.5f);
	uv = mul(uv, RotationMatrix);
	uv += float2(0.5f, 0.5f);
	vector		vNoiseColor = g_WindowTexture.Sample(DefaultSampler, uv);

	vector vSourColor = g_SourTexture.Sample(DefaultSampler, In.vTexUV + vNoiseColor.r * 0.05f);
	vector vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vColor = vSourColor + vector(vColor.rgb, 0.f);
	if (0.1f >= Out.vColor.a)
		discard;

	Out.vSecondColor = Out.vColor;
	Out.vMask = (vector)0.f;
	return Out;
}

technique11		DefaultTechnique
{
	pass Rect_Blend	//0
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_RECT_DEFFERED();
	}

	pass Rect_NonBlend //1
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_RECT_DEFFERED();
	}

	pass Rect_WireFrame //2
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(FillMode_Wireframe);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_RECT_DEFFERED();
	}

	pass Rect_UIBlend //3
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_RECT();
	}

	pass Rect_UI_HP //4
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_HP();
	}

	pass Sprite //5
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);

		VertexShader = compile vs_5_0 VS_MAIN_SPRITE();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BULLETTESLA();
	}

	pass SpriteXY //6
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_SPRITEXY();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_RECT_DEFFERED();
	}

	pass Sprite4X4 //7
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_SPRITE4X4();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_RECT_DEFFERED();
	}

	pass Rect_Monster_HP //8
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_HP();
	}

	pass HIT //9
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_HIT();
	}

	pass Ray //10
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_HIT();
	}
	pass None //11
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_RECT_DEFFERED();
	}
	pass Victory //12
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_VICTORY();
	}
	pass Sprite4X2 //13
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_SPRITE4X2();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_RECT();
	}

	pass GrayRed //14
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_GRAY();
	}
	pass GrayBlue //15
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_GRAYBLUE();
	}
	pass SimpleAlpha //16
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHA();
	}
	pass ColorAlpha //17
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHACOLOR_DEFFERED();
	}

	pass CullNoneDecreaseAlpha //18
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_VICTORY_DEFFERED();
	}

	pass CoolTime //19
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_COOLTIME();
	}

	pass FinalSprite //20
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);

		VertexShader = compile vs_5_0 VS_MAIN_FINAL_SPRITE();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHA_DEFFERED();
	}

	pass Rect_UINonBlend //21
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_RECT();
	}
	pass SpriteAlpha //22
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_FINAL_SPRITE();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHADOWN_DEFFERED();
	}
	pass MinusAlpha //23
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHADOWN_DEFFERED();
	}
	pass RealAlpha //24
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHA_DEFFERED();
	}
	pass NoiseUV_Plus_R //25
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NOISE_DEFFERED();
	}

	pass Add_Color //26
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ADDCOLOR_DEFFERED();
	}
	pass Mul_Color //27
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MULCOLOR_DEFFERED();
	}
	pass NoiseUV_Plus_X //28
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NOISE_X_DEFFERED();
	}
	pass Kappa //29
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_SPRITEXY();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NOISE_DEFFERED();
	}
	pass NoiseRotation //30
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NOISE_ROTATION_DEFFERED();
	}

	pass Distortion //31
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_RECT_DISTORTION();
	}
	pass Distortion_UI //32
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_RECT_DISTORTION();
	}
	pass Mix_MoveUV //33
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_RECT_MIXMASK_MOVEUV();
	}
	pass RealGray //34
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_REALGRAY();
	}
	pass Add_Texture //35
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ADDTEXTURECOLOR_DEFFERED();
	}
	pass FinalSpriteAlpha //36
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_none);

		VertexShader = compile vs_5_0 VS_MAIN_FINAL_SPRITE();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_RECTALPHA();
	}
}