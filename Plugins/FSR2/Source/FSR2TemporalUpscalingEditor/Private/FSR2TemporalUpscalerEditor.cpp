// This file is part of the FidelityFX Super Resolution 2.1 Unreal Engine Plugin.
//
// Copyright (c) 2022 Advanced Micro Devices, Inc. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
#pragma once

#include "FSR2TemporalUpscalerEditor.h"
#include "FSR2TemporalUpscaling.h"
#include "LegacyScreenPercentageDriver.h"

#include "Editor.h"
#include "SLevelViewportToolBar.h"
#include "ToolMenus.h"

#define LOCTEXT_NAMESPACE "FSR2TemporalUpscalerEditor"

class FFSR2EditorViewportClientData final : public FEditorViewportClient::ICustomTemporalUpscalerData
{
public:
	bool bIsFSR2Enabled;
	uint32 FSR2Quality;
};

FFSR2TemporalUpscalerEditor::FFSR2TemporalUpscalerEditor(IFSR2TemporalUpscalingModule* Module)
: TemporalUpscalingModule(Module)
{

}

static TSharedPtr<FFSR2EditorViewportClientData> GetViewportData(FEditorViewportClient* ViewportClient)
{
	static const auto CVarFSR2Enabled = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.FidelityFX.FSR2.Enabled"));
	static const auto CVarFSR2EnabledInEditor = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.FidelityFX.FSR2.EnabledInEditorViewport"));
	static const auto CVarFSR2Quality = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.FidelityFX.FSR2.QualityMode"));

	TSharedPtr<FEditorViewportClient::ICustomTemporalUpscalerData> CustomData = ViewportClient->GetCustomTemporalUpscalerData();
	TSharedPtr<FFSR2EditorViewportClientData> FSR2ViewportData;

	if (CustomData)
	{
		FSR2ViewportData = StaticCastSharedPtr<FFSR2EditorViewportClientData>(CustomData);
		check(FSR2ViewportData);
	}
	else
	{
		FSR2ViewportData = MakeShared<FFSR2EditorViewportClientData>();
		FSR2ViewportData->bIsFSR2Enabled = (CVarFSR2Enabled && CVarFSR2Enabled->GetValueOnAnyThread() && CVarFSR2EnabledInEditor && CVarFSR2EnabledInEditor->GetValueOnAnyThread());
		FSR2ViewportData->FSR2Quality = CVarFSR2Quality->GetValueOnAnyThread();
		ViewportClient->SetCustomTemporalUpscalerData(FSR2ViewportData);
	}

	return FSR2ViewportData;
}

void FFSR2TemporalUpscalerEditor::SetupEditorViewFamily(FSceneViewFamily& ViewFamily, FEditorViewportClient* ViewportClient)
{
	static const auto CVarFSR2Enabled = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.FidelityFX.FSR2.Enabled"));
	static const auto CVarFSR2EnabledInEditor = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.FidelityFX.FSR2.EnabledInEditorViewport"));
	static const auto CVarFSR2Quality = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.FidelityFX.FSR2.QualityMode"));
	
	ITemporalUpscaler const* Upscaler = TemporalUpscalingModule->GetTemporalUpscaler();
	bool bEnabledInEditor = false;
	if (CVarFSR2Enabled && CVarFSR2Enabled->GetValueOnAnyThread() && CVarFSR2Quality && Upscaler && ViewFamily.GetTemporalUpscalerInterface() == nullptr)
	{
		TSharedPtr<FFSR2EditorViewportClientData> FSR2ViewportData = GetViewportData(ViewportClient);

		if (FSR2ViewportData->bIsFSR2Enabled)
		{
			bEnabledInEditor = true;
			ViewFamily.EngineShowFlags.ScreenPercentage = (CVarFSR2Enabled && CVarFSR2Enabled->GetValueOnAnyThread());
			if (ViewFamily.EngineShowFlags.ScreenPercentage)
			{
				float ResFraction = TemporalUpscalingModule->GetResolutionFraction(FSR2ViewportData->FSR2Quality);
				ViewFamily.SetScreenPercentageInterface(new FLegacyScreenPercentageDriver(ViewFamily, ResFraction, false));
			}
			ViewFamily.SetTemporalUpscalerInterface(Upscaler);
		}
	}
	if (Upscaler)
	{
		TemporalUpscalingModule->SetEnabledInEditor(bEnabledInEditor);
	}
}

static bool IsQualityModeSelected(const ICustomEditorStaticScreenPercentage::FViewportMenuEntryArguments Arguments, uint32 Mode)
{
	TSharedPtr<FFSR2EditorViewportClientData> FSR2ViewportData = GetViewportData(&Arguments.Viewport->GetLevelViewportClient());
	return (FSR2ViewportData->FSR2Quality == Mode);
}

static void SetQualityModeSelected(const ICustomEditorStaticScreenPercentage::FViewportMenuEntryArguments Arguments, uint32 Mode)
{
	TSharedPtr<FFSR2EditorViewportClientData> FSR2ViewportData = GetViewportData(&Arguments.Viewport->GetLevelViewportClient());
	FSR2ViewportData->FSR2Quality = Mode;
}

static void MakeFSR2SettingsMenu(UToolMenu* Menu, FFSR2TemporalUpscalerEditor* UpscalerEditor, const ICustomEditorStaticScreenPercentage::FViewportMenuEntryArguments Arguments)
{
	FToolMenuSection& Section = Menu->AddSection("Section");
	Section.AddMenuEntry(
		"FSR2Enabled",
		LOCTEXT("FSR2EnabledOption", "Enabled"),
		LOCTEXT("FSR2EnabledToolTip", "Use FidelityFX Super Resolution 2.1 to upscale."),
		FSlateIcon(),
		FUIAction(
			FExecuteAction::CreateLambda([Arguments]() {
				TSharedPtr<FFSR2EditorViewportClientData> FSR2ViewportData = GetViewportData(&Arguments.Viewport->GetLevelViewportClient());
				FSR2ViewportData->bIsFSR2Enabled = !FSR2ViewportData->bIsFSR2Enabled;
				}),
			FCanExecuteAction(),
			FIsActionChecked::CreateLambda([Arguments](){
				TSharedPtr<FFSR2EditorViewportClientData> FSR2ViewportData = GetViewportData(&Arguments.Viewport->GetLevelViewportClient());
				return FSR2ViewportData->bIsFSR2Enabled;
				})
		),
		EUserInterfaceActionType::ToggleButton
	);

	Section.AddSeparator("Quality");

	Section.AddMenuEntry(
		"QualityMode",
		LOCTEXT("QualityModeOption", "Quality"),
		LOCTEXT("QualityModeToolTip", "Maximize quality of upscaled image."),
		FSlateIcon(),
		FUIAction(
			FExecuteAction::CreateStatic(&SetQualityModeSelected, Arguments, 1u),
			FCanExecuteAction(),
			FIsActionChecked::CreateStatic(&IsQualityModeSelected, Arguments, 1u)
		),
		EUserInterfaceActionType::RadioButton
	);

	Section.AddMenuEntry(
		"BalancedMode",
		LOCTEXT("BalancedModeOption", "Balanced"),
		LOCTEXT("BalancedModeOptionToolTip", "Balance upscaling quality versus performance."),
		FSlateIcon(),
		FUIAction(
			FExecuteAction::CreateStatic(&SetQualityModeSelected, Arguments, 2u),
			FCanExecuteAction(),
			FIsActionChecked::CreateStatic(&IsQualityModeSelected, Arguments, 2u)
		),
		EUserInterfaceActionType::RadioButton
	);

	Section.AddMenuEntry(
		"PerformanceMode",
		LOCTEXT("PerformanceModeOption", "Performance"),
		LOCTEXT("PerformanceModeOptionToolTip", "Prioritise performance over quality of upscaled image."),
		FSlateIcon(),
		FUIAction(
			FExecuteAction::CreateStatic(&SetQualityModeSelected, Arguments, 3u),
			FCanExecuteAction(),
			FIsActionChecked::CreateStatic(&IsQualityModeSelected, Arguments, 3u)
		),
		EUserInterfaceActionType::RadioButton
	);

	Section.AddMenuEntry(
		"UltraPerformanceMode",
		LOCTEXT("UltraPerformanceModeOption", "Ultra Performance"),
		LOCTEXT("UltraPerformanceModeOptionToolTip", "Maximize performance of upscaling over quality."),
		FSlateIcon(),
		FUIAction(
			FExecuteAction::CreateStatic(&SetQualityModeSelected, Arguments, 4u),
			FCanExecuteAction(),
			FIsActionChecked::CreateStatic(&IsQualityModeSelected, Arguments, 4u)
		),
		EUserInterfaceActionType::RadioButton
	);
}

bool FFSR2TemporalUpscalerEditor::GenerateEditorViewportOptionsMenuEntry(const ICustomEditorStaticScreenPercentage::FViewportMenuEntryArguments& Arguments)
{
	static const auto CVarFSR2Enabled = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.FidelityFX.FSR2.Enabled"));
	FLevelEditorViewportClient& Viewport = Arguments.Viewport->GetLevelViewportClient();

	ITemporalUpscaler* Upscaler = TemporalUpscalingModule->GetTemporalUpscaler();
	if (CVarFSR2Enabled && CVarFSR2Enabled->GetValueOnAnyThread() && Upscaler && Viewport.EngineShowFlags.Lighting && Viewport.IsPerspective())
	{
		Arguments.Section->AddSubMenu(
			"FSR2Settings",
			LOCTEXT("FSR2SettingsSubMenu", "FidelityFX Super Resolution 2.1"),
			LOCTEXT("FSR2SettingsSubMenu_ToolTip", "Settings for FidelityFX Super Resolution 2.1 in the Editor viewport."),
			FNewToolMenuDelegate::CreateStatic(&MakeFSR2SettingsMenu, this, Arguments));
	}
	return false;
}

#undef LOCTEXT_NAMESPACE