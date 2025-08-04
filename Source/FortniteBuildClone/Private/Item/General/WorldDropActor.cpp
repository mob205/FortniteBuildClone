// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/General/WorldDropActor.h"
#include "Component/InventoryComponent.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Net/UnrealNetwork.h"
#include "Player/FBCCharacterBase.h"
#include "Player/FBCPlayerState.h"

AWorldDropActor::AWorldDropActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	NetDormancy = DORM_DormantAll;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	SetRootComponent(MeshComponent);
}

void AWorldDropActor::InitializeFromItemData(const UItemData* ItemData)
{
	CurrentItemData = ItemData;

	CurrentItemActor = GetWorld()->SpawnActorDeferred<AEquippedItemActor>(ItemData->GetActorClass(), GetActorTransform(), this);
	CurrentItemActor->SetItemData(ItemData);
	CurrentItemActor->FinishSpawning(GetActorTransform());
	
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

	if (AFBCCharacterBase* Character = Cast<AFBCCharacterBase>(Interactor))
	{
		UInventoryComponent* InventoryComponent = Character->GetInventoryComponent();
		if (InventoryComponent->ServerTryAddItem(CurrentItemActor))
		{
			Destroy();
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

void AWorldDropActor::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWorldDropActor, CurrentItemActor);
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
	if (ItemMesh.IsValid())
	{
		MeshComponent->SetStaticMesh(ItemMesh.Get());
		return;
	}
		
	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	Streamable.RequestAsyncLoad(
		ItemMesh.ToSoftObjectPath(),
		FStreamableDelegate::CreateLambda(
			[ItemMesh, this]()
			{
				if (ItemMesh.IsValid())
				{
					MeshComponent->SetStaticMesh(ItemMesh.Get());
				}
			}));
#endif
}

void AWorldDropActor::OnRep_CurrentItemActor()
{
	if (CurrentItemActor && CurrentItemActor->GetItemData())
	{
		CurrentItemData = CurrentItemActor->GetItemData();
		UpdateFromItemData();
	}
}

