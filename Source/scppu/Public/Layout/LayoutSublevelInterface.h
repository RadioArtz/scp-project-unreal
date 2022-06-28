

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LayoutSublevelInterface.generated.h"

class ALayout;
class ULayoutCell;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class ULayoutSublevelInterface : public UInterface
{
	GENERATED_BODY()
};

class SCPPU_API ILayoutSublevelInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent)
		void OnLayoutDataReceived(ALayout* Layout, ULayoutCell* Cell, FRandomStream RandomStream);
};
