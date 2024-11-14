// Fill out your copyright notice in the Description page of Project Settings.
// AttackTask.cpp
#include "AttackTask.h"
#include "BossAIController.h"
#include "GameFramework/Actor.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UAttackTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ABossAIController* AIController = Cast<ABossAIController>(OwnerComp.GetAIOwner());

	if (AIController)
	{
		// 执行攻击逻辑
		AttackEnemy(AIController->GetPawn());
		AIController->GetBlackboardComponent()->SetValueAsBool("bIsAttacking", false);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}

void UAttackTask::AttackEnemy(AActor* AIControllerOwner)
{
	ABossAIController* AIController = Cast<ABossAIController>(AIControllerOwner->GetInstigatorController());

	if (AIController && AIController->TurnManager)
	{
		//进行敌人行动回合
		AIController->TurnManager->ExecuteTurnActions(false);
		UE_LOG(LogTemp, Warning, TEXT("Enemy chess attacking."));
	}
}
