	// Fill out your copyright notice in the Description page of Project Settings.

	#pragma once

	#include "CoreMinimal.h"
#include "Boss.h"
#include "GridNode.h"
	#include "Components/StaticMeshComponent.h"
	#include "Chess.h"

	#include "GameFramework/Actor.h"
	#include "Chessboard.generated.h"

	class UProceduralMeshComponent;
	class UMaterial;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChessSpawnedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameOver,bool,PlayerWin);

	UCLASS(HideCategories=(Materials))
	class FIT3039_API AChessboard : public AActor
	{
		GENERATED_BODY()
		
	public:	
		// Sets default values for this actor's properties
		AChessboard();

		//spawn Chess
		UPROPERTY(BlueprintAssignable)
		FOnChessSpawnedDelegate OnChessSpawned;

		UPROPERTY(BlueprintAssignable)
		FOnGameOver GameOver;
		
	//基础-棋盘单位大小
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Size;

		//基础-棋盘行数
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Roll;

		//setting in editor
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Column;
		
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
		float DistanceX; 

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
		float DistanceY;
		

		//基础-棋盘的棋格
		UPROPERTY()
		TMap<FGridVector, UGridNode*> NodeMap;

		//基础-生成棋盘
		UFUNCTION(BlueprintCallable)
		void GenerateChessBoard();

		UFUNCTION(BlueprintCallable)
		UGridNode* GetNode(FGridVector InCoord) const;

		//基础-生成棋格入口
		UFUNCTION()
		void GenerateNodes(float InSize, int InRoll, int InColumn);
		
		
		//模型-棋盘模型
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UProceduralMeshComponent* MapMesh;

		//模型-可以放置格子的材质
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UMaterialInterface* NormalMaterial;

		//模型-不可以放置格子的材质
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UMaterialInterface* HighlightMaterial;
		
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UMaterialInterface* EnemyMaterial;
		
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UMaterialInterface* DebugMaterial;
		

		//rayCast for Summon chess
		UFUNCTION(BlueprintCallable, Category = "RayCast")
		void EnableRayCast();

		UFUNCTION(BlueprintCallable, Category = "RayCast")
		void DisableRayCast();

		
		
		//模型-生成棋盘模型
		UFUNCTION(BlueprintCallable)
		void GenerateChessBoardMesh();

		//棋格碰撞体积创建
		UPROPERTY(EditAnywhere,BlueprintReadWrite)
		TArray<UBoxComponent*> CollisionComponents;

		//棋格碰撞体积映射到对应棋格
		UPROPERTY(EditAnywhere,BlueprintReadWrite)
		TMap<UBoxComponent*, UGridNode*> CollisionComponentToNodeMap;

		//Ray cast-get node
		UFUNCTION(BlueprintCallable)
		UGridNode *GetNodeFromRaycast(const FVector& StartPoint, const FVector& EndPoint) const;
		
		// 当前被射线照射的棋格
		UPROPERTY(EditAnywhere,BlueprintReadWrite)
		UGridNode* CurrentHoveredNode = nullptr;
		
		UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Chessboard")
		TSubclassOf<AChess> BP_ChessClass;
		
		UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Chessboard")
		bool IsChessSpawn = false;
		
		//tracer-deal hovered Node
		UFUNCTION(BlueprintCallable, Category = "Chessboard")
		void OnGridNodeHovered(UGridNode* HoveredNode);
		
		
		//Material
		UFUNCTION(BlueprintCallable)
		void SetNodeMaterial(UGridNode* InNode, UMaterialInterface* InMaterial);

		//Material
		UFUNCTION(BlueprintCallable)
		void SetAllInteractableMaterial();

		//Material
		UFUNCTION(BlueprintCallable)
		void ResetNodeMaterial(UGridNode* InNode);

		//Material
		UFUNCTION(BlueprintCallable)
		void ResetNodeMaterialAll();
		
		bool IsCoordValid(const FGridVector& Coord) const;

		TArray<AChess*> GetSummonedChess() const;

		void ExecuteAttackOrder(AChess* Attacker);

		void ApplyRitualToolEffect(AChess* RitualTool);

		void EnemyChessSetUp(AChess* Chess, UGridNode* GridNode);
		
	protected:
		// Called when the game starts or when spawned
		virtual void BeginPlay() override;

		virtual void PostInitProperties() override;


	public:	
		// Called every frame
		virtual void Tick(float DeltaTime) override;

		UPROPERTY(EditAnywhere,BlueprintReadWrite)
		int32 PlayerHealth = 10;
		UPROPERTY(EditAnywhere,BlueprintReadWrite)
		int32 EnemyHealth = 10;

		void CheckGameOver();

		AChess* GetTargetInPriority(AChess* Attacker,bool& bAttackMainBoss);

		UPROPERTY(EditAnywhere,BlueprintReadWrite)
		ABoss* CurrentBoss;
		
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Coordinates")
		USceneComponent* BossCoordinate; //for the card animation 

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Coordinates")
		USceneComponent* PlayerCoordinate;  

		UGridNode* GetNodeFromCollisionComponent(UBoxComponent* CollisionComponent) const;

		

	private:
		bool bIsRayCastingEnabled = false;

	
		TArray<AChess*> SummonedChess;
		
	};
