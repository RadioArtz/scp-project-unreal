

#include "LayoutGeneratorCell.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Kismet/KismetMathLibrary.h" 
#include "DrawDebugHelpers.h"
#include "LayoutGeneratorSpawnValidator.h"
#include "LayoutGeneratorMain.h"

FVector ULayoutGeneratorCell::GetWorldLocation()
{
	return FVector(Location.X * LayoutGenerator->CellSize, Location.Y * LayoutGenerator->CellSize, 0) + LayoutGenerator->GetActorLocation();
}

FRotator ULayoutGeneratorCell::GetWorldRotation()
{
	return FRotator(0, Rotation * 90, 0);
}

FString ULayoutGeneratorCell::GetUniqueName()
{
	return FString::Printf(TEXT("%s__X%i_Y%i"), *LayoutGenerator->GetName(), Location.X, Location.Y);
}

void ULayoutGeneratorCell::GetNeighbouringCells(bool bOnlyReturnConnectedCells, ULayoutGeneratorCell*& OutCellPositiveX, ULayoutGeneratorCell*& OutCellPositiveY, ULayoutGeneratorCell*& OutCellNegativeX, ULayoutGeneratorCell*& OutCellNegativeY)
{
	OutCellPositiveX = LayoutGenerator->GetCell(FIntVector2D(Location.X + 1, Location.Y));
	OutCellPositiveY = LayoutGenerator->GetCell(FIntVector2D(Location.X, Location.Y + 1));
	OutCellNegativeX = LayoutGenerator->GetCell(FIntVector2D(Location.X - 1, Location.Y));
	OutCellNegativeY = LayoutGenerator->GetCell(FIntVector2D(Location.X, Location.Y - 1));

	if (bOnlyReturnConnectedCells)
	{
		if (!HasDoor.bPositiveX)
		{
			OutCellPositiveX = nullptr;
		}

		if (!HasDoor.bPositiveY)
		{
			OutCellPositiveY = nullptr;
		}

		if (!HasDoor.bNegativeX)
		{
			OutCellNegativeX = nullptr;
		}

		if (!HasDoor.bNegativeY)
		{
			OutCellNegativeY = nullptr;
		}
	}
}

bool ULayoutGeneratorCell::LoadLevel()
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
			LevelStreamingDynamic->bDisableDistanceStreaming = true;
			LevelStreamingDynamic->LevelColor = FLinearColor::MakeRandomColor();
			LevelStreamingDynamic->OnLevelLoaded.AddDynamic(this, &ULayoutGeneratorCell::OnLevelLoaded);
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

bool ULayoutGeneratorCell::UnloadLevel()
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

void ULayoutGeneratorCell::GetAllActorsInLevel(TArray<AActor*>& OutActors)
{
	OutActors.Empty();

	if (LevelStreamingDynamic != nullptr && LevelStreamingDynamic->HasLoadedLevel())
	{
		for (int i = 0; i < LevelStreamingDynamic->GetLoadedLevel()->Actors.Num(); i++)
		{
			if (IsValid(LevelStreamingDynamic->GetLoadedLevel()->Actors[i]))
			{
				OutActors.Add(LevelStreamingDynamic->GetLoadedLevel()->Actors[i]);
			}
		}
	}
}

bool ULayoutGeneratorCell::IsPointInLevelBounds(FVector Point)
{
	if (LevelStreamingDynamic != nullptr && LevelStreamingDynamic->HasLoadedLevel())
	{
		bool bInBounds;
		TArray<AActor*> LevelActors;

		bInBounds = false;
		GetAllActorsInLevel(LevelActors);

		for (int i = 0; i < LevelActors.Num(); i++)
		{
			FVector ActorOrigin;
			FVector ActorExtent;

			LevelActors[i]->GetActorBounds(false, ActorOrigin, ActorExtent, false);
			bInBounds = bInBounds || UKismetMathLibrary::IsPointInBox(Point, ActorOrigin, ActorExtent);
		}

		return bInBounds;
	}
	else
	{
		return false;
	}
}

bool ULayoutGeneratorCell::DoesPathExist(ULayoutGeneratorCell* Goal)
{
	// Check if it even makes sense to look for a path
	if (Goal == nullptr || Goal->HasDoor == FLayoutGeneratorCellSides(false, false, false, false))
	{
		return false;
	}

	TArray<ULayoutGeneratorCell*> CellQueue;
	TArray<ULayoutGeneratorCell*> CellsChecked;
	int CurrentIteration;
	int MaxIteration;

	CellQueue.Add(this);
	CurrentIteration = 0;
	MaxIteration = LayoutGenerator->Grid.Num();

	while (CellQueue.Num() > 0)
	{
		// We've somehow hit the max iteration limit
		if (CurrentIteration > MaxIteration)
		{
			return false;
		}

		ULayoutGeneratorCell* ThisCell;
		ULayoutGeneratorCell* PosXCell;
		ULayoutGeneratorCell* PosYCell;
		ULayoutGeneratorCell* NegXCell;
		ULayoutGeneratorCell* NegYCell;

		ThisCell = CellQueue[0];
		CellQueue.Remove(ThisCell);
		CellsChecked.Add(ThisCell);
		CurrentIteration++;

		// We found a valid path for our goal
		if (ThisCell == Goal)
		{
			return true;
		}

		PosXCell = LayoutGenerator->GetCell(FIntVector2D(ThisCell->Location.X + 1, ThisCell->Location.Y));
		PosYCell = LayoutGenerator->GetCell(FIntVector2D(ThisCell->Location.X, ThisCell->Location.Y + 1));
		NegXCell = LayoutGenerator->GetCell(FIntVector2D(ThisCell->Location.X - 1, ThisCell->Location.Y));
		NegYCell = LayoutGenerator->GetCell(FIntVector2D(ThisCell->Location.X, ThisCell->Location.Y - 1));

		// Add connected neighbours which were not already added prior to the queue //
		if (PosXCell != nullptr && !CellsChecked.Contains(PosXCell) && ThisCell->HasDoor.bPositiveX)
		{
			CellQueue.Add(PosXCell);
		}

		if (PosYCell != nullptr && !CellsChecked.Contains(PosYCell) && ThisCell->HasDoor.bPositiveY)
		{
			CellQueue.Add(PosYCell);
		}

		if (NegXCell != nullptr && !CellsChecked.Contains(NegXCell) && ThisCell->HasDoor.bNegativeX)
		{
			CellQueue.Add(NegXCell);
		}

		if (NegYCell != nullptr && !CellsChecked.Contains(NegYCell) && ThisCell->HasDoor.bNegativeY)
		{
			CellQueue.Add(NegYCell);
		}
		////
	}

	// Our search found nothing, so no path found :(
	return false;
}

void ULayoutGeneratorCell::DrawDebug(float Duration)
{
	FVector StartLocation;
	FVector EndLocation;
	FString TextToDraw;

	// Draw door paths //
	if (HasDoor.bPositiveX)
	{
		StartLocation = GetWorldLocation();
		EndLocation = GetWorldLocation() + FVector(LayoutGenerator->CellSize * .4f, 0, 0);
		DrawDebugLine(LayoutGenerator->GetWorld(), StartLocation, EndLocation, FColor::Green, false, Duration, 0, 20.f);
	}

	if (HasDoor.bPositiveY)
	{
		StartLocation = GetWorldLocation();
		EndLocation = GetWorldLocation() + FVector(0, LayoutGenerator->CellSize * .4f, 0);
		DrawDebugLine(LayoutGenerator->GetWorld(), StartLocation, EndLocation, FColor::Green, false, Duration, 0, 20.f);
	}

	if (HasDoor.bNegativeX)
	{
		StartLocation = GetWorldLocation();
		EndLocation = GetWorldLocation() - FVector(LayoutGenerator->CellSize * .4f, 0, 0);
		DrawDebugLine(LayoutGenerator->GetWorld(), StartLocation, EndLocation, FColor::Green, false, Duration, 0, 20.f);
	}

	if (HasDoor.bNegativeY)
	{
		StartLocation = GetWorldLocation();
		EndLocation = GetWorldLocation() - FVector(0, LayoutGenerator->CellSize * .4f, 0);
		DrawDebugLine(LayoutGenerator->GetWorld(), StartLocation, EndLocation, FColor::Green, false, Duration, 0, 20.f);
	}
	////

	// Draw disabled neighbours //
	if (ShouldDisableNeighbour.bPositiveX)
	{
		StartLocation = GetWorldLocation();
		EndLocation = GetWorldLocation() + FVector(LayoutGenerator->CellSize * .2f, 0, 0);
		DrawDebugLine(LayoutGenerator->GetWorld(), StartLocation, EndLocation, FColor::Red, false, Duration, 0, 20.f);
	}

	if (ShouldDisableNeighbour.bPositiveY)
	{
		StartLocation = GetWorldLocation();
		EndLocation = GetWorldLocation() + FVector(0, LayoutGenerator->CellSize * .2f, 0);
		DrawDebugLine(LayoutGenerator->GetWorld(), StartLocation, EndLocation, FColor::Red, false, Duration, 0, 20.f);
	}

	if (ShouldDisableNeighbour.bNegativeX)
	{
		StartLocation = GetWorldLocation();
		EndLocation = GetWorldLocation() - FVector(LayoutGenerator->CellSize * .2f, 0, 0);
		DrawDebugLine(LayoutGenerator->GetWorld(), StartLocation, EndLocation, FColor::Red, false, Duration, 0, 20.f);
	}

	if (ShouldDisableNeighbour.bNegativeY)
	{
		StartLocation = GetWorldLocation();
		EndLocation = GetWorldLocation() - FVector(0, LayoutGenerator->CellSize * .2f, 0);
		DrawDebugLine(LayoutGenerator->GetWorld(), StartLocation, EndLocation, FColor::Red, false, Duration, 0, 20.f);
	}
	////

	// Draw properties as text //
	TextToDraw.Appendf(TEXT("Grid Location: X=%i Y=%i (Elem=%i)\n"), Location.X, Location.Y, Location.X * LayoutGenerator->GridSize.X + Location.Y);
	TextToDraw.Appendf(TEXT("Grid Rotation: %i\n"), Rotation);
	TextToDraw.Appendf(TEXT("World Location: %s\n"), *GetWorldLocation().ToString());
	TextToDraw.Appendf(TEXT("World Rotation: %s\n"), *GetWorldRotation().ToString());
	TextToDraw.Appendf(TEXT("Room Row Name: %s\n"), *RoomRowName.ToString());
	TextToDraw.Appendf(TEXT("Unique Seed: %i\n"), UniqueSeed);
	TextToDraw.Appendf(TEXT("Loaded Level: %s\n"), *LevelAsset.ToString());

	DrawDebugString(LayoutGenerator->GetWorld(), GetWorldLocation() + FVector(0.f, 0.f, 200.f), TextToDraw, nullptr, FColor::White, Duration, true, .85f);
	////
}

void ULayoutGeneratorCell::BeginDestroy()
{
	Super::BeginDestroy();

	UnloadLevel();
}

bool ULayoutGeneratorCell::SetRoom(const FName NewRoomRowName, const bool bForce)
{
	// Abort cell generation if (cell is disabled OR already generated) AND we don't force it
	if ((!bIsEnabled || bIsGenerated) && !bForce)
	{
		return false;
	}

	bool bSuccess;
	FLayoutGeneratorRoomGenerationSettings SourceSettings;
	ULayoutGeneratorCell* PosXCell;
	ULayoutGeneratorCell* PosYCell;
	ULayoutGeneratorCell* NegXCell;
	ULayoutGeneratorCell* NegYCell;
	FLayoutGeneratorCellSides IsDoorRequired;
	FLayoutGeneratorCellSides IsDoorBlocked;

	RoomRowName = NewRoomRowName;
	SourceSettings = LayoutGenerator->RoomGenerationData[RoomRowName];
	HasDoor = SourceSettings.HasDoor;
	ShouldDisableNeighbour = SourceSettings.ShouldDisableNeighbour;

	// Fetch info from neighbouring cells //
	GetNeighbouringCells(false , PosXCell, PosYCell, NegXCell, NegYCell);

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

	// Check if the room fits and if not, rotate it
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
			// Room fits, we don't need to rotate it
			break;
		}
		else
		{
			// Rotate the room to the right
				HasDoor.RotateRight();
				ShouldDisableNeighbour.RotateRight();

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
			LevelAsset = SourceSettings.Levels[FRandomStream(UniqueSeed).RandHelper(SourceSettings.Levels.Num() - 1)];
		}

		return true;
	}
	else
	{
		RoomRowName = "None";
		HasDoor = FLayoutGeneratorCellSides();
		ShouldDisableNeighbour = FLayoutGeneratorCellSides();
		Rotation = 0;
		LevelAsset = nullptr;
		return false;
	}
}

bool ULayoutGeneratorCell::IsRoomValid()
{
	return false;
}

void ULayoutGeneratorCell::OnLevelLoaded()
{
	// Send cell via interface to every actor of this level
	TArray<AActor*> LevelActors;
	FRandomStream TempRStream;

	GetAllActorsInLevel(LevelActors);
	TempRStream = FRandomStream(UniqueSeed);

	for (int i = 0; i < LevelActors.Num(); i++)
	{
		if (LevelActors[i]->Implements<ULayoutGeneratorReceiveCellInterface>())
		{
			ILayoutGeneratorReceiveCellInterface::Execute_OnCellReceived(LevelActors[i], this, FRandomStream(TempRStream.RandHelper(100000000)));
		}
	}

	UE_LOG(LogLayoutGenerator, Display, TEXT("%s: Level %s (%s) loaded."), *LayoutGenerator->GetName(), *GetUniqueName(), *LevelAsset.GetUniqueID().ToString());
}
