#include "MyMeshVertexFactory.h"
#include "MeshMaterialShader.h"
#include "MaterialDomain.h"
#include "MeshDrawShaderBindings.h"


IMPLEMENT_VERTEX_FACTORY_TYPE(FMyMeshVertexFactory,
	"/Plugins/CustomShader/Public/MyMeshVertexFactory.ush",
	EVertexFactoryFlags::UsedWithMaterials // 可用用于材质
	| EVertexFactoryFlags::SupportsManualVertexFetch // 支持Manual vertex fetch
);

IMPLEMENT_GLOBAL_SHADER_PARAMETER_STRUCT(FMyMeshVFParameters, "MyMeshVF");

FMyMeshVertexFactory::FMyMeshVertexFactory(ERHIFeatureLevel::Type InFeatureLevel)
	: FVertexFactory(InFeatureLevel),
	PositionVertexBuffer(nullptr),
	TangentVertexBuffer(nullptr) {
}


void FMyMeshVertexFactory::InitRHI(FRHICommandListBase& RHICmdList) {
	check(Streams.Num() == 0);

	FVertexDeclarationElementList Elements;
	// Position
	FVertexStreamComponent PosComponent;
	PosComponent.VertexBuffer = PositionVertexBuffer;
	PosComponent.Stride = sizeof(FVector3f);
	PosComponent.Offset = 0;
	PosComponent.VertexStreamUsage = EVertexStreamUsage::Default;
	PosComponent.Type = VET_Float3;
	Elements.Add(AccessStreamComponent(PosComponent, 0));

	// Tangent X
	FVertexStreamComponent TangentXComponent;
	TangentXComponent.VertexBuffer = TangentVertexBuffer;
	TangentXComponent.Stride = sizeof(FPackedNormal) * 2;
	TangentXComponent.Offset = 0;
	TangentXComponent.VertexStreamUsage = EVertexStreamUsage::Default;
	TangentXComponent.Type = VET_PackedNormal;
	Elements.Add(AccessStreamComponent(TangentXComponent, 1));

	// Tangent Z
	FVertexStreamComponent TangentZComponent;
	TangentZComponent.VertexBuffer = TangentVertexBuffer;
	TangentZComponent.Stride = sizeof(FPackedNormal) * 2;
	TangentZComponent.Offset = sizeof(FPackedNormal);
	TangentZComponent.VertexStreamUsage = EVertexStreamUsage::Default;
	TangentZComponent.Type = VET_PackedNormal;
	Elements.Add(AccessStreamComponent(TangentZComponent, 2));

	InitDeclaration(Elements, EVertexInputStreamType::Default);
	check(GetDeclaration());
}

// 决定对应Shader是否编译
bool FMyMeshVertexFactory::ShouldCompilePermutation(const FVertexFactoryShaderPermutationParameters& Parameters) {
	return RHISupportsManualVertexFetch(Parameters.Platform);
}

// 修改Shader编译参数
void FMyMeshVertexFactory::ModifyCompilationEnvironment(const FVertexFactoryShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment) {
	if (RHISupportsManualVertexFetch(Parameters.Platform)) {
		OutEnvironment.SetDefineIfUnset(TEXT("MANUAL_VERTEX_FETCH"), TEXT("1"));
	}
}

void FMyMeshVertexFactory::ValidateCompiledResult(const FVertexFactoryType* Type, EShaderPlatform Platform, const FShaderParameterMap& ParameterMap, TArray<FString>& OutErrors) {

}

void FMyMeshVertexFactory::ReleaseRHI() {
	UniformBuffer.SafeRelease();
	FVertexFactory::ReleaseRHI();
}



void FMyMeshVFShaderParameters::GetElementShaderBindings(
	const FSceneInterface* Scene,
	const FSceneView* View,
	const FMeshMaterialShader* Shader,
	const EVertexInputStreamType InputStreamType,
	ERHIFeatureLevel::Type FeatureLevel,
	const FVertexFactory* VertexFactory,
	const FMeshBatchElement& BatchElement,
	FMeshDrawSingleShaderBindings& ShaderBindings,
	FVertexInputStreamArray& VertexStreams) const
{
	FMyMeshVertexFactory* CustomShaderVF = (FMyMeshVertexFactory*)VertexFactory;
	const auto& ShaderParameter = Shader->GetUniformBufferParameter<FMyMeshVFParameters>();
	ShaderBindings.Add(ShaderParameter, CustomShaderVF->UniformBuffer);
}

IMPLEMENT_TYPE_LAYOUT(FMyMeshVFShaderParameters);
IMPLEMENT_VERTEX_FACTORY_PARAMETER_TYPE(FMyMeshVertexFactory, EShaderFrequency::SF_Vertex, FMyMeshVFShaderParameters);