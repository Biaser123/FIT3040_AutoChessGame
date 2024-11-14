// Fill out your copyright notice in the Description page of Project Settings.

#include "AutoChess/Chessboard.h"

#include "Card.h"
#include "ProceduralMeshComponent.h"


AChessboard::AChessboard()
{
	PrimaryActorTick.bCanEverTick = true;

	MapMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("MapMesh"));
	MapMesh->SetupAttachment(RootComponent);

	BossCoordinate = CreateDefaultSubobject<USceneComponent>(TEXT("BossCoordinate"));
	PlayerCoordinate = CreateDefaultSubobject<USceneComponent>(TEXT("PlayerCoordinate"));
	BossCoordinate->SetupAttachment(RootComponent);
	PlayerCoordinate->SetupAttachment(RootComponent);
}


void AChessboard::GenerateChessBoard()
{
	// clean the collision
	for (UBoxComponent* OldCollisionComponent : CollisionComponents)
	{
		if (OldCollisionComponent)
		{
			OldCollisionComponent->DestroyComponent();
		}
	}
	CollisionComponents.Reset();

	// clean the map
	CollisionComponentToNodeMap.Reset();
    
	//reset and regenerate
	NodeMap.Reset();
	GenerateNodes(Size, Roll, Column);
	GenerateChessBoardMesh();
}

UGridNode* AChessboard::GetNode(FGridVector InCoord) const
{
	if (NodeMap.Contains(InCoord))
		return NodeMap[InCoord];
	else
		return nullptr;
}

void AChessboard::GenerateNodes(float InSize, int InRoll, int InColumn)
{
	if (!GetWorld())
	{
		UE_LOG(LogTemp, Error, TEXT("World is null in GenerateNodes"));
		return;
	}
	
	int TotalColumns = InColumn;

	for (int X = 0; X < InRoll; X++)
	{
		for (int Y = 0; Y < TotalColumns; Y++)
		{
			FVector Location = FVector(X * (InSize + DistanceX), Y * (InSize + DistanceY), 0) + GetActorLocation();
			

			FGridVector Coordinate(X, Y);

			// Determine if this node is in the enemy area
			EOccupantType OccupantType = (Y < InColumn / 2) ? EOccupantType::Ot_Enemy : EOccupantType::Ot_Empty;

			// Create node
			UGridNode* Node = NewObject<UGridNode>(this, UGridNode::StaticClass());
			if (Node)
			{
				Node->InitNode(this, Location, Coordinate, InSize, OccupantType);
				NodeMap.Add(Coordinate, Node);
				
				// Create collision for each node 
				// Calculate the center of the chessboard for correct alignment
				FVector BoardCenter = GetActorLocation() + FVector((InRoll - 1) * (InSize + DistanceX) / 2, (InColumn - 1) * (InSize + DistanceY) / 2, 0);
				FVector AdjustedLocation = BoardCenter + (Location - GetActorLocation());

				// Calculate relative position of each collision component based on the grid position
				FVector RelativeLocation = FVector(X * (InSize + DistanceX), Y * (InSize + DistanceY), 0);
				FRotator RelativeRotation = FRotator::ZeroRotator; // Assuming each grid is aligned without additional rotation

				// Create collision for each node
				UBoxComponent* CollisionComponent = NewObject<UBoxComponent>(this, UBoxComponent::StaticClass(), NAME_None, RF_NoFlags, nullptr, false);
				if (CollisionComponent)
				{
					CollisionComponent->RegisterComponent();

					if (MapMesh && MapMesh->IsRegistered())
					{
						CollisionComponent->AttachToComponent(MapMesh, FAttachmentTransformRules::KeepRelativeTransform);
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("MapMesh is not initialized or registered"));
					}

					CollisionComponent->SetRelativeLocation(RelativeLocation);  // Set relative position within MapMesh
					CollisionComponent->SetRelativeRotation(RelativeRotation);  // Set relative rotation within MapMesh
					CollisionComponent->SetBoxExtent(FVector(InSize / 2, InSize / 2, 5));
					CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
					CollisionComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

					CollisionComponents.Add(CollisionComponent);
				}



				


					// Update the map
					CollisionComponentToNodeMap.Add(CollisionComponent, Node);
				}
			}
		}
	
}


void AChessboard::EnableRayCast()
{
	bIsRayCastingEnabled = true;
}

void AChessboard::DisableRayCast()
{
	bIsRayCastingEnabled = false;
}

void AChessboard::GenerateChessBoardMesh()
{
	int Index = 0;
	MapMesh->ClearAllMeshSections();
	for (auto a : NodeMap)
	{
		if (!a.Value)
			continue;
		TArray<FVector> Vertices;
		TArray<int32> Indices;
		TArray<FVector> Normals;
		TArray<FVector2D> UV1, EmptyArray;
		TArray<FColor> VertexColors;
		TArray<FVector> Tangents;
		TArray<FProcMeshTangent> MeshTangents;
		a.Value->MeshIndex = Index;
		a.Value->DrawNode(Vertices, Indices, Normals, UV1, VertexColors, Tangents, GetActorLocation());
		for (FVector& b : Tangents)
			MeshTangents.Add(FProcMeshTangent(b, false));
		MapMesh->CreateMeshSection(Index, Vertices, Indices, Normals, UV1, EmptyArray, EmptyArray, EmptyArray, VertexColors, MeshTangents, false);
		//reset
		ResetNodeMaterial(a.Value);
		Index++;
	}
}

UGridNode* AChessboard::GetNodeFromRaycast(const FVector& StartPoint, const FVector& EndPoint) const
{
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;

	// ray cast
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartPoint, EndPoint, ECC_Visibility, CollisionParams);

	if (bHit && HitResult.GetComponent())
	{
		UBoxComponent* HitBoxComponent = Cast<UBoxComponent>(HitResult.GetComponent());
		if (HitBoxComponent)
		{
			UGridNode* const* FoundNode = CollisionComponentToNodeMap.Find(HitBoxComponent);
			if (FoundNode)
			{
				return *FoundNode;
			}
		}
	}
	return nullptr;
}

void AChessboard::OnGridNodeHovered(UGridNode* HoveredNode)
{
	if (HoveredNode->GetOccupant() == EOccupantType::Ot_Empty)
	{
		if (BP_ChessClass != nullptr)
		{
			// 查找与 HoveredNode 对应的碰撞体
			UBoxComponent* CollisionComponent = nullptr;
			for (const auto& Pair : CollisionComponentToNodeMap)
			{
				if (Pair.Value == HoveredNode)
				{
					CollisionComponent = Pair.Key;
					break;
				}
			}

			if (CollisionComponent)
			{
				FVector SpawnLocation = CollisionComponent->GetComponentLocation();
				FRotator SpawnRotation = CollisionComponent->GetComponentRotation();

				// 在 Y 轴上调整 90 度来面对玩家
				//SpawnRotation.Yaw += 90.0f;

				//SpawnLocation.Z += 3;  // 提升棋子高度以防与地面重叠

				// 生成棋子并应用旋转
				AChess* Chess = GetWorld()->SpawnActor<AChess>(BP_ChessClass, SpawnLocation, SpawnRotation);

				if (Chess)
				{
					HoveredNode->SetOccupyingChess(Chess);
					Chess->SetCurrentNode(HoveredNode);
					SummonedChess.Add(Chess);

					HoveredNode->SetOccupant(EOccupantType::Ot_Occupied);
					IsChessSpawn = true;
					OnChessSpawned.Broadcast();
					ResetNodeMaterialAll();
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("No collision component found for hovered node"));
			}
		}
	}
}



void AChessboard::SetNodeMaterial(UGridNode* InNode, UMaterialInterface* InMaterial)
{
	if (!InNode || !InMaterial)
		return;
	MapMesh->SetMaterial(InNode->MeshIndex, InMaterial);
}

void AChessboard::SetAllInteractableMaterial()
{
	for (auto a : NodeMap)
	{
		if (!a.Value)
			continue;
		if(a.Value->GetOccupant() == EOccupantType::Ot_Empty)
			SetNodeMaterial(a.Value,HighlightMaterial);
	}
}

void AChessboard::ResetNodeMaterial(UGridNode* InNode)
{
	if (!InNode)
		return;

	// Check the occupant type and set the material accordingly
	if (InNode->GetOccupant() == EOccupantType::Ot_Enemy)
	{
		SetNodeMaterial(InNode, EnemyMaterial);
	}
	else
	{
		SetNodeMaterial(InNode, NormalMaterial);
	}
}

void AChessboard::ResetNodeMaterialAll()
{
	for (auto a : NodeMap)
	{
		if (!a.Value)
			continue;
		ResetNodeMaterial(a.Value);
	}
}

bool AChessboard::IsCoordValid(const FGridVector& Coord) const
{
	return Coord.X >= 0 && Coord.X < Roll && Coord.Y >= 0 && Coord.Y < Column;
}

void AChessboard::PostInitProperties()
{
	Super::PostInitProperties();
	//GenerateChessBoard();
}

void AChessboard::BeginPlay()
{
	Super::BeginPlay();

	if (GetWorld() == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("World is null in BeginPlay"));
	}
	
	GenerateChessBoard();

	//testing 
	// if (BP_ChessClass != nullptr)
	// {
		// 获取第一个目标网格节点
		// UGridNode* Node1 = GetNode(FGridVector(0, 0));  // 根据需要调整坐标
		// if (Node1)
		// {
		// 	FVector SpawnLocation = Node1->Location;
		// 	FRotator SpawnRotation(0.0f, 90.0f, 0.0f);  // 方向可调整
		// 	AChess* EnemyChess1 = GetWorld()->SpawnActor<AChess>(BP_ChessClass, SpawnLocation, SpawnRotation);
		// 	if (EnemyChess1)
		// 	{
		// 		EnemyChess1->bIsPlayerControlled = false;  // 设置为敌方控制
		// 		EnemyChess1->SetCurrentNode(Node1);        // 设置棋子的当前节点
		// 		Node1->SetOccupyingChess(EnemyChess1);     // 在节点中注册棋子
		// 		SummonedChess.Add(EnemyChess1);
		// 	}
		// }
	//
	// 	// 获取第二个目标网格节点
	// 	UGridNode* Node2 = GetNode(FGridVector(1, 0));  // 根据需要调整坐标
	// 	if (Node2)
	// 	{
	// 		FVector SpawnLocation = Node2->Location;
	// 		FRotator SpawnRotation(0.0f, 90.0f, 0.0f);  
	// 		AChess* EnemyChess2 = GetWorld()->SpawnActor<AChess>(BP_ChessClass, SpawnLocation, SpawnRotation);
	// 		if (EnemyChess2)
	// 		{
	// 			EnemyChess2->bIsPlayerControlled = false;  // 设置为敌方控制
	// 			EnemyChess2->SetCurrentNode(Node2);        // 设置棋子的当前节点
	// 			Node2->SetOccupyingChess(EnemyChess2);     // 在节点中注册棋子
	// 			SummonedChess.Add(EnemyChess2);
	// 		}
	// 	}
	// }
}

void AChessboard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsRayCastingEnabled)
	{
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		if (PlayerController)
		{
			FVector2D MousePosition;
			if (PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y))
			{
				UE_LOG(LogTemp, Log, TEXT("Mouse Position: X=%f, Y=%f"), MousePosition.X, MousePosition.Y);
				
				FVector WorldLocation, WorldDirection;
				if (PlayerController->DeprojectScreenPositionToWorld(MousePosition.X, MousePosition.Y, WorldLocation, WorldDirection))
				{
					FVector StartPoint = WorldLocation;
					FVector EndPoint = StartPoint + (WorldDirection * 10000.0f); 

					UE_LOG(LogTemp, Log, TEXT("World Location: %s, World Direction: %s"), *WorldLocation.ToString(), *WorldDirection.ToString());

					UGridNode* HoveredNode = GetNodeFromRaycast(StartPoint, EndPoint);
					
					if(HoveredNode)
					{
						UE_LOG(LogTemp, Log, TEXT("Hovered Node Found at: %s"), *HoveredNode->Location.ToString());
						OnGridNodeHovered(HoveredNode);
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("No node hit by raycast."));
					}
					DisableRayCast();
				}
			}
		}		
	}
}

TArray<AChess*> AChessboard::GetSummonedChess() const
{
	// Return all summoned chess pieces
	return SummonedChess;
}

void AChessboard::ExecuteAttackOrder(AChess* Attacker)
{
	if (!Attacker || Attacker->Health <= 0)
		return;

	bool bAttackMainBoss = false;
	AChess* Target = GetTargetInPriority(Attacker, bAttackMainBoss);

	if (bAttackMainBoss)
	{
		if (Attacker->bIsPlayerControlled)
		{
			if(CurrentBoss)
			{
				//control the animation, and use the anim notify to reset the bool
				CurrentBoss->bIsDefending = true;
			}
			EnemyHealth -= Attacker->AttackDamage;
			UE_LOG(LogTemp, Log, TEXT("Player's chess piece attacked enemy main boss! Enemy health is now %d."), EnemyHealth);
		}
		else
		{
			if(CurrentBoss)
			{
				CurrentBoss->bIsAttacking = true;
			}
			PlayerHealth -= Attacker->AttackDamage;
			UE_LOG(LogTemp, Log, TEXT("Enemy's chess piece attacked player main boss! Player health is now %d."), PlayerHealth);
		}
		CheckGameOver();
	}
	else if (Target && Target->Health > 0)
	{
		Attacker->Attack(Target);

		// 标记要移除的目标
		TArray<AChess*> ChessToRemove;
		if (Target->Health <= 0)
		{
			Target->Die();
			ChessToRemove.Add(Target);
		}

		UE_LOG(LogTemp, Log, TEXT("Attacker of type %d attacked target of type %d! Target health is now %d."),
			static_cast<int32>(Attacker->ChessType),
			static_cast<int32>(Target->ChessType),
			Target->Health);

		// 在循环后移除棋子
		for (AChess* Chess : ChessToRemove)
		{
			SummonedChess.RemoveSingle(Chess);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid or dead target, skipping attack."));
	}
}

AChess* AChessboard::GetTargetInPriority(AChess* Attacker, bool& bAttackMainBoss)
{
    AChess* ClosestSameRow = nullptr;
    AChess* ClosestAdjacentColumn = nullptr;

    bAttackMainBoss = false;

    FGridVector AttackerCoord = Attacker->GetCurrentNode()->Coordinate;

    int32 MinDistanceSameRow = INT_MAX;
    int32 MinDistanceAdjacentColumn = INT_MAX;

    for (AChess* ChessPiece : SummonedChess)
    {
        if (ChessPiece && ChessPiece->IsPlayerControlled() != Attacker->IsPlayerControlled() && ChessPiece->Health > 0)
        {
            FGridVector EnemyCoord = ChessPiece->GetCurrentNode()->Coordinate;

            if (EnemyCoord.X == AttackerCoord.X)
            {
                int32 Distance = FMath::Abs(EnemyCoord.Y - AttackerCoord.Y);
                if (Distance < MinDistanceSameRow)
                {
                    MinDistanceSameRow = Distance;
                    ClosestSameRow = ChessPiece;
                }
            }
            else if (FMath::Abs(EnemyCoord.X - AttackerCoord.X) == 1)
            {
                int32 Distance = FMath::Abs(EnemyCoord.Y - AttackerCoord.Y);
                if (Distance < MinDistanceAdjacentColumn)
                {
                    MinDistanceAdjacentColumn = Distance;
                    ClosestAdjacentColumn = ChessPiece;
                }
            }
        }
    }

    // 优先选择最近的棋子，无论是同一行还是相邻列
    if (MinDistanceAdjacentColumn < MinDistanceSameRow && ClosestAdjacentColumn)
    {
        UE_LOG(LogTemp, Log, TEXT("Targeting ClosestAdjacentColumn at (%d, %d)"), ClosestAdjacentColumn->GetCurrentNode()->Coordinate.X, ClosestAdjacentColumn->GetCurrentNode()->Coordinate.Y);
        return ClosestAdjacentColumn;
    }
    else if (ClosestSameRow)
    {
        UE_LOG(LogTemp, Log, TEXT("Targeting ClosestSameRow at (%d, %d)"), ClosestSameRow->GetCurrentNode()->Coordinate.X, ClosestSameRow->GetCurrentNode()->Coordinate.Y);
        return ClosestSameRow;
    }

    bAttackMainBoss = true;
    UE_LOG(LogTemp, Log, TEXT("No valid enemy found, targeting Main Boss"));
    return nullptr;
}



void AChessboard::ApplyRitualToolEffect(AChess* RitualTool)
{
	if (!RitualTool || RitualTool->GetChessType() != ECardType::Ritual_Tools)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid Ritual Tool or wrong chess type."));
		return;
	}

	// 获取 Ritual Tool 所在的当前节点
	UGridNode* ToolNode = RitualTool->GetCurrentNode();
	if (!ToolNode)
	{
		UE_LOG(LogTemp, Warning, TEXT("Ritual Tool is not placed on a valid node."));
		return;
	}

	FGridVector ToolCoord = ToolNode->Coordinate;

	// 遍历所有召唤的棋子，检查是否是同排的友方 Ghost 棋子
	for (AChess* ChessPiece : SummonedChess)
	{
		// 检查棋子是否有效，并且是否为友方 Ghost 棋子
		if (ChessPiece && ChessPiece->IsPlayerControlled() == RitualTool->IsPlayerControlled() && ChessPiece->GetChessType() == ECardType::Ghost)
		{
			UGridNode* GhostNode = ChessPiece->GetCurrentNode();
			if (!GhostNode)
			{
				UE_LOG(LogTemp, Warning, TEXT("Ghost chess piece is not placed on a valid node, skipping effect."));
				continue;
			}

			FGridVector GhostCoord = GhostNode->Coordinate;

			// 检查是否在同一排
			if (GhostCoord.X == ToolCoord.X)
			{
				// 调用 Ghost 棋子的治疗函数，增加 1 点血量
				ChessPiece->ApplyHealing(1);

				// 输出日志
				UE_LOG(LogTemp, Log, TEXT("Ritual Tool %s increased health of Ghost at (%d, %d) to %d."),
					   *RitualTool->GetName(),
					   GhostCoord.X, GhostCoord.Y,
					   ChessPiece->Health);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Chess piece is either invalid or not a friendly Ghost."));
		}
	}
}


void AChessboard::EnemyChessSetUp(AChess* Chess, UGridNode* GridNode)
{
	Chess->bIsPlayerControlled = false;  // 设置为敌方控制
	Chess->SetCurrentNode(GridNode);        // 设置棋子的当前节点
	GridNode->SetOccupyingChess(Chess);     // 在节点中注册棋子
	SummonedChess.Add(Chess);
}

void AChessboard::CheckGameOver()
{
	if (PlayerHealth <= 0)
	{
		GameOver.Broadcast(false);
	}
	else if (EnemyHealth <= 0)
	{
		GameOver.Broadcast(true);
	}
}


UGridNode* AChessboard::GetNodeFromCollisionComponent(UBoxComponent* CollisionComponent) const
{
	if (CollisionComponentToNodeMap.Contains(CollisionComponent))
	{
		return CollisionComponentToNodeMap[CollisionComponent];
	}
	return nullptr;
}
