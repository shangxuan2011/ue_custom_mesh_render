// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PrimitiveComponent.h"
#include "MyMeshComponent.generated.h"

/**
 * 
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class CUSTOMSHADER_API UMyMeshComponent : public UPrimitiveComponent
{
	GENERATED_BODY()
public:
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
	virtual void GetUsedMaterials(TArray<UMaterialInterface*>& OutMaterials, bool bGetDebugMaterials) const override;

	UMaterialInterface* GetMyMaterial() const;
private:
	UPROPERTY(EditAnywhere, Category = Rendering)
	TObjectPtr<UMaterialInterface> MyMaterial;
};
