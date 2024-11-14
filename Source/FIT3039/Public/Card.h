// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Card.generated.h"

UENUM(BlueprintType)
enum class ECardType : uint8
{
	Oblation UMETA(DisplayName = "Oblation"),
	Ghost UMETA(DisplayName = "Ghost"),
	Talisman  UMETA(DisplayName = "Talisman"),   //符咒
	Ritual_Tools UMETA(DisplayName = "Ritual Tools") //法器
};

UENUM(BlueprintType)
enum class ERaceType : uint8
{
	Zombie UMETA(DisplayName = "Zombie"),
	Ghost UMETA(DisplayName = "Ghost"),
	Demon UMETA(DisplayName = "Demon"),
	General UMETA(DisplayName = "General")  //无种族
};


UCLASS()
class FIT3039_API ACard : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
