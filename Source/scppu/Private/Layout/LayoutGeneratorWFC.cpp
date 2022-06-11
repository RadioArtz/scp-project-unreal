


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

	UE_LOG(LogLayout, Log, TEXT("%s: Starting layout generation with seed '%d'"), *this->GetName(), NewSeed);
	FDateTime StartTime = FDateTime::UtcNow();

	// Get all possible row names and save them
	TArray<FName> AllRowNames = Layout->DataTable->GetRowNames();

	// Initiate and fill UncollapsedWavefunctions
	TMap<FIntVector2, TArray<FName>> UncollapsedWavefunctions;

	for (auto Kvp : Layout->Grid)
	{
		UncollapsedWavefunctions.Add(Kvp.Key, TArray<FName>());
	}

	// Collapse wavefunctiones until there are no more left or we hit a contradiction
	while (UncollapsedWavefunctions.Num() > 0)
	{
		for (auto& Kvp : UncollapsedWavefunctions)
		{
			Kvp.Value.Empty(AllRowNames.Num());
			for (auto RowName : AllRowNames)
			{
				// Check every rotation until we find a valid one
				for (int i = 0; i < 4; i++)
				{
					bool bIsValid = Layout->GetCell(Kvp.Key)->IsRowNameValid(RowName, i);
					if (bIsValid)
					{
						// Add to list of possibilites and break since we only need one valid rotation
						Kvp.Value.Add(RowName);
						break;
					}
				}
			}
		}

		// Get the wavefunction with the least RowNames
		FIntVector2 BestKey;
		for (auto& Kvp : UncollapsedWavefunctions)
		{
			if (!UncollapsedWavefunctions.Contains(BestKey) || Kvp.Value.Num() < UncollapsedWavefunctions[BestKey].Num())
			{
				BestKey = Kvp.Key;
			}
		}

		// Ignore this cell when its blocked by another cell
		if (Layout->GetCell(BestKey)->IsBlockedByNeighbour())
		{
			UncollapsedWavefunctions.Remove(BestKey);
			continue;
		}

		// If we find a wavefunction with no possible rows, we've hit a contradiction, time to abort...
		if (UncollapsedWavefunctions[BestKey].Num() <= 0)
		{
			UE_LOG(LogLayout, Error, TEXT("%s: Found a contradiction, wavefunction of %s has no possible outcome. Aborting..."), *this->GetName(), *Layout->GetCell(BestKey)->UniqueSublevelName);
			return false;
		}

		// Collapse wavefunction (set cell to a possible row)
		FName FinalRowName = UncollapsedWavefunctions[BestKey][Layout->RandStream.RandRange(0, UncollapsedWavefunctions[BestKey].Num() - 1)];
		for (int i = 0; i < 4; i++)
		{
			bool bIsValid = Layout->GetCell(BestKey)->IsRowNameValid(FinalRowName, i);
			if (bIsValid)
			{
				Layout->GetCell(BestKey)->SetRowName(FinalRowName, i);
				break;
			}
		}

		UncollapsedWavefunctions.Remove(BestKey);
	}

	FDateTime EndTime = FDateTime::UtcNow();
	FTimespan GenerationTimeSpan = EndTime - StartTime;
	UE_LOG(LogLayout, Log, TEXT("%s: Finished layout generation, took %f seconds"), *this->GetName(), GenerationTimeSpan.GetTotalSeconds());

	return true;
}

void ULayoutGeneratorWFC::AsyncGenerateLayout(ALayout* Layout, int32 NewSeed)
{
	AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this, Layout, NewSeed]() mutable
	{
			this->GenerateLayout(Layout, NewSeed);
			// Call delegate
	});
}
