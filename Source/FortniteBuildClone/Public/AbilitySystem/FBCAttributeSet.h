// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "FBCAttributeSet.generated.h"

// This macro does NOT work for actually defining attributes, since UHT runs before C++ preprocessor
// As such, UPROPERTY() and UFUNCTION() won't actually be recognized by UHT
// Still helpful for quickly defining attributes, since IDE can expand macro
#define ATTRIBUTE_DEFINITION(PropertyName, CategoryName)\
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_##PropertyName, Category = CategoryName) \
	FGameplayAttributeData PropertyName; \
	ATTRIBUTE_ACCESSORS(UFBCAttributeSet, PropertyName); \
	UFUNCTION() \
	void OnRep_##PropertyName(const FGameplayAttributeData& OldValue) const \
	{ \
		GAMEPLAYATTRIBUTE_REPNOTIFY(UFBCAttributeSet, PropertyName, OldValue); \
	} 

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

class FLifetimeProperty;

/**
 * 
 */
UCLASS()
class FORTNITEBUILDCLONE_API UFBCAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Wood, Category = "Material Count")
	FGameplayAttributeData Wood;
	ATTRIBUTE_ACCESSORS(UFBCAttributeSet, Wood);
	UFUNCTION()
	void OnRep_Wood(const FGameplayAttributeData& OldValue) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UFBCAttributeSet, Wood, OldValue);
	};
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Brick, Category = "Material Count")
	FGameplayAttributeData Brick;
	ATTRIBUTE_ACCESSORS(UFBCAttributeSet, Brick);
	UFUNCTION()
	void OnRep_Brick(const FGameplayAttributeData& OldValue) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UFBCAttributeSet, Brick, OldValue);
	};
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Metal, Category = "Material Count") FGameplayAttributeData Metal;
	ATTRIBUTE_ACCESSORS(UFBCAttributeSet, Metal);
	UFUNCTION()
	void OnRep_Metal(const FGameplayAttributeData& OldValue) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UFBCAttributeSet, Metal, OldValue);
	};
};
