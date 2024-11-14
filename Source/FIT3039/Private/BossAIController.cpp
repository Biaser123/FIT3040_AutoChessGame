// BossAIController.cpp
#include "BossAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "Kismet/GameplayStatics.h"

void ABossAIController::BeginPlay()
{
	Super::BeginPlay();

	ChessboardInstance = Cast<AChessboard>(UGameplayStatics::GetActorOfClass(GetWorld(), AChessboard::StaticClass()));

	if (BossBehaviorTree)
	{
		// Dereference the Blackboard TObjectPtr to get the raw pointer
		UBlackboardComponent* BlackboardComponent = Blackboard.Get();
        
		UseBlackboard(BossBlackboardData, BlackboardComponent);
		
		RunBehaviorTree(BossBehaviorTree);
	}

	if (Blackboard)
	{
		Blackboard->SetValueAsString("RowNamesString", "4,5");
	}
}


void ABossAIController::TriggerSummoningPhase()
{
	UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
	
	
	BlackboardComp->SetValueAsBool("bIsSummoning", true);
    
	// 输出日志来检查设置是否成功
	bool IsSummoning = BlackboardComp->GetValueAsBool("bIsSummoning");
	UE_LOG(LogTemp, Log, TEXT("bIsSummoning has been set to: %s"), IsSummoning ? TEXT("true") : TEXT("false"));
}


void ABossAIController::TriggerAttackingPhase()
{
	if (BossBehaviorTree)
	{
		// set the summoning bool to true
		GetBlackboardComponent()->SetValueAsBool("bIsAttacking", true);
	}
}

