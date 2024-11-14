// Copyright Epic Games, Inc. All Rights Reserved.

#include "FIT3039GameMode.h"
#include "FIT3039Character.h"
#include "UObject/ConstructorHelpers.h"

AFIT3039GameMode::AFIT3039GameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
