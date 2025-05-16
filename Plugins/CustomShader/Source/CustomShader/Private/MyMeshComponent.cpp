// Fill out your copyright notice in the Description page of Project Settings.


#include "MyMeshComponent.h"
#include "PrimitiveSceneProxy.h"
#include "MyMeshVertexFactory.h"
#include "MaterialDomain.h"
#include "MeshDrawShaderBindings.h"
#include "MeshMaterialShader.h"

static const uint32 NumVertices = 36;

class FMyMeshSceneProxy : public FPrimitiveSceneProxy {
public:
	FMyMeshSceneProxy(UMyMeshComponent* InComponent);
	virtual SIZE_T GetTypeHash() const override;
	virtual uint32 GetMemoryFootprint() const override;
	void CreateRenderThreadResources(FRHICommandListBase& RHICmdList) override;
	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override;
	virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override;
	~FMyMeshSceneProxy();
private:
	FMyMeshVertexFactory VertexFactory;
	TUniquePtr<FVertexBuffer> PositionVertexBuffer;
	TUniquePtr<FVertexBuffer> TangentVertexBuffer;
	TUniquePtr<FIndexBuffer> IndexBuffer;
private:
	TUniquePtr<FVertexBuffer> UVBuffer;
	FShaderResourceViewRHIRef UVBufferSRV;
private:
	FMaterialRenderProxy* MyMaterialRenderProxy;
};



FMyMeshSceneProxy::FMyMeshSceneProxy(UMyMeshComponent* InComponent) 
	:FPrimitiveSceneProxy(InComponent),
	VertexFactory(InComponent->GetWorld()->GetFeatureLevel()) {
	MyMaterialRenderProxy = InComponent->GetMyMaterial() ? InComponent->GetMyMaterial()->GetRenderProxy() : nullptr;
}

SIZE_T FMyMeshSceneProxy::GetTypeHash() const {
	static size_t UniquePointer;
	return reinterpret_cast<size_t>(&UniquePointer);
}

uint32 FMyMeshSceneProxy::GetMemoryFootprint() const {
	return sizeof(*this) + GetAllocatedSize();
}

// Position index buffer
class FMyCubePositionVertexBuffer : public FVertexBuffer {
public:
	virtual void InitRHI(FRHICommandListBase& RHICmdList) override {
		FVector3f Data[]{
			{-50.0f, -50.0f, -50.0f}, { 50.0f, -50.0f, -50.0f}, { 50.0f,  50.0f, -50.0f}, { 50.0f,  50.0f, -50.0f}, {-50.0f,  50.0f, -50.0f}, {-50.0f, -50.0f, -50.0f},
			{-50.0f, -50.0f,  50.0f}, { 50.0f,  50.0f,  50.0f}, { 50.0f, -50.0f,  50.0f}, { 50.0f,  50.0f,  50.0f}, {-50.0f, -50.0f,  50.0f}, {-50.0f,  50.0f,  50.0f},
			{-50.0f,  50.0f,  50.0f}, {-50.0f, -50.0f, -50.0f}, {-50.0f,  50.0f, -50.0f}, {-50.0f, -50.0f, -50.0f}, {-50.0f,  50.0f,  50.0f}, {-50.0f, -50.0f,  50.0f},
			{ 50.0f,  50.0f,  50.0f}, { 50.0f,  50.0f, -50.0f}, { 50.0f, -50.0f, -50.0f}, { 50.0f, -50.0f, -50.0f}, { 50.0f, -50.0f,  50.0f}, { 50.0f,  50.0f,  50.0f},
			{-50.0f, -50.0f, -50.0f}, { 50.0f, -50.0f,  50.0f}, { 50.0f, -50.0f, -50.0f}, { 50.0f, -50.0f,  50.0f}, {-50.0f, -50.0f, -50.0f}, {-50.0f, -50.0f,  50.0f},
			{-50.0f,  50.0f, -50.0f}, { 50.0f,  50.0f, -50.0f}, { 50.0f,  50.0f,  50.0f}, { 50.0f,  50.0f,  50.0f}, {-50.0f,  50.0f,  50.0f}, {-50.0f,  50.0f, -50.0f},
		};
		constexpr uint32 BufferSize = NumVertices * sizeof(FVector3f);
		FRHIResourceCreateInfo CreateInfo{ TEXT("MyCubePositionVertexBuffer") };
		VertexBufferRHI = RHICmdList.CreateVertexBuffer(BufferSize, BUF_Static, CreateInfo);
		void* LockedData = RHICmdList.LockBuffer(VertexBufferRHI, 0, BufferSize, RLM_WriteOnly);
		FMemory::Memcpy(LockedData, Data, BufferSize);
		RHICmdList.UnlockBuffer(VertexBufferRHI);
	}
};

// vertex index Buffer
class FMyCubeTangentVertexBuffer : public FVertexBuffer {
public:
	virtual void InitRHI(FRHICommandListBase& RHICmdList) override {
		struct FTangentData {
			FPackedNormal TangentX, TangentZ;
		};
		FTangentData Data[NumVertices];
		for (int32 i = 0; i < 6; ++i) { Data[i] = { FVector3f{-1.0f,  0.0f,  0.0f}, FVector3f{ 0.0f,  0.0f, -1.0f} }; }
		for (int32 i = 0; i < 6; ++i) { Data[i + 6] = { FVector3f{ 1.0f,  0.0f,  0.0f}, FVector3f{ 0.0f,  0.0f,  1.0f} }; }
		for (int32 i = 0; i < 6; ++i) { Data[i + 12] = { FVector3f{ 0.0f,  0.0f,  1.0f}, FVector3f{-1.0f,  0.0f,  0.0f} }; }
		for (int32 i = 0; i < 6; ++i) { Data[i + 18] = { FVector3f{ 0.0f,  0.0f, -1.0f}, FVector3f{ 1.0f,  0.0f,  0.0f} }; }
		for (int32 i = 0; i < 6; ++i) { Data[i + 24] = { FVector3f{ 1.0f,  0.0f,  0.0f}, FVector3f{ 0.0f, -1.0f,  0.0f} }; }
		for (int32 i = 0; i < 6; ++i) { Data[i + 30] = { FVector3f{ 1.0f,  0.0f,  0.0f}, FVector3f{ 0.0f,  1.0f,  0.0f} }; }
		constexpr uint32 BufferSize = NumVertices * sizeof(FTangentData);
		FRHIResourceCreateInfo CreateInfo{ TEXT("MyCubeTangentVertexBuffer") };
		VertexBufferRHI = RHICmdList.CreateVertexBuffer(BufferSize, BUF_Static, CreateInfo);
		void* LockedData = RHICmdList.LockBuffer(VertexBufferRHI, 0, BufferSize, RLM_WriteOnly);
		FMemory::Memcpy(LockedData, Data, BufferSize);
		RHICmdList.UnlockBuffer(VertexBufferRHI);
	}
};

// Index buffer
class FMyCubeIndexBuffer : public FIndexBuffer {
public:
	virtual void InitRHI(FRHICommandListBase& RHICmdList) override {
		uint16 Data[NumVertices];
		for (int32 i = 0; i < NumVertices; ++i) {
			Data[i] = (uint16)i;
		}
		constexpr uint32 BufferSize = NumVertices * sizeof(uint16);
		FRHIResourceCreateInfo CreateInfo{ TEXT("MyCubeIndexBuffer") };
		IndexBufferRHI = RHICmdList.CreateIndexBuffer(sizeof(uint16), BufferSize, BUF_Static, CreateInfo);
		void* LockedData = RHICmdList.LockBuffer(IndexBufferRHI, 0, BufferSize, RLM_WriteOnly);
		FMemory::Memcpy(LockedData, Data, BufferSize);
		RHICmdList.UnlockBuffer(IndexBufferRHI);
	}
};

// UVBuffer
class FMyCubeUVBuffer : public FVertexBuffer {
public:
	virtual void InitRHI(FRHICommandListBase& RHICmdList) override {
		TResourceArray<FVector2DHalf> Data; // 使用半精度浮点
		Data = {
			{ 0.0f,  0.0f }, { 1.0f,  0.0f }, { 1.0f,  1.0f }, { 1.0f,  1.0f }, { 0.0f,  1.0f }, { 0.0f,  0.0f },
			{ 0.0f,  0.0f }, { 1.0f,  1.0f }, { 1.0f,  0.0f }, { 1.0f,  1.0f }, { 0.0f,  0.0f }, { 0.0f,  1.0f },
			{ 1.0f,  0.0f }, { 0.0f,  1.0f }, { 1.0f,  1.0f }, { 0.0f,  1.0f }, { 1.0f,  0.0f }, { 0.0f,  0.0f },
			{ 1.0f,  0.0f }, { 1.0f,  1.0f }, { 0.0f,  1.0f }, { 0.0f,  1.0f }, { 0.0f,  0.0f }, { 1.0f,  0.0f },
			{ 0.0f,  1.0f }, { 1.0f,  0.0f }, { 1.0f,  1.0f }, { 1.0f,  0.0f }, { 0.0f,  1.0f }, { 0.0f,  0.0f },
			{ 0.0f,  1.0f }, { 1.0f,  1.0f }, { 1.0f,  0.0f }, { 1.0f,  0.0f }, { 0.0f,  0.0f }, { 0.0f,  1.0f },
		};
		constexpr uint32 BufferSize = NumVertices * sizeof(FVector2DHalf);
		FRHIResourceCreateInfo CreateInfo{ TEXT("MyCubeUVBuffer") };
		CreateInfo.ResourceArray = &Data;
		VertexBufferRHI = RHICmdList.CreateVertexBuffer(BufferSize, BUF_Static, CreateInfo);
	}
};


void FMyMeshSceneProxy::CreateRenderThreadResources(FRHICommandListBase& RHICmdList) {
	PositionVertexBuffer = MakeUnique<FMyCubePositionVertexBuffer>();
	PositionVertexBuffer->InitResource(RHICmdList);
	TangentVertexBuffer = MakeUnique<FMyCubeTangentVertexBuffer>();
	TangentVertexBuffer->InitResource(RHICmdList);
	IndexBuffer = MakeUnique<FMyCubeIndexBuffer>();
	IndexBuffer->InitResource(RHICmdList);;
	// 初始化VertexFactory
	VertexFactory.PositionVertexBuffer = PositionVertexBuffer.Get();
	VertexFactory.TangentVertexBuffer = TangentVertexBuffer.Get();
	VertexFactory.InitResource(RHICmdList);

	// 初始化UVBuffer
	UVBuffer = MakeUnique<FMyCubeUVBuffer>();
	UVBuffer->InitResource(RHICmdList);

	UVBufferSRV = RHICmdList.CreateShaderResourceView(UVBuffer->GetRHI(), sizeof(FVector2DHalf), PF_G16R16F);
	// 创建UniformBuffer
	FMyMeshVFParameters Parameters;
	Parameters.VertexFetch_Parameters = { 0, 1, 0, 0 };
	Parameters.VertexFetch_TexCoordBuffer = UVBufferSRV;
	VertexFactory.UniformBuffer = FMyMeshVFParameters::CreateUniformBuffer(Parameters, UniformBuffer_MultiFrame);
}

FPrimitiveViewRelevance FMyMeshSceneProxy::GetViewRelevance(const FSceneView* View) const {
	FPrimitiveViewRelevance Result;
	Result.bDrawRelevance = IsShown(View);
	Result.bDynamicRelevance = true; // GetDynamicMeshElements
	Result.bStaticRelevance = false; // 静态网格
	return Result;
}

void FMyMeshSceneProxy::GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const {
	// 如果指定材质就使用，否则使用默认材质
	FMaterialRenderProxy* MaterialRenderProxy = MyMaterialRenderProxy ? MyMaterialRenderProxy : UMaterial::GetDefaultMaterial(MD_Surface)->GetRenderProxy();
	// 逐view 搜集
	for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ++ViewIndex) {
		if (VisibilityMap & (1 << ViewIndex)) {
			// Create a mesh batch
			FMeshBatch& MeshBatch = Collector.AllocateMesh();
			MeshBatch.bUseAsOccluder = false;
			MeshBatch.VertexFactory = &VertexFactory;
			MeshBatch.MaterialRenderProxy = MaterialRenderProxy;
			MeshBatch.ReverseCulling = IsLocalToWorldDeterminantNegative();
			MeshBatch.DepthPriorityGroup = SDPG_World;
			MeshBatch.Type = EPrimitiveType::PT_TriangleList;

			// Setup index buffer
			FMeshBatchElement& Element = MeshBatch.Elements[0];
			Element.FirstIndex = 0;
			Element.NumPrimitives = NumVertices / 3;
			Element.IndexBuffer = IndexBuffer.Get();
			Element.MinVertexIndex = 0;
			Element.MaxVertexIndex = NumVertices - 1;

			// Primitive uniform buffer is required.
			Element.PrimitiveUniformBuffer = GetUniformBuffer();
			MeshBatch.bCanApplyViewModeOverrides = false;
			Collector.AddMesh(ViewIndex, MeshBatch);
		}
	}
}

FMyMeshSceneProxy::~FMyMeshSceneProxy() {
	VertexFactory.ReleaseResource();
	if (PositionVertexBuffer)PositionVertexBuffer->ReleaseResource();
	if (TangentVertexBuffer)TangentVertexBuffer->ReleaseResource();
	if (IndexBuffer)IndexBuffer->ReleaseResource();
	if (UVBuffer)UVBuffer->ReleaseResource();
	UVBufferSRV.SafeRelease();
}


FPrimitiveSceneProxy* UMyMeshComponent::CreateSceneProxy() {
	return new FMyMeshSceneProxy(this);
}

UMaterialInterface* UMyMeshComponent::GetMyMaterial() const {
	return MyMaterial.Get();
}

void UMyMeshComponent::GetUsedMaterials(TArray<UMaterialInterface*>& OutMaterials, bool bGetDebugMaterials) const {
	if (MyMaterial) {
		OutMaterials.Add(MyMaterial.Get());
	}
}

FBoxSphereBounds UMyMeshComponent::CalcBounds(const FTransform& LocalToWorld) const {
	FBoxSphereBounds MyBounds{ {0.0,0.0,0.0}, {50.0, 50.0, 50.0}, 50.0 };
	return MyBounds.TransformBy(LocalToWorld);
}


