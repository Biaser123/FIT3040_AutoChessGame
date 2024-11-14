#include "AutoChess/TurnManager.h"
#include "BossAIController.h"
#include "Card.h"
#include "AutoChess/Chessboard.h"
#include "Engine/World.h"

ATurnManager::ATurnManager()
{
    PrimaryActorTick.bCanEverTick = false;
    MoveSpeedMultiplier = 0.5f;
}

void ATurnManager::BeginPlay()
{
    Super::BeginPlay();
}

void ATurnManager::SetChessboard(AChessboard* InChessboard)
{
    Chessboard = InChessboard;
}

void ATurnManager::EndPlayerTurn()
{
    OnFightStart.Broadcast();
    if (Chessboard)
    {
        UE_LOG(LogTemp, Warning, TEXT("Player turn ended, executing player's chess actions..."));
        ExecuteTurnActions(true);
    }
}

void ATurnManager::ExecuteTurnActions(bool bIsPlayerTurnParam)
{
    if (!Chessboard)
    {
        UE_LOG(LogTemp, Error, TEXT("Chessboard reference is null in ExecuteTurnActions."));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("Executing actions for %s turn..."), bIsPlayerTurnParam ? TEXT("player's") : TEXT("enemy's"));

    // 清空上一次的 FilteredChess 列表
    FilteredChess.Empty();

    // 获取当前回合能行动的棋子，将法器和鬼魂都加入 FilteredChess
    for (AChess* Chess : Chessboard->GetSummonedChess())
    {
        if (Chess && Chess->IsPlayerControlled() == bIsPlayerTurnParam)
        {
            FilteredChess.Add(Chess);
        }
    }

    // 如果没有任何棋子，直接跳转到下一阶段
    if (FilteredChess.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No active chess pieces found, skipping to next phase."));
        if (bIsPlayerTurnParam)
        {
            TriggerSummoningPhase();
        }
        else
        {
            TriggerDrawCardPhase();
        }
        return;
    }

    // 按照从左到右、从上到下的顺序排序 FilteredChess
    FilteredChess.Sort([](const AChess& A, const AChess& B)
    {
        UGridNode* NodeA = A.GetCurrentNode();
        UGridNode* NodeB = B.GetCurrentNode();
        if (NodeA->Coordinate.X == NodeB->Coordinate.X)
        {
            return NodeA->Coordinate.Y < NodeB->Coordinate.Y;
        }
        return NodeA->Coordinate.X < NodeB->Coordinate.X;
    });

    ProcessNextChessAction(0, bIsPlayerTurnParam);
}




void ATurnManager::ProcessNextChessAction(int32 CurrentIndex, bool bIsPlayerTurnParam)
{

    if (bIsGameOver) 
    {
        UE_LOG(LogTemp, Warning, TEXT("Game is over, actions are stopped."));
        return;
    }
    if (CurrentIndex >= FilteredChess.Num())
    {
        UE_LOG(LogTemp, Warning, TEXT("All chess actions completed for this turn."));
        if (bIsPlayerTurnParam)
        {
            TriggerSummoningPhase();
        }
        else
        {
            TriggerDrawCardPhase();
        }
        return;
    }

    CurrentChess = FilteredChess[CurrentIndex];
    
    if (!IsValid(CurrentChess))
    {
        UE_LOG(LogTemp, Warning, TEXT("Current chess is no longer valid, skipping to next chess action."));
        ProcessNextChessAction(CurrentIndex + 1, bIsPlayerTurnParam);
        return;
    }
    
    UE_LOG(LogTemp, Log, TEXT("Processing chess piece: %s, Health: %d"), *CurrentChess->GetName(), CurrentChess->Health);

    if (CurrentChess->GetChessType() == ECardType::Ritual_Tools)
    {
        // 如果是法器，播放法器动画并在0.5秒后应用效果
        CurrentChess->PlayRitualToolAnimation();

        FTimerHandle EffectTimerHandle;
        GetWorld()->GetTimerManager().SetTimer(EffectTimerHandle, [this, CurrentIndex, bIsPlayerTurnParam]()
        {
            // 应用法器效果
            Chessboard->ApplyRitualToolEffect(CurrentChess);
            UE_LOG(LogTemp, Log, TEXT("Ritual tool effect applied for: %s"), *CurrentChess->GetName());

            // 在应用法器效果后，再等待0.5秒再进入下一个棋子的行动
            FTimerHandle NextActionTimerHandle;
            GetWorld()->GetTimerManager().SetTimer(NextActionTimerHandle, [this, CurrentIndex, bIsPlayerTurnParam]()
            {
                // 继续下一个棋子的行动
                ProcessNextChessAction(CurrentIndex + 1, bIsPlayerTurnParam);
            }, 0.5f, false);

        }, 0.5f, false);
    }

    else if (CurrentChess->GetChessType() == ECardType::Ghost)
    {
        // 如果是鬼魂，进行攻击逻辑
        if (CurrentChess->bFirstRound)
        {
            CurrentChess->bFirstRound = false;
            ProcessNextChessAction(CurrentIndex + 1, bIsPlayerTurnParam);
            return;
        }

        bool bAttackMainBoss = false;
        TargetChess = Chessboard->GetTargetInPriority(CurrentChess, bAttackMainBoss);

        if (!IsValid(TargetChess) || TargetChess->Health <= 0)
        {
            if (bAttackMainBoss)
            {
                TargetLocation = CurrentChess->bIsPlayerControlled ? 
                    Chessboard->BossCoordinate->GetComponentLocation() : 
                    Chessboard->PlayerCoordinate->GetComponentLocation();

                UE_LOG(LogTemp, Log, TEXT("%s is attacking the main boss at location: %s"), *CurrentChess->GetName(), *TargetLocation.ToString());
                StartMove(CurrentChess, nullptr, 1.0f, CurrentChess->GetActorLocation(), TargetLocation, CurrentChess->GetActorLocation());
            }
            return;
        }

        StartLocation = CurrentChess->GetActorLocation();
        TargetLocation = TargetChess->GetActorLocation();
        ReturnLocation = StartLocation;
        MoveTime = 1.0f;

        UE_LOG(LogTemp, Log, TEXT("%s is attacking target: %s at location: %s"), *CurrentChess->GetName(), *TargetChess->GetName(), *TargetLocation.ToString());
        StartMove(CurrentChess, TargetChess, MoveTime, StartLocation, TargetLocation, ReturnLocation);
    }
    else
    {
        // 如果不是法器或鬼魂，跳过当前棋子
        ProcessNextChessAction(CurrentIndex + 1, bIsPlayerTurnParam);
    }
}

void ATurnManager::StartMove(AChess* InCurrentChess, AChess* InTarget, float InMoveTime, const FVector& InStartLocation, const FVector& InTargetLocation, const FVector& InReturnLocation)
{
    UE_LOG(LogTemp, Warning, TEXT("Starting movement to target..."));

    CurrentChess = InCurrentChess;
    TargetChess = InTarget;
    StartLocation = InStartLocation;
    TargetLocation = InTargetLocation;
    ReturnLocation = InReturnLocation;
    MoveTime = InMoveTime;

    MoveStartTime = GetWorld()->GetTimeSeconds();

    InCurrentChess->PlayMoveAnimation();

    GetWorld()->GetTimerManager().SetTimer(MoveTimerHandle, this, &ATurnManager::UpdateMoveToTarget, 0.01f, true);
}

void ATurnManager::UpdateMoveToTarget()
{
    if (!IsValid(CurrentChess))
    {
        UE_LOG(LogTemp, Warning, TEXT("Current chess is no longer valid during movement."));
        GetWorld()->GetTimerManager().ClearTimer(MoveTimerHandle);
        return;
    }

    float CurrentTime = GetWorld()->GetTimeSeconds();
    float TimeElapsed = CurrentTime - MoveStartTime;
    float MoveFraction = FMath::Clamp(TimeElapsed / MoveTime, 0.0f, 1.0f);

    FVector NewLocation = FMath::Lerp(StartLocation, TargetLocation, MoveFraction);
    CurrentChess->SetActorLocation(NewLocation);

    if (MoveFraction >= 1.0f)
    {
        GetWorld()->GetTimerManager().ClearTimer(MoveTimerHandle);
        UE_LOG(LogTemp, Log, TEXT("%s has reached the target location and is attacking."), *CurrentChess->GetName());
        Chessboard->ExecuteAttackOrder(CurrentChess);
        ReturnToStart();
    }
}

void ATurnManager::ReturnToStart()
{
    MoveStartTime = GetWorld()->GetTimeSeconds();
    MoveTime = 0.5f;

    GetWorld()->GetTimerManager().SetTimer(ReturnTimerHandle, this, &ATurnManager::UpdateReturnToStart, 0.01f, true);
}

void ATurnManager::UpdateReturnToStart()
{
    if (!IsValid(CurrentChess))
    {
        GetWorld()->GetTimerManager().ClearTimer(ReturnTimerHandle);
        return;
    }

    float CurrentTime = GetWorld()->GetTimeSeconds();
    float TimeElapsed = CurrentTime - MoveStartTime;
    float MoveFraction = FMath::Clamp(TimeElapsed / MoveTime, 0.0f, 1.0f);

    FVector NewReturnLocation = FMath::Lerp(TargetLocation, ReturnLocation, MoveFraction);
    CurrentChess->SetActorLocation(NewReturnLocation);

    if (MoveFraction >= 1.0f)
    {
        GetWorld()->GetTimerManager().ClearTimer(ReturnTimerHandle);
        ProcessNextChessAction(FilteredChess.Find(CurrentChess) + 1, CurrentChess->IsPlayerControlled());
    }
}

void ATurnManager::TriggerSummoningPhase()
{
    if (BossAIController)
    {
        BossAIController->TriggerSummoningPhase();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("BossAIController is null, skipping summoning phase."));
    }
}

void ATurnManager::TriggerDrawCardPhase()
{
    OnCouldDrawCard.Broadcast();
}


void ATurnManager::StopAllChessActions()
{
    bIsGameOver = true;
    GetWorld()->GetTimerManager().ClearAllTimersForObject(this); // 清除所有计时器，停止棋子行动
}