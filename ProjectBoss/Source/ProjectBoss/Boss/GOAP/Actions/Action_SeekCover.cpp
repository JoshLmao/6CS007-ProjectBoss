// Fill out your copyright notice in the Description page of Project Settings.


#include "Action_SeekCover.h"
#include "NavigationSystem.h"
#include "NavMesh/RecastNavMesh.h"
#include "DrawDebugHelpers.h"
#include "../../BossCharacter.h"

UAction_SeekCover::UAction_SeekCover()
{
	name = "seek cover";
	cost = 0.0f;

	// Preconditions
	preconditions.Add(CreateAtom("in-cover", false));

	// Effects
	effects.Add(CreateAtom("in-cover", true));

    rndLoc = FVector::ZeroVector;
}

bool UAction_SeekCover::checkProceduralPrecondition(APawn* pawn)
{
	Super::checkProceduralPrecondition(pawn);

	return true;
}

bool UAction_SeekCover::doAction(APawn* pawn)
{
	Super::doAction(pawn);

    if (rndLoc == FVector::ZeroVector)
    {
        rndLoc = UNavigationSystemV1::GetRandomReachablePointInRadius(pawn->GetWorld(), pawn->GetActorLocation(), 3000.0f);
    }

    ABossCharacter* boss = Cast<ABossCharacter>(pawn);
    bool reached = boss->MoveToLocation(rndLoc);
    if (reached)
    {
        rndLoc = FVector::ZeroVector;
        return true;
    }

   /* 
   UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

    AActor* owner = pawn->GetOwner();
    INavigationDataInterface* const NavData = NavSys->GetNavDataForActor(*owner);

    const ARecastNavMesh* const RecastNavMesh = Cast<const ARecastNavMesh>(NavData);

    FRecastDebugGeometry NavMeshTileGeo;
    NavMeshTileGeo.bGatherNavMeshEdges = true;

    TArray<FVector> PointsToGatherTiles = { pawn->GetActorLocation() };
    TSet<int32> AddedTileIndices;

    for (const FVector& GatherPoint : PointsToGatherTiles)
    {
        int32 TileX, TileY;
        RecastNavMesh->GetNavMeshTileXY(GatherPoint, TileX, TileY);

        TArray<int32> TileIndices;
        RecastNavMesh->GetNavMeshTilesAt(TileX, TileY, TileIndices);

        for (int32 i = 0; i < TileIndices.Num(); ++i)
        {
            if (!AddedTileIndices.Contains(TileIndices[i]))
            {
                RecastNavMesh->GetDebugGeometry(NavMeshTileGeo, TileIndices[i]);
                AddedTileIndices.Add(TileIndices[i]);
            }
        }
    }
    
    for (int32 i = 0; i < NavMeshTileGeo.NavMeshEdges.Num(); i += 2)
    {
        DrawDebugCapsule(GetWorld(), NavMeshTileGeo.NavMeshEdges[i], 200.0f, 50.0f, FQuat::Identity, FColor::Green, false, 1.0f, 0, 1.0f);
        DrawDebugCapsule(GetWorld(), NavMeshTileGeo.NavMeshEdges[i + 1], 200.0f, 50.0f, FQuat::Identity, FColor::Red, false, 1.0f, 0, 1.0f);
    }
    */

	return false;
}
