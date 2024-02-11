

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Views/STableRow.h"

class SCPPUEDITOR_API SObjectListViewRow : public SMultiColumnTableRow<TSharedPtr<FWeakObjectPtr>>
{
public:

	SLATE_BEGIN_ARGS(SObjectListViewRow) {}
	SLATE_ARGUMENT(TSharedPtr<FWeakObjectPtr>, WeakObjectPtr)
	SLATE_END_ARGS()

	//// Functions ////	
public:
	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView);
	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override;

private:
	FReply OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) override;

	//// Properties ////
private:
	TSharedPtr<FWeakObjectPtr> WeakObjectPtr;
};
