////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// RenderCore includes
#include "RenderResource.h"
#include "GlobalShader.h"
#include "ShaderParameterUtils.h"

template<bool HasColor, bool HasUv0, bool HasUv1, bool HasUv2, bool HasCoverage>
class FNoesisVertexDeclaration : public FRenderResource
{
public:

	FVertexDeclarationRHIRef VertexDeclarationRHI;

	typedef FVector2D FNoesisVertexPosition;
	typedef FColor FNoesisVertexColor;
	typedef FVector2D FNoesisVertexUv;
	typedef float FNoesisVertexCoverage;

	static const uint16 Stride = sizeof(FNoesisVertexPosition) + (HasColor ? sizeof(FNoesisVertexColor) : 0) + (HasUv0 ? sizeof(FNoesisVertexUv) : 0) + (HasUv1 ? sizeof(FNoesisVertexUv) : 0) + (HasUv2 ? sizeof(FNoesisVertexUv) : 0) + (HasCoverage ? sizeof(FNoesisVertexCoverage) : 0);

	virtual ~FNoesisVertexDeclaration()
	{
	}

	virtual void InitRHI() override
	{
		FVertexDeclarationElementList Elements;
		uint8 Offset = 0;
		Elements.Add(FVertexElement(0, Offset, VET_Float2, 0, Stride));
		Offset += sizeof(FNoesisVertexPosition);
		if (HasColor)
		{
			Elements.Add(FVertexElement(0, Offset, VET_Color, 1, Stride));
			Offset += sizeof(FNoesisVertexColor);
		}
		if (HasUv0)
		{
			Elements.Add(FVertexElement(0, Offset, VET_Float2, 2, Stride));
			Offset += sizeof(FNoesisVertexUv);
		}
		if (HasUv1)
		{
			Elements.Add(FVertexElement(0, Offset, VET_Float2, 3, Stride));
			Offset += sizeof(FNoesisVertexUv);
		}
		if (HasUv2)
		{
			Elements.Add(FVertexElement(0, Offset, VET_UShort4N, 4, Stride));
			Offset += sizeof(FNoesisVertexUv);
		}
		if (HasCoverage)
		{
			Elements.Add(FVertexElement(0, Offset, VET_Float1, 5, Stride));
			Offset += sizeof(FNoesisVertexCoverage);
		}
		check(Offset == Stride);
		VertexDeclarationRHI = RHICreateVertexDeclaration(Elements);
	}

	virtual void ReleaseRHI() override
	{
		VertexDeclarationRHI.SafeRelease();
	}
};

typedef FNoesisVertexDeclaration<false, false, false, false, false> FNoesisPosVertexDeclaration;
typedef FNoesisVertexDeclaration<true, false, false, false, false> FNoesisPosColorVertexDeclaration;
typedef FNoesisVertexDeclaration<false, true, false, false, false> FNoesisPosTex0VertexDeclaration;
typedef FNoesisVertexDeclaration<true, false, false, false, true> FNoesisPosColorCoverageVertexDeclaration;
typedef FNoesisVertexDeclaration<false, true, false, false, true> FNoesisPosTex0CoverageVertexDeclaration;
typedef FNoesisVertexDeclaration<true, false, true, false, false> FNoesisPosColorTex1VertexDeclaration;
typedef FNoesisVertexDeclaration<false, true, true, false, false> FNoesisPosTex0Tex1VertexDeclaration;
typedef FNoesisVertexDeclaration<true, false, true, true, false> FNoesisPosColorTex1Tex2VertexDeclaration;
typedef FNoesisVertexDeclaration<false, true, true, true, false> FNoesisPosTex0Tex1Tex2VertexDeclaration;

extern TGlobalResource<FNoesisPosVertexDeclaration> GNoesisPosVertexDeclaration;
extern TGlobalResource<FNoesisPosColorVertexDeclaration> GNoesisPosColorVertexDeclaration;
extern TGlobalResource<FNoesisPosTex0VertexDeclaration> GNoesisPosTex0VertexDeclaration;
extern TGlobalResource<FNoesisPosColorCoverageVertexDeclaration> GNoesisPosColorCoverageVertexDeclaration;
extern TGlobalResource<FNoesisPosTex0CoverageVertexDeclaration> GNoesisPosTex0CoverageVertexDeclaration;
extern TGlobalResource<FNoesisPosColorTex1VertexDeclaration> GNoesisPosColorTex1VertexDeclaration;
extern TGlobalResource<FNoesisPosTex0Tex1VertexDeclaration> GNoesisPosTex0Tex1VertexDeclaration;
extern TGlobalResource<FNoesisPosColorTex1Tex2VertexDeclaration> GNoesisPosColorTex1Tex2VertexDeclaration;
extern TGlobalResource<FNoesisPosTex0Tex1Tex2VertexDeclaration> GNoesisPosTex0Tex1Tex2VertexDeclaration;

BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FNoesisVSConstants, )
SHADER_PARAMETER(FMatrix, projectionMtx)
END_GLOBAL_SHADER_PARAMETER_STRUCT()

BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FNoesisTextureSize, )
SHADER_PARAMETER(FVector4, textureSize)
END_GLOBAL_SHADER_PARAMETER_STRUCT()

BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FNoesisPSConstants, )
SHADER_PARAMETER(FVector4, rgba)
SHADER_PARAMETER_ARRAY(FVector4, radialGrad, [2])
SHADER_PARAMETER(float, opacity)
END_GLOBAL_SHADER_PARAMETER_STRUCT()

BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FNoesisEffects, )
SHADER_PARAMETER_ARRAY(FVector4, effectsParams, [8])
END_GLOBAL_SHADER_PARAMETER_STRUCT()

class FNoesisVSBase : public FGlobalShader
{
	DECLARE_INLINE_TYPE_LAYOUT(FNoesisVSBase, NonVirtual);
public:
	FNoesisVSBase(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{
		VSConstantsBuffer.Bind(Initializer.ParameterMap, FNoesisVSConstants::StaticStructMetadata.GetShaderVariableName());
		TextureSizeBuffer.Bind(Initializer.ParameterMap, FNoesisTextureSize::StaticStructMetadata.GetShaderVariableName());
	}

	FNoesisVSBase()
	{
	}

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters) { return true; }

	void SetVSConstants(FRHICommandList& RHICmdList, const FUniformBufferRHIRef& VSConstants)
	{
		FRHIVertexShader* ShaderRHI = RHICmdList.GetBoundVertexShader();

		check(VSConstantsBuffer.IsBound());
		SetUniformBufferParameter(RHICmdList, ShaderRHI, VSConstantsBuffer, VSConstants);
	}

	void SetTextureSize(FRHICommandList& RHICmdList, const FUniformBufferRHIRef& TextureSize)
	{
		FRHIVertexShader* ShaderRHI = RHICmdList.GetBoundVertexShader();

		check(TextureSizeBuffer.IsBound());
		SetUniformBufferParameter(RHICmdList, ShaderRHI, TextureSizeBuffer, TextureSize);
	}

	LAYOUT_FIELD(FShaderUniformBufferParameter, VSConstantsBuffer);
	LAYOUT_FIELD(FShaderUniformBufferParameter, TextureSizeBuffer)
};

template<bool HasColor, bool HasUv0, bool HasUv1, bool HasUv2, bool HasCoverage, bool GenSt1>
class FNoesisVS : public FNoesisVSBase
{
	DECLARE_SHADER_TYPE(FNoesisVS, Global);

	FNoesisVS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FNoesisVSBase(Initializer)
	{
	}

	FNoesisVS()
	{
	}

	static bool ShouldCache(EShaderPlatform Platform)
	{
		return true;
	}

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
		OutEnvironment.SetDefine(TEXT("HAS_COLOR"), HasColor);
		OutEnvironment.SetDefine(TEXT("HAS_UV0"), HasUv0);
		OutEnvironment.SetDefine(TEXT("HAS_UV1"), HasUv1);
		OutEnvironment.SetDefine(TEXT("HAS_UV2"), HasUv2);
		OutEnvironment.SetDefine(TEXT("HAS_COVERAGE"), HasCoverage);
		OutEnvironment.SetDefine(TEXT("GEN_ST1"), GenSt1);
	}
};

// The commas in the template parameter list break the IMPLEMENT_SHADER_TYPE macro, so we need typedefs.
typedef FNoesisVS<false, false, false, false, false, false> FNoesisPosVS;
typedef FNoesisVS<true, false, false, false, false, false> FNoesisPosColorVS;
typedef FNoesisVS<false, true, false, false, false, false> FNoesisPosTex0VS;
typedef FNoesisVS<true, false, false, false, true, false> FNoesisPosColorCoverageVS;
typedef FNoesisVS<false, true, false, false, true, false> FNoesisPosTex0CoverageVS;
typedef FNoesisVS<true, false, true, false, false, false> FNoesisPosColorTex1VS;
typedef FNoesisVS<false, true, true, false, false, false> FNoesisPosTex0Tex1VS;
typedef FNoesisVS<true, false, true, false, false, true> FNoesisPosColorTex1SDFVS;
typedef FNoesisVS<false, true, true, false, false, true> FNoesisPosTex0Tex1SDFVS;
typedef FNoesisVS<true, false, true, true, false, false> FNoesisPosColorTex1Tex2VS;
typedef FNoesisVS<false, true, true, true, false, false> FNoesisPosTex0Tex1Tex2VS;

class FNoesisPSBase : public FGlobalShader
{
	DECLARE_INLINE_TYPE_LAYOUT(FNoesisPSBase, NonVirtual);

public:
	FNoesisPSBase(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{
		PSConstantsBuffer.Bind(Initializer.ParameterMap, FNoesisPSConstants::StaticStructMetadata.GetShaderVariableName());
		TextureSizeBuffer.Bind(Initializer.ParameterMap, FNoesisTextureSize::StaticStructMetadata.GetShaderVariableName());
		EffectsBuffer.Bind(Initializer.ParameterMap, FNoesisEffects::StaticStructMetadata.GetShaderVariableName());
		PatternTexture.Bind(Initializer.ParameterMap, TEXT("patternTex"));
		PatternSampler.Bind(Initializer.ParameterMap, TEXT("patternSampler"));
		RampsTexture.Bind(Initializer.ParameterMap, TEXT("rampsTex"));
		RampsSampler.Bind(Initializer.ParameterMap, TEXT("rampsSampler"));
		ImageTexture.Bind(Initializer.ParameterMap, TEXT("imageTex"));
		ImageSampler.Bind(Initializer.ParameterMap, TEXT("imageSampler"));
		GlyphsTexture.Bind(Initializer.ParameterMap, TEXT("glyphsTex"));
		GlyphsSampler.Bind(Initializer.ParameterMap, TEXT("glyphsSampler"));
		ShadowTexture.Bind(Initializer.ParameterMap, TEXT("shadowTex"));
		ShadowSampler.Bind(Initializer.ParameterMap, TEXT("shadowSampler"));
	}

	FNoesisPSBase()
	{
	}

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters) { return true; }

	void SetPSConstants(FRHICommandList& RHICmdList, const FUniformBufferRHIRef& PSConstants)
	{
		FRHIPixelShader* ShaderRHI = RHICmdList.GetBoundPixelShader();

		check(PSConstantsBuffer.IsBound());
		SetUniformBufferParameter(RHICmdList, ShaderRHI, PSConstantsBuffer, PSConstants);
	}

	void SetTextureSize(FRHICommandList& RHICmdList, const FUniformBufferRHIRef& TextureSize)
	{
		FRHIPixelShader* ShaderRHI = RHICmdList.GetBoundPixelShader();

		check(TextureSizeBuffer.IsBound());
		SetUniformBufferParameter(RHICmdList, ShaderRHI, TextureSizeBuffer, TextureSize);
	}

	void SetEffects(FRHICommandList& RHICmdList, const FUniformBufferRHIRef& Effects)
	{
		FRHIPixelShader* ShaderRHI = RHICmdList.GetBoundPixelShader();

		check(EffectsBuffer.IsBound());
		SetUniformBufferParameter(RHICmdList, ShaderRHI, EffectsBuffer, Effects);
	}

	void SetPatternTexture(FRHICommandList& RHICmdList, FRHITexture* PatternTextureResource, FRHISamplerState* PatternSamplerResource)
	{
		FRHIPixelShader* ShaderRHI = RHICmdList.GetBoundPixelShader();

		SetTextureParameter(RHICmdList, ShaderRHI, PatternTexture, PatternSampler, PatternSamplerResource, PatternTextureResource);
	}

	void SetRampsTexture(FRHICommandList& RHICmdList, FRHITexture* RampsTextureResource, FRHISamplerState* RampsSamplerResource)
	{
		FRHIPixelShader* ShaderRHI = RHICmdList.GetBoundPixelShader();

		SetTextureParameter(RHICmdList, ShaderRHI, RampsTexture, RampsSampler, RampsSamplerResource, RampsTextureResource);
	}

	void SetImageTexture(FRHICommandList& RHICmdList, FRHITexture* ImageTextureResource, FRHISamplerState* ImageSamplerResource)
	{
		FRHIPixelShader* ShaderRHI = RHICmdList.GetBoundPixelShader();

		SetTextureParameter(RHICmdList, ShaderRHI, ImageTexture, ImageSampler, ImageSamplerResource, ImageTextureResource);
	}

	void SetGlyphsTexture(FRHICommandList& RHICmdList, FRHITexture* GlyphsTextureResource, FRHISamplerState* GlyphsSamplerResource)
	{
		FRHIPixelShader* ShaderRHI = RHICmdList.GetBoundPixelShader();

		SetTextureParameter(RHICmdList, ShaderRHI, GlyphsTexture, GlyphsSampler, GlyphsSamplerResource, GlyphsTextureResource);
	}

	void SetShadowTexture(FRHICommandList& RHICmdList, FRHITexture* ShadowTextureResource, FRHISamplerState* ShadowSamplerResource)
	{
		FRHIPixelShader* ShaderRHI = RHICmdList.GetBoundPixelShader();

		SetTextureParameter(RHICmdList, ShaderRHI, ShadowTexture, ShadowSampler, ShadowSamplerResource, ShadowTextureResource);
	}

	LAYOUT_FIELD(FShaderUniformBufferParameter, PSConstantsBuffer)
	LAYOUT_FIELD(FShaderUniformBufferParameter, TextureSizeBuffer)
	LAYOUT_FIELD(FShaderUniformBufferParameter, EffectsBuffer)
	LAYOUT_FIELD(FShaderResourceParameter, PatternTexture)
	LAYOUT_FIELD(FShaderResourceParameter, PatternSampler)
	LAYOUT_FIELD(FShaderResourceParameter, RampsTexture)
	LAYOUT_FIELD(FShaderResourceParameter, RampsSampler)
	LAYOUT_FIELD(FShaderResourceParameter, ImageTexture)
	LAYOUT_FIELD(FShaderResourceParameter, ImageSampler)
	LAYOUT_FIELD(FShaderResourceParameter, GlyphsTexture)
	LAYOUT_FIELD(FShaderResourceParameter, GlyphsSampler)
	LAYOUT_FIELD(FShaderResourceParameter, ShadowTexture)
	LAYOUT_FIELD(FShaderResourceParameter, ShadowSampler)
};

template<Noesis::Shader::Enum Effect, bool PatternSRGB = false>
class FNoesisPS : public FNoesisPSBase
{
	DECLARE_SHADER_TYPE(FNoesisPS, Global);

	FNoesisPS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FNoesisPSBase(Initializer)
	{
	}

	FNoesisPS()
	{
	}

	static bool ShouldCache(EShaderPlatform Platform)
	{
		return true;
	}

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
		OutEnvironment.SetDefine(TEXT("EFFECT"), Effect);

		// MediaTextures don't work correctly if they are not rendered to SRGB textures.
		// We need to convert them on the shaders.
		// MediaTextures are sampled from the pattern textures.
		// The shaders that need to do this conversion are more expensive, so we are still
		// setting SRGB to false when we import textures. But at least this means that
		// user textures should work regardless.
		OutEnvironment.SetDefine(TEXT("PATTERN_SRGB"), PatternSRGB);
	}
};

// The commas in the template parameter list break the IMPLEMENT_SHADER_TYPE macro, so we need typedefs.
typedef FNoesisPS<Noesis::Shader::RGBA> FNoesisRgbaPS;
typedef FNoesisPS<Noesis::Shader::Mask> FNoesisMaskPS;

typedef FNoesisPS<Noesis::Shader::Path_Solid> FNoesisPathSolidPS;
typedef FNoesisPS<Noesis::Shader::Path_Linear> FNoesisPathLinearPS;
typedef FNoesisPS<Noesis::Shader::Path_Radial> FNoesisPathRadialPS;
typedef FNoesisPS<Noesis::Shader::Path_Pattern> FNoesisPathPatternPS;

typedef FNoesisPS<Noesis::Shader::PathAA_Solid> FNoesisPathAaSolidPS;
typedef FNoesisPS<Noesis::Shader::PathAA_Linear> FNoesisPathAaLinearPS;
typedef FNoesisPS<Noesis::Shader::PathAA_Radial> FNoesisPathAaRadialPS;
typedef FNoesisPS<Noesis::Shader::PathAA_Pattern> FNoesisPathAaPatternPS;

typedef FNoesisPS<Noesis::Shader::SDF_Solid> FNoesisSDFSolidPS;
typedef FNoesisPS<Noesis::Shader::SDF_Linear> FNoesisSDFLinearPS;
typedef FNoesisPS<Noesis::Shader::SDF_Radial> FNoesisSDFRadialPS;
typedef FNoesisPS<Noesis::Shader::SDF_Pattern> FNoesisSDFPatternPS;

typedef FNoesisPS<Noesis::Shader::SDF_LCD_Solid> FNoesisSDFLCDSolidPS;
typedef FNoesisPS<Noesis::Shader::SDF_LCD_Linear> FNoesisSDFLCDLinearPS;
typedef FNoesisPS<Noesis::Shader::SDF_LCD_Radial> FNoesisSDFLCDRadialPS;
typedef FNoesisPS<Noesis::Shader::SDF_LCD_Pattern> FNoesisSDFLCDPatternPS;

typedef FNoesisPS<Noesis::Shader::Image_Opacity_Solid> FNoesisImageOpacitySolidPS;
typedef FNoesisPS<Noesis::Shader::Image_Opacity_Linear> FNoesisImageOpacityLinearPS;
typedef FNoesisPS<Noesis::Shader::Image_Opacity_Radial> FNoesisImageOpacityRadialPS;
typedef FNoesisPS<Noesis::Shader::Image_Opacity_Pattern> FNoesisImageOpacityPatternPS;

typedef FNoesisPS<Noesis::Shader::Image_Shadow35V> FNoesisImageShadow35VPS;
typedef FNoesisPS<Noesis::Shader::Image_Shadow63V> FNoesisImageShadow63VPS;
typedef FNoesisPS<Noesis::Shader::Image_Shadow127V> FNoesisImageShadow127VPS;

typedef FNoesisPS<Noesis::Shader::Image_Shadow35H_Solid> FNoesisImageShadow35HSolidPS;
typedef FNoesisPS<Noesis::Shader::Image_Shadow35H_Linear> FNoesisImageShadow35HLinearPS;
typedef FNoesisPS<Noesis::Shader::Image_Shadow35H_Radial> FNoesisImageShadow35HRadialPS;
typedef FNoesisPS<Noesis::Shader::Image_Shadow35H_Pattern> FNoesisImageShadow35HPatternPS;

typedef FNoesisPS<Noesis::Shader::Image_Shadow63H_Solid> FNoesisImageShadow63HSolidPS;
typedef FNoesisPS<Noesis::Shader::Image_Shadow63H_Linear> FNoesisImageShadow63HLinearPS;
typedef FNoesisPS<Noesis::Shader::Image_Shadow63H_Radial> FNoesisImageShadow63HRadialPS;
typedef FNoesisPS<Noesis::Shader::Image_Shadow63H_Pattern> FNoesisImageShadow63HPatternPS;

typedef FNoesisPS<Noesis::Shader::Image_Shadow127H_Solid> FNoesisImageShadow127HSolidPS;
typedef FNoesisPS<Noesis::Shader::Image_Shadow127H_Linear> FNoesisImageShadow127HLinearPS;
typedef FNoesisPS<Noesis::Shader::Image_Shadow127H_Radial> FNoesisImageShadow127HRadialPS;
typedef FNoesisPS<Noesis::Shader::Image_Shadow127H_Pattern> FNoesisImageShadow127HPatternPS;

typedef FNoesisPS<Noesis::Shader::Image_Blur35V> FNoesisImageBlur35VPS;
typedef FNoesisPS<Noesis::Shader::Image_Blur63V> FNoesisImageBlur63VPS;
typedef FNoesisPS<Noesis::Shader::Image_Blur127V> FNoesisImageBlur127VPS;

typedef FNoesisPS<Noesis::Shader::Image_Blur35H_Solid> FNoesisImageBlur35HSolidPS;
typedef FNoesisPS<Noesis::Shader::Image_Blur35H_Linear> FNoesisImageBlur35HLinearPS;
typedef FNoesisPS<Noesis::Shader::Image_Blur35H_Radial> FNoesisImageBlur35HRadialPS;
typedef FNoesisPS<Noesis::Shader::Image_Blur35H_Pattern> FNoesisImageBlur35HPatternPS;

typedef FNoesisPS<Noesis::Shader::Image_Blur63H_Solid> FNoesisImageBlur63HSolidPS;
typedef FNoesisPS<Noesis::Shader::Image_Blur63H_Linear> FNoesisImageBlur63HLinearPS;
typedef FNoesisPS<Noesis::Shader::Image_Blur63H_Radial> FNoesisImageBlur63HRadialPS;
typedef FNoesisPS<Noesis::Shader::Image_Blur63H_Pattern> FNoesisImageBlur63HPatternPS;

typedef FNoesisPS<Noesis::Shader::Image_Blur127H_Solid> FNoesisImageBlur127HSolidPS;
typedef FNoesisPS<Noesis::Shader::Image_Blur127H_Linear> FNoesisImageBlur127HLinearPS;
typedef FNoesisPS<Noesis::Shader::Image_Blur127H_Radial> FNoesisImageBlur127HRadialPS;
typedef FNoesisPS<Noesis::Shader::Image_Blur127H_Pattern> FNoesisImageBlur127HPatternPS;

// Read the comment next to PATTERN_SRGB in FNoesisPS::ModifyCompilationEnvironment
typedef FNoesisPS<Noesis::Shader::Path_Pattern, true> FNoesisPathPatternSRGBPS;
typedef FNoesisPS<Noesis::Shader::PathAA_Pattern, true> FNoesisPathAaPatternSRGBPS;
typedef FNoesisPS<Noesis::Shader::SDF_Pattern, true> FNoesisSDFPatternSRGBPS;
typedef FNoesisPS<Noesis::Shader::SDF_LCD_Pattern, true> FNoesisSDFLCDPatternSRGBPS;
typedef FNoesisPS<Noesis::Shader::Image_Opacity_Pattern, true> FNoesisImageOpacityPatternSRGBPS;
typedef FNoesisPS<Noesis::Shader::Image_Shadow35H_Pattern, true> FNoesisImageShadow35HPatternSRGBPS;
typedef FNoesisPS<Noesis::Shader::Image_Shadow63H_Pattern, true> FNoesisImageShadow63HPatternSRGBPS;
typedef FNoesisPS<Noesis::Shader::Image_Shadow127H_Pattern, true> FNoesisImageShadow127HPatternSRGBPS;
typedef FNoesisPS<Noesis::Shader::Image_Blur35H_Pattern, true> FNoesisImageBlur35HPatternSRGBPS;
typedef FNoesisPS<Noesis::Shader::Image_Blur63H_Pattern, true> FNoesisImageBlur63HPatternSRGBPS;
typedef FNoesisPS<Noesis::Shader::Image_Blur127H_Pattern, true> FNoesisImageBlur127HPatternSRGBPS;
