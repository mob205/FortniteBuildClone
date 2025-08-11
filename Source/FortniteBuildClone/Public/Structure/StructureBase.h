// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GameFramework/Actor.h"
#include "Interface/Damageable.h"
#include "StructureBase.generated.h"

class UStructureGroundingComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamageTaken, float, NewHealth);

USTRUCT(BlueprintType)
struct FHarvestInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> HarvestEffectClass{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinMagnitude{};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxMagnitude{};
};

UCLASS()
class FORTNITEBUILDCLONE_API AStructureBase : public AActor, public IDamageable
{
	GENERATED_BODY()

public:
	AStructureBase();

	UStructureGroundingComponent* GetGroundingComponent() const { return GroundingComponent; }

	UFUNCTION(BlueprintCallable)
	float GetHealth() const { return Health; }
	
	UFUNCTION(BlueprintCallable)
	void SetHealth(float InHealth);

	UFUNCTION(BlueprintCallable)
	void ModifyHealth(float Amount);

	UPROPERTY(BlueprintAssignable)
	FOnDamageTaken OnDamageTaken;
	
	virtual void Damage(FGameplayEffectSpecHandle DamageEffectSpec) override;
protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TArray<FHarvestInfo> HarvestInfo;

	UPROPERTY(EditAnywhere)
	float MaxHealth{100};
	
	UFUNCTION()
	void DisableStructure();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> StaticMesh;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UStructureGroundingComponent* GroundingComponent{};

private:
	UFUNCTION()
	void OnRep_Health(float OldHealth);

	void ApplyHarvest(AActor* Harvester);
	
	UPROPERTY(ReplicatedUsing=OnRep_Health)
	float Health{};
};

