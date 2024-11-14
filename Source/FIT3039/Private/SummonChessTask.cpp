#include "SummonChessTask.h"
#include "BossAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AutoChess/Chessboard.h"
#include "AutoChess/GridNode.h"
#include "CardDataStruct.h"
#include "Engine/DataTable.h"
#include "TimerManager.h"

EBTNodeResult::Type USummonChessTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    CachedAIController = Cast<ABossAIController>(OwnerComp.GetAIOwner());
    if (!CachedAIController)
    {
        UE_LOG(LogTemp, Error, TEXT("AI Controller is not set."));
        return EBTNodeResult::Failed;
    }

    if (!CachedAIController->Chessboard)
    {
        UE_LOG(LogTemp, Error, TEXT("Chessboard is not set in BossAIController."));
        return EBTNodeResult::Failed;
    }

    // 找到所有符合条件的节点
    AvailableNodes.Reset();
    for (auto& Elem : CachedAIController->Chessboard->NodeMap)
    {
        UGridNode* Node = Elem.Value;
        if (Node && Node->GetOccupant() == EOccupantType::Ot_Enemy && !Node->GetOccupyingChess())
        {
            AvailableNodes.Add(Node);
        }
    }

    if (AvailableNodes.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No available nodes for summoning. Skipping summoning task."));
        // 直接返回成功，表示跳过该任务
        CachedAIController->GetBlackboardComponent()->SetValueAsBool("bIsSummoning", false);
        CachedAIController->GetBlackboardComponent()->SetValueAsBool("bIsAttacking", true);
        return EBTNodeResult::Succeeded;
    }

    // 从 AvailableNodes 中随机选择两个节点
    TArray<UGridNode*> SelectedNodes;
    int32 NumToSelect = FMath::Min(2, AvailableNodes.Num());
    while (SelectedNodes.Num() < NumToSelect)
    {
        int32 RandomIndex = FMath::RandRange(0, AvailableNodes.Num() - 1);
        UGridNode* SelectedNode = AvailableNodes[RandomIndex];
        if (!SelectedNodes.Contains(SelectedNode))
        {
            SelectedNodes.Add(SelectedNode);
        }
    }

    // 将选择的节点存储在 AvailableNodes 中
    AvailableNodes = SelectedNodes;

    // 开始召唤
    CurrentSummonIndex = 0;
    SummonNextPiece();

    return EBTNodeResult::InProgress;
}


void USummonChessTask::SummonNextPiece()
{
    if (CurrentSummonIndex < AvailableNodes.Num())
    {
        // Select a card from the DataTable for summoning
        FCardData* CardData = SelectCardForSummoning(CachedAIController);
        if (!CardData)
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to select card data for summoning."));
            return;
        }

        // Perform the summoning logic using the selected card data
        SummonChessPiece(CachedAIController->GetPawn(), *CardData);

        CurrentSummonIndex++;

        // 延时 0.5 秒后召唤下一枚棋子
        CachedAIController->GetWorld()->GetTimerManager().SetTimer(SummonTimerHandle, this, &USummonChessTask::SummonNextPiece, 0.5f, false);
    }
    else
    {
        // All pieces summoned, set attacking
        CachedAIController->GetBlackboardComponent()->SetValueAsBool("bIsSummoning", false);
        CachedAIController->GetBlackboardComponent()->SetValueAsBool("bIsAttacking", true);
    }
}

FCardData* USummonChessTask::SelectCardForSummoning(ABossAIController* AIController)
{
    if (!AIController->CardDataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("CardDataTable is not set in BossAIController."));
        return nullptr;
    }

    // 从黑板中获取 RowNamesString
    TArray<FName> RowNamesToSummon;
    if (UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent())
    {
        FString RowNamesString = BlackboardComp->GetValueAsString("RowNamesString");

        if (!RowNamesString.IsEmpty())
        {
            TArray<FString> RowNameStrings;
            RowNamesString.ParseIntoArray(RowNameStrings, TEXT(","), true);
            for (const FString& NameString : RowNameStrings)
            {
                FString TrimmedNameString = NameString.TrimStartAndEnd();
                RowNamesToSummon.Add(FName(*TrimmedNameString));
            }
        }
    }

    // 获取数据表中的所有 RowName
    TArray<FCardData*> MatchingCards;
    for (const FName& RowName : AIController->CardDataTable->GetRowNames())
    {
        if (RowNamesToSummon.Contains(RowName))
        {
            FCardData* CardData = AIController->CardDataTable->FindRow<FCardData>(RowName, TEXT(""));
            if (CardData)
            {
                MatchingCards.Add(CardData);
            }
        }
    }

    if (MatchingCards.Num() > 0)
    {
        // 随机选择一个卡牌数据
        int32 RandomIndex = FMath::RandRange(0, MatchingCards.Num() - 1);
        return MatchingCards[RandomIndex];
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No matching cards found for summoning with specified RowNames."));
        return nullptr;
    }
}


void USummonChessTask::SummonChessPiece(AActor* AIControllerOwner, const FCardData& CardData)
{
    ABossAIController* AIController = Cast<ABossAIController>(AIControllerOwner->GetInstigatorController());

    if (AIController && AIController->Chessboard)
    {
        // 获取当前目标节点
        UGridNode* TargetNode = AvailableNodes[CurrentSummonIndex]; // 当前召唤节点

        if (TargetNode)
        {
            // 使用 AIController 中指定的蓝图类来生成棋子
            UClass* ChessPieceClass = AIController->AChessBlueprintClass;

            if (ChessPieceClass)
            {
                // 查找与 TargetNode 关联的碰撞体
                UBoxComponent* CollisionComponent = nullptr;
                for (const auto& Pair : AIController->Chessboard->CollisionComponentToNodeMap)
                {
                    if (Pair.Value == TargetNode)
                    {
                        CollisionComponent = Pair.Key;
                        break;
                    }
                }

                if (CollisionComponent)
                {
                    // 使用碰撞体的位置和旋转来生成棋子
                    FVector SpawnLocation = CollisionComponent->GetComponentLocation();
                    FRotator SpawnRotation = CollisionComponent->GetComponentRotation();

                    // 在 Y 轴上调整 -90 度来对齐
                   //SpawnRotation.Yaw -= 90.0f;

                    AIController->CardInfoPass = CardData;

                    // 使用调整后的位置和旋转来生成棋子
                    AChess* SummonedChess = AChess::SpawnChess(GetWorld(), ChessPieceClass, SpawnLocation, SpawnRotation, false);

                    if (SummonedChess)
                    {
                        // 设置棋子节点
                        SummonedChess->SetCurrentNode(TargetNode);
                        TargetNode->SetOccupyingChess(SummonedChess);
                        AIController->Chessboard->EnemyChessSetUp(SummonedChess, TargetNode);

                        UE_LOG(LogTemp, Log, TEXT("Boss AI summoned chess piece of type %s at (%s)"), *CardData.CardName, *TargetNode->Location.ToString());
                    }
                    else
                    {
                        UE_LOG(LogTemp, Error, TEXT("Failed to summon chess piece at (%s)"), *TargetNode->Location.ToString());
                    }
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("No collision component found for TargetNode"));
                }
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("No chess piece class found for summoning."));
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("No valid target node found for summoning."));
        }
    }
}


            
                

