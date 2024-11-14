// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "GameFramework/PlayerController.h"
#include "FIT3039PlayerController.generated.h"

class UInputMappingContext;

/**
 *
 */
UCLASS()
class FIT3039_API AFIT3039PlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:

	/** Input Mapping Context to be used for player input */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* InputMappingContext;

	/** Switch to First Person Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* SwitchToFirstPersonAction;

	/** Switch to Third Person Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* SwitchToThirdPersonAction;
	// Begin Actor interface
protected:

	void SwitchToThirdPerson();
	void SwitchToFirstPerson();

	virtual void BeginPlay() override;

	// End Actor interface
};
