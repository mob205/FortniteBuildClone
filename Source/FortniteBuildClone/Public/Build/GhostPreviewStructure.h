// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GhostPreviewStructure.generated.h"

class UStaticMeshComponent;
class UMaterialInterface;

UCLASS()
class FORTNITEBUILDCLONE_API AGhostPreviewStructure : public AActor
{
	GENERATED_BODY()
	
public:	
	AGhostPreviewStructure();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> Root;

	void SetMaterial(UMaterialInterface* Material);
};
