#pragma once 

UENUM(BlueprintType)
enum class EFBCResourceType : uint8
{
	FBCMat_Wood UMETA(DisplayName = "Wood"),
	FBCMat_Brick UMETA(DisplayName = "Brick"),
	FBCMat_Metal UMETA(DisplayName = "Metal"),
	FBCMat_Max UMETA(DisplayName = "Max")
};