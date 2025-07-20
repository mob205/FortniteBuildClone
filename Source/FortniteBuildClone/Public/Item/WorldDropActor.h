// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/ItemData.h"
#include "GameFramework/Actor.h"
#include "Interface/Interactable.h"
#include "StructUtils/InstancedStruct.h"
#include "WorldDropActor.generated.h"

UCLASS()
class FORTNITEBUILDCLONE_API AWorldDropActor : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	AWorldDropActor();

	void SetItemData(UItemData* ItemData);

	virtual void StartInteract_Implementation(AActor* Interactor) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UItemData> CurrentItemData;

	UPROPERTY(EditAnywhere)
	FInstancedStruct ItemInstanceInfo;
private:
	void UpdateFromItemData();
	void LoadItemMesh(const TSoftObjectPtr<UStaticMesh>& ItemMesh);

	bool bWasPickedUp{};

	TObjectPtr<AEquippedItemActor> EquippedActor;
	
};
