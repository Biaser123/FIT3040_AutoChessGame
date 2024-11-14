#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TurnManager.generated.h"

// 前置声明
class AChessboard;
class AChess;
class ABossAIController;

UCLASS()
class FIT3039_API ATurnManager : public AActor
{
    GENERATED_BODY()

public:
    ATurnManager();

protected:
    virtual void BeginPlay() override;

public:
    // 设置棋盘
    UFUNCTION(BlueprintCallable)
    void SetChessboard(AChessboard* InChessboard);

 
    UFUNCTION(BlueprintCallable)
    void EndPlayerTurn();

    // 执行回合行动
    UFUNCTION(BlueprintCallable)
    void ExecuteTurnActions(bool bIsPlayerTurnParam);
    
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    ABossAIController* BossAIController;

    UFUNCTION(BlueprintCallable)
    void StopAllChessActions();

private:
    // 按序执行下一步棋子的行动
    void ProcessNextChessAction(int32 CurrentIndex, bool bIsPlayerTurnParam);

    // 移动棋子到目标位置
    void StartMove(AChess* CurrentChess, AChess* Target, float MoveTime, const FVector& StartLocation, const FVector& TargetLocation, const FVector& ReturnLocation);

    // 更新移动中的棋子位置
    void UpdateMoveToTarget();

    // 将棋子移回初始位置
    void ReturnToStart();

    // 更新返回中的棋子位置
    void UpdateReturnToStart();

    // 触发召唤阶段
    void TriggerSummoningPhase();

    // 触发抽卡阶段
    void TriggerDrawCardPhase();

   // void ProcessRitualToolEffect(AChess* RitualTool);

private:
    // 存储棋盘引用
    AChessboard* Chessboard;

    bool bIsGameOver = false;

    // 用于处理移动和返回的计时器
    FTimerHandle MoveTimerHandle;
    FTimerHandle ReturnTimerHandle;

    // 当前移动中的棋子信息
    AChess* CurrentChess;
    AChess* TargetChess;
    FVector StartLocation;
    FVector TargetLocation;
    FVector ReturnLocation;

    // 移动的初始时间和时间差
    float MoveStartTime;
    float MoveTime;

    // 控制移动速度的倍增器
    float MoveSpeedMultiplier;

    // 事件广播
    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFightStart);
    UPROPERTY(BlueprintAssignable)
    FOnFightStart OnFightStart;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCouldDrawCard);
    UPROPERTY(BlueprintAssignable)
    FOnCouldDrawCard OnCouldDrawCard;

    TArray<AChess*> FilteredChess;
};
