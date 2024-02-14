

#include "scppuEditor.h"
#include "Modules/ModuleManager.h"
#include "ObjectViewer/SObjectViewer.h"

IMPLEMENT_GAME_MODULE(FScppuEditorModule, scppuEditor);

void FScppuEditorModule::StartupModule()
{
	if (FSlateApplication::IsInitialized())
	{
		SObjectViewer::RegisterTabSpawner(FGlobalTabmanager::Get());
	}
}

void FScppuEditorModule::ShutdownModule()
{
	SObjectViewer::UnregisterTabSpawner(FGlobalTabmanager::Get());
}
