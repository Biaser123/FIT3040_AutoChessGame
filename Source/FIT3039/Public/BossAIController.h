// BossAIController.h
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AutoChess/Chessboard.h"
#include "AutoChess/TurnManager.h"
#include "FIT3039/Public/CardDataStruct.h"
#include "BossAIController.generated.h"

UCLASS()
class FIT3039_API ABossAIController : public AAIController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	AChessboard* Chessboard;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	ATurnManager* TurnManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	UBehaviorTree* BossBehaviorTree;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Card Data")
	UDataTable* CardDataTable;

	// Reference to the AChess Blueprint class to spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chess")
	TSubclassOf<AChess> AChessBlueprintClass;

	// Reference to the chessboard instance
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chessboard")
	AChessboard* ChessboardInstance;


	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FCardData CardInfoPass;

	

	
	void TriggerSummoningPhase();
	void TriggerAttackingPhase();


protected:
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	UBlackboardData* BossBlackboardData;

};
