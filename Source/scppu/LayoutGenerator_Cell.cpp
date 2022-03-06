

#include "LayoutGenerator_Cell.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Kismet/KismetMathLibrary.h" 
#include "DrawDebugHelpers.h"
#include "LayoutGenerator_SpawnValidator.h"
#include "LayoutGenerator_Main.h"

FVector ULayoutGenerator_Cell::GetWorldLocation()
{
	return FVector(Location.X * LayoutGenerator->CellSize, Location.Y * LayoutGenerator->CellSize, 0) + LayoutGenerator->GetActorLocation();
}

FRotator ULayoutGenerator_Cell::GetWorldRotation()
{
	return FRotator(0, Rotation * 90, 0);
}

FString ULayoutGenerator_Cell::GetUniqueName()
{
	FString UniqueName;

	UniqueName = LayoutGenerator->GetName();
	UniqueName.Append("__X");
	UniqueName.AppendInt(Location.X);
	UniqueName.Append("_Y");
	UniqueName.AppendInt(Location.Y);

	return UniqueName;
}

bool ULayoutGenerator_Cell::LoadLevel()
{
	if (bIsEnabled && bIsGenerated && !LevelAsset.IsNull() && LevelStreamingDynamic == nullptr)
	{
		bool bSuccess;

		LevelStreamingDynamic = ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(LayoutGenerator->GetWorld(), LevelAsset, GetWorldLocation(), GetWorldRotation(), bSuccess, GetUniqueName());

		if (bSuccess)
		{
			LevelStreamingDynamic->bShouldBlockOnLoad = false;
			LevelStreamingDynamic->bShouldBlockOnUnload = false;
			LevelStreamingDynamic->SetShouldBeVisible(false);
			LevelStreamingDynamic->bDisableDistanceStreaming = false;
			LevelStreamingDynamic->LevelColor = FLinearColor::MakeRandomColor();
			LevelStreamingDynamic->OnLevelLoaded.AddDynamic(this, &ULayoutGenerator_Cell::OnLevelLoaded);

			DrawCellDebug();
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool ULayoutGenerator_Cell::UnloadLevel()
{
	if (LevelStreamingDynamic != nullptr)
	{
		LevelStreamingDynamic->SetIsRequestingUnloadAndRemoval(true);
		LevelStreamingDynamic = nullptr;
		return true;

	} 
	else
	{
		return false;
	}
}

void ULayoutGenerator_Cell::GetAllActorsInLevel(TArray<AActor*>& OutActors)
{
	if (LevelStreamingDynamic != nullptr && LevelStreamingDynamic->HasLoadedLevel())
	{
		OutActors = LevelStreamingDynamic->GetLoadedLevel()->Actors;	
	}
}

bool ULayoutGenerator_Cell::IsPointInLevelBounds(FVector Point)
{
	if (LevelStreamingDynamic != nullptr && LevelStreamingDynamic->HasLoadedLevel())
	{
		bool bInBounds;
		TArray<AActor*> LevelActors;

		bInBounds = false;
		LevelActors = LevelStreamingDynamic->GetLoadedLevel()->Actors;

		for (int i = 0; i < LevelActors.Num(); i++)
		{
			FVector ActorOrigin;
			FVector ActorExtent;

			LevelActors[i]->GetActorBounds(false, ActorOrigin, ActorExtent, false);
			bInBounds = bInBounds && UKismetMathLibrary::IsPointInBox(Point, ActorOrigin, ActorExtent);
		}

		return bInBounds;
	}
	else
	{
		return false;
	}
}

void ULayoutGenerator_Cell::BeginDestroy()
{
	Super::BeginDestroy();

	UnloadLevel();
}

void ULayoutGenerator_Cell::DrawCellDebug()
{
	FVector StartLocation;
	FVector EndLocation;

	// Draw door paths //
	if (HasDoor.bPositiveX)
	{
		StartLocation = GetWorldLocation();
		EndLocation = GetWorldLocation() + FVector(LayoutGenerator->CellSize * .4f, 0, 0);
		DrawDebugLine(LayoutGenerator->GetWorld(), StartLocation, EndLocation, FColor::Green, true, 1.f, 0, 10.f);
	}

	if (HasDoor.bPositiveY)
	{
		StartLocation = GetWorldLocation();
		EndLocation = GetWorldLocation() + FVector(0, LayoutGenerator->CellSize * .4f, 0);
		DrawDebugLine(LayoutGenerator->GetWorld(), GetWorldLocation(), EndLocation, FColor::Green, true, 1.f, 0, 10.f);
	}

	if (HasDoor.bNegativeX)
	{
		StartLocation = GetWorldLocation();
		EndLocation = GetWorldLocation() - FVector(LayoutGenerator->CellSize * .4f, 0, 0);
		DrawDebugLine(LayoutGenerator->GetWorld(), GetWorldLocation(), EndLocation, FColor::Green, true, 1.f, 0, 10.f);
	}

	if (HasDoor.bNegativeY)
	{
		StartLocation = GetWorldLocation();
		EndLocation = GetWorldLocation() - FVector(0, LayoutGenerator->CellSize * .4f, 0);
		DrawDebugLine(LayoutGenerator->GetWorld(), GetWorldLocation(), EndLocation, FColor::Green, true, 1.f, 0, 10.f);
	}
	////

	// Draw disabled neighbours //
	if (ShouldDisableNeighbour.bPositiveX)
	{
		StartLocation = GetWorldLocation();
		EndLocation = GetWorldLocation() + FVector(LayoutGenerator->CellSize * .2f, 0, 0);
		DrawDebugLine(LayoutGenerator->GetWorld(), StartLocation, EndLocation, FColor::Red, true, 1.f, 0, 10.f);
	}

	if (ShouldDisableNeighbour.bPositiveY)
	{
		StartLocation = GetWorldLocation();
		EndLocation = GetWorldLocation() + FVector(0, LayoutGenerator->CellSize * .2f, 0);
		DrawDebugLine(LayoutGenerator->GetWorld(), GetWorldLocation(), EndLocation, FColor::Red, true, 1.f, 0, 10.f);
	}

	if (ShouldDisableNeighbour.bNegativeX)
	{
		StartLocation = GetWorldLocation();
		EndLocation = GetWorldLocation() - FVector(LayoutGenerator->CellSize * .2f, 0, 0);
		DrawDebugLine(LayoutGenerator->GetWorld(), GetWorldLocation(), EndLocation, FColor::Red, true, 1.f, 0, 10.f);
	}

	if (ShouldDisableNeighbour.bNegativeY)
	{
		StartLocation = GetWorldLocation();
		EndLocation = GetWorldLocation() - FVector(0, LayoutGenerator->CellSize * .2f, 0);
		DrawDebugLine(LayoutGenerator->GetWorld(), GetWorldLocation(), EndLocation, FColor::Red, true, 1.f, 0, 10.f);
	}
	////

}

bool ULayoutGenerator_Cell::SetRoom(const FName NewRoomRowName, const bool bForce)
{
	// Abort cell generation if (cell is disabled OR already generated) AND we don't force it
	if ((!bIsEnabled || bIsGenerated) && !bForce)
	{
		return false;
	}

	bool bSuccess;
	FRoomGenerationSettings SourceSettings;
	ULayoutGenerator_Cell* PosXCell;
	ULayoutGenerator_Cell* PosYCell;
	ULayoutGenerator_Cell* NegXCell;
	ULayoutGenerator_Cell* NegYCell;
	FCellSides IsDoorRequired;
	FCellSides IsDoorBlocked;

	RoomRowName = NewRoomRowName;
	SourceSettings = LayoutGenerator->RoomGenerationData[RoomRowName];
	HasDoor = SourceSettings.HasDoor;
	ShouldDisableNeighbour = SourceSettings.ShouldDisableNeighbour;

	// Fetch info from neighbouring cells //
	PosXCell = LayoutGenerator->GetCell(FIntVector2D(Location.X + 1, Location.Y));
	PosYCell = LayoutGenerator->GetCell(FIntVector2D(Location.X, Location.Y + 1));
	NegXCell = LayoutGenerator->GetCell(FIntVector2D(Location.X - 1, Location.Y));
	NegYCell = LayoutGenerator->GetCell(FIntVector2D(Location.X, Location.Y - 1));

	// Positive X
	IsDoorRequired.bPositiveX = (PosXCell != nullptr && PosXCell->HasDoor.bNegativeX);
	IsDoorBlocked.bPositiveX = (PosXCell == nullptr || !PosXCell->bIsEnabled) || (PosXCell->bIsGenerated && !PosXCell->HasDoor.bNegativeX);

	// Positive Y
	IsDoorRequired.bPositiveY = (PosYCell != nullptr && PosYCell->HasDoor.bNegativeY);
	IsDoorBlocked.bPositiveY = (PosYCell == nullptr || !PosYCell->bIsEnabled) || (PosYCell->bIsGenerated && !PosYCell->HasDoor.bNegativeY);

	// Negative X
	IsDoorRequired.bNegativeX = (NegXCell != nullptr && NegXCell->HasDoor.bPositiveX);
	IsDoorBlocked.bNegativeX = (NegXCell == nullptr || !NegXCell->bIsEnabled) || (NegXCell->bIsGenerated && !NegXCell->HasDoor.bPositiveX);

	// Negative Y
	IsDoorRequired.bNegativeY = (NegYCell != nullptr && NegYCell->HasDoor.bPositiveY);
	IsDoorBlocked.bNegativeY = (NegYCell == nullptr || !NegYCell->bIsEnabled) || (NegYCell->bIsGenerated && !NegYCell->HasDoor.bPositiveY);

	// Check if the room fits and when not rotate it
	for (int i = 0; i < 4; i++)
	{
		bSuccess = true;

		// Positive X
		bSuccess = bSuccess && ((IsDoorRequired.bPositiveX && HasDoor.bPositiveX) || !IsDoorRequired.bPositiveX);
		bSuccess = bSuccess && ((IsDoorBlocked.bPositiveX && !HasDoor.bPositiveX) || !IsDoorBlocked.bPositiveX);

		// Positive Y
		bSuccess = bSuccess && ((IsDoorRequired.bPositiveY && HasDoor.bPositiveY) || !IsDoorRequired.bPositiveY);
		bSuccess = bSuccess && ((IsDoorBlocked.bPositiveY && !HasDoor.bPositiveY) || !IsDoorBlocked.bPositiveY);

		// Negative X
		bSuccess = bSuccess && ((IsDoorRequired.bNegativeX && HasDoor.bNegativeX) || !IsDoorRequired.bNegativeX);
		bSuccess = bSuccess && ((IsDoorBlocked.bNegativeX && !HasDoor.bNegativeX) || !IsDoorBlocked.bNegativeX);

		// Negative Y
		bSuccess = bSuccess && ((IsDoorRequired.bNegativeY && HasDoor.bNegativeY) || !IsDoorRequired.bNegativeY);
		bSuccess = bSuccess && ((IsDoorBlocked.bNegativeY && !HasDoor.bNegativeY) || !IsDoorBlocked.bNegativeY);

		// Check pre spawn validators
		for (auto Elem : SourceSettings.PreSpawnValidator)
		{
			bSuccess = bSuccess && Elem != nullptr && LayoutGenerator->SpawnValidators[Elem]->IsSpawnValid(LayoutGenerator, Location);
		}

		if (bSuccess)
		{
			break;
		}
		else
		{
			// Rotate the room right
			FCellSides PreviousDoorLocation = HasDoor;

			HasDoor.bPositiveX = PreviousDoorLocation.bNegativeY;
			HasDoor.bPositiveY = PreviousDoorLocation.bPositiveX;
			HasDoor.bNegativeX = PreviousDoorLocation.bPositiveY;
			HasDoor.bNegativeY = PreviousDoorLocation.bNegativeX;

			Rotation += 1;
		}
	}

	if (bSuccess || bForce)
	{
		bIsGenerated = true;

		// Disable neighbouring cells if we need to
		if (PosXCell != nullptr)
		{
			PosXCell->bIsEnabled = PosXCell->bIsEnabled && !ShouldDisableNeighbour.bPositiveX;
		}

		if (PosYCell != nullptr)
		{
			PosYCell->bIsEnabled = PosYCell->bIsEnabled && !ShouldDisableNeighbour.bPositiveY;
		}

		if (NegXCell != nullptr)
		{
			NegXCell->bIsEnabled = NegXCell->bIsEnabled && !ShouldDisableNeighbour.bNegativeX;
		}

		if (NegYCell != nullptr)
		{
			NegYCell->bIsEnabled = NegYCell->bIsEnabled && !ShouldDisableNeighbour.bNegativeY;
		}

		// Assing random level asset to use
		if (SourceSettings.Levels.Num() > 0)
		{
			LevelAsset = SourceSettings.Levels[FRandomStream(CellSeed).RandHelper(SourceSettings.Levels.Num() - 1)];
		}

		return true;
	}
	else
	{
		RoomRowName = "None";
		HasDoor = FCellSides();
		ShouldDisableNeighbour = FCellSides();
		Rotation = 0;
		LevelAsset = nullptr;
		return false;
	}
}

void ULayoutGenerator_Cell::OnLevelLoaded()
{
	// Send cell via interface to every actor of this level
	TArray<AActor*> LevelActors;
	FRandomStream TempRStream;

	LevelActors = LevelStreamingDynamic->GetLoadedLevel()->Actors;
	TempRStream = FRandomStream(CellSeed);

	for (int i = 0; i < LevelActors.Num(); i++)
	{
		if (LevelActors[i]->Implements<ULayoutGenerator_ReceiveCellInterface>())
		{
			ILayoutGenerator_ReceiveCellInterface::Execute_OnCellReceived(LevelActors[i], this, FRandomStream(TempRStream.RandHelper(100000000)));
		}
	}
}
