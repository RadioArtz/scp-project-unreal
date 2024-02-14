#include "ObjectViewer/SObjectListViewRow.h"
#include "SlateOptMacros.h"
#include "Subsystems/AssetEditorSubsystem.h" 

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SObjectListViewRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
{
	this->WeakObjectPtr = InArgs._WeakObjectPtr;

	SMultiColumnTableRow<TSharedPtr<FWeakObjectPtr>>::Construct(
		FSuperRowType::FArguments(),
		InOwnerTableView
	);
}

TSharedRef<SWidget> SObjectListViewRow::GenerateWidgetForColumn(const FName& ColumnName)
{
	UObject* Object = this->WeakObjectPtr->GetEvenIfUnreachable();
	if (Object == nullptr)
	{
		return SNullWidget::NullWidget;
	}

	FLinearColor TextColor = FLinearColor(1.f, 1.f, 1.f, 1.f);
	if (Object->HasAnyInternalFlags(EInternalObjectFlags::RootSet))
	{
		TextColor = FLinearColor(0.25f, .95f, .95f, 1.f);
	}
	else if (Object->GetWorld() && Object->GetWorld()->WorldType == EWorldType::PIE)
	{
		TextColor = FLinearColor(.95f, .90f, .35f, 1.f);
	}

	if (Object->HasAnyInternalFlags(EInternalObjectFlags::Unreachable | EInternalObjectFlags::PendingKill))
	{
		TextColor.A = .5f;
	}

	if (ColumnName == "Name")
	{
		return SNew(SBox)
			.Padding(FMargin(4, 2, 4, 2))
			[
				SNew(STextBlock)
				.Text(FText::FromString(Object->GetName()))
				.ToolTipText(FText::FromString(Object->GetName()))
				.ColorAndOpacity(FSlateColor(TextColor))
			];
	}

	if (ColumnName == "ClassName")
	{
		return SNew(SBox)
			.Padding(FMargin(4, 2, 4, 2))
			[
				SNew(STextBlock)
				.Text(FText::FromString(Object->GetClass()->GetName()))
				.ToolTipText(FText::FromString(Object->GetClass()->GetName()))
				.ColorAndOpacity(FSlateColor(TextColor))
			];
	}

	if (ColumnName == "OuterName")
	{
		FString OuterName = "<No Outer>";
		if (Object->GetOuter())
		{
			OuterName = Object->GetOuter()->GetName();
		}

		return SNew(SBox)
			.Padding(FMargin(4, 2, 4, 2))
			[
				SNew(STextBlock)
				.Text(FText::FromString(OuterName))
				.ToolTipText(FText::FromString(OuterName))
				.ColorAndOpacity(FSlateColor(TextColor))
			];
	}

	if (ColumnName == "Address")
	{
		return SNew(SBox)
			.Padding(FMargin(4, 2, 4, 2))
			[
				SNew(STextBlock)
				.Text(FText::FromString(FString::Printf(TEXT("0x%p"), Object)))
				.ToolTipText(FText::FromString(FString::Printf(TEXT("0x%p"), Object)))
				.ColorAndOpacity(FSlateColor(TextColor))
			];
	}

	if (ColumnName == "OuterChain")
	{
		FString OuterPath = "<No Outer>";
		if (Object->GetOuter())
		{
			if (Object->GetOuter()->IsA<UPackage>())
			{
				OuterPath = Object->GetOuter()->GetName();
			}
			else
			{
				OuterPath = Object->GetFullGroupName(true);
			}
		}

		return SNew(SBox)
			.Padding(FMargin(4, 2, 4, 2))
			[
				SNew(STextBlock)
				.Text(FText::FromString(OuterPath))
				.ToolTipText(FText::FromString(OuterPath))
				.ColorAndOpacity(FSlateColor(TextColor))
			];
	}

	if (ColumnName == "ReferencePath")
	{
		return SNew(SBox)
			.Padding(FMargin(4, 2, 4, 2))
			[
				SNew(STextBlock)
				.Text(FText::FromString(Object->GetPathName()))
				.ToolTipText(FText::FromString(Object->GetPathName()))
				.ColorAndOpacity(FSlateColor(TextColor))
			];
	}

	return SNullWidget::NullWidget;
}

FReply SObjectListViewRow::OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		UObject* Object = this->WeakObjectPtr->Get();
		if (IsValid(Object))
		{
			GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(Object);
		}
		
		return FReply::Handled();
	}
	
	return FReply::Unhandled();
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
