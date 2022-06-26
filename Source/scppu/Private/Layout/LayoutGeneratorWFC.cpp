


#include "Layout/LayoutGeneratorWFC.h"
#include "Layout/Layout.h"
#include "Layout/LayoutCell.h"
#include "Layout/LayoutSpawnValidator.h"

bool ULayoutGeneratorWFC::Generate(ALayout* Layout, int32 NewSeed)
{
	bool bLayoutInitialized = Layout->Initialize(NewSeed);
	if (!bLayoutInitialized)
	{
		return false;
	}

	UE_LOG(LogLayout, Log, TEXT("%s: Starting layout generation for '%s' with seed '%d'"), *this->GetName(), *Layout->GetName(), NewSeed);
	FDateTime StartTime = FDateTime::UtcNow();
	bool bSuccess = this->GenerateInternal(Layout, NewSeed);
	if (bSuccess)
	{
		FDateTime EndTime = FDateTime::UtcNow();
		FTimespan GenerationTimeSpan = EndTime - StartTime;
		UE_LOG(LogLayout, Log, TEXT("%s: Finished layout generation, took %f seconds"), *this->GetName(), GenerationTimeSpan.GetTotalSeconds());
		return true;
	}
	else
	{
		Layout->Clear();
		return false;
	}
}

void ULayoutGeneratorWFC::AsyncGenerate(ALayout* Layout, int32 NewSeed, FLayoutGeneratorWFCDone OnDone)
{
	// Needs to be on GT because this function will create uobjects
	bool bLayoutInitialized = Layout->Initialize(NewSeed);
	if (!bLayoutInitialized)
	{
		OnDone.Execute(false);
		return;
	}

	UE_LOG(LogLayout, Log, TEXT("%s: Starting async layout generation for '%s' with seed '%d'"), *this->GetName(), *Layout->GetName(), NewSeed);
	UE_LOG(LogLayout, Log, TEXT("%s: '%s' and its members will be borrowed by a seperate thread. Actions with them should be avoided during this time to avoid race condtions!"), *this->GetName(), *Layout->GetName());
	FDateTime StartTime = FDateTime::UtcNow();
	AsyncTask(ENamedThreads::AnyHiPriThreadHiPriTask, [this, Layout, NewSeed, OnDone, StartTime]() {
		bool bSuccess = this->GenerateInternal(Layout, NewSeed);
		UE_LOG(LogLayout, Log, TEXT("%s: '%s' and its memebers have been released from the seperate thread!"), *this->GetName(), *Layout->GetName());
		if (bSuccess)
		{
			AsyncTask(ENamedThreads::GameThread, [this, Layout, OnDone, StartTime]() {
				// Doesn't need to be on GT but idk it just feels right
				FDateTime EndTime = FDateTime::UtcNow();
				FTimespan GenerationTimeSpan = EndTime - StartTime;
				UE_LOG(LogLayout, Log, TEXT("%s: Finished layout generation, took %f seconds"), *this->GetName(), GenerationTimeSpan.GetTotalSeconds());
				OnDone.Execute(true);
			});
		}
		else 
		{
			AsyncTask(ENamedThreads::GameThread, [this, Layout, OnDone]() {
				// Needs to be on GT because this function will dispose uobjects
				Layout->Clear();
				OnDone.Execute(false);
			});
		}
	});
}

bool ULayoutGeneratorWFC::GenerateInternal(ALayout* Layout, int32 Seed)
{
	FRandomStream RStream = FRandomStream(Seed);

	// Get all possible rows and save them
	TMap<FName, FLayoutCellGenerationSettings*> DataTableMap;
	TMap<FName, int32> RequiredInstances;
	TMap<FName, int32> MaximumInstances;
	for (auto Kvp : Layout->DataTable->GetRowMap())
	{
		FLayoutCellGenerationSettings* Row = (FLayoutCellGenerationSettings*)Kvp.Value;
		RequiredInstances.Add(Kvp.Key, Row->RequiredInstances);
		MaximumInstances.Add(Kvp.Key, Row->MaximumInstances);
		DataTableMap.Add(Kvp.Key, Row);
	}

	// Initiate and fill CellPossibilities
	TMap<FIntVector2, TArray<FCellPossibility>> CellPossibilities;
	for (auto Kvp : Layout->Grid)
	{
		CellPossibilities.Add(Kvp.Key, TArray<FCellPossibility>());
	}

	// Select random cells and generate the required rows
	for (auto& Kvp : RequiredInstances)
	{
		TArray<FIntVector2> UntestedCells;
		CellPossibilities.GenerateKeyArray(UntestedCells);
		while (Kvp.Value > 0)
		{
			if (UntestedCells.Num() <= 0)
			{
				UE_LOG(LogLayout, Error, TEXT("%s: Required instance '%s' could not be set. Aborting..."), *this->GetName(), *Kvp.Key.ToString());
				return false;
			}

			// Choose a random cell
			FIntVector2 CurrentCellKey = UntestedCells[RStream.RandRange(0, UntestedCells.Num() - 1)];

			// Check every rotation until we find a valid one
			int32 StartRotation = RStream.RandRange(0, 3);
			for (int i = StartRotation; i < StartRotation + 4; i++)
			{
				bool bIsValid = Layout->GetCell(CurrentCellKey)->IsRowNameValid(Kvp.Key, i);
				if (bIsValid)
				{
					// Add to list of possibilites and break since we only need one valid rotation
					Layout->GetCell(CurrentCellKey)->SetRowName(Kvp.Key, i);
					UE_LOG(LogLayout, Verbose, TEXT("%s: Set '%s' to '%s' (required instance)"), *this->GetName(), *Layout->GetCell(CurrentCellKey)->GetName(), *Kvp.Key.ToString());

					// Update required instances variable
					Kvp.Value--;

					// Update maximum instances variable
					MaximumInstances[Kvp.Key]--;

					// Remove cell from possibilities
					CellPossibilities.Remove(CurrentCellKey);
					break;
				}
			}

			UntestedCells.Remove(CurrentCellKey);
		}
	}

	// Generate cells until there are no more left or we hit a contradiction
	while (CellPossibilities.Num() > 0)
	{
		// Update possiblities of all cells (update the certainty of wavefunctions) (here is the bottleneck)
		for (auto& KvpCell : CellPossibilities)
		{
			KvpCell.Value.Empty(DataTableMap.Num());
			for (auto& KvpData : DataTableMap)
			{
				// Skip this row if the maximum instance limit has been reached (is not <= 0 to make -1 work as infinte)
				if (MaximumInstances[KvpData.Key] == 0)
				{
					continue;
				}

				// Check every rotation until we find a valid one
				int32 StartRotation = RStream.RandRange(0, 3);
				for (int i = StartRotation; i < StartRotation + 4; i++)
				{
					bool bIsValid = Layout->GetCell(KvpCell.Key)->IsRowNameValid(KvpData.Key, i);
					if (bIsValid)
					{
						// Add to list of possibilites and break since we only need one valid rotation
						KvpCell.Value.Add(FCellPossibility(KvpData.Key, i));
						UE_LOG(LogLayout, Verbose, TEXT("%s: '%s' is valid for '%s' and was added to possibilities"), *this->GetName(), *KvpData.Key.ToString(), *Layout->GetCell(KvpCell.Key)->GetName());
						break;
					}
				}
			}
		}

		// Get the cell with the least possibilities (wavefunction which is most certain)
		FIntVector2 CurrentCellKey = FIntVector2(-1, -1);
		for (auto Kvp : CellPossibilities)
		{
			if (CurrentCellKey == FIntVector2(-1, -1) || Kvp.Value.Num() < CellPossibilities[CurrentCellKey].Num())
			{
				CurrentCellKey = Kvp.Key;
			}
		}

		// Reset this cell when its blocked by another cell and go to the next iteration 
		if (Layout->GetCell(CurrentCellKey)->IsBlockedByNeighbour())
		{
			Layout->GetCell(CurrentCellKey)->SetRowName("None", 0);
			CellPossibilities.Remove(CurrentCellKey);
			continue;
		}

		// If we find a cell with no possible rows, we've hit a contradiction, time to abort...
		if (CellPossibilities[CurrentCellKey].Num() <= 0)
		{
			UE_LOG(LogLayout, Error, TEXT("%s: Found a contradiction, '%s' has no possible row. Aborting..."), *this->GetName(), *Layout->GetCell(CurrentCellKey)->GetName());
			return false;
		}

		// Set cell to a possible row (collapse wavefunction)
		TArray<FCellPossibility> WeightedCellPossibilities;
		for (auto Elem : CellPossibilities[CurrentCellKey])
		{
			FLayoutCellGenerationSettings Row = *DataTableMap[Elem.RowName];
			for (int i = 0; i < Row.SpawnPoolEntries; i++)
			{
				WeightedCellPossibilities.Add(Elem);
			}
		}

		FCellPossibility CurrentCellPossibility = WeightedCellPossibilities[RStream.RandRange(0, WeightedCellPossibilities.Num() - 1)];
		Layout->GetCell(CurrentCellKey)->SetRowName(CurrentCellPossibility.RowName, CurrentCellPossibility.Rotation);
		UE_LOG(LogLayout, Verbose, TEXT("%s: Set '%s' to '%s'"), *this->GetName(), *Layout->GetCell(CurrentCellKey)->GetName(), *CurrentCellPossibility.RowName.ToString());

		// Update maximum instances variable
		MaximumInstances[CurrentCellPossibility.RowName]--;

		// Remove from CellPossibilities
		CellPossibilities.Remove(CurrentCellKey);
	}

	// Run post spawn validation
	for (auto Kvp : Layout->Grid)
	{
		if (Kvp.Value->RowName.IsNone())
		{
			continue;
		}

		for (auto Elem : DataTableMap[Kvp.Value->RowName]->PostSpawnValidators)
		{
			ULayoutSpawnValidator* Validator = Elem.GetDefaultObject();
			bool bIsValid = Validator->IsValidSpawn(Layout, Kvp.Value, FRandomStream(RStream.RandRange(0, MAX_int32 - 1)));
			if (!bIsValid)
			{
				UE_LOG(LogLayout, Error, TEXT("%s: Post spawn validator '%s' returned false for '%s'. Aborting..."), *this->GetName(), *Elem->GetName(), *Kvp.Value->GetName());
				return false;
			}
		}
	}

	return true;
}
