

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "StrayWorldSettingsCleanupSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogStrayWorldSettingsCleanupSubsystem, Log, All);

UCLASS(Transient)
class SCPPUEDITOR_API UStrayWorldSettingsCleanupSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()
	
	//// Functions ////
public:
	virtual bool DoesSupportWorldType(EWorldType::Type WorldType) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual TStatId GetStatId() const override { return this->GetStatID(); }
	virtual bool IsTickableInEditor() const override { return true; }
	virtual bool IsTickable() const override;
	virtual void Tick(float DeltaTime) override;
	virtual void Deinitialize() override;

	//// Properties ////
public:
	bool bFinishedCleanup = false;
};
