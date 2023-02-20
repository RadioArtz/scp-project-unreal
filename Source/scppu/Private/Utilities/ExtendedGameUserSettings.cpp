


#include "Utilities/ExtendedGameUserSettings.h"

UExtendedGameUserSettings* UExtendedGameUserSettings::GetExtendedGameUserSettings()
{
	return Cast<UExtendedGameUserSettings>(UGameUserSettings::GetGameUserSettings());
}

void UExtendedGameUserSettings::SetFSR1Enabled(bool bEnable)
{
	this->bUseFSR1 = bEnable;
}

bool UExtendedGameUserSettings::IsFSR1Enabled() const
{
	return this->bUseFSR1;
}

void UExtendedGameUserSettings::SetFSR2Quality(int32 Value)
{
	this->FSR2QualityLevel = FMath::Clamp(Value, 0, 4);
}

int UExtendedGameUserSettings::GetFSR2Quality() const
{
	return this->FSR2QualityLevel;
}

void UExtendedGameUserSettings::SetScreenGamma(float Value)
{
	this->ScreenGammaLevel = FMath::Clamp(Value, 0.5f, 5.0f);
}

float UExtendedGameUserSettings::GetScreenGamma() const
{
	return this->ScreenGammaLevel;
}

void UExtendedGameUserSettings::SetTextureStreamingEnabled(bool bEnabled)
{
	this->bUseTextureStreaming = bEnabled;
}

bool UExtendedGameUserSettings::IsTextureStreamingEnabled() const
{
	return this->bUseTextureStreaming;
}

void UExtendedGameUserSettings::SetCameraShakeEnabled(bool bEnabled)
{
	this->bUseCameraShake = bEnabled;
}

bool UExtendedGameUserSettings::IsCameraShakeEnabled() const
{
	return this->bUseCameraShake;
}

void UExtendedGameUserSettings::SetViewbobStrength(float Value)
{
	this->ViewBobStrength = Value;
}

float UExtendedGameUserSettings::GetViewbobStrength() const
{
	return this->ViewBobStrength;
}

void UExtendedGameUserSettings::SetFOV(int Value)
{
	this->FOV = Value;
}

int UExtendedGameUserSettings::GetFOV() const
{
	return this->FOV;
}

void UExtendedGameUserSettings::ApplyNonResolutionSettings()
{
	Super::ApplyNonResolutionSettings();

	// Update FSR1 CVar
	{
		FString ConfigSection = TEXT("SystemSettings");
#if WITH_EDITOR
		if (GIsEditor)
		{
			ConfigSection = TEXT("SystemSettingsEditor");
		}
#endif
		int FSR1Value = 0;
		if (GConfig->GetInt(*ConfigSection, TEXT("r.FidelityFX.FSR.Enabled"), FSR1Value, GEngineIni))
		{
			// FSR1 was already set by system settings. We are capable of setting it here.
		}
		else
		{
			static auto CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.FidelityFX.FSR.Enabled"));
			CVar->Set(this->IsFSR1Enabled(), ECVF_SetByGameSetting);
		}
	}

// FSR2 only supports windows
#if PLATFORM_WINDOWS 
	// Update FSR2 CVar
	{
		FString ConfigSection = TEXT("SystemSettings");
#if WITH_EDITOR
		if (GIsEditor)
		{
			ConfigSection = TEXT("SystemSettingsEditor");
		}
#endif
		int FSR2EnabledValue = 0;
		int FSR2QualityValue = 0;
		if (GConfig->GetInt(*ConfigSection, TEXT("r.FidelityFX.FSR2.Enabled"), FSR2EnabledValue, GEngineIni) || GConfig->GetInt(*ConfigSection, TEXT("r.FidelityFX.FSR2.QualityMode"), FSR2QualityValue, GEngineIni))
		{
			// FSR2 was already set by system settings. We are capable of setting it here.
		}
		else
		{
			{
				static auto CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.FidelityFX.FSR2.Enabled"));
				CVar->Set(this->FSR2QualityLevel != 0, ECVF_SetByGameSetting);
			}
			{
				static auto CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.FidelityFX.FSR2.QualityMode"));
				CVar->Set(this->FSR2QualityLevel, ECVF_SetByGameSetting);
			}
		}
	}
#endif

	// Update Gamma
	if (GEngine == nullptr || GIsEditor)
	{
		// Do not set gamma in PIE because the whole editor will be affected.
	}
	else
	{
		GEngine->Exec(nullptr, *FString::Printf(TEXT("gamma %f"), this->GetScreenGamma()));
	}

	// Update Texture Streaming CVar
	{
		FString ConfigSection = TEXT("SystemSettings");
#if WITH_EDITOR
		if (GIsEditor)
		{
			ConfigSection = TEXT("SystemSettingsEditor");
		}
#endif
		int TextureStreamingValue = 0;
		if (GConfig->GetInt(*ConfigSection, TEXT("r.TextureStreaming"), TextureStreamingValue, GEngineIni))
		{
			// Texture Streaming was already set by system settings. We are capable of setting it here.
		}
		else
		{
			static auto CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.TextureStreaming"));
			CVar->Set(this->IsTextureStreamingEnabled(), ECVF_SetByGameSetting);
		}
	}
}
