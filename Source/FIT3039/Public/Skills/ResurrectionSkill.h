// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkillInterface.h"
#include "UObject/NoExportTypes.h"
#include "ResurrectionSkill.generated.h"

/**
 * 
 */
UCLASS()
class FIT3039_API UResurrectionSkill : public UObject, public ISkillInterface
{
	GENERATED_BODY()

	virtual void ApplySkillEffect_Implementation() override;
};
