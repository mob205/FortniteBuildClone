#pragma once 

UENUM(BlueprintType)
enum class EFBCMaterialType : uint8
{
	FBCMat_Wood UMETA(DisplayName = "Wood"),
	FBCMat_Brick UMETA(DisplayName = "Brick"),
	FBCMat_Metal UMETA(DisplayName = "Metal"),
};