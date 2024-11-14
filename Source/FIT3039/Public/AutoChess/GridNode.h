// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "UObject/NoExportTypes.h"

#include "GridNode.generated.h"

class AChess;
/**
 * 
 */
class AChessboard;
class AActor;

/*棋格坐标*/
USTRUCT(BlueprintType)
struct  FGridVector
{
public:
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int X = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Y = 0;

	FGridVector() {};
	FGridVector(int InX, int InY)
	{
		this->X = InX;
		this->Y = InY;
	}

	//重载运算符==，满足TMap中Key的规范
	FORCEINLINE friend bool operator==(const FGridVector& Lhs, const FGridVector& Rhs)
	{
		return (Lhs.X == Rhs.X) && (Lhs.Y == Rhs.Y);
	}

	//重载运算符+，满足基本加法运算
	FGridVector operator+ (const FGridVector& F)
	{
		return FGridVector(this->X + F.X, this->Y + F.Y);
	}

};

//GetTypeHash，满足TMap中Key的规范
FORCEINLINE uint32 GetTypeHash(const FGridVector& Key)
{
	return HashCombine(GetTypeHash(Key.X), GetTypeHash(Key.Y));
}



UENUM(BlueprintType)
enum class EOccupantType : uint8
{
	Ot_Enemy, //Enemy part
	Ot_Empty, // No occupant
	Ot_Occupied, // Occupied by chessa
};

UCLASS()
class FIT3039_API UGridNode : public UObject
{
	GENERATED_BODY()

public:
	//基础-初始化函数
	UFUNCTION(BlueprintCallable)
	virtual void InitNode(AChessboard* InChessboard, FVector InLocation, FGridVector InCoordinate, float InSize, EOccupantType InOccupantType);
	
	bool IsOccupied() const;
	
	void SetOccupant(EOccupantType NewOccupant);
	
	EOccupantType GetOccupant() const;
	
	//基础-棋格大小
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Size;

	//基础-世界空间坐标
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Location;

	//基础-棋格形状
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FGridVector Coordinate;
	
	//基础-所属棋盘
	UPROPERTY(BlueprintReadWrite)
	AChessboard* Chessboard;

	//进入棋格
	UFUNCTION(BlueprintCallable)
	virtual void EnterNode(AActor* InActor);

	//离开棋格
	UFUNCTION(BlueprintCallable)
	virtual void LeaveNode(AActor* InActor);


	//setter getter for crop
	
	AChess* GetOccupyingChess() const;
	
	void SetOccupyingChess(class AChess* InChess);

	
	//模型-棋格模型索引
	UPROPERTY()
	int MeshIndex;
	
	
	//模型-绘制模型
	UFUNCTION()
	virtual void DrawNode(
		TArray<FVector>& InVertices,
		TArray<int32>& InIndecies,
		TArray<FVector>& InNormals,
		TArray<FVector2D>& InUV,
		TArray<FColor>& InVertexColors,
		TArray<FVector>& InTangents,
		FVector InOffset
		);


	
private:
	EOccupantType Occupant;

	class AChess* OccupyChess;
	
};
