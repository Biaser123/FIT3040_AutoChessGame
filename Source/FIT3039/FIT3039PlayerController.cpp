#include "FIT3039PlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputComponent.h"
#include "FIT3039Character.h"

void AFIT3039PlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Get the enhanced input subsystem
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		// Add the mapping context so we get controls
		Subsystem->AddMappingContext(InputMappingContext, 0);
	}

	// Bind input actions to functions
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (SwitchToFirstPersonAction)
		{
			EnhancedInputComponent->BindAction(SwitchToFirstPersonAction, ETriggerEvent::Triggered, this, &AFIT3039PlayerController::SwitchToFirstPerson);
		}

		if (SwitchToThirdPersonAction)
		{
			EnhancedInputComponent->BindAction(SwitchToThirdPersonAction, ETriggerEvent::Triggered, this, &AFIT3039PlayerController::SwitchToThirdPerson);
		}
	}
}

void AFIT3039PlayerController::SwitchToFirstPerson()
{
	if (AFIT3039Character* MyCharacter = Cast<AFIT3039Character>(GetCharacter()))
	{
		MyCharacter->SwitchToPlayerCamera();
	}
}

void AFIT3039PlayerController::SwitchToThirdPerson()
{
	if (AFIT3039Character* MyCharacter = Cast<AFIT3039Character>(GetCharacter()))
	{
		MyCharacter->SwitchToCustomCamera();
	}
}
