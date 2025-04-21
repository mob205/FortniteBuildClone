#include "BitGridCustomization.h"

#include "PropertyHandle.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "IPropertyUtilities.h"
#include "Data/BitGrid.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Text/STextBlock.h"



TSharedRef<IPropertyTypeCustomization> FBitGridCustomization::MakeInstance()
{
	return MakeShareable(new FBitGridCustomization());
}

void FBitGridCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle,
	IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	TSharedPtr<IPropertyHandle> DimensionsHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FBitGrid, Dimensions));
	TSharedPtr<IPropertyHandle> BitFieldHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FBitGrid, BitField));

	if (DimensionsHandle.IsValid())
	{
		// Show dimensions normally
		ChildBuilder.AddProperty(DimensionsHandle.ToSharedRef());

		// Automatically redraw the grid if dimensions is updated
		DimensionsHandle->SetOnPropertyValueChanged(
			FSimpleDelegate::CreateLambda([BitFieldHandle, &CustomizationUtils]()
			{
				// Reset all bits
				BitFieldHandle->SetValue(0);
				
				CustomizationUtils.GetPropertyUtilities()->ForceRefresh();
			})
		);
	}

	if (BitFieldHandle.IsValid())
	{
		// Show read-only value of the bit field
		ChildBuilder.AddProperty(BitFieldHandle.ToSharedRef()).IsEnabled(false);
	}

	int GridSize{};
	if (DimensionsHandle->GetValue(GridSize) != FPropertyAccess::Success)
	{
		// Fallback to 3x3
		GridSize = 3;
	}

	// 5x5 (25 bits) is max, since BitField is 32 bit
	GridSize = FMath::Clamp(GridSize, 1, 5);

	ChildBuilder.AddCustomRow(FText::FromString("Bit Grid"))
	.WholeRowContent()
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			GenerateBitGrid(GridSize, BitFieldHandle)
		]
	];
}

TSharedRef<SWidget> FBitGridCustomization::GenerateBitGrid(int GridSize, TSharedPtr<IPropertyHandle> BitFieldHandle)
{
	TSharedRef<SVerticalBox> Grid = SNew(SVerticalBox);

	for (int Row = 0; Row < GridSize; ++Row)
	{
		TSharedRef<SHorizontalBox> RowBox = SNew(SHorizontalBox);

		for (int Col = 0; Col < GridSize; ++Col)
		{
			const int BitIndex = Row * GridSize + Col;

			RowBox->AddSlot().AutoWidth().Padding(2)
			[
				SNew(SCheckBox).IsChecked_Lambda([BitFieldHandle, BitIndex]()
				{
					// Status of each checkbox corresponds to whether the bit index is on or off
					int32 BitField{};
					BitFieldHandle->GetValue(BitField);
					return (BitField & (1 << BitIndex)) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
				})
				.OnCheckStateChanged_Lambda([BitFieldHandle, BitIndex](ECheckBoxState NewState)
				{
					int32 BitField{};
					BitFieldHandle->GetValue(BitField);

					// Set the corresponding bit to reflect the new check
					if (NewState == ECheckBoxState::Checked)
					{
						BitField |= (1 << BitIndex);
					}
					else
					{
						BitField &= ~(1 << BitIndex);
					}
					BitFieldHandle->SetValue(BitField);
				})
			];
		}
		Grid->AddSlot().AutoHeight()[RowBox];
	}

	return Grid;
}

