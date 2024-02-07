


#include "StrayWorldSettingsCleanupSubsystem.h"
#include "GameFramework/WorldSettings.h"

DEFINE_LOG_CATEGORY(LogStrayWorldSettingsCleanupSubsystem);

bool UStrayWorldSettingsCleanupSubsystem::DoesSupportWorldType(EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Editor;
}

void UStrayWorldSettingsCleanupSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogStrayWorldSettingsCleanupSubsystem, Log, TEXT("%s: Initialized!"), *this->GetName());
}

bool UStrayWorldSettingsCleanupSubsystem::IsTickable() const
{
	return !this->bFinishedCleanup;
}

void UStrayWorldSettingsCleanupSubsystem::Tick(float DeltaTime)
{
	if (!this->GetWorld()->bIsWorldInitialized)
	{
		return;
	}

	UE_LOG(LogStrayWorldSettingsCleanupSubsystem, Verbose, TEXT("%s: Starting clean up..."), *this->GetName());

	TArray<AActor*> LevelActors = this->GetWorld()->PersistentLevel->Actors;
	for (int i = 0; i < LevelActors.Num(); i++)
	{
		AActor* Actor = LevelActors[i];
		if (!IsValid(Actor))
		{
			continue;
		}

		if (!Actor->IsA<AWorldSettings>())
		{
			continue;
		}

		if (Actor == (AActor*)this->GetWorld()->GetWorldSettings())
		{
			continue;
		}

		UE_LOG(LogStrayWorldSettingsCleanupSubsystem, Warning, TEXT("%s: Stray world settings '%s' actor found and destroyed, resave level or actors to complete clean up"), *this->GetName(), *Actor->GetName());
		Actor->Destroy();
	}

	this->bFinishedCleanup = true;
	UE_LOG(LogStrayWorldSettingsCleanupSubsystem, Verbose, TEXT("%s: Clean up done"), *this->GetName());
}

void UStrayWorldSettingsCleanupSubsystem::Deinitialize()
{
	Super::Deinitialize();
	UE_LOG(LogStrayWorldSettingsCleanupSubsystem, Log, TEXT("%s: Deinitialized!"), *this->GetName());
}
