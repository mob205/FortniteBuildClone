// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/Data/ItemData.h"
#include "GameFramework/Actor.h"
#include "Interface/Interactable.h"
#include "WorldDropActor.generated.h"

UCLASS()
class FORTNITEBUILDCLONE_API AWorldDropActor : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	AWorldDropActor();

	void InitializeFromItemData(const UItemData* ItemData);
	void InitializeFromItemActor(AEquippedItemActor* InItemActor);

	virtual void StartInteract_Implementation(AActor* Interactor) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(EditAnywhere)
	TObjectPtr<const UItemData> CurrentItemData;
private:
	void UpdateFromItemData();
	void LoadItemMesh(const TSoftObjectPtr<UStaticMesh>& ItemMesh);

	bool bWasPickedUp{};

	UFUNCTION()
	void OnRep_CurrentItemActor();
	
	UPROPERTY(ReplicatedUsing = OnRep_CurrentItemActor)
	TObjectPtr<AEquippedItemActor> CurrentItemActor;
	
};
