// Fill out your copyright notice in the Description page of Project Settings.


#include "AutoChess//GridNode.h"
void UGridNode::InitNode(AChessboard* InChessboard, FVector InLocation, FGridVector InCoordinate, float InSize, EOccupantType InOccupantType)
{
	Chessboard = InChessboard;
	Location = InLocation;
	Coordinate = InCoordinate;
	Size = InSize;
	Occupant = InOccupantType;
}

bool UGridNode::IsOccupied() const
{
	return Occupant != EOccupantType::Ot_Empty;
}

void UGridNode::SetOccupant(EOccupantType NewOccupant)
{
	Occupant = NewOccupant;
}

EOccupantType UGridNode::GetOccupant() const
{
	return Occupant;
}


void UGridNode::EnterNode(AActor* InActor)
{
	
}


void UGridNode::LeaveNode(AActor* InActor)
{
}

void UGridNode::SetOccupyingChess(AChess* InChess)
{
	OccupyChess = InChess;
}

// UGridNode.cpp

AChess* UGridNode::GetOccupyingChess() const
{
	return OccupyChess;
}


void UGridNode::DrawNode(TArray<FVector>& InVertices,
                         TArray<int32>& InIndecies,
                         TArray<FVector>& InNormals,
                         TArray<FVector2D>& InUV,
                         TArray<FColor>& InVertexColors,
                         TArray<FVector>& InTangents,
                         FVector InOffset)
{
	// vertex
	TArray<FVector> QuadVerts;
	QuadVerts.AddUninitialized(4);
	QuadVerts[0] = FVector(Size / 2, Size / 2, 0) + Location - InOffset;
	QuadVerts[1] = FVector(-Size / 2, Size / 2, 0) + Location - InOffset;
	QuadVerts[2] = FVector(-Size / 2, -Size / 2, 0) + Location - InOffset;
	QuadVerts[3] = FVector(Size / 2, -Size / 2, 0) + Location - InOffset;

	// vertex index
	TArray<int32> Indices;
	Indices.AddUninitialized(6); 
	Indices[0] = 0; Indices[1] = 2; Indices[2] = 1;
	Indices[3] = 0; Indices[4] = 3; Indices[5] = 2;

	// normal
	TArray<FVector> Normals;
	Normals.Init(FVector(0, 0, 1), 4);

	// UV
	TArray<FVector2D> UV;
	UV.AddUninitialized(4);
	UV[0] = FVector2D(1, 0);
	UV[1] = FVector2D(0, 0);
	UV[2] = FVector2D(0, 1);
	UV[3] = FVector2D(1, 1);

	// 顶点颜色
	TArray<FColor> VertexColors;
	VertexColors.Init(FColor::White, 4);

	// 切线
	TArray<FVector> Tangents;
	Tangents.Init(FVector(1, 0, 0), 4); 

	// 回传
	InVertices = QuadVerts;
	InIndecies = Indices;
	InNormals = Normals;
	InUV = UV;
	InVertexColors = VertexColors;
	InTangents = Tangents;
}



