

#pragma once 

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class FGlobalTabmanager;

class SCPPUEDITOR_API SObjectViewer : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SObjectViewer)
	{

	}
	SLATE_END_ARGS()

	//// Functions ////	
public:
	static void RegisterTabSpawner(const TSharedRef<FGlobalTabmanager>& InTabManager);
	static void UnregisterTabSpawner(const TSharedRef<FGlobalTabmanager>& InTabManager);
	static TSharedRef<SDockTab> CreateObjectViewerTab(const FSpawnTabArgs& Args);
	void Construct(const FArguments& InArgs);
	void Refresh(bool bForce = false);
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime);

private:
	TSharedRef<ITableRow> OnGenerateRowCallback(TSharedPtr<FWeakObjectPtr> InRowWeakObjectPtr, const TSharedRef<STableViewBase>& OwnerTable);
	void OnFilterTextCommittedCallback(const FText& Text, ETextCommit::Type);
	void OnClassFilterSetCallback(const UClass* Class);
	const UClass* GetClassFilter() const;
	void OnHideTemplatesStateChangedCallback(ECheckBoxState CheckboxState);
	void OnHideScriptClassesStateChangedCallback(ECheckBoxState CheckboxState);
	void OnHideEngineClassesStateChangedCallback(ECheckBoxState CheckboxState);
	void OnOnlyShowPIEStateChangedCallback(ECheckBoxState CheckboxState);
	void OnAutoRefreshStateChangedCallback(ECheckBoxState CheckboxState);
	FReply OnRefreshClickedCallback();

	//// Properties ////
public:
	static const FName TabId;
private:
	TSharedPtr<SHeaderRow> HeaderRow;
	TSharedPtr<STableViewBase> ListView;
	TSharedPtr<STextBlock> InfoTextBlock;
	TArray<TSharedPtr<FWeakObjectPtr>> VisibleObjects;
	FString NameFilter = "";
	bool bHideTemplates = true;
	bool bHideScriptClasses = true;
	bool bHideEngineClasses = true;
	bool bOnlyShowPIE = false;
	bool bAutoRefresh = true;
	float TimeSinceLastAutoRefresh = 0.f;
	const UClass* ClassFilter = UObject::StaticClass();
};

