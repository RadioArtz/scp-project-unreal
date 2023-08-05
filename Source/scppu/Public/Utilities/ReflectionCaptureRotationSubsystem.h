

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ReflectionCaptureRotationSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogReflectionCaptureSubsystem, Log, All);

UCLASS(Transient)
class SCPPU_API UReflectionCaptureRotationSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

		//// Functions ////
public:
	virtual bool DoesSupportWorldType(EWorldType::Type WorldType) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual TStatId GetStatId() const override { return this->GetStatID(); }
	virtual void Tick(float DeltaTime) override;
	virtual void Deinitialize() override;

	void OnLevelAddedToWorldCallback(ULevel* Level, UWorld* World);
	void OnLevelRemovedFromWorldCallback(ULevel* Level, UWorld* World);

private:
	FTransform FindLevelTransform(ULevel* Level);
	UTextureCube* CopyAndRotateTextureCube(UTextureCube* SourceTexture, float Angle);
	
		//// Properties ////
private:
	UMaterialInstanceDynamic* RotatorMaterial;
	TArray<UTexture*> AllCubemaps;
};

UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "Reflection Capture Rotation Subsystem"))
class SCPPU_API UReflectionSubsystemSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly)
		TSoftObjectPtr<UMaterialInterface> RotatorMaterialAsset;

	// The path of the reflection component the reflection data was taken from
	UPROPERTY(Transient, VisibleAnywhere, Category = "Debug")
		FString ReflectionComponentPath;

	// The texture that gets supplied to the material via the 'SourceTexture' parameter
	UPROPERTY(Transient, VisibleAnywhere, Category = "Debug")
		UTexture* InTexture;

	// The angle that gets supplied to the material via the 'RotationAngle' parameter
	UPROPERTY(Transient, VisibleAnywhere, Category = "Debug")
		float InAngle;

	// The render target on which the material gets drawn to
	UPROPERTY(Transient, VisibleAnywhere, Category = "Debug")
		UTexture* RenderTarget;

	// The resulting static texture generated from the render target
	UPROPERTY(Transient, VisibleAnywhere, Category = "Debug")
		UTexture* OutTexture;
};