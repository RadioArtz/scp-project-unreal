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

	bool bObjectIsPendingKill = Object->IsUnreachable() || Object->IsPendingKill();

	if (ColumnName == "Name")
	{
		return SNew(SBox)
			.Padding(FMargin(4, 2, 4, 2))
			[
				SNew(STextBlock)
				.Text(FText::FromString(Object->GetName()))
				.ToolTipText(FText::FromString(Object->GetName()))
				.RenderOpacity(bObjectIsPendingKill ? .5f : 1.f)
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
				.RenderOpacity(bObjectIsPendingKill ? .5f : 1.f)
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
				.RenderOpacity(bObjectIsPendingKill ? .5f : 1.f)
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
				.RenderOpacity(bObjectIsPendingKill ? .5f : 1.f)
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
				.RenderOpacity(bObjectIsPendingKill ? .5f : 1.f)
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
