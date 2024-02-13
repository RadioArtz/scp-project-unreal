#include "Utilities/ExtendedGameUserSettings.h"

UExtendedGameUserSettings* UExtendedGameUserSettings::GetExtendedGameUserSettings()
{
	return Cast<UExtendedGameUserSettings>(UGameUserSettings::GetGameUserSettings());
}

TArray<EUpscalerType> UExtendedGameUserSettings::GetSupportedUpscalers() const
{
	TArray<EUpscalerType> SupportedUpscalers = { EUpscalerType::None };
	const TMap<FName, EUpscalerType> ModuleNamesToUpscalerTypes = {
		{"FSR", EUpscalerType::FSR1},
		{"FSR2", EUpscalerType::FSR2},
		{"DLSS", EUpscalerType::DLSS3}
	};

	for (auto Kvp : ModuleNamesToUpscalerTypes)
	{
		if (FModuleManager::Get().IsModuleLoaded(Kvp.Key))
		{
			SupportedUpscalers.Add(Kvp.Value);
		}
	}

	return SupportedUpscalers;
}

void UExtendedGameUserSettings::SetActiveUpscaler(EUpscalerType NewActiveUpscaler)
{
	if (!this->GetSupportedUpscalers().Contains(NewActiveUpscaler))
	{
		return;
	}

	this->ActiveUpscaler = NewActiveUpscaler;
}

EUpscalerType UExtendedGameUserSettings::GetActiveUpscaler() const
{
	if (this->GetSupportedUpscalers().Contains(this->ActiveUpscaler))
	{
		return this->ActiveUpscaler;
	}

	return EUpscalerType::None;
}

void UExtendedGameUserSettings::SetUpscalerQualityMode(EUpscalerQualityMode NewUpscalerQualityMode)
{
	this->UpscalerQualityMode = NewUpscalerQualityMode;
}

EUpscalerQualityMode UExtendedGameUserSettings::GetUpscalerQualityMode() const
{
	return this->UpscalerQualityMode;
}

void UExtendedGameUserSettings::SetScreenPercentage(int Value)
{
	this->ScreenPercentage = FMath::Clamp(Value, 25, 200);
}

int UExtendedGameUserSettings::GetScreenPercentage() const
{
	return FMath::Clamp(this->ScreenPercentage, 25, 200);
}

void UExtendedGameUserSettings::SetScreenGamma(float Value)
{
	this->ScreenGammaLevel = FMath::Clamp(Value, .5f, 5.f);
}

float UExtendedGameUserSettings::GetScreenGamma() const
{
	return FMath::Clamp(this->ScreenGammaLevel, .5f, 5.f);
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
	this->FOV = FMath::Clamp(Value, 10, 130);
}

int UExtendedGameUserSettings::GetFOV() const
{
	return FMath::Clamp(this->FOV, 10, 130);
}

void UExtendedGameUserSettings::SetTesselation(bool bEnabled)
{
	this->bUseTesselation = bEnabled;
}

bool UExtendedGameUserSettings::GetTesselation() const
{
	return this->bUseTesselation;
}

void UExtendedGameUserSettings::ApplyNonResolutionSettings()
{
	Super::ApplyNonResolutionSettings();
	this->DisableAllUpscalers();
	this->EnableActiveUpscaler(); // Also sets screen percentage if no upscaler is active

	const IConsoleManager& ConsoleManager = IConsoleManager::Get();
	FString ConfigSection = TEXT("SystemSettings");

#if WITH_EDITOR
	if (GIsEditor)
	{
		ConfigSection = TEXT("SystemSettingsEditor");
	}
#endif

	// Update Gamma (Only when not in editor because it affects slate ui as well)
	if (true || GEngine && !GIsEditor)
	{
		GEngine->DisplayGamma = this->GetScreenGamma();
	}

	// Update Texture Streaming CVar
	int TextureStreamingValue = 0;
	if (!GConfig->GetInt(*ConfigSection, TEXT("r.TextureStreaming"), TextureStreamingValue, GEngineIni))
	{
		ConsoleManager.FindConsoleVariable(TEXT("r.TextureStreaming"))->Set(this->IsTextureStreamingEnabled(), ECVF_SetByGameSetting);
	}
}

void UExtendedGameUserSettings::DisableAllUpscalers()
{
	const TArray<EUpscalerType> SupportedUpscalers = this->GetSupportedUpscalers();
	const IConsoleManager& ConsoleManager = IConsoleManager::Get();
	FString ConfigSection = TEXT("SystemSettings");

#if WITH_EDITOR
	if (GIsEditor)
	{
		ConfigSection = TEXT("SystemSettingsEditor");
	}
#endif

	for (EUpscalerType Upscaler : SupportedUpscalers)
	{
		switch (Upscaler)
		{
			case EUpscalerType::None:
				break;

			case EUpscalerType::FSR1:
				ConsoleManager.FindConsoleVariable(TEXT("r.FidelityFX.FSR.Enabled"))->Set(0, EConsoleVariableFlags::ECVF_SetByGameSetting);
				break;

			case EUpscalerType::FSR2:
				ConsoleManager.FindConsoleVariable(TEXT("r.FidelityFX.FSR2.Enabled"))->Set(0, EConsoleVariableFlags::ECVF_SetByGameSetting);
				break;

			case EUpscalerType::DLSS3:
				ConsoleManager.FindConsoleVariable(TEXT("r.NGX.DLSS.Enable"))->Set(0, EConsoleVariableFlags::ECVF_SetByGameSetting);
				break;

			default:
				checkNoEntry();
		}
	}
}

void UExtendedGameUserSettings::EnableActiveUpscaler()
{
	const IConsoleManager& ConsoleManager = IConsoleManager::Get();
	FString ConfigSection = TEXT("SystemSettings");

#if WITH_EDITOR
	if (GIsEditor)
	{
		ConfigSection = TEXT("SystemSettingsEditor");
	}
#endif

	// sanity check
	switch (this->GetActiveUpscaler())
	{
		case EUpscalerType::None:
			ConsoleManager.FindConsoleVariable(TEXT("r.ScreenPercentage"))->Set(this->GetScreenPercentage(), EConsoleVariableFlags::ECVF_SetByGameSetting);
			break;

		case EUpscalerType::FSR1:
		{
			ConsoleManager.FindConsoleVariable(TEXT("r.FidelityFX.FSR.Enabled"))->Set(1, EConsoleVariableFlags::ECVF_SetByGameSetting);

			const TMap<EUpscalerQualityMode, float> UpscalerQualityModeToScreenPercentage = {
				{EUpscalerQualityMode::Quality, .77f},
				{EUpscalerQualityMode::Balanced, .67f},
				{EUpscalerQualityMode::Performance, .59f},
				{EUpscalerQualityMode::UltraPerformance, .5f}
			};

			const float FSRScreenPercentage = UpscalerQualityModeToScreenPercentage[this->GetUpscalerQualityMode()];
			ConsoleManager.FindConsoleVariable(TEXT("r.ScreenPercentage"))->Set(FSRScreenPercentage, EConsoleVariableFlags::ECVF_SetByGameSetting);
			break;
		}

		case EUpscalerType::FSR2:
		{
			ConsoleManager.FindConsoleVariable(TEXT("r.FidelityFX.FSR2.Enabled"))->Set(1, EConsoleVariableFlags::ECVF_SetByGameSetting);

			const int QualityMode = ((int)this->GetUpscalerQualityMode()) + 1;
			ConsoleManager.FindConsoleVariable(TEXT("r.FidelityFX.FSR2.QualityMode"))->Set(QualityMode, EConsoleVariableFlags::ECVF_SetByGameSetting);
			break;
		}

		case EUpscalerType::DLSS3:
		{
			ConsoleManager.FindConsoleVariable(TEXT("r.NGX.DLSS.Enable"))->Set(1, EConsoleVariableFlags::ECVF_SetByGameSetting);
			ConsoleManager.FindConsoleVariable(TEXT("r.NGX.DLSS.Quality.Auto"))->Set(0, EConsoleVariableFlags::ECVF_SetByGameSetting);

			const int QualityMode = (((int)this->GetUpscalerQualityMode()) - 1) * -1;
			ConsoleManager.FindConsoleVariable(TEXT("r.NGX.DLSS.Quality"))->Set(QualityMode, EConsoleVariableFlags::ECVF_SetByGameSetting);
			break;
		}

		default:
			checkNoEntry();
	}
}
