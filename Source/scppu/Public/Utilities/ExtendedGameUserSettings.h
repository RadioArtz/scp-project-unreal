

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "ExtendedGameUserSettings.generated.h"

UCLASS()
class SCPPU_API UExtendedGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()
	
	//// Properties ////
protected:
	// If true, FSR 1 will be used
	UPROPERTY(config)
		bool bUseFSR1 = false;

	/**
	 * Quality level for FSR 2
	 *	0 = Disabled
	 *	1 = Quality
	 *	...
	 *	4 = Ultra Performance
	 */
	UPROPERTY(config)
		int32 FSR2QualityLevel = 0;

	// Gamma level to use. Higher value = more brightness
	UPROPERTY(config)
		float ScreenGammaLevel = 2.2f;

	// If true, texture streaming will be used
	UPROPERTY(config)
		bool bUseTextureStreaming = true;

	// If true camera shake triggered by Elevators for example can play.
	UPROPERTY(config)
		bool bUseCameraShake = true;

	// Controls amount of viewbobbing
	UPROPERTY(config)
		float ViewBobStrength = 1.f;

	UPROPERTY(config)
		int FOV = 80;

	//// Functions ////	
public:
	// Returns the game local machine settings (resolution, windowing mode, scalability settings, etc...), but it's extended and has more settings
	UFUNCTION(BlueprintCallable, Category = Settings)
		static UExtendedGameUserSettings* GetExtendedGameUserSettings();

	// Sets the user setting for FSR1
	UFUNCTION(BlueprintCallable, Category = Settings)
		void SetFSR1Enabled(bool bEnable);

	// Returns the user setting for FSR1
	UFUNCTION(BlueprintCallable, Category = Settings)
		bool IsFSR1Enabled() const;

	// Sets the user setting for FSR2 (0..4, higher is stronger)
	// @param Value 0:disabled, 1:quality, 2:balanced, 3:performance, 4:ultra performance (gets clamped if needed)
	UFUNCTION(BlueprintCallable, Category = Settings)
		void SetFSR2Quality(int32 Value);

	// Returns the user setting for FSR2 (0..4, higher is stronger)
	UFUNCTION(BlueprintCallable, Category = Settings)
		int GetFSR2Quality() const;

	// Sets the user setting for screen gamma as a 0.5f..5.0f value
	UFUNCTION(BlueprintCallable, Category = Settings)
		void SetScreenGamma(float Value);

	// Returns the user setting for screen gamma as a 0.5f..5.0f value
	UFUNCTION(BlueprintCallable, Category = Settings)
		float GetScreenGamma() const;

	// Sets the user setting for texture streaming
	UFUNCTION(BlueprintCallable, Category = Settings)
		void SetTextureStreamingEnabled(bool bEnabled);

	// Returns the user setting for texture streaming
	UFUNCTION(BlueprintCallable, Category = Settings)
		bool IsTextureStreamingEnabled() const;

	// Sets if Camera shake such as from Elevators is enabled.
	UFUNCTION(BlueprintCallable, Category = "Settings | Gameplay")
		void SetCameraShakeEnabled(bool bEnabled);

	// Returns if Camera shake such as from Elevators is enabled.
	UFUNCTION(BlueprintCallable, Category = "Settings | Gameplay")
		bool IsCameraShakeEnabled() const;

	// Sets the user setting for how strong viewbob can be.
	UFUNCTION(BlueprintCallable, Category = "Settings | Gameplay")
		void SetViewbobStrength(float Value);

	// Returns the user setting for how strong viewbob can be.
	UFUNCTION(BlueprintCallable, Category = "Settings | Gameplay")
		float GetViewbobStrength() const;

	UFUNCTION(BlueprintCallable, Category = "Settings | Gameplay")
		int GetFOV() const;

	UFUNCTION(BlueprintCallable, Category = "Settings | Gameplay")
		void SetFOV(int Value);

	virtual void ApplyNonResolutionSettings() override;
};
