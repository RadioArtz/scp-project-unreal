


#include "Audio/ExtendedAudioComponent.h"
#include "Utilities/ExtendedPhysicalMaterial.h"
#include "AudioDevice.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

DEFINE_LOG_CATEGORY(LogExtendedAudio);

void UExtendedAudioComponent::Play(float StartTime)
{
	if (this->GetWorld()->GetTimerManager().IsTimerActive(this->FSoftStopTimer))
	{
		UE_LOG(LogExtendedAudio, Verbose, TEXT("%s: Playing requested before cleanup was initiated. Cleaning up now..."), *this->GetName());
		this->GetWorld()->GetTimerManager().ClearTimer(this->FSoftStopTimer);
		this->SoftStopCleanup();
	}

	Super::Play(StartTime);
	const UExtendedAudioSettings* AudioSettings = GetDefault<UExtendedAudioSettings>();
	this->GetWorld()->GetTimerManager().ClearTimer(this->DynamicReverbUpdateTimer);

	if (AudioSettings->bUseDynamicReverb && this->bDynamicReverb)
	{
		FTimerDelegate Delegate;
		Delegate.BindUObject(this, &UExtendedAudioComponent::UpdateDynamicReverb);
		this->GetWorld()->GetTimerManager().SetTimer(DynamicReverbUpdateTimer, Delegate, AudioSettings->UpdateInterval + FMath::RandRange(.0f, .01f), true, FMath::RandRange(.0f, .1f));
		this->SubmixReverbEffectSettings.bBypassEarlyReflections = true;
		this->SubmixReverbEffectSettings.Gain = 1.0f;

		if (!IsValid(this->SubmixEffectReverbPreset))
		{
			this->SubmixEffectReverbPreset = NewObject<USubmixEffectReverbPreset>(this);
			this->SubmixEffectReverbPreset->SetSettings(this->SubmixReverbEffectSettings);
		}

		if (!IsValid(this->ReverbSubmix))
		{
			this->ReverbSubmix = NewObject<USoundSubmix>(this);
		}

		this->ReverbSubmix->SubmixEffectChain.AddUnique(this->SubmixEffectReverbPreset);
		this->GetAudioDevice()->RegisterSoundSubmix(this->ReverbSubmix, true);
	}
}

void UExtendedAudioComponent::Stop()
{
	Super::Stop();
	this->GetWorld()->GetTimerManager().ClearTimer(this->DynamicReverbUpdateTimer);

	if (IsValid(this->ReverbSubmix))
	{
		this->GetAudioDevice()->UnregisterSoundSubmix(this->ReverbSubmix);
		this->ReverbSubmix->RemoveFromRoot();
	}
}

void UExtendedAudioComponent::SoftStop()
{
	this->BeforeSoftStopVolumeMultiplier = this->VolumeMultiplier;
	this->SetVolumeMultiplier(0.0005f);

	FTimerDelegate Delegate;
	Delegate.BindUObject(this, &UExtendedAudioComponent::SoftStopCleanup);
	this->GetWorld()->GetTimerManager().SetTimer(this->FSoftStopTimer, Delegate, this->SubmixReverbEffectSettings.DecayTime, false);
	UE_LOG(LogExtendedAudio, Verbose, TEXT("%s: Soft stop requested. Cleaning up in %f seconds..."), *this->GetName(), this->SubmixReverbEffectSettings.DecayTime);
}

void UExtendedAudioComponent::SoftStopCleanup()
{
	this->Stop();
	this->SetVolumeMultiplier(this->BeforeSoftStopVolumeMultiplier);
	UE_LOG(LogExtendedAudio, Verbose, TEXT("%s: Successfully cleaned up."), *this->GetName());
}

void UExtendedAudioComponent::UpdateDynamicReverb()
{
	const UExtendedAudioSettings* AudioSettings = GetDefault<UExtendedAudioSettings>();

	// Don't calucalte anything if the audio is not actually played right now (e.g. too far away)
	if (!this->IsPlaying())
	{
		return;
	}

	// Don't apply dynamic reverb if we are in a volume with applied static reverb
	if (AudioSettings->bDisableDynamicReverbInReverbVolume)
	{
		TArray<AActor*> AudioVolumes;
		UGameplayStatics::GetAllActorsOfClass(this->GetWorld(), AAudioVolume::StaticClass(), AudioVolumes);
		for (int i = 0; i < AudioVolumes.Num(); i++)
		{
			AAudioVolume* AudioVolume = (AAudioVolume*)AudioVolumes[i];
			if (AudioVolume->EncompassesPoint(this->GetComponentLocation()) && AudioVolume->GetEnabled() && AudioVolume->GetReverbSettings().bApplyReverb)
			{
				this->SetSubmixSend(this->ReverbSubmix, 0.0f);
				UE_LOG(LogExtendedAudio, Verbose, TEXT("%s: In a reverb volume. Not applying dynamic reverb."), *this->GetName());
				return;
			}
		}
	}

	float AvgDistanceMetersWeighted = .0f; // in m
	float AvgHighFrequencyAbsorptionCoefficient = .0f;
	float AvgLowFrequencyAbsorptionCoefficient = .0f;

	const float PHI = PI * (3.f - FMath::Sqrt(5.f)); // Golden Ratio in rad
	int BlockingHits = 0;
	for (int i = 0; i < AudioSettings->NumSamplingTraces; i++)
	{
		FVector SphereUnitDirection;

		// Distribute unit points on a sphere (based on fibonacci sphere algorithm)
		{
			float y = 1.f - (i / (AudioSettings->NumSamplingTraces - 1.f)) * 2.f; // Y >= -1, Y <= 1
			float Radius = FMath::Sqrt(1.f - FMath::Square(y)); // Radius at y
			float x = FMath::Cos(PHI * i) * Radius;
			float z = FMath::Sin(PHI * i) * Radius;
			SphereUnitDirection = FVector(x, y, z);
		}

		// Trace surrounings based on said unit points
		{
			FHitResult Hit;
			FCollisionQueryParams CollisionParams;
			CollisionParams.TraceTag = FName("DynamicReverbSystem");
			CollisionParams.bTraceComplex = true;
			FVector TraceEnd = this->GetComponentLocation() + (SphereUnitDirection * AudioSettings->MaxTraceDistance);
			this->GetWorld()->LineTraceSingleByChannel(Hit, this->GetComponentLocation(), TraceEnd, ECollisionChannel::ECC_GameTraceChannel1, CollisionParams);
			if (Hit.IsValidBlockingHit())
			{
				const UExtendedPhysicalMaterial* ExtendedPhysMat;
				float DistanceMeters = Hit.Distance / 100.f;

				if (IsValid(Hit.PhysMaterial.Get()))
				{
					ExtendedPhysMat = UExtendedPhysicalMaterial::ToExtendedPhysicalMaterial(Hit.PhysMaterial.Get());
				}
				else
				{
					ExtendedPhysMat = GetDefault<UExtendedPhysicalMaterial>();
				}

				AvgDistanceMetersWeighted += FMath::Pow(DistanceMeters, AudioSettings->DistanceWeightExponent);
				AvgHighFrequencyAbsorptionCoefficient += ExtendedPhysMat->HighFrequencyAbsorptionCoefficient;
				AvgLowFrequencyAbsorptionCoefficient += ExtendedPhysMat->LowFrequencyAbsorptionCoefficient;
				BlockingHits++;
			}
		}
	}

	if (BlockingHits > 0)
	{
		AvgDistanceMetersWeighted = FMath::Pow(AvgDistanceMetersWeighted / BlockingHits, 1.f / AudioSettings->DistanceWeightExponent);
		AvgHighFrequencyAbsorptionCoefficient = AvgHighFrequencyAbsorptionCoefficient / BlockingHits;
		AvgLowFrequencyAbsorptionCoefficient = AvgLowFrequencyAbsorptionCoefficient / BlockingHits;
	}
	else
	{
		AvgDistanceMetersWeighted = .0f;
		AvgHighFrequencyAbsorptionCoefficient = .0f;
		AvgLowFrequencyAbsorptionCoefficient = .0f;
	}

	// Estimate room volume by calulating a sphere volume based on average distance
	float EstRoomVolume = (4.f / 3.f) * PI * FMath::Pow(AvgDistanceMetersWeighted, 3.f); // in m3

	// Estimate surface per hit by calulating sphere surface based on average distance
	// Magic number to compensate for inaccurate estimation of surface per hit
	float MagicNumber = 1.3f;
	float EstSurfacePerHit = 4.f * PI * FMath::Square(AvgDistanceMetersWeighted) * MagicNumber;; // in m2

	float HitRatio = (float)BlockingHits / (float)AudioSettings->NumSamplingTraces;
	float AvgHighFrequencyAbsorptionSurface = AvgHighFrequencyAbsorptionCoefficient * EstSurfacePerHit;
	float AvgLowFrequencyAbsorptionSurface = AvgLowFrequencyAbsorptionCoefficient * EstSurfacePerHit;

	// Calculate reverb based on sabines formular
	float ReverbDelay = (AvgDistanceMetersWeighted * 2.f) / AudioSettings->SpeedOfSound;
	float HighFrequencyDecayTime = (.163f * EstRoomVolume / AvgHighFrequencyAbsorptionSurface);
	float LowFrequencyDecayTime = (.163f * EstRoomVolume / AvgLowFrequencyAbsorptionSurface);
	float ReverbGain = HitRatio * AudioSettings->ReverbWetLevel;

	// Calulate dry level (maybe move this into a more frequent timer function)
	float DistanceToPlayer = FVector::Distance(this->GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetCameraLocation(), this->GetComponentLocation());
	float DryGain = FMath::GetMappedRangeValueClamped(FVector2D(AudioSettings->ReverbDryLevelFalloffDistanceStart, AudioSettings->ReverbDryLevelFalloffDistanceEnd), FVector2D(1.f, .0f), DistanceToPlayer);
	DryGain = FMath::Pow(DryGain, 3); 

	// Log it all
	UE_LOG(LogExtendedAudio, Verbose, TEXT("%s: --- Reverb calculation:"), * this->GetName());
	UE_LOG(LogExtendedAudio, Verbose, TEXT("%s: Hit ratio: %f"), *this->GetName(), HitRatio)
	UE_LOG(LogExtendedAudio, Verbose, TEXT("%s: Avg. distance weighted (m): %f"), *this->GetName(), AvgDistanceMetersWeighted)
	UE_LOG(LogExtendedAudio, Verbose, TEXT("%s: Avg. high frequency absorption: %f"), *this->GetName(), AvgHighFrequencyAbsorptionSurface);
	UE_LOG(LogExtendedAudio, Verbose, TEXT("%s: Avg. low frequency absorption: %f"), *this->GetName(), AvgLowFrequencyAbsorptionSurface);
	UE_LOG(LogExtendedAudio, Verbose, TEXT("%s: Est. room volume (m3): %f"), *this->GetName(), EstRoomVolume);
	UE_LOG(LogExtendedAudio, Verbose, TEXT("%s: Est. surface per hit (m2): %f"), *this->GetName(), EstSurfacePerHit)
	UE_LOG(LogExtendedAudio, Verbose, TEXT("%s: Reverb gain: %f"), *this->GetName(), ReverbGain);
	UE_LOG(LogExtendedAudio, Verbose, TEXT("%s: Reverb delay (s): %f"), *this->GetName(), ReverbDelay);
	UE_LOG(LogExtendedAudio, Verbose, TEXT("%s: High frequency decay time (s): %f"), *this->GetName(), HighFrequencyDecayTime);
	UE_LOG(LogExtendedAudio, Verbose, TEXT("%s: Low frequency decay time (s): %f"), *this->GetName(), LowFrequencyDecayTime);
	UE_LOG(LogExtendedAudio, Verbose, TEXT("%s: Dry gain: %f"), *this->GetName(), DryGain);
	UE_LOG(LogExtendedAudio, Verbose, TEXT("%s: ---"), *this->GetName());

	// Apply calculated values
	this->SubmixReverbEffectSettings.DecayHFRatio = HighFrequencyDecayTime / LowFrequencyDecayTime;
	this->SubmixReverbEffectSettings.DecayTime = LowFrequencyDecayTime;
	this->SubmixReverbEffectSettings.LateDelay = ReverbDelay;
	this->SubmixEffectReverbPreset->SetSettings(this->SubmixReverbEffectSettings);
	this->GetAudioDevice()->SetSubmixWetLevel(this->ReverbSubmix, ReverbGain);
	this->GetAudioDevice()->SetSubmixDryLevel(this->ReverbSubmix, DryGain);
	this->SetSubmixSend(this->ReverbSubmix, 1.f);
}




