


#include "Layout/LayoutCell.h"
#include "Layout/BaseLayoutSpawnValidator.h"
#include "Layout/LayoutSublevelInterface.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Engine/LevelActorContainer.h"
#include "DrawDebugHelpers.h"
#include "PrecomputedLightVolume.h"
#include "Engine/MapBuildDataRegistry.h"

FLayoutCellSides ULayoutCell::GetRequiredConnections()
{
	ULayoutCell* CellPX;
	ULayoutCell* CellPY;
	ULayoutCell* CellNX;
	ULayoutCell* CellNY;
	FLayoutCellSides RequiredConnections;
	this->GetLayout()->GetNeighbouringCells(this, false, CellPX, CellPY, CellNX, CellNY);
	RequiredConnections.bPX = IsValid(CellPX) && CellPX->HasConnections.bNX;
	RequiredConnections.bPY = IsValid(CellPY) && CellPY->HasConnections.bNY;
	RequiredConnections.bNX = IsValid(CellNX) && CellNX->HasConnections.bPX;
	RequiredConnections.bNY = IsValid(CellNY) && CellNY->HasConnections.bPY;
	return RequiredConnections;
}

FLayoutCellSides ULayoutCell::GetBlockedConnections()
{
	ULayoutCell* CellPX;
	ULayoutCell* CellPY;
	ULayoutCell* CellNX;
	ULayoutCell* CellNY;
	FLayoutCellSides BlockedConnections;
	this->GetLayout()->GetNeighbouringCells(this, false, CellPX, CellPY, CellNX, CellNY);
	BlockedConnections.bPX = !IsValid(CellPX) || CellPX->IsBlockedByNeighbour() || (CellPX->bIsGenerated && !CellPX->HasConnections.bNX);
	BlockedConnections.bPY = !IsValid(CellPY) || CellPY->IsBlockedByNeighbour() || (CellPY->bIsGenerated && !CellPY->HasConnections.bNY);
	BlockedConnections.bNX = !IsValid(CellNX) || CellNX->IsBlockedByNeighbour() || (CellNX->bIsGenerated && !CellNX->HasConnections.bPX);
	BlockedConnections.bNY = !IsValid(CellNY) || CellNY->IsBlockedByNeighbour() || (CellNY->bIsGenerated && !CellNY->HasConnections.bPY);
	return BlockedConnections;
}

bool ULayoutCell::IsBlockedByNeighbour()
{
	ULayoutCell* CellPX;
	ULayoutCell* CellPY;
	ULayoutCell* CellNX;
	ULayoutCell* CellNY;
	bool bIsBlocked = false;
	this->GetLayout()->GetNeighbouringCells(this, false, CellPX, CellPY, CellNX, CellNY);
	bIsBlocked = bIsBlocked || (IsValid(CellPX) && CellPX->DisableNeighbouringCells.bNX);
	bIsBlocked = bIsBlocked || (IsValid(CellPY) && CellPY->DisableNeighbouringCells.bNY);
	bIsBlocked = bIsBlocked || (IsValid(CellNX) && CellNX->DisableNeighbouringCells.bPX);
	bIsBlocked = bIsBlocked || (IsValid(CellNY) && CellNY->DisableNeighbouringCells.bPY);
	return bIsBlocked;
}

bool ULayoutCell::IsRowNameValid(FName InRowName, int InRotation)
{
	if (this->GetLayout()->DataTable->FindRowUnchecked(InRowName) == nullptr)
	{
		UE_LOG(LogLayout, Error, TEXT("%s: '%s' is not a valid row entry inside '%s'"), *this->GetName(), *InRowName.ToString(), *this->GetLayout()->DataTable->GetName());
		return false;
	}

	bool bIsValid = true;
	InRotation = InRotation % 4;

	// Save original properties so we can overwrite them temporarily for the pre spawn validators
	int PrevRotation = this->Rotation;
	FName PrevRowName = this->RowName;
	FLayoutCellSides PrevHasConnection = this->HasConnections;
	FLayoutCellSides PrevDisableNeighbouringCell = this->DisableNeighbouringCells;

	// Get values from datatable row
	FLayoutCellGenerationSettings Row = *(FLayoutCellGenerationSettings*)this->GetLayout()->DataTable->FindRowUnchecked(InRowName);

	// Rotate values from datatable row
	for (int i = 1; i < InRotation + 1; i++)
	{
		Row.HasConnections.RotateRight();
		Row.DisableNeighbouringCells.RotateRight();
	}

	// Set values from row for pre spawn validators
	this->RowName = InRowName;
	this->Rotation = InRotation;
	this->HasConnections = Row.HasConnections;
	this->DisableNeighbouringCells = Row.DisableNeighbouringCells;

	// Run pre spawn validation
	for (auto Elem : Row.PreSpawnValidators)
	{
		bIsValid = bIsValid && Elem.GetDefaultObject()->IsValidSpawn(this->GetLayout(), this, FRandomStream(this->UniqueSeed));
	}

	// Reset properties to their original state
	this->Rotation = PrevRotation;
	this->RowName = PrevRowName;
	this->HasConnections = PrevHasConnection;
	this->DisableNeighbouringCells = PrevDisableNeighbouringCell;

	// Get info from neighbouring cells 
	ULayoutCell* CellPX = nullptr;
	ULayoutCell* CellPY = nullptr;
	ULayoutCell* CellNX = nullptr;
	ULayoutCell* CellNY = nullptr;
	FLayoutCellSides RequiredConnections = this->GetRequiredConnections();
	FLayoutCellSides BlockedConnections = this->GetBlockedConnections();

	// Only retrive neighbouring cells if we actually need them
	if (Row.DisableNeighbouringCells != FLayoutCellSides(false, false, false, false))
	{
		this->GetLayout()->GetNeighbouringCells(this, false, CellPX, CellPY, CellNX, CellNY);
	}

	// Check if all connections fit (ignore warning, IsValid() checks for nullptr)
	//PX
	bIsValid = bIsValid && ((RequiredConnections.bPX && Row.HasConnections.bPX) || !RequiredConnections.bPX);
	bIsValid = bIsValid && ((BlockedConnections.bPX && !Row.HasConnections.bPX) || !BlockedConnections.bPX);
	bIsValid = bIsValid && (!Row.DisableNeighbouringCells.bPX || !IsValid(CellPX) || (Row.DisableNeighbouringCells.bPX && !CellPX->bIsGenerated && !CellPX->IsRequiredToGenerate() && !CellPX->IsBlockedByNeighbour()));

	//PY
	bIsValid = bIsValid && ((RequiredConnections.bPY && Row.HasConnections.bPY) || !RequiredConnections.bPY);
	bIsValid = bIsValid && ((BlockedConnections.bPY && !Row.HasConnections.bPY) || !BlockedConnections.bPY);
	bIsValid = bIsValid && (!Row.DisableNeighbouringCells.bPY || !IsValid(CellPY) || (Row.DisableNeighbouringCells.bPY && !CellPY->bIsGenerated && !CellPY->IsRequiredToGenerate() && !CellPY->IsBlockedByNeighbour()));

	//NX
	bIsValid = bIsValid && ((RequiredConnections.bNX && Row.HasConnections.bNX) || !RequiredConnections.bNX);
	bIsValid = bIsValid && ((BlockedConnections.bNX && !Row.HasConnections.bNX) || !BlockedConnections.bNX);
	bIsValid = bIsValid && (!Row.DisableNeighbouringCells.bNX || !IsValid(CellNX) || (Row.DisableNeighbouringCells.bNX && !CellNX->bIsGenerated && !CellNX->IsRequiredToGenerate() && !CellNX->IsBlockedByNeighbour()));

	//NY
	bIsValid = bIsValid && ((RequiredConnections.bNY && Row.HasConnections.bNY) || !RequiredConnections.bNY);
	bIsValid = bIsValid && ((BlockedConnections.bNY && !Row.HasConnections.bNY) || !BlockedConnections.bNY);
	bIsValid = bIsValid && (!Row.DisableNeighbouringCells.bNY || !IsValid(CellNY) || (Row.DisableNeighbouringCells.bNY && !CellNY->bIsGenerated && !CellNY->IsRequiredToGenerate() && !CellNY->IsBlockedByNeighbour()));

	return bIsValid;
}

void ULayoutCell::SetRowName(FName NewRowName, int NewRotation)
{
	if (NewRowName == "None")
	{
		this->RowName = "None";
		this->HasConnections = FLayoutCellSides();
		this->DisableNeighbouringCells = FLayoutCellSides();
		this->Rotation = NewRotation % 4;
		this->LevelAsset = nullptr;
		this->UnloadSublevel(true);
		this->bIsGenerated = true;
		this->bLevelAlwaysVisible = false;
		this->bLevelAlwaysLoaded = false;
		return;
	}

	if (this->GetLayout()->DataTable->FindRowUnchecked(NewRowName) == nullptr)
	{
		UE_LOG(LogLayout, Error, TEXT("%s: '%s' is not a valid row entry inside '%s'"), *this->GetName(), *NewRowName.ToString(), *this->GetLayout()->DataTable->GetName());
		return;
	}

	FRandomStream RStream = FRandomStream(this->UniqueSeed);
	FLayoutCellGenerationSettings Row = *(FLayoutCellGenerationSettings*)this->GetLayout()->DataTable->FindRowUnchecked(NewRowName);
	NewRotation = NewRotation % 4;

	// Rotate values from row
	for (int i = 1; i < NewRotation + 1; i++)
	{
		Row.HasConnections.RotateRight();
		Row.DisableNeighbouringCells.RotateRight();
	}

	// Set values from row
	this->RowName = NewRowName;
	this->Rotation = NewRotation;
	this->HasConnections = Row.HasConnections;
	this->DisableNeighbouringCells = Row.DisableNeighbouringCells;

	// Set level asset
	if (Row.Levels.Num() > 0)
	{
		this->LevelAsset = Row.Levels[RStream.RandRange(0, Row.Levels.Num() - 1)];
	}

	this->UnloadSublevel(true);
	this->bIsGenerated = true;
	this->bLevelAlwaysVisible = Row.bLevelAlwaysVisible;
	this->bLevelAlwaysLoaded = Row.bLevelAlwaysLoaded;
}

void ULayoutCell::ResetRowName()
{
	this->RowName = "None";
	this->HasConnections = FLayoutCellSides();
	this->DisableNeighbouringCells = FLayoutCellSides();
	this->Rotation = 0;
	this->LevelAsset = nullptr;
	this->UnloadSublevel(true);
	this->bIsGenerated = false;
}

void ULayoutCell::LoadSublevel(bool bShowSublevel)
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
	this->Sublevel = ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(this->GetWorld(), this->LevelAsset, this->GetWorldLocation(), this->GetWorldRotation(), bLevelLoaded, this->GetSublevelName());
	if (bLevelLoaded)
	{
		this->Sublevel->bShouldBlockOnLoad = false;
		this->Sublevel->bShouldBlockOnUnload = false;
		this->Sublevel->bDisableDistanceStreaming = true;
		this->Sublevel->bInitiallyVisible = false;
		this->Sublevel->OnLevelLoaded.AddDynamic(this, &ULayoutCell::OnSublevelLoadedCallback);
		this->Sublevel->OnLevelShown.AddDynamic(this, &ULayoutCell::OnSublevelShownCallback);

		if (bShowSublevel)
		{
			this->ShowSublevel();
		}
	}
}

void ULayoutCell::UnloadSublevel(bool bForce)
{
	if (!IsValid(this->Sublevel))
	{
		return;
	}

	if (!bForce && this->bLevelAlwaysLoaded) 
	{
		return;
	}

	this->Sublevel->SetIsRequestingUnloadAndRemoval(true);
	this->Sublevel = nullptr;
}

void ULayoutCell::ShowSublevel()
{
	if (!IsValid(this->Sublevel))
	{
		return;
	}

	this->Sublevel->SetShouldBeVisible(true);
}

void ULayoutCell::HideSublevel(bool bForce)
{
	if (!IsValid(this->Sublevel))
	{
		return;
	}

	if (!bForce && this->bLevelAlwaysVisible)
	{
		return;
	}

	this->Sublevel->SetShouldBeVisible(false);
}

void ULayoutCell::GetAllActorsOfClassInSublevel(TSubclassOf<AActor> ActorClass, TArray<AActor*>& OutActors)
{
	if (IsValid(this->Sublevel) && this->Sublevel->HasLoadedLevel())
	{
		for (auto Elem : this->Sublevel->GetLoadedLevel()->Actors)
		{
			if (IsValid(Elem) && Elem->IsA(ActorClass))
			{
				OutActors.Add(Elem);
			}
		}
	}
}

bool ULayoutCell::TransferSublevelActorToPresistentLevel(AActor* Actor)
{
	if (!IsValid(Actor))
	{
		return false;
	}

	if (Actor->GetRootComponent()->Mobility != EComponentMobility::Movable)
	{
		return false;
	}

	if (!IsValid(this->Sublevel) || !this->Sublevel->HasLoadedLevel())
	{
		return false;
	}

	Actor->Modify(true);
	FString ActorName;
	Actor->GetName(ActorName);
	Actor->Rename(*ActorName, this->GetWorld()->PersistentLevel, REN_NonTransactional);
	return true;
}

bool ULayoutCell::TransferPresistentLevelActorToSublevel(AActor* Actor)
{
	if (!IsValid(Actor))
	{
		return false;
	}

	if (Actor->GetRootComponent()->Mobility != EComponentMobility::Movable)
	{
		return false;
	}

	if (!IsValid(this->Sublevel) || !this->Sublevel->HasLoadedLevel())
	{
		return false;
	}

	if (!Actor->IsInPersistentLevel())
	{
		return false;
	}

	FString ActorName;
	Actor->GetName(ActorName);
	Actor->Rename(*ActorName, this->Sublevel->GetLoadedLevel(), REN_NonTransactional);
	return true;
}

void ULayoutCell::OnSublevelLoadedCallback()
{
	FRandomStream RStream = FRandomStream(this->UniqueSeed);
	TArray<AActor*> LevelActors;
	this->GetAllActorsOfClassInSublevel(AActor::StaticClass(), LevelActors);

	for (auto Elem : LevelActors)
	{
		if (Elem->Implements<ULayoutSublevelInterface>())
		{
			ILayoutSublevelInterface::Execute_OnLayoutDataReceived(Elem, this->GetLayout(), this, RStream.RandRange(0, MAX_int32 - 1));
		}
	}

	UE_LOG(LogTemp, Error, TEXT("%s: %s"), *this->GetName(), *this->GetWorldLocation().ToString());
}

void ULayoutCell::OnSublevelShownCallback()
{
	/*
	// Apply offset to precomuted lightning volume
	*/

	UE_LOG(LogTemp, Error, TEXT("%s: Room: %s"), *this->GetName(), *this->RowName.ToString());
	if (this->Sublevel->GetLoadedLevel()->MapBuildData != nullptr)
	{
		FSceneInterface* Scene = this->GetWorld()->Scene;
		FGuid BuildDataId = this->Sublevel->GetLoadedLevel()->LevelBuildDataId;
		UMapBuildDataRegistry* Registry = this->Sublevel->GetLoadedLevel()->MapBuildData;
		FPrecomputedLightVolume* Volume = this->Sublevel->GetLoadedLevel()->PrecomputedLightVolume;

		FPrecomputedLightVolumeData* OriginalLightVolumeData = Registry->GetLevelPrecomputedLightVolumeBuildData(BuildDataId);
		if (OriginalLightVolumeData && OriginalLightVolumeData->IsInitialized())
		{
			FExposedPrecomputedLightVolumeData* ExposedLightVolumeData = reinterpret_cast<FExposedPrecomputedLightVolumeData*>(OriginalLightVolumeData);
			FPrecomputedLightVolumeData* NewLightVolumeData = new FPrecomputedLightVolumeData();
			FTransform LevelTransform = FTransform(this->GetWorldRotation(), this->GetWorldLocation(), FVector::OneVector);

			Volume->RemoveFromScene(Scene);
			NewLightVolumeData->Initialize(OriginalLightVolumeData->GetBounds().MoveTo(LevelTransform.GetLocation()));
			ExposedLightVolumeData->HighQualityLightmapOctree.FindAllElements([NewLightVolumeData, LevelTransform](const FVolumeLightingSample& VolumeSample)
				{
					FVolumeLightingSample NewSample = FVolumeLightingSample();
					NewSample.DirectionalLightShadowing = VolumeSample.DirectionalLightShadowing;
					NewSample.Lighting = VolumeSample.Lighting;
					NewSample.PackedSkyBentNormal = VolumeSample.PackedSkyBentNormal;
					NewSample.Position = LevelTransform.TransformPosition(VolumeSample.Position);
					NewSample.Radius = VolumeSample.Radius;
					NewLightVolumeData->AddHighQualityLightingSample(NewSample);

			//FColor DebugColor = NewSample.Lighting.CalcIntegral().ToFColor(false);
			//DrawDebugPoint(this->GetWorld(), NewSample.Position, 5.f, DebugColor, true);
				});

			ExposedLightVolumeData->LowQualityLightmapOctree.FindAllElements([NewLightVolumeData, LevelTransform](const FVolumeLightingSample& VolumeSample)
				{
					FVolumeLightingSample NewSample = FVolumeLightingSample();
					NewSample.DirectionalLightShadowing = VolumeSample.DirectionalLightShadowing;
					NewSample.Lighting = VolumeSample.Lighting;
					NewSample.PackedSkyBentNormal = VolumeSample.PackedSkyBentNormal;
					NewSample.Position = LevelTransform.TransformPosition(VolumeSample.Position);
					NewSample.Radius = VolumeSample.Radius;
					NewLightVolumeData->AddLowQualityLightingSample(NewSample);
				});

			NewLightVolumeData->FinalizeSamples();
			Registry->AddLevelPrecomputedLightVolumeBuildData(BuildDataId, NewLightVolumeData);
			Volume->AddToScene(Scene, Registry, BuildDataId);
			Registry->AddLevelPrecomputedLightVolumeBuildData(BuildDataId, OriginalLightVolumeData);

			FBoxSphereBounds LevelBounds = FBoxSphereBounds(300.f);
			LevelBounds.Origin = LevelTransform.GetLocation();
			UE_LOG(LogTemp, Error, TEXT("Octree bounds center: Original: %s New: %s"), *OriginalLightVolumeData->GetBounds().GetCenter().ToString(), *NewLightVolumeData->GetBounds().GetCenter().ToString());
			UE_LOG(LogTemp, Error, TEXT("Octree intersects with Level bounds: %i"), Volume->IntersectBounds(LevelBounds));
			UE_LOG(LogTemp, Error, TEXT("I tried my best rotating!"));
		}
	}
}

bool ULayoutCell::IsPointInSublevelBounds(FVector Point)
{
	TArray<AActor*> LevelActors;
	this->GetAllActorsOfClassInSublevel(AActor::StaticClass(), LevelActors);

	for (auto Elem : LevelActors)
	{
		FVector Origin;
		FVector Extend;
		Elem->GetActorBounds(false, Origin, Extend, false);

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
	if (this->HasConnections.bPX)
	{
		FVector End = this->GetWorldLocation() + FVector(this->GetLayout()->CellSize * 0.4, 0, 0);
		DrawDebugLine(this->GetWorld(), this->GetWorldLocation(), End, FColor::Green, false, Duration, 0, 20.0f);
	}

	if (this->HasConnections.bPY)
	{
		FVector End = this->GetWorldLocation() + FVector(0, this->GetLayout()->CellSize * 0.4, 0);
		DrawDebugLine(this->GetWorld(), this->GetWorldLocation(), End, FColor::Green, false, Duration, 0, 20.0f);
	}

	if (this->HasConnections.bNX)
	{
		FVector End = this->GetWorldLocation() - FVector(this->GetLayout()->CellSize * 0.4, 0, 0);
		DrawDebugLine(this->GetWorld(), this->GetWorldLocation(), End, FColor::Green, false, Duration, 0, 20.0f);
	}

	if (this->HasConnections.bNY)
	{
		FVector End = this->GetWorldLocation() - FVector(0, this->GetLayout()->CellSize * 0.4, 0);
		DrawDebugLine(this->GetWorld(), this->GetWorldLocation(), End, FColor::Green, false, Duration, 0, 20.0f);
	}

	// Disabled neighbouring cells
	if (this->DisableNeighbouringCells.bPX)
	{
		FVector End = this->GetWorldLocation() + FVector(this->GetLayout()->CellSize * 0.2, 0, 0);
		DrawDebugLine(this->GetWorld(), this->GetWorldLocation(), End, FColor::Red, false, Duration, 0, 20.0f);
	}

	if (this->DisableNeighbouringCells.bPY)
	{
		FVector End = this->GetWorldLocation() + FVector(0, this->GetLayout()->CellSize * 0.2, 0);
		DrawDebugLine(this->GetWorld(), this->GetWorldLocation(), End, FColor::Red, false, Duration, 0, 20.0f);
	}

	if (this->DisableNeighbouringCells.bNX)
	{
		FVector End = this->GetWorldLocation() - FVector(this->GetLayout()->CellSize * 0.2, 0, 0);
		DrawDebugLine(this->GetWorld(), this->GetWorldLocation(), End, FColor::Red, false, Duration, 0, 20.0f);
	}

	if (this->DisableNeighbouringCells.bNY)
	{
		FVector End = this->GetWorldLocation() - FVector(0, this->GetLayout()->CellSize * 0.2, 0);
		DrawDebugLine(this->GetWorld(), this->GetWorldLocation(), End, FColor::Red, false, Duration, 0, 20.0f);
	}

	// Propeties
	if (bShowText)
	{
		FString Text;
		Text.Appendf(TEXT("UObject Name: %s\n"), *this->GetName());
		Text.Appendf(TEXT("Grid Location: X=%i Y=%i (i=%i)\n"), this->Location.X, this->Location.Y, this->Location.X * this->GetLayout()->GridSize.X + this->Location.Y);
		Text.Appendf(TEXT("Grid Rotation: %i\n"), this->Rotation);
		Text.Appendf(TEXT("World Location: %s\n"), *this->GetWorldLocation().ToString());
		Text.Appendf(TEXT("World Rotation: %s\n"), *this->GetWorldRotation().ToString());
		Text.Appendf(TEXT("Is Generated: %s\n"), this->bIsGenerated ? TEXT("true") : TEXT("false"));
		Text.Appendf(TEXT("Is Blocked by Neighbour: %s\n"), this->IsBlockedByNeighbour() ? TEXT("true") : TEXT("false"));
		Text.Appendf(TEXT("Unique Seed: %i\n"), this->UniqueSeed);
		Text.Appendf(TEXT("Row Name: %s\n"), *this->RowName.ToString());
		Text.Appendf(TEXT("Sublevel Asset: %s\n"), *this->LevelAsset.ToString());
		Text.Appendf(TEXT("Unique Sublevel Name: %s\n"), *this->GetSublevelName());

		DrawDebugString(this->GetWorld(), GetWorldLocation() + FVector(0, 0, 200.0f), Text, nullptr, FColor::White, Duration, true, 1.0f);
	}
}

void ULayoutCell::BeginDestroy()
{
	Super::BeginDestroy();
	this->UnloadSublevel(true);
}
