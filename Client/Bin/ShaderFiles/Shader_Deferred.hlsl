cbuffer LightDesc
{
	vector			g_vLightDir;
	vector			g_vLightPos;
	float			g_fLightRange;
	vector			g_vLightDiffuse;
	vector			g_vLightAmbient;
	vector			g_vLightSpecular;
	vector			g_vCamPosition;
};

cbuffer MtrlDesc
{
	vector			g_vMtrlDiffuse = (vector)1.f;
	vector			g_vMtrlAmbient = vector(0.4f, 0.4f, 0.4f, 1.f);
	vector			g_vMtrlSpecular = (vector)1.f;
};

cbuffer Matrices
{
	matrix			g_ProjMatrixInv;
	matrix			g_ViewMatrixInv;
};

cbuffer Weight
{
	float g_fWeight0 = 1.0f;
	float g_fWeight1 = 0.9f;
	float g_fWeight2 = 0.55f;
	float g_fWeight3 = 0.18f;
	float g_fWeight4 = 0.1f;
	float g_fTexelPower = 4.f;
};

cbuffer CatoonRendering
{
	float	mask[9] = {
		-1.f, -1.f, -1.f,
		-1.f, 8.f, -1.f,
		-1.f, -1.f, -1.f,
	};

	float coord[3] = { -1.f, 0.f, 1.f };
	float divider = 1.f;
	float WINCX = 1280.f;
	float WINCY = 720.f;
};


texture2D			g_TargetTexture;
texture2D			g_DiffuseTexture;
texture2D			g_NormalTexture;
texture2D			g_DepthTexture;
texture2D			g_ShadeTexture;
texture2D			g_OutlineTexture;
texture2D			g_NoiseTexture;

sampler DefaultSampler = sampler_state
{
	// D3D11_SAMPLER_DESC
	filter = min_mag_mip_linear;
	AddressU = wrap;
	AddressV = wrap;
};

cbuffer	RenderingPipeLine
{
	matrix			g_WorldMatrix;
	matrix			g_ViewMatrix;
	matrix			g_ProjMatrix;
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

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

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

PS_OUT PS_MAIN_RECT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_TargetTexture.Sample(DefaultSampler, In.vTexUV);
	if (Out.vColor.a == 0.f)
		discard;
	return Out;
}

struct PS_OUT_LIGHT
{
	vector		vShade : SV_TARGET0;
	vector		vOutline : SV_TARGET1;
};


struct PS_OUT_BLENDTEX
{
	vector		vColor : SV_TARGET0;
	vector		vSecondColor : SV_TARGET1;
	vector		vMask : SV_TARGET2;
};

PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
	PS_OUT_LIGHT	Out = (PS_OUT_LIGHT)0;

	vector	vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);
	vector	vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexUV);
	float	fViewZ = vDepthDesc.x * 300.f;

	// 노말을 다시 -1~1 범위로 복구
	vector	vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
	float	fShade = max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f);
	//Out.vShade = saturate(dot(normalize(g_vLightDir) * -1.f, vNormal));
	
	//Out.vShade = g_vLightDiffuse * saturate(dot(normalize(g_vLightDir) * -1.f, vNormal)) + (g_vLightAmbient * g_vMtrlAmbient);

	if (vDepthDesc.z > 0.5f)
	{
		//vector vWorldPos;

		//vWorldPos.x = (In.vTexUV.x * 2.f - 1.f) * fViewZ;
		//vWorldPos.y = (In.vTexUV.y * -2.f + 1.f) * fViewZ;
		//vWorldPos.z = vDepthDesc.y * fViewZ;
		//vWorldPos.w = 1.f * fViewZ;

		//vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
		//vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

		//vector vLookCam = normalize(g_vCamPosition - vWorldPos);

		//float fLine = (1.f - saturate(dot(vNormal, vLookCam)));
		//if (fLine > vDepthDesc.z)
		//{
		//	
		//	//if (1.f < fGray)
		//	//	Out.vOutline = vector(0.f, 0.f, 0.f, 1.f);
		//	//else
		//	//	Out.vOutline = vector(1.f, 1.f, 1.f, 1.f);


		//	//Out.vOutline = vector(0.f, 0.f, 0.f, 1.f);
		//}
		//else
		//	Out.vOutline = (vector)1.f;

		Out.vShade = g_vLightDiffuse * saturate(ceil(int(fShade * 2.f))) / 2.f + (g_vLightAmbient * vector(1.f,1.f,1.f,1.f));
	}
	else
	{
		//Out.vOutline = (vector)1.f;
		if (0.f < vDepthDesc.w)
			Out.vShade = g_vLightDiffuse * saturate(ceil(int(fShade * 8.f)) / 8.f + (g_vLightAmbient * g_vMtrlAmbient));
		else 
			Out.vShade = g_vLightDiffuse * saturate(fShade) + (g_vLightAmbient * g_vMtrlAmbient);
	}
	//vector vOutLine = vector(1.f, 1.f, 1.f, 1.f);

	vector		vColor = (vector)0.f;
	float3		vGrayScale = float3(0.3f, 0.59f, 0.11f);

	for (uint i = 0; i < 9; i++)
	{
		vector	vNormalCatoon = g_NormalTexture.Sample(DefaultSampler, In.vTexUV + float2(coord[i % 3] / WINCX, coord[i / 3] / WINCY));
		vector	vNormalC = vector(vNormalCatoon.xyz * 2.f - 1.f, 0.f);
		vColor += mask[i] * vNormalC;
	}
	float fGray = 1.f - dot(vColor.rgb, vGrayScale);
	Out.vOutline = vector(fGray, fGray, fGray, 1.f);
	Out.vShade.a = 1.f;

	return Out;	
}

PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
	PS_OUT_LIGHT	Out = (PS_OUT_LIGHT)0;

	vector		vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexUV);
	float		fViewZ = vDepthDesc.x * 300.f;

	vector		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	vector		vWorldPos;

	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 * w나누기. */
	vWorldPos.x = (In.vTexUV.x * 2.f - 1.f);
	vWorldPos.y = (In.vTexUV.y * -2.f + 1.f);
	vWorldPos.z = vDepthDesc.y; /* 0 ~ 1 */
	vWorldPos.w = 1.f;

	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 */
	vWorldPos.x = (In.vTexUV.x * 2.f - 1.f) * fViewZ;
	vWorldPos.y = (In.vTexUV.y * -2.f + 1.f) * fViewZ;
	vWorldPos.z = vDepthDesc.y * fViewZ; /* 0 ~ f */
	vWorldPos.w = 1.f * fViewZ;

	/* 로컬위치 * 월드행렬 * 뷰행렬 */
	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* 로컬위치 * 월드행렬 */
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

	vector		vLightDir = vWorldPos - g_vLightPos;
	float		fDistance = length(vLightDir);

	float		fAtt = max(g_fLightRange - fDistance, 0.f) / g_fLightRange;
	float		fShade = saturate(dot(normalize(vLightDir) * -1.f, vNormal));

	if (vDepthDesc.z > 0.5f)
	{
		Out.vShade = g_vLightDiffuse * saturate(ceil(int(fShade * 2.f))) / 2.f + (g_vLightAmbient * vector(1.f, 1.f, 1.f, 1.f)) * fAtt;
	}
	else
	{
		if (0.f < vDepthDesc.w)
			Out.vShade = g_vLightDiffuse * saturate(ceil(int(fShade * 8.f)) / 8.f + (g_vLightAmbient * g_vMtrlAmbient)) * fAtt;
		else
			Out.vShade = (g_vLightDiffuse * saturate(dot(normalize(vLightDir) * -1.f, vNormal)) + (g_vLightAmbient * g_vMtrlAmbient)) * fAtt;
			//Out.vShade = g_vLightDiffuse * saturate(fShade) + (g_vLightAmbient * g_vMtrlAmbient) * fAtt;
	}

	//Out.vShade = (g_vLightDiffuse * saturate(dot(normalize(vLightDir) * -1.f, vNormal)) + (g_vLightAmbient * g_vMtrlAmbient)) * fAtt;

	Out.vShade.a = 1.f;

	//vector		vColor = (vector)0.f;
	//float3		vGrayScale = float3(0.3f, 0.59f, 0.11f);

	//for (uint i = 0; i < 9; i++)
	//{
	//	vector	vNormalCatoon = g_NormalTexture.Sample(DefaultSampler, In.vTexUV + float2(coord[i % 3] / WINCX, coord[i / 3] / WINCY));
	//	vector	vNormalC = vector(vNormalCatoon.xyz * 2.f - 1.f, 0.f);
	//	vColor += mask[i] * vNormalC;
	//}
	//float fGray = 1.f - dot(vColor.rgb, vGrayScale);
	//Out.vOutline = vector(fGray, fGray, fGray, 1.f);

	//Out.vOutline = (vector)1.f;

	//vector		vReflect = reflect(normalize(vLightDir), vNormal);


	//vector		vLook = vWorldPos - g_vCamPosition;

	//Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(saturate(dot(normalize(vReflect) * -1.f, normalize(vLook))), 30.f) * fAtt;
	//Out.vSpecular.a = 0.f;

	return Out;
}

struct PS_OUT_COLOR
{
	vector		vFirstColor : SV_TARGET0;
	vector		vSecondColor : SV_TARGET1;
};

PS_OUT_COLOR PS_MAIN_MAKEALLTEXTURE(PS_IN In)
{
	PS_OUT_COLOR		Out = (PS_OUT_COLOR)0;

	vector		vDiffuseDesc = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vShadeDesc = g_ShadeTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vFirstColor = vDiffuseDesc * vShadeDesc;
	Out.vSecondColor = Out.vFirstColor;
	if (Out.vFirstColor.a == 0.0f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_DOWNSAMPLING(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 vTexUV = In.vTexUV;
	uint iTexUV_X = uint(vTexUV.x * 1280.f);
	uint iTexUV_Y = uint(vTexUV.y * 720.f);

	vTexUV = float2((float)iTexUV_X / 640.f, (float)iTexUV_Y / 360.f);

	vector	vMask = g_DepthTexture.Sample(DefaultSampler, vTexUV);

	vector vColor = g_DiffuseTexture.Sample(DefaultSampler, vTexUV);
	Out.vColor = vColor * vMask.r;

	return Out;
}

PS_OUT PS_MAIN_UPSAMPLING(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	if (Out.vColor.a == 0.f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_HORIZENBLUR(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 vTexUV = In.vTexUV;
	uint iTexUV_X = uint(vTexUV.x * 1280.f);
	uint iTexUV_Y = uint(vTexUV.y * 720.f);

	vTexUV = float2((float)iTexUV_X / 640.f, (float)iTexUV_Y / 360.f);
	float2 vUV[9];
	for (uint i = 0; i < 9; i++)
	{
		vUV[i] = vTexUV + float2((-4.f + (float)i) / 640.f * g_fTexelPower, 0.f);
	}

	vector	vColor = float4(0.f, 0.f, 0.f, 0.f);

	if (vUV[0].x >= 0.f && vUV[0].x <= 1.f)
		vColor += g_DiffuseTexture.Sample(DefaultSampler, vUV[0]) * g_fWeight4;
	if (vUV[1].x >= 0.f && vUV[1].x <= 1.f)
		vColor += g_DiffuseTexture.Sample(DefaultSampler, vUV[1]) * g_fWeight3;
	if (vUV[2].x >= 0.f && vUV[2].x <= 1.f)
		vColor += g_DiffuseTexture.Sample(DefaultSampler, vUV[2]) * g_fWeight2;
	if (vUV[3].x >= 0.f && vUV[3].x <= 1.f)
		vColor += g_DiffuseTexture.Sample(DefaultSampler, vUV[3]) * g_fWeight1;
	if (vUV[4].x >= 0.f && vUV[4].x <= 1.f)
		vColor += g_DiffuseTexture.Sample(DefaultSampler, vUV[4]) * g_fWeight0;
	if (vUV[5].x >= 0.f && vUV[5].x <= 1.f)
		vColor += g_DiffuseTexture.Sample(DefaultSampler, vUV[5]) * g_fWeight1;
	if (vUV[6].x >= 0.f && vUV[6].x <= 1.f)
		vColor += g_DiffuseTexture.Sample(DefaultSampler, vUV[6]) * g_fWeight2;
	if (vUV[7].x >= 0.f && vUV[7].x <= 1.f)
		vColor += g_DiffuseTexture.Sample(DefaultSampler, vUV[7]) * g_fWeight3;
	if (vUV[8].x >= 0.f && vUV[8].x <= 1.f)
		vColor += g_DiffuseTexture.Sample(DefaultSampler, vUV[8]) * g_fWeight4;


	//vector	vColor = float4(0.f, 0.f, 0.f, 0.f);

	//vColor += g_DiffuseTexture.Sample(DefaultSampler, vTexUV + float2(-4.f / 640.f * g_fTexelPower, 0.f)) * g_fWeight4;
	//vColor += g_DiffuseTexture.Sample(DefaultSampler, vTexUV + float2(-3.f / 640.f * g_fTexelPower, 0.f)) * g_fWeight3;
	//vColor += g_DiffuseTexture.Sample(DefaultSampler, vTexUV + float2(-2.f / 640.f * g_fTexelPower, 0.f)) * g_fWeight2;
	//vColor += g_DiffuseTexture.Sample(DefaultSampler, vTexUV + float2(-1.f / 640.f * g_fTexelPower, 0.f)) * g_fWeight1;
	//vColor += g_DiffuseTexture.Sample(DefaultSampler, vTexUV + float2(0.f, 0.f)) * g_fWeight0;
	//vColor += g_DiffuseTexture.Sample(DefaultSampler, vTexUV + float2(1.f / 640.f * g_fTexelPower, 0.f)) * g_fWeight1;
	//vColor += g_DiffuseTexture.Sample(DefaultSampler, vTexUV + float2(2.f / 640.f * g_fTexelPower, 0.f)) * g_fWeight2;
	//vColor += g_DiffuseTexture.Sample(DefaultSampler, vTexUV + float2(3.f / 640.f * g_fTexelPower, 0.f)) * g_fWeight3;
	//vColor += g_DiffuseTexture.Sample(DefaultSampler, vTexUV + float2(4.f / 640.f * g_fTexelPower, 0.f)) * g_fWeight4;

	if (vColor.a == 0.f)
		discard;
	//vColor.a = 1.f;
	Out.vColor = vColor;

	return Out;	
}

PS_OUT PS_MAIN_VERTICALBLUR(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 vTexUV = In.vTexUV;
	uint iTexUV_X = uint(vTexUV.x * 1280.f);
	uint iTexUV_Y = uint(vTexUV.y * 720.f);

	vTexUV = float2((float)iTexUV_X / 640.f, (float)iTexUV_Y / 360.f);

	vector	vColor = float4(0.f, 0.f, 0.f, 0.f);

	vColor += g_DiffuseTexture.Sample(DefaultSampler, vTexUV + float2(0.f, -4.f / 360.f * g_fTexelPower)) * g_fWeight4;
	vColor += g_DiffuseTexture.Sample(DefaultSampler, vTexUV + float2(0.f, -3.f / 360.f * g_fTexelPower)) * g_fWeight3;
	vColor += g_DiffuseTexture.Sample(DefaultSampler, vTexUV + float2(0.f, -2.f / 360.f * g_fTexelPower)) * g_fWeight2;
	vColor += g_DiffuseTexture.Sample(DefaultSampler, vTexUV + float2(0.f, -1.f / 360.f * g_fTexelPower)) * g_fWeight1;
	vColor += g_DiffuseTexture.Sample(DefaultSampler, vTexUV + float2(0.f, 0.f)) * g_fWeight0;
	vColor += g_DiffuseTexture.Sample(DefaultSampler, vTexUV + float2(0.f, 1.f / 360.f * g_fTexelPower)) * g_fWeight1;
	vColor += g_DiffuseTexture.Sample(DefaultSampler, vTexUV + float2(0.f, 2.f / 360.f * g_fTexelPower)) * g_fWeight2;
	vColor += g_DiffuseTexture.Sample(DefaultSampler, vTexUV + float2(0.f, 3.f / 360.f * g_fTexelPower)) * g_fWeight3;
	vColor += g_DiffuseTexture.Sample(DefaultSampler, vTexUV + float2(0.f, 4.f / 360.f * g_fTexelPower)) * g_fWeight4;

	if (vColor.a == 0.f)
		discard;
	//vColor.a = 1.f;

	Out.vColor = vColor;

	return Out;
}

PS_OUT PS_MAIN_NONBLEND(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vAllDesc = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vBlurDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vBlurMask = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vOutlineDesc = g_OutlineTexture.Sample(DefaultSampler, In.vTexUV);
	if (vAllDesc.a == 0.0f)
		discard;
	//if (vBlurMask.r == 0.f)
		Out.vColor = (vAllDesc + vBlurDesc) * vOutlineDesc;
		Out.vColor.a = 1.f;
	//else
	//	Out.vColor = vAllDesc * vOutlineDesc;
	return Out;
}

PS_OUT PS_MAIN_BLEND(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vAllDesc = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vBlurDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexUV);

	if (vAllDesc.a == 0.0f)
		discard;

	Out.vColor = vAllDesc/* + vBlurDesc*/;

	return Out;
}



PS_OUT_BLENDTEX PS_MAIN_NONBLENDTEXTURE(PS_IN In)
{
	PS_OUT_BLENDTEX		Out = (PS_OUT_BLENDTEX)0;

	Out.vColor = g_TargetTexture.Sample(DefaultSampler, In.vTexUV);

	if (Out.vColor.a == 0.f)
		discard;
	Out.vSecondColor = Out.vColor;

	Out.vMask = vector(0.f, 0.f, 0.f, 0.f);

	return Out;
}

BlendState	NonBlending
{
	BlendEnable[0] = false;
};

BlendState	Blending_One
{
	BlendEnable[0] = true;
	BlendEnable[1] = true;

	BlendOp = Add;
	SrcBlend = one;
	DestBlend = one;


};

DepthStencilState  NonZTestAndWriteState
{
	DepthEnable = false;
	DepthWriteMask = zero;
};

RasterizerState CullMode_ccw
{
	FillMode = Solid;
	CullMode = back;
	FrontCounterClockwise = false;
};

technique11		DefaultTechnique
{
	pass DebugBuffer
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_RECT();
	}	

	pass Light_Directional
	{
		SetBlendState(Blending_One, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
	}

	pass Light_Point
	{
		SetBlendState(Blending_One, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_POINT();
	}

	pass NonBlendBlur
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NONBLEND();
	}

	pass MakeAllTexture
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MAKEALLTEXTURE();
	}

	pass DownSampling
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DOWNSAMPLING();
	}

	pass UpSampling
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_UPSAMPLING();
	}
	pass HorizenBlur
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_HORIZENBLUR();
	}
	pass VerticalBlur
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_VERTICALBLUR();
	}
	pass BlendBlur
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLEND();
	}
	pass SettingNonBlend
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NONBLENDTEXTURE();
	}
	//pass NonBlendTexture
	//{
	//	SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
	//	SetDepthStencilState(NonZTestAndWriteState, 0);
	//	SetRasterizerState(CullMode_ccw);

	//	VertexShader = compile vs_5_0 VS_MAIN();
	//	GeometryShader = NULL;
	//	PixelShader = compile ps_5_0 PS_MAIN_NONBLENDTEXTURE();
	//}
}