


#include "Layout/LayoutCell.h"
#include "Layout/Layout.h"
#include "Engine/LevelStreamingDynamic.h"

FVector ULayoutCell::GetWorldLocation()
{
    return (FVector(this->Location.X, this->Location.Y, 0) * Owner->CellSize) + Owner->GetActorLocation();
}

FRotator ULayoutCell::GetWorldRotation()
{
    return FRotator(0, this->Rotation * 90, 0);
}

bool ULayoutCell::SetRowName(FName NewRowName, bool bForce)
{
    // later
    return false;
}

bool ULayoutCell::LoadSublevel()
{
    if (this->Sublevel != nullptr)
    {
        return false;
    }

    if (this->LevelAsset == nullptr)
    {
        return false;
    }

    bool bLevelLoaded;
    this->Sublevel = ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(this->Owner->GetWorld(), this->LevelAsset, this->GetWorldLocation(), this->GetWorldRotation(), bLevelLoaded, this->UniqueSublevelName);
    if (bLevelLoaded)
    {
        this->Sublevel->bShouldBlockOnLoad = false;
        this->Sublevel->bShouldBlockOnUnload = false;
        this->Sublevel->bDisableDistanceStreaming = true;
        this->Sublevel->SetShouldBeVisible(false);
    }

    return bLevelLoaded;
}

bool ULayoutCell::UnloadSublevel()
{
    if (this->Sublevel == nullptr)
    {
        return false;
    }

    this->Sublevel->SetIsRequestingUnloadAndRemoval(true);
    this->Sublevel = nullptr;
    return true;
}

void ULayoutCell::GetAllActorsInSublevel(TArray<AActor*>& OutActors)
{
    if (this->Sublevel != nullptr && this->Sublevel->HasLoadedLevel())
    {
        for (AActor* elem : this->Sublevel->GetLoadedLevel()->Actors)
        {
            if (IsValid(elem))
            {
                OutActors.Add(elem);
            }
        }
    }
}

bool ULayoutCell::IsPointInSublevelBounds(FVector Point)
{
    TArray<AActor*> LevelActors;
    this->GetAllActorsInSublevel(LevelActors);

    for (AActor* elem : LevelActors)
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

void ULayoutCell::DrawDebug(float Duration)
{
    //later
}

void ULayoutCell::BeginDestroy()
{
    Super::BeginDestroy();
    this->UnloadSublevel();
}
