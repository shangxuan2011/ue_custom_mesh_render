#pragma once

#include "CoreMinimal.h"
#include "ShaderParameters.h"
#include "Components.h"
#include "VertexFactory.h"
#include "GlobalRenderResources.h"

// Shader parameter
BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FMyMeshVFParameters, )
	SHADER_PARAMETER_SRV(Buffer<float2>, VertexFetch_TexCoordBuffer)
	// UV������Ϣ����ȫ���õ�����ռλ��
	SHADER_PARAMETER(FInt32Vector4, VertexFetch_Parameters) //(ColorIndexMask, NumTexCoords, LightMapCoordIndex, EffectiveBaseVertexIndex)
END_GLOBAL_SHADER_PARAMETER_STRUCT()


class FMyMeshVertexFactory : public FVertexFactory {
	DECLARE_VERTEX_FACTORY_TYPE(FMyMeshVertexFactory);
public:
	FVertexBuffer* PositionVertexBuffer;
	FVertexBuffer* TangentVertexBuffer;
	FUniformBufferRHIRef UniformBuffer;

	FMyMeshVertexFactory(ERHIFeatureLevel::Type InFeatureLevel);

	virtual void InitRHI(FRHICommandListBase& RHICmdList) override;
	virtual void ReleaseRHI() override;

	// ������ӦShader�Ƿ����
	static bool ShouldCompilePermutation(const FVertexFactoryShaderPermutationParameters& Parameters);

	// �޸�Shader�������
	static void ModifyCompilationEnvironment(const FVertexFactoryShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);

	// ��֤Shader�������Ƿ����Ԥ��
	static void ValidateCompiledResult(const FVertexFactoryType* Type, EShaderPlatform Platform, const FShaderParameterMap& ParameterMap, TArray<FString>& OutErrors);

};


// Shader uniform buffer binding
class FMyMeshVFShaderParameters : public FVertexFactoryShaderParameters {
	DECLARE_TYPE_LAYOUT(FMyMeshVFShaderParameters, NonVirtual);
public:
	void GetElementShaderBindings(
		const FSceneInterface* Scene,
		const FSceneView* View,
		const FMeshMaterialShader* Shader,
		const EVertexInputStreamType InputStreamType,
		ERHIFeatureLevel::Type FeatureLevel,
		const FVertexFactory* VertexFactory,
		const FMeshBatchElement& BatchElement,
		FMeshDrawSingleShaderBindings& ShaderBindings,
		FVertexInputStreamArray& VertexStreams) const;
};