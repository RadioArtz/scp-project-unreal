

#include "scppuEditor.h"
#include "Modules/ModuleManager.h"
#include "ObjectViewer/SObjectViewer.h"
#include "SparseLightmapConversionSampleVolume.h"

IMPLEMENT_GAME_MODULE(FScppuEditorModule, scppuEditor);

void FScppuEditorModule::StartupModule()
{
	if (FSlateApplication::IsInitialized())
	{
		SObjectViewer::RegisterTabSpawner(FGlobalTabmanager::Get());
	}

	UActorFactorySparseLightmapConversionSampleVolume* ActorFactory = NewObject<UActorFactorySparseLightmapConversionSampleVolume>(GetTransientPackage(), UActorFactorySparseLightmapConversionSampleVolume::StaticClass());
	GEditor->ActorFactories.Add(ActorFactory);
}

void FScppuEditorModule::ShutdownModule()
{
	SObjectViewer::UnregisterTabSpawner(FGlobalTabmanager::Get());
}
