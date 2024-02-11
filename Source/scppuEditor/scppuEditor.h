

#pragma once

#include "CoreMinimal.h"

class FScppuEditorModule : public IModuleInterface
{
public:
	void StartupModule() override;
	void ShutdownModule() override;
};
