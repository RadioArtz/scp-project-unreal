

#pragma once

#include "CoreMinimal.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundSubmix.h"
#include "SubmixEffects/AudioMixerSubmixEffectReverb.h"
#include "ExtendedAudioComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogExtendedAudio, Log, All);

UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class SCPPU_API UExtendedAudioComponent : public UAudioComponent
{
	GENERATED_BODY()
	
		//// Properties ////
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = DynamicReverb)
		bool bDynamicReverb = true;

	FTimerHandle DynamicReverbUpdateTimer;
	FSubmixEffectReverbSettings SubmixReverbEffectSettings;

	UPROPERTY()
		USubmixEffectReverbPreset* SubmixEffectReverbPreset;

	UPROPERTY()
		USoundSubmix* ReverbSubmix;

	FTimerHandle FSoftStopTimer;
	float BeforeSoftStopVolumeMultiplier;

		//// Functions ////	
public:
	UFUNCTION(BlueprintCallable, Category = "Audio|Components|Audio")
		virtual void SoftStop();

	virtual void Play(float StartTime = 0.0f) override;
	virtual void Stop() override;

protected:
	virtual void UpdateDynamicReverb();
	virtual void SoftStopCleanup();
};

UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "Extended Audio Settings"))
class SCPPU_API UExtendedAudioSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = DynamicReverb)
		bool bUseDynamicReverb = true;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = DynamicReverb, meta = (EditCondition = bUseDynamicReverb))
		bool bDisableDynamicReverbInReverbVolume = true;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = DynamicReverb, meta = (ClampMin = 10, ClampMax = 2000, EditCondition = bUseDynamicReverb))
		int NumSamplingTraces = 250;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = DynamicReverb, meta = (ClampMin = 1.f, ClampMax = 100000.f, EditCondition = bUseDynamicReverb))
		float MaxTraceDistance = 10000.f;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = DynamicReverb, meta = (DisplayName = "Speed of Sound (m/s)", ClampMin = 1.f, ClampMax = 800.f, EditCondition = bUseDynamicReverb))
		float SpeedOfSound = 343.f;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = DynamicReverb, meta = (ClampMin = 0.25f, ClampMax = 4.f, EditCondition = bUseDynamicReverb))
		float DistanceWeightExponent = 1.5f;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = DynamicReverb, meta = (DisplayName = "Update Interval (secs)", ClampMin = 0.01f, UIMax = 3.f, EditCondition = bUseDynamicReverb))
		float UpdateInterval = .5f;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = DynamicReverb, meta = (ClampMin = 0.f, ClampMax = 1.f, EditCondition = bUseDynamicReverb))
		float ReverbWetLevel = 0.5f;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = DynamicReverb, meta = (ClampMin = 0.f, UIMax = 500.f, EditCondition = bUseDynamicReverb))
		float ReverbDryLevelFalloffDistanceStart = 250.f;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = DynamicReverb, meta = (ClampMin = 500.f, UIMax = 8000.f, EditCondition = bUseDynamicReverb))
		float ReverbDryLevelFalloffDistanceEnd = 4000.f;
};
