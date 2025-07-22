// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/WorldDropActor.h"
#include "Component/InventoryComponent.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Player/FBCPlayerState.h"

AWorldDropActor::AWorldDropActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	NetDormancy = DORM_DormantAll;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
}

void AWorldDropActor::InitializeFromItemData(const UItemData* ItemData)
{
	CurrentItemData = ItemData;

	CurrentItemActor = GetWorld()->SpawnActor<AEquippedItemActor>(ItemData->GetActorClass());
	CurrentItemActor->SetOwner(this);
	CurrentItemActor->SetItemData(ItemData);
	
	UpdateFromItemData();
}

void AWorldDropActor::InitializeFromItemActor(AEquippedItemActor* InItemActor)
{
	CurrentItemData = InItemActor->GetItemData();
	CurrentItemActor = InItemActor;
	UpdateFromItemData();
}

void AWorldDropActor::StartInteract_Implementation(AActor* Interactor)
{
	if (!Interactor->HasAuthority()) { return; }

	// Currently, the inventory component lives on the player state - drill down to find it
	if (APawn* Pawn = Cast<APawn>(Interactor))
	{
		if (AFBCPlayerState* PlayerState = Cast<AFBCPlayerState>(Pawn->GetPlayerState()))
		{
			UInventoryComponent* InventoryComponent = PlayerState->GetInventoryComponent();
			if (InventoryComponent->ServerTryAddItem(CurrentItemActor))
			{
				Destroy();
			}
		}
	}
}

#if WITH_EDITOR
void AWorldDropActor::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	UpdateFromItemData();
}
#endif

void AWorldDropActor::BeginPlay()
{
	Super::BeginPlay();

	if (CurrentItemData && !CurrentItemActor)
	{
		InitializeFromItemData(CurrentItemData);
	}
}

void AWorldDropActor::UpdateFromItemData()
{
	if (CurrentItemData)
	{
		LoadItemMesh(CurrentItemData->GetWorldDropMesh());
	}
}

void AWorldDropActor::LoadItemMesh(const TSoftObjectPtr<UStaticMesh>& ItemMesh)
{
#if !UE_SERVER
	if (ItemMesh.Get() != nullptr)
	{
		MeshComponent->SetStaticMesh(ItemMesh.Get());
		return;
	}
		
	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	Streamable.RequestAsyncLoad(
		ItemMesh.ToSoftObjectPath(),
		FStreamableDelegate::CreateLambda(
			[&ItemMesh, this]()
			{
				if (ItemMesh.Get() != nullptr)
				{
					MeshComponent->SetStaticMesh(ItemMesh.Get());
				}
			}));
#endif
}

