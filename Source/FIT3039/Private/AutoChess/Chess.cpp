#include "AutoChess/Chess.h"

#include "Card.h"

// Sets default values
AChess::AChess()
{
	PrimaryActorTick.bCanEverTick = true;
	Health = 2;
	AttackDamage = 1;
	ChessType = ECardType::Ghost; // Default type
	RaceType = ERaceType::General; //default
	bIsPlayerControlled = true;
	// Create a root component
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	// Create the Chess Mesh and attach it to the root
	ChessMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SMMesh"));
	ChessMesh->SetupAttachment(RootComponent); 

}

void AChess::BeginPlay()
{
	Super::BeginPlay();
	
}

void AChess::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AChess::Attack(AChess *TargetChess)
{
	
	TargetChess->Health -= AttackDamage;
	UE_LOG(LogTemp, Log, TEXT("Chess piece of type %d attacked!"), static_cast<int32>(ChessType));
	
	TargetChess->OnStatsUpdated.Broadcast(TargetChess->Health, TargetChess->AttackDamage);
	
}


void AChess::PlayMoveAnimation()
{
	if (ChessMesh && MoveAnimation)
	{
		ChessMesh->PlayAnimation(MoveAnimation, false);
	}
}

void AChess::PlayRitualToolAnimation()
{
	if (ChessMesh && MoveAnimation)
	{
		ChessMesh->PlayAnimation(RitualToolAnimation, false);
	}
	
}




void AChess::ApplyHealing(int32 HealAmount)
{
	Health += HealAmount;
	OnStatsUpdated.Broadcast(Health, AttackDamage);
	// 输出日志信息
	UE_LOG(LogTemp, Log, TEXT("%s was healed by %d. New Health: %d"), *GetName(), HealAmount, Health);
}



UGridNode* AChess::GetCurrentNode() const
{
	return CurrentNode;
}

void AChess::SetCurrentNode(UGridNode* NewNode)
{
	CurrentNode = NewNode;
}

void AChess::Die()
{
	// clean the pointer when die
	if (CurrentNode)
	{
		CurrentNode->SetOccupyingChess(nullptr);
		if (CurrentNode->GetOccupant() == EOccupantType::Ot_Occupied)
		{
			CurrentNode->SetOccupant(EOccupantType::Ot_Empty);
		}
		
		CurrentNode = nullptr;
		//还没处理清除棋盘中的summon map
	}
	

	
	UE_LOG(LogTemp, Log, TEXT("Chess piece of type %d died!"), static_cast<int32>(ChessType));
	
	Destroy();
}

ECardType AChess::GetChessType() const
{
	return ChessType;
}


void AChess::InitializeChess(bool bPlayerControlled)
{
	bIsPlayerControlled = bPlayerControlled;
}


AChess* AChess::SpawnChess(UWorld* World, TSubclassOf<AChess> ChessClass, FVector Location, FRotator Rotation, bool bPlayerControlled)
{
	if (!World || !*ChessClass) return nullptr;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    
	// Spawn the blueprint subclass instance
	AChess* SpawnedChess = World->SpawnActor<AChess>(ChessClass, Location, Rotation, SpawnParams);
    
	if (SpawnedChess)
	{
		// Initialize with the correct player control state
		SpawnedChess->InitializeChess(bPlayerControlled);
	}

	return SpawnedChess;
}
