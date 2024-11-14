// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "FIT3039/Public/CardDataStruct.h"
#include "SummonChessTask.generated.h"

class UGridNode;
class ABossAIController;

/**
 * 
 */
UCLASS()
class FIT3039_API USummonChessTask : public UBTTaskNode
{
	GENERATED_BODY()

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;


protected:
	

	// Function to select a card for summoning
	FCardData* SelectCardForSummoning(ABossAIController* AIController);

	// Function to summon a chess piece using card data
	void SummonChessPiece(AActor* AIControllerOwner, const FCardData& CardData);

	void SummonNextPiece();
    
	TArray<UGridNode*> AvailableNodes;
	int32 CurrentSummonIndex;
	ABossAIController* CachedAIController;
	FTimerHandle SummonTimerHandle;
};
