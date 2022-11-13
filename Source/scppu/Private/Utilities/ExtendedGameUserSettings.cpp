


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

void UExtendedGameUserSettings::SetFSR2Quality(int Value)
{
	this->FSR2QualityLevel = FMath::Clamp(Value, 0, 4);
}

int UExtendedGameUserSettings::GetFSR2Quality() const
{
	return this->FSR2QualityLevel;
}

void UExtendedGameUserSettings::SetScreenGamma(float Value)
{
	this->ScreenGammaLevel = Value;
}

float UExtendedGameUserSettings::GetScreenGamma() const
{
	return this->ScreenGammaLevel;
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

	// Update Gamma
	if (GEngine != nullptr)
	{
		if (!GEngine->IsEditor())
		{
			GEngine->Exec(nullptr, *FString::Printf(TEXT("gamma %f"), this->GetScreenGamma()));
		}
	}
}
