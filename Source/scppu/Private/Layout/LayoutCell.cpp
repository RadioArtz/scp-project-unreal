


#include "Layout/LayoutCell.h"
#include "Layout/Layout.h"
#include "Layout/LayoutSpawnValidator.h"
#include "Layout/LayoutSublevelInterface.h"
#include "Engine/LevelStreamingDynamic.h"
#include "DrawDebugHelpers.h"

FVector ULayoutCell::GetWorldLocation()
{
	return (FVector(this->Location.X, this->Location.Y, 0) * this->Owner->CellSize) + this->Owner->GetActorLocation();
}

FRotator ULayoutCell::GetWorldRotation()
{
	return FRotator(0, this->Rotation * 90, 0);
}

bool ULayoutCell::IsRowNameValid(FName InRowName, int InRotation)
{
	if (this->Owner->DataTable->FindRow<FLayoutCellGenerationSettings>(InRowName, "") == nullptr)
	{
		UE_LOG(LogLayout, Warning, TEXT("%s: %s is not a valid entry inside %s"), *this->GetName(), *InRowName.ToString(), *this->Owner->DataTable->GetName());
		return false;
	}

	bool bIsValid = true;
	InRotation = InRotation % 4;

	// Save original properties so we can overwrite them temporarily for the pre spawn validators
	int PrevRotation = this->Rotation;
	FName PrevRowName = this->RowName;
	FLayoutCellSides PrevHasConnection = this->HasConnection;
	FLayoutCellSides PrevDisableNeighbouringCell = this->DisableNeighbouringCell;

	// Get the values from datatable row so the pre spawn validators can use them
	FLayoutCellGenerationSettings Row = *this->Owner->DataTable->FindRow<FLayoutCellGenerationSettings>(InRowName, "");
	this->RowName = InRowName;
	this->HasConnection = Row.HasConnection;
	this->DisableNeighbouringCell = Row.DisableNeighbouringCell;

	// Rotate the values from datatable row
	for (int i = 1; i < InRotation + 1; i++)
	{
		this->Rotation = i;
		this->HasConnection.RotateRight();
		this->DisableNeighbouringCell.RotateRight();
	}

	// Run pre spawn validation
	for (auto elem : Row.PreSpawnValidator)
	{
		ULayoutSpawnValidator* Validator = this->Owner->GetOrCreateSpawnValidator(elem);
		if (Validator == nullptr)
		{
			continue;
		}

		bIsValid = bIsValid && Validator->IsValidSpawn(this->Owner, this, FRandomStream(this->UniqueSeed));
	}

	// Get info from neighbouring cells (should this be its own function?)
	ULayoutCell* CellPX;
	ULayoutCell* CellPY;
	ULayoutCell* CellNX;
	ULayoutCell* CellNY;
	FLayoutCellSides ConnectionRequired;
	FLayoutCellSides ConnectionBlocked;
	this->Owner->GetNeighbouringCells(this, false, CellPX, CellPY, CellNX, CellNY);

	//PX
	ConnectionRequired.bPX = IsValid(CellPX) && CellPX->HasConnection.bNX;
	ConnectionBlocked.bPX = !IsValid(CellPX) || !CellPX->bIsEnabled || (CellPX->bIsGenerated && !CellPX->HasConnection.bNX);

	//PY
	ConnectionRequired.bPY = IsValid(CellPY) && CellPY->HasConnection.bNY;
	ConnectionBlocked.bPY = !IsValid(CellPY) || !CellPY->bIsEnabled || (CellPY->bIsGenerated && !CellPY->HasConnection.bNY);

	//NX
	ConnectionRequired.bNX = IsValid(CellNX) && CellNX->HasConnection.bPX;
	ConnectionBlocked.bNX = !IsValid(CellNX) || !CellNX->bIsEnabled || (CellNX->bIsGenerated && !CellNX->HasConnection.bPX);

	//NY
	ConnectionRequired.bNY = IsValid(CellNY) && CellNY->HasConnection.bPY;
	ConnectionBlocked.bNY = !IsValid(CellNY) || !CellNY->bIsEnabled || (CellNY->bIsGenerated && !CellNY->HasConnection.bPY);

	// Check if all connections fit
	//PX
	bIsValid = bIsValid && ((ConnectionRequired.bPX && this->HasConnection.bPX) || !ConnectionRequired.bPX);
	bIsValid = bIsValid && ((ConnectionBlocked.bPX && !this->HasConnection.bPX) || !ConnectionBlocked.bPX);

	//PY
	bIsValid = bIsValid && ((ConnectionRequired.bPY && this->HasConnection.bPY) || !ConnectionRequired.bPY);
	bIsValid = bIsValid && ((ConnectionBlocked.bPY && !this->HasConnection.bPY) || !ConnectionBlocked.bPY);

	//NX
	bIsValid = bIsValid && ((ConnectionRequired.bNX && this->HasConnection.bNX) || !ConnectionRequired.bNX);
	bIsValid = bIsValid && ((ConnectionBlocked.bNX && !this->HasConnection.bNX) || !ConnectionBlocked.bNX);

	//NY
	bIsValid = bIsValid && ((ConnectionRequired.bNY && this->HasConnection.bNY) || !ConnectionRequired.bNY);
	bIsValid = bIsValid && ((ConnectionBlocked.bNY && !this->HasConnection.bNY) || !ConnectionBlocked.bNY);

	// Reset properties to their original state
	this->Rotation = PrevRotation;
	this->RowName = PrevRowName;
	this->HasConnection = PrevHasConnection;
	this->DisableNeighbouringCell = PrevDisableNeighbouringCell;

	return bIsValid;
}

void ULayoutCell::SetRowName(FName NewRowName, int NewRotation)
{
	if (NewRowName == "")
	{
		this->bIsGenerated = false;
		this->RowName = "";
		this->Rotation = 0;
		this->HasConnection = FLayoutCellSides();
		this->DisableNeighbouringCell = FLayoutCellSides();
		this->LevelAsset = nullptr;
		this->UnloadSublevel();
		return;
	}

	if (this->Owner->DataTable->FindRow<FLayoutCellGenerationSettings>(NewRowName, "") == nullptr)
	{
		UE_LOG(LogLayout, Warning, TEXT("%s: %s is not a valid row entry inside data table (%s)"), *this->GetName(), *NewRowName.ToString(), *this->Owner->DataTable->GetName());
		return;
	}

	FRandomStream RStream = FRandomStream(this->UniqueSeed);
	FLayoutCellGenerationSettings Row = *this->Owner->DataTable->FindRow<FLayoutCellGenerationSettings>(NewRowName, "");
	NewRotation = NewRotation % 4;
	this->RowName = NewRowName;
	this->HasConnection = Row.HasConnection;
	this->DisableNeighbouringCell = Row.DisableNeighbouringCell;

	// Rotate the values from row
	for (int i = 1; i < NewRotation + 1; i++)
	{
		this->Rotation = i;
		this->HasConnection.RotateRight();
		this->DisableNeighbouringCell.RotateRight();
	}

	// Set level asset
	if (Row.Levels.Num() > 0)
	{
		this->LevelAsset = Row.Levels[RStream.RandRange(0, Row.Levels.Num() - 1)];
	}

	bIsGenerated = true;
}

void ULayoutCell::LoadSublevel()
{
	if (IsValid(this->Sublevel))
	{
		return;
	}

	if (this->LevelAsset.IsNull())
	{
		return;
	}

	bool bLevelLoaded;
	this->Sublevel = ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(this->Owner->GetWorld(), this->LevelAsset, this->GetWorldLocation(), this->GetWorldRotation(), bLevelLoaded, this->UniqueSublevelName);
	if (bLevelLoaded)
	{
		this->Sublevel->bShouldBlockOnLoad = false;
		this->Sublevel->bShouldBlockOnUnload = false;
		this->Sublevel->bDisableDistanceStreaming = true;
		this->Sublevel->SetShouldBeVisible(false);
		this->Sublevel->OnLevelLoaded.AddDynamic(this, &ULayoutCell::OnSublevelLoadedCallback);
	}
}

void ULayoutCell::UnloadSublevel()
{
	if (!IsValid(this->Sublevel))
	{
		return;
	}

	this->Sublevel->SetIsRequestingUnloadAndRemoval(true);
	this->Sublevel = nullptr;
}

void ULayoutCell::GetAllActorsOfClassInSublevel(TSubclassOf<AActor> ActorClass, TArray<AActor*>& OutActors)
{
	if (IsValid(this->Sublevel) && this->Sublevel->HasLoadedLevel())
	{
		for (auto elem : this->Sublevel->GetLoadedLevel()->Actors)
		{
			if (IsValid(elem) && elem->IsA(ActorClass))
			{
				OutActors.Add(elem);
			}
		}
	}
}

void ULayoutCell::OnSublevelLoadedCallback()
{
	FRandomStream RStream = FRandomStream(this->UniqueSeed);
	TArray<AActor*> LevelActors;
	this->GetAllActorsOfClassInSublevel(AActor::StaticClass(), LevelActors);

	for (auto elem : LevelActors)
	{
		if (elem->Implements<ULayoutSublevelInterface>())
		{
			ILayoutSublevelInterface::Execute_OnLayoutDataReceived(elem, this->Owner, this, RStream.RandRange(MIN_int32, MAX_int32));
		}
	}
}

bool ULayoutCell::IsPointInSublevelBounds(FVector Point)
{
	TArray<AActor*> LevelActors;
	this->GetAllActorsOfClassInSublevel(AActor::StaticClass(), LevelActors);

	for (auto elem : LevelActors)
	{
		FVector Origin;
		FVector Extend;
		elem->GetActorBounds(false, Origin, Extend, false);

		if ((Point.X < Origin.X + Point.X && Point.X > Origin.X - Point.X) && (Point.Y < Origin.Y + Point.Y && Point.Y > Origin.Y - Point.Y) && (Point.Z < Origin.Z + Point.Z && Point.Z > Origin.Z - Point.Z))
		{
			return true;
		}
	}

	return false;
}

void ULayoutCell::DrawDebug(float Duration, bool bShowText) //Change this into a switchable debug command somehow
{
	if (Duration < 0.0)
	{
		Duration = 1e+18;
	}

	// Connection paths
	if (this->HasConnection.bPX)
	{
		FVector End = this->GetWorldLocation() + FVector(this->Owner->CellSize * 0.4, 0, 0);
		DrawDebugLine(this->Owner->GetWorld(), this->GetWorldLocation(), End, FColor::Green, false, Duration, 0, 20.0f);
	}

	if (this->HasConnection.bPY)
	{
		FVector End = this->GetWorldLocation() + FVector(0, this->Owner->CellSize * 0.4, 0);
		DrawDebugLine(this->Owner->GetWorld(), this->GetWorldLocation(), End, FColor::Green, false, Duration, 0, 20.0f);
	}

	if (this->HasConnection.bNX)
	{
		FVector End = this->GetWorldLocation() - FVector(this->Owner->CellSize * 0.4, 0, 0);
		DrawDebugLine(this->Owner->GetWorld(), this->GetWorldLocation(), End, FColor::Green, false, Duration, 0, 20.0f);
	}

	if (this->HasConnection.bNY)
	{
		FVector End = this->GetWorldLocation() - FVector(0, this->Owner->CellSize * 0.4, 0);
		DrawDebugLine(this->Owner->GetWorld(), this->GetWorldLocation(), End, FColor::Green, false, Duration, 0, 20.0f);
	}

	// Disabled neighbouring cells
	if (this->DisableNeighbouringCell.bPX)
	{
		FVector End = this->GetWorldLocation() + FVector(this->Owner->CellSize * 0.2, 0, 0);
		DrawDebugLine(this->Owner->GetWorld(), this->GetWorldLocation(), End, FColor::Red, false, Duration, 0, 20.0f);
	}

	if (this->DisableNeighbouringCell.bPY)
	{
		FVector End = this->GetWorldLocation() + FVector(0, this->Owner->CellSize * 0.2, 0);
		DrawDebugLine(this->Owner->GetWorld(), this->GetWorldLocation(), End, FColor::Red, false, Duration, 0, 20.0f);
	}

	if (this->DisableNeighbouringCell.bNX)
	{
		FVector End = this->GetWorldLocation() - FVector(this->Owner->CellSize * 0.2, 0, 0);
		DrawDebugLine(this->Owner->GetWorld(), this->GetWorldLocation(), End, FColor::Red, false, Duration, 0, 20.0f);
	}

	if (this->DisableNeighbouringCell.bNY)
	{
		FVector End = this->GetWorldLocation() - FVector(0, this->Owner->CellSize * 0.2, 0);
		DrawDebugLine(this->Owner->GetWorld(), this->GetWorldLocation(), End, FColor::Red, false, Duration, 0, 20.0f);
	}

	// Propeties
	if (bShowText)
	{
		FString Text;
		Text.Appendf(TEXT("Grid Location: X=%i Y=%i (i=%i)\n"), this->Location.X, this->Location.Y, this->Location.X * this->Owner->GridSize.X + this->Location.Y);
		Text.Appendf(TEXT("Grid Rotation: %i\n"), this->Rotation);
		Text.Appendf(TEXT("World Location: %s\n"), *this->GetWorldLocation().ToString());
		Text.Appendf(TEXT("World Rotation: %s\n"), *this->GetWorldRotation().ToString());
		Text.Appendf(TEXT("Is Enabled: %s\n"), this->bIsEnabled ? TEXT("true") : TEXT("false"));
		Text.Appendf(TEXT("Is Generated: %s\n"), this->bIsGenerated ? TEXT("true") : TEXT("false"));
		Text.Appendf(TEXT("Unique Seed: %i\n"), this->UniqueSeed);
		Text.Appendf(TEXT("Row Name: %s\n"), *this->RowName.ToString());
		Text.Appendf(TEXT("Sublevel Asset: %s\n"), *this->LevelAsset.ToString());
		Text.Appendf(TEXT("Unique Sublevel Name: %s\n"), *this->UniqueSublevelName);

		DrawDebugString(this->Owner->GetWorld(), GetWorldLocation() + FVector(0, 0, 200.0f), Text, nullptr, FColor::White, Duration, true, 1.0f);
	}
}

void ULayoutCell::BeginDestroy()
{
	Super::BeginDestroy();
	this->UnloadSublevel();
}
