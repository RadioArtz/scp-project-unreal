#include "ObjectViewer/SObjectViewer.h"
#include "ObjectViewer/SObjectListViewRow.h"
#include "SlateOptMacros.h"
#include "EngineUtils.h"
#include "PropertyCustomizationHelpers.h"
#include "Editor/WorkspaceMenuStructure/Public/WorkspaceMenuStructure.h"
#include "Editor/WorkspaceMenuStructure/Public/WorkspaceMenuStructureModule.h"
#include "Widgets/Layout/SSeparator.h" 

const FName SObjectViewer::TabId = "ObjectViewer";

void SObjectViewer::RegisterTabSpawner(const TSharedRef<FGlobalTabmanager>& InTabManager)
{
	InTabManager->RegisterNomadTabSpawner(SObjectViewer::TabId, FOnSpawnTab::CreateStatic(&SObjectViewer::CreateObjectViewerTab))
		.SetDisplayName(FText::FromString("Object Viewer"))
		.SetTooltipText(FText::FromString("Displays all exisitng UObject instances and allows you to open the asset editor for them."))
		.SetGroup(WorkspaceMenu::GetMenuStructure().GetDeveloperToolsMiscCategory())
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "GenericEditor.Tabs.Properties"));
}

void SObjectViewer::UnregisterTabSpawner(const TSharedRef<FGlobalTabmanager>& InTabManager)
{
	InTabManager->UnregisterNomadTabSpawner(SObjectViewer::TabId);
}

TSharedRef<SDockTab> SObjectViewer::CreateObjectViewerTab(const FSpawnTabArgs& Args)
{
	TSharedRef<SDockTab> Tab = SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		.Label(FText::FromString("Object Viewer"))
		[
			SNew(SObjectViewer)
		];

	return Tab;
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SObjectViewer::Construct(const FArguments& InArgs)
{
	TSharedPtr<SScrollBar> VerticalScrollBar = SNew(SScrollBar)
		.Orientation(EOrientation::Orient_Vertical)
		.Thickness(FVector2D(12.0f, 12.0f));

	this->HeaderRow = SNew(SHeaderRow);
	this->HeaderRow->AddColumn(SHeaderRow::Column("Name")
		[
			SNew(SBox)
			.VAlign(EVerticalAlignment::VAlign_Fill)
			.HAlign(EHorizontalAlignment::HAlign_Fill)
			[
				SNew(STextBlock)
				.Text(FText::FromString("Name"))
			]
		]);

	this->HeaderRow->AddColumn(SHeaderRow::Column("ClassName")
		[
			SNew(SBox)
				.VAlign(EVerticalAlignment::VAlign_Fill)
				.HAlign(EHorizontalAlignment::HAlign_Fill)
				[
					SNew(STextBlock)
					.Text(FText::FromString("Class Name"))
				]
		]);

	this->HeaderRow->AddColumn(SHeaderRow::Column("OuterName")
		[
			SNew(SBox)
				.VAlign(EVerticalAlignment::VAlign_Fill)
				.HAlign(EHorizontalAlignment::HAlign_Fill)
				[
					SNew(STextBlock)
					.Text(FText::FromString("Outer Name"))
				]
		]);

	this->HeaderRow->AddColumn(SHeaderRow::Column("Address")
		[
			SNew(SBox)
				.VAlign(EVerticalAlignment::VAlign_Fill)
				.HAlign(EHorizontalAlignment::HAlign_Fill)
				[
					SNew(STextBlock)
					.Text(FText::FromString("Address"))
				]
		]);

	this->HeaderRow->AddColumn(SHeaderRow::Column("OuterChain")
		[
			SNew(SBox)
				.VAlign(EVerticalAlignment::VAlign_Fill)
				.HAlign(EHorizontalAlignment::HAlign_Fill)
				[
					SNew(STextBlock)
					.Text(FText::FromString("Outer Chain"))
				]
		]);

	this->HeaderRow->AddColumn(SHeaderRow::Column("ReferencePath")
		[
			SNew(SBox)
				.VAlign(EVerticalAlignment::VAlign_Fill)
				.HAlign(EHorizontalAlignment::HAlign_Fill)
				[
					SNew(STextBlock)
					.Text(FText::FromString("Reference Path"))
				]
		]);

	this->ListView = SNew(SListView<TSharedPtr<FWeakObjectPtr>>)
		.ListItemsSource(&this->VisibleObjects)
		.HeaderRow(this->HeaderRow)
		.SelectionMode(ESelectionMode::Single)
		.ExternalScrollbar(VerticalScrollBar)
		.ConsumeMouseWheel(EConsumeMouseWheel::Always)
		.AllowOverscroll(EAllowOverscroll::No)
		.OnGenerateRow(this, &SObjectViewer::OnGenerateRowCallback);

	this->InfoTextBlock = SNew(STextBlock);

	ChildSlot
	[
		SNew(SBorder)
		.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.Padding(FMargin(4, 2, 4, 2))
				.VAlign(EVerticalAlignment::VAlign_Center)
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(FText::FromString("Name Filter:"))
				]
				+ SHorizontalBox::Slot()
				.Padding(FMargin(4, 2, 4, 2))
				.VAlign(EVerticalAlignment::VAlign_Center)
				[
					SNew(SEditableTextBox)
					.HintText(FText::FromString("Search..."))
					.OnTextCommitted(this, &SObjectViewer::OnFilterTextCommittedCallback)
				]
				+ SHorizontalBox::Slot()
				.Padding(FMargin(4, 2, 4, 2))
				.VAlign(EVerticalAlignment::VAlign_Center)
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(FText::FromString("Class Filter:"))
				]
				+ SHorizontalBox::Slot()
				.Padding(FMargin(4, 2, 4, 2))
				.VAlign(EVerticalAlignment::VAlign_Center)
				.AutoWidth()
				[
					SNew(SClassPropertyEntryBox)
					.MetaClass(UObject::StaticClass())
					.SelectedClass(this, &SObjectViewer::GetClassFilter)
					.ShowDisplayNames(true)
					.ShowTreeView(true)
					.AllowNone(true)
					.AllowAbstract(true)
					.OnSetClass(this, &SObjectViewer::OnClassFilterSetCallback)
				]
				+ SHorizontalBox::Slot()
				.Padding(FMargin(4, 2, 1, 2))
				.VAlign(EVerticalAlignment::VAlign_Center)
				.AutoWidth()
				[
					SNew(SCheckBox)
					.IsChecked(this->bHideTemplates)
					.OnCheckStateChanged(this, &SObjectViewer::OnHideTemplatesStateChangedCallback)
				]
				+ SHorizontalBox::Slot()
				.Padding(FMargin(1, 2, 4, 2))
				.VAlign(EVerticalAlignment::VAlign_Center)
				.AutoWidth()
				[
					SNew(STextBlock)
						.Text(FText::FromString("Hide Templates"))
				]
				+ SHorizontalBox::Slot()
				.Padding(FMargin(4, 2, 1, 2))
				.VAlign(EVerticalAlignment::VAlign_Center)
				.AutoWidth()
				[
					SNew(SCheckBox)
					.IsChecked(this->bHideScriptClasses)
					.OnCheckStateChanged(this, &SObjectViewer::OnHideScriptClassesStateChangedCallback)
				]
				+ SHorizontalBox::Slot()
				.Padding(FMargin(1, 2, 4, 2))
				.VAlign(EVerticalAlignment::VAlign_Center)
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(FText::FromString("Hide Script Classes"))
				]
				+ SHorizontalBox::Slot()
				.Padding(FMargin(4, 2, 1, 2))
				.VAlign(EVerticalAlignment::VAlign_Center)
				.AutoWidth()
				[
					SNew(SCheckBox)
					.IsChecked(this->bHideEngineClasses)
					.OnCheckStateChanged(this, &SObjectViewer::OnHideEngineClassesStateChangedCallback)
				]
				+ SHorizontalBox::Slot()
				.Padding(FMargin(1, 2, 4, 2))
				.VAlign(EVerticalAlignment::VAlign_Center)
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(FText::FromString("Hide Engine Classes"))
				]
				+ SHorizontalBox::Slot()
				.Padding(FMargin(4, 2, 1, 2))
				.VAlign(EVerticalAlignment::VAlign_Center)
				.AutoWidth()
				[
					SNew(SCheckBox)
					.IsChecked(this->bOnlyShowPIE)
					.OnCheckStateChanged(this, &SObjectViewer::OnOnlyShowPIEStateChangedCallback)
				]
				+ SHorizontalBox::Slot()
				.Padding(FMargin(1, 2, 4, 2))
				.VAlign(EVerticalAlignment::VAlign_Center)
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(FText::FromString("Only Show PIE"))
				]
				+ SHorizontalBox::Slot()
				.Padding(FMargin(16, 2, 4, 2))
				.VAlign(EVerticalAlignment::VAlign_Center)
				.AutoWidth()
				[
					SNew(SButton)
					.Text(FText::FromString("Refresh"))
					.OnClicked(this, &SObjectViewer::OnRefreshClickedCallback)
				]
				+ SHorizontalBox::Slot()
				.Padding(FMargin(4, 2, 1, 2))
				.VAlign(EVerticalAlignment::VAlign_Center)
				.AutoWidth()
				[
					SNew(SCheckBox)
					.IsChecked(this->bAutoRefresh)
					.OnCheckStateChanged(this, &SObjectViewer::OnAutoRefreshStateChangedCallback)
				]
				+ SHorizontalBox::Slot()
				.Padding(FMargin(1, 2, 4, 2))
				.VAlign(EVerticalAlignment::VAlign_Center)
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(FText::FromString("Auto Refresh"))
				]
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SSeparator)
			]
			+ SVerticalBox::Slot()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.FillWidth(1)
				[
					SNew(SBox)
					.Padding(2)
					[
						this->ListView.ToSharedRef()
					]
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					VerticalScrollBar.ToSharedRef()
				]
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SSeparator)
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SBox)
				.Padding(FMargin(6, 6, 6, 10))
				[
					this->InfoTextBlock.ToSharedRef()
				]

			]
		]
	];

	this->Refresh(true);
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SObjectViewer::Refresh(bool bForce)
{
	uint32 NumLastVisibleObjects = this->VisibleObjects.Num();
	this->VisibleObjects.Empty();
	uint32 NumVisibleObjects = 0;
	uint32 NumVisibleObjectsPendingKill = 0;
	for (TObjectIterator<UObject> It; It; ++It)
	{
		if (!this->NameFilter.IsEmpty() && !It->GetName().Contains(this->NameFilter))
		{
			continue;
		}

		if (!It->IsA(this->ClassFilter))
		{
			continue;
		}

		if (this->bHideTemplates && It->IsTemplate())
		{
			continue;
		}

		if (this->bHideScriptClasses && It->GetPathName().StartsWith("/Script/"))
		{
			continue;
		}

		if (this->bHideEngineClasses && It->GetPathName().StartsWith("/Engine/"))
		{
			continue;
		}

		if (this->bOnlyShowPIE && (!It->GetWorld() || It->GetWorld()->WorldType != EWorldType::PIE))
		{
			continue;
		}

		if (It->HasAnyInternalFlags(EInternalObjectFlags::Unreachable | EInternalObjectFlags::PendingKill))
		{
			NumVisibleObjectsPendingKill++;
		}
		
		NumVisibleObjects++;
		this->VisibleObjects.Add(MakeShared<FWeakObjectPtr>(FWeakObjectPtr(*It)));
	}

	this->InfoTextBlock->SetText(FText::FromString(FString::Printf(TEXT("Displaying %i objects (%i pending kill)"), NumVisibleObjects, NumVisibleObjectsPendingKill)));
	
	// We do this to minimze refreshing the list because doing so resets tool tips and selection
	// However this does not catch all changes, hence the force option
	if (NumVisibleObjects != NumLastVisibleObjects || bForce)
	{
		this->ListView->RequestListRefresh();
	}
}

void SObjectViewer::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	this->TimeSinceLastAutoRefresh += InDeltaTime;
	if (this->bAutoRefresh && this->TimeSinceLastAutoRefresh > 1.5f)
	{
		this->Refresh();
		this->TimeSinceLastAutoRefresh = 0.f;
	}
}

const UClass* SObjectViewer::GetClassFilter() const
{
	return this->ClassFilter;
}

TSharedRef<ITableRow> SObjectViewer::OnGenerateRowCallback(TSharedPtr<FWeakObjectPtr> InRowWeakObjectPtr, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(SObjectListViewRow, OwnerTable)
		.WeakObjectPtr(InRowWeakObjectPtr);
}

void SObjectViewer::OnFilterTextCommittedCallback(const FText& Text, ETextCommit::Type)
{
	this->NameFilter = Text.ToString();
	this->Refresh(true);
}

void SObjectViewer::OnClassFilterSetCallback(const UClass* Class)
{
	if (!IsValid(Class))
	{
		Class = UObject::StaticClass();
	}

	this->ClassFilter = Class;
	this->Refresh(true);
}

void SObjectViewer::OnHideTemplatesStateChangedCallback(ECheckBoxState CheckboxState)
{
	this->bHideTemplates = CheckboxState == ECheckBoxState::Checked;
	this->Refresh(true);
}

void SObjectViewer::OnHideScriptClassesStateChangedCallback(ECheckBoxState CheckboxState)
{
	this->bHideScriptClasses = CheckboxState == ECheckBoxState::Checked;
	this->Refresh(true);
}

void SObjectViewer::OnHideEngineClassesStateChangedCallback(ECheckBoxState CheckboxState)
{
	this->bHideEngineClasses = CheckboxState == ECheckBoxState::Checked;
	this->Refresh(true);
}

void SObjectViewer::OnOnlyShowPIEStateChangedCallback(ECheckBoxState CheckboxState)
{
	this->bOnlyShowPIE = CheckboxState == ECheckBoxState::Checked;
	this->Refresh(true);
}

void SObjectViewer::OnAutoRefreshStateChangedCallback(ECheckBoxState CheckboxState)
{
	this->bAutoRefresh = CheckboxState == ECheckBoxState::Checked;
}

FReply SObjectViewer::OnRefreshClickedCallback()
{
	this->Refresh(true);
	return FReply::Handled();
}
