


#include "Layout/LayoutGeneratorWFC.h"
#include "Layout/Layout.h"
#include "Layout/LayoutCell.h"

bool ULayoutGeneratorWFC::GenerateLayout(ALayout* Layout, int32 NewSeed)
{
	bool bLayoutInitialized = Layout->InitializeLayout(Layout->GridSize, Layout->CellSize, NewSeed);
	if (!bLayoutInitialized)
	{
		return false;
	}

	UE_LOG(LogLayout, Log, TEXT("%s: Starting layout generation for '%s' with seed '%d'"), *this->GetName(), *Layout->GetName(), NewSeed);
	FDateTime StartTime = FDateTime::UtcNow();

	// Get all possible rows and save them
	TMap<FName, FLayoutCellGenerationSettings*> DataTableMap;
	TMap<FName, int32> RequiredInstances;
	TMap<FName, int32> MaximumInstances;
	for (auto Kvp : Layout->DataTable->GetRowMap())
	{
		FLayoutCellGenerationSettings* Row = (FLayoutCellGenerationSettings*)Kvp.Value;
		if (Row->RequiredInstances > 0)
		{
			RequiredInstances.Add(Kvp.Key, Row->RequiredInstances);
		}

		if (Row->MaximumInstances != 0)
		{
			MaximumInstances.Add(Kvp.Key, Row->MaximumInstances);
			DataTableMap.Add(Kvp.Key, Row);
		}
	}

	// Initiate and fill UncollapsedWavefunctions
	TMap<FIntVector2, TArray<FWavefunctionPossibility>> UncollapsedWavefunctions;
	for (auto& Kvp : Layout->Grid)
	{
		UncollapsedWavefunctions.Add(Kvp.Key, TArray<FWavefunctionPossibility>());
	}

	// TODO: Select random cells and generate the required rows (to avoid them generate next to each other OR not at all)

	// Collapse wavefunctiones until there are no more left or we hit a contradiction
	while (UncollapsedWavefunctions.Num() > 0)
	{
		for (auto& KvpWave : UncollapsedWavefunctions)
		{
			KvpWave.Value.Empty(DataTableMap.Num());
			for (auto KvpData : DataTableMap)
			{
				// Check every rotation until we find a valid one
				int32 StartRotation = Layout->RStream.RandRange(0, 3);
				for (int i = StartRotation; i < StartRotation + 4; i++)
				{
					bool bIsValid = Layout->GetCell(KvpWave.Key)->IsRowNameValid(KvpData.Key, i);
					if (bIsValid)
					{
						// Add to list of possibilites and break since we only need one valid rotation
						KvpWave.Value.Add(FWavefunctionPossibility(KvpData.Key, i));
						break;
					}
				}
			}
		}

		// Get the wavefunction with the least RowNames
		FIntVector2 BestWavefunctionKey;
		for (auto& Kvp : UncollapsedWavefunctions)
		{
			if (!UncollapsedWavefunctions.Contains(BestWavefunctionKey) || Kvp.Value.Num() < UncollapsedWavefunctions[BestWavefunctionKey].Num())
			{
				BestWavefunctionKey = Kvp.Key;
			}
		}

		// Reset this cell when its blocked by another cell and go to the next iteration 
		if (Layout->GetCell(BestWavefunctionKey)->IsBlockedByNeighbour())
		{
			Layout->GetCell(BestWavefunctionKey)->SetRowName("", 0);
			UncollapsedWavefunctions.Remove(BestWavefunctionKey);
			continue;
		}

		// If we find a wavefunction with no possible rows, we've hit a contradiction, time to abort...
		if (UncollapsedWavefunctions[BestWavefunctionKey].Num() <= 0)
		{
			UE_LOG(LogLayout, Error, TEXT("%s: Found a contradiction, wavefunction of '%s' has no possible outcome. Aborting..."), *this->GetName(), *Layout->GetCell(BestWavefunctionKey)->GetName());
			Layout->ClearLayout();
			return false;
		}

		// Collapse wavefunction (set cell to a possible row)
		TArray<FWavefunctionPossibility> WeightedWavefunctionPossibility;
		for (auto elem : UncollapsedWavefunctions[BestWavefunctionKey])
		{
			FLayoutCellGenerationSettings Row = *Layout->DataTable->FindRow<FLayoutCellGenerationSettings>(elem.RowName, "");
			for (int i = 0; i < Row.SpawnPoolEntries; i++)
			{
				WeightedWavefunctionPossibility.Add(elem);
			}
		}

		FWavefunctionPossibility RandomPossibility = WeightedWavefunctionPossibility[Layout->RStream.RandRange(0, WeightedWavefunctionPossibility.Num() - 1)];
		Layout->GetCell(BestWavefunctionKey)->SetRowName(RandomPossibility.RowName, RandomPossibility.Rotation);

		// Update maximum instances variable
		MaximumInstances[RandomPossibility.RowName] = MaximumInstances[RandomPossibility.RowName] - 1;
		if (MaximumInstances[RandomPossibility.RowName] == 0)
		{
			DataTableMap.Remove(RandomPossibility.RowName);
			MaximumInstances.Remove(RandomPossibility.RowName);
		}

		// Update required instances variable if necessary
		if (RequiredInstances.Contains(RandomPossibility.RowName))
		{
			RequiredInstances[RandomPossibility.RowName] = RequiredInstances[RandomPossibility.RowName] - 1;
			if (RequiredInstances[RandomPossibility.RowName] == 0)
			{
				RequiredInstances.Remove(RandomPossibility.RowName);
			}
		}

		// Remove from uncollapsed wavefunctions
		UncollapsedWavefunctions.Remove(BestWavefunctionKey);
	}

	// Check if every required row got generated TODO: Instant of aborting spawn every required row at the beginning
	if (RequiredInstances.Num() > 0)
	{
		UE_LOG(LogLayout, Error, TEXT("%s: Could not generate every required row. Aborting..."), *this->GetName());
		Layout->ClearLayout();
		return false;
	}

	FDateTime EndTime = FDateTime::UtcNow();
	FTimespan GenerationTimeSpan = EndTime - StartTime;
	UE_LOG(LogLayout, Log, TEXT("%s: Finished layout generation, took %f seconds"), *this->GetName(), GenerationTimeSpan.GetTotalSeconds());
	return true;
}

void ULayoutGeneratorWFC::AsyncGenerateLayout(ALayout* Layout, int32 NewSeed)
{
}
