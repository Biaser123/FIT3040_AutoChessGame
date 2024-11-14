// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataTable.h"
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CardDataStruct.generated.h"

class ISkillInterface;
enum class ERaceType : uint8;
enum class ECardType : uint8;

UENUM(BlueprintType)
enum class ECardSkill : uint8
{
	None UMETA(DisplayName = "None"),
	Flying UMETA(DisplayName = "Flying"),
	Charge UMETA(DisplayName = "Charge"),
	Resurrection UMETA(DisplayName = "Resurrection")
};

/**
 * 
 */
USTRUCT(BlueprintType)
struct FCardData : public FTableRowBase
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="RowName"))
	FName RowName;
	
	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="CardName",MakeStructureDefaultValue="Card Name"))
	FString CardName;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="CardType", MakeStructureDefaultValue="Seed"))
	ECardType CardType;
	
	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="CardDescription", MakeStructureDefaultValue="Card Description"))
	FString CardDescription;
	
	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Card_Image", MakeStructureDefaultValue="None"))
	TObjectPtr<UTexture2D> Card_Image;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Card_Background", MakeStructureDefaultValue="None"))
	TObjectPtr<UTexture2D> Card_Background;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="OblationPoint", MakeStructureDefaultValue="1"))
	int OblationPoint;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Attack", MakeStructureDefaultValue="1"))
	int Attack;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Health", MakeStructureDefaultValue="1"))
	int Health;

	/** Race Type */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Race", MakeStructureDefaultValue="Ghost"))
	ERaceType Race;

	/** Skills */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Skills"))
	TArray<ECardSkill> Skills;
	
	bool operator==(const FCardData& Other) const
	{
		return CardName == Other.CardName; 
	}
};


