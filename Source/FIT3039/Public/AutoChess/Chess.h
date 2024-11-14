#pragma once

#include "CoreMinimal.h"
#include "GridNode.h"
#include "GameFramework/Actor.h"
#include "Chess.generated.h"

enum class ERaceType : uint8;
enum class ECardType : uint8;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStatsUpdated, int, Health, int, AttackDamage);

UCLASS()
class FIT3039_API AChess : public AActor
{
	GENERATED_BODY()
    
public:    
	// Sets default values for this actor's properties
	AChess();

	void InitializeChess(bool bPlayerControlled);
	

	static AChess* SpawnChess(UWorld* World, TSubclassOf<AChess> ChessClass, FVector Location, FRotator Rotation, bool bPlayerControlled);


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimationAsset* MoveAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimationAsset* RitualToolAnimation;
	
	UFUNCTION(BlueprintCallable, Category = "ChessPiece|Animation")
	void PlayMoveAnimation();

	UFUNCTION(BlueprintCallable, Category = "ChessPiece|Animation")
	void PlayRitualToolAnimation();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:    
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Mesh")
	USkeletalMeshComponent *ChessMesh;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chess")
	int32 AttackDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chess")
	int32 Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chess")
	ECardType ChessType;

	UPROPERTY(BlueprintAssignable, Category = "Stats")
	FOnStatsUpdated OnStatsUpdated;

	bool bFirstRound = true;

	// Getter for ChessType
	ECardType GetChessType() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chess")
	ERaceType RaceType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chess")
	bool bIsPlayerControlled;

	void Attack(AChess *TargetChess);

	
	void ApplyHealing(int32 HealAmount);

	
	bool IsPlayerControlled() const { return bIsPlayerControlled; }
	
	// Get and set functions for the current node
	UFUNCTION(BlueprintCallable, Category = "Chess")
	UGridNode* GetCurrentNode() const;

	UFUNCTION(BlueprintCallable, Category = "Chess")
	void SetCurrentNode(UGridNode* NewNode);

	UFUNCTION(BlueprintCallable, Category = "Chess")
	void Die();
	
private:
	UPROPERTY()
	UGridNode* CurrentNode;


	
};

