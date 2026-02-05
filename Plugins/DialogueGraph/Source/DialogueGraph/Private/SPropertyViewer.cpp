#include "SPropertyViewer.h"

#include "PropertyCustomizationHelpers.h"
#include "Widgets/Input/SNumericEntryBox.h"

void SPropertyViewer::Construct(const FArguments& InArgs)
{
	HiddenPropertyNames.Empty();
	SAssignNew(PropertiesVerticalBox, SVerticalBox);
	ChildSlot
	[
		PropertiesVerticalBox.ToSharedRef()
	];
}

void SPropertyViewer::SetObject(UObject* Data)
{
	PropertiesVerticalBox->ClearChildren();
	if (Data == nullptr) {return;} //DATA SHOULDNT BE NULL
	for (TFieldIterator<FProperty> It(Data->GetClass()); It; ++It)
	{
		FProperty* Prop = *It;

		if (!Prop->HasAnyPropertyFlags(CPF_Edit) ||
			Prop->HasAnyPropertyFlags(CPF_Transient | CPF_EditConst) ||
			HiddenPropertyNames.Contains(Prop->GetName()))
			continue;
		
		PropertiesVerticalBox->AddSlot()
		.Padding(0,2.0f)
		.AutoHeight()
		[
			GeneratePropertySlate(Prop, Data).ToSharedRef()
		];
	}
}

void SPropertyViewer::SetObject(UObject* Data, TArray<FString> PropertyToHide)
{
	HiddenPropertyNames = PropertyToHide;
	SetObject(Data);
}

TSharedPtr<SHorizontalBox> SPropertyViewer::GeneratePropertySlate(FProperty* Prop, UObject* Data)
{
	return SNew(SHorizontalBox)
		+SHorizontalBox::Slot()
		.MinWidth(90)
		.MaxWidth(90)
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock)
			.Text(FText::FromString(Prop->GetName()))
			.AutoWrapText(false)
			.OverflowPolicy(ETextOverflowPolicy::Ellipsis)
		]
		+SHorizontalBox::Slot()
		.MaxWidth(130)
		.MinWidth(130)
		.HAlign(HAlign_Fill)
		[
			GeneratePropertyValueSlate(Prop, Data).ToSharedRef()
		]
		;
}

TSharedPtr<SWidget> SPropertyViewer::GeneratePropertyValueSlate(FProperty* Prop, UObject* Data)
{
	if (FInt8Property* Int8Prop = CastField<FInt8Property>(Prop))
	{
		int8* PropertyPtr = Int8Prop->ContainerPtrToValuePtr<int8>(Data);
		return GenerateNumericPropertyValueSlate(PropertyPtr);
	}
	else if (FInt16Property* Int16Prop = CastField<FInt16Property>(Prop))
	{
		int16* PropertyPtr = Int16Prop->ContainerPtrToValuePtr<int16>(Data);
		return GenerateNumericPropertyValueSlate(PropertyPtr);
	}
	else if (FIntProperty* Int32Prop = CastField<FIntProperty>(Prop))
	{
		int* PropertyPtr = Int32Prop->ContainerPtrToValuePtr<int>(Data);
		return GenerateNumericPropertyValueSlate(PropertyPtr);
	}
	else if (FInt64Property* Int64Prop = CastField<FInt64Property>(Prop))
	{
		int64* PropertyPtr = Int64Prop->ContainerPtrToValuePtr<int64>(Data);
		return GenerateNumericPropertyValueSlate(PropertyPtr);
	}
	else if (FByteProperty* ByteProp = CastField<FByteProperty>(Prop))
	{
		uint8* PropertyPtr = Int64Prop->ContainerPtrToValuePtr<uint8>(Data);
		return GenerateNumericPropertyValueSlate(PropertyPtr);
	}
	else if (FUInt16Property* UInt16Prop = CastField<FUInt16Property>(Prop))
	{
		uint16* PropertyPtr = UInt16Prop->ContainerPtrToValuePtr<uint16>(Data);
		return GenerateNumericPropertyValueSlate(PropertyPtr);
	}
	else if (FUInt32Property* UInt32Prop = CastField<FUInt32Property>(Prop))
	{
		uint32* PropertyPtr = UInt32Prop->ContainerPtrToValuePtr<uint32>(Data);
		return GenerateNumericPropertyValueSlate(PropertyPtr);
	}
	else if (FUInt64Property* UInt64Prop = CastField<FUInt64Property>(Prop))
	{
		uint64* PropertyPtr = UInt16Prop->ContainerPtrToValuePtr<uint64>(Data);
		return GenerateNumericPropertyValueSlate(PropertyPtr);
	}
	else if (FFloatProperty* FloatProp = CastField<FFloatProperty>(Prop))
	{
		float* PropertyPtr = FloatProp->ContainerPtrToValuePtr<float>(Data);
		return GenerateNumericPropertyValueSlate(PropertyPtr);
	}
	else if (FDoubleProperty* DoubleProp = CastField<FDoubleProperty>(Prop))
	{
		double* PropertyPtr = DoubleProp->ContainerPtrToValuePtr<double>(Data);
		return GenerateNumericPropertyValueSlate(PropertyPtr);
	}
	else if (FBoolProperty* BoolProp = CastField<FBoolProperty>(Prop))
	{
		bool* PropertyPtr = BoolProp->ContainerPtrToValuePtr<bool>(Data);
		return GenerateBoolPropertyValueSlate(PropertyPtr);
	}
	else if (FObjectProperty* ObjProp = CastField<FObjectProperty>(Prop))
	{
		void* PropertyPtr = ObjProp->ContainerPtrToValuePtr<void>(Data);
		return GenerateObjectPropertyValueSlate(PropertyPtr,ObjProp);
	}
	else if (FArrayProperty* ArrProp = CastField<FArrayProperty>(Prop))
	{
		void* PropertyPtr = ArrProp->ContainerPtrToValuePtr<void>(Data);
		return GenerateArrayPropertyValueSlate(ArrProp,PropertyPtr);
	}
	FString PropertyName = FString("No (");
	PropertyName.Append(Prop->GetCPPType());
	PropertyName.Append(")");
	return SNew(STextBlock)
		.Text(FText::FromString(PropertyName))
		.AutoWrapText(false)  // don't wrap lines
		.OverflowPolicy(ETextOverflowPolicy::Ellipsis)
		.ColorAndOpacity(FLinearColor(FColor::Red));
}

TSharedPtr<SWidget> SPropertyViewer::GeneratePropertyValueSlateFromArrayPtr(FProperty* Prop, FScriptArrayHelper ArrayHelper, int index)
{
	void* ArrPtr = ArrayHelper.GetRawPtr(index);
	if (FInt8Property* Int8Prop = CastField<FInt8Property>(Prop))
	{
		int8* PropertyPtr = reinterpret_cast<int8*>(ArrPtr);
		return GenerateNumericPropertyValueSlate(PropertyPtr);
	}
	else if (FInt16Property* Int16Prop = CastField<FInt16Property>(Prop))
	{
		int16* PropertyPtr = reinterpret_cast<int16*>(ArrPtr);
		return GenerateNumericPropertyValueSlate(PropertyPtr);
	}
	else if (FIntProperty* Int32Prop = CastField<FIntProperty>(Prop))
	{
		int* PropertyPtr = reinterpret_cast<int*>(ArrPtr);
		return GenerateNumericPropertyValueSlate(PropertyPtr);
	}
	else if (FInt64Property* Int64Prop = CastField<FInt64Property>(Prop))
	{
		int64* PropertyPtr = reinterpret_cast<int64*>(ArrPtr);
		return GenerateNumericPropertyValueSlate(PropertyPtr);
	}
	else if (FByteProperty* ByteProp = CastField<FByteProperty>(Prop))
	{
		uint8* PropertyPtr = reinterpret_cast<uint8*>(ArrPtr);
		return GenerateNumericPropertyValueSlate(PropertyPtr);
	}
	else if (FUInt16Property* UInt16Prop = CastField<FUInt16Property>(Prop))
	{
		uint16* PropertyPtr = reinterpret_cast<uint16*>(ArrPtr);
		return GenerateNumericPropertyValueSlate(PropertyPtr);
	}
	else if (FUInt32Property* UInt32Prop = CastField<FUInt32Property>(Prop))
	{
		uint32* PropertyPtr = reinterpret_cast<uint32*>(ArrPtr);
		return GenerateNumericPropertyValueSlate(PropertyPtr);
	}
	else if (FUInt64Property* UInt64Prop = CastField<FUInt64Property>(Prop))
	{
		uint64* PropertyPtr = reinterpret_cast<uint64*>(ArrPtr);
		return GenerateNumericPropertyValueSlate(PropertyPtr);
	}
	else if (FFloatProperty* FloatProp = CastField<FFloatProperty>(Prop))
	{
		float* PropertyPtr = reinterpret_cast<float*>(ArrPtr);
		return GenerateNumericPropertyValueSlate(PropertyPtr);
	}
	else if (FDoubleProperty* DoubleProp = CastField<FDoubleProperty>(Prop))
	{
		double* PropertyPtr = reinterpret_cast<double*>(ArrPtr);
		return GenerateNumericPropertyValueSlate(PropertyPtr);
	}
	else if (FBoolProperty* BoolProp = CastField<FBoolProperty>(Prop))
	{
		bool* PropertyPtr = reinterpret_cast<bool*>(ArrPtr);
		return GenerateBoolPropertyValueSlate(PropertyPtr);
	}
	else if (FObjectProperty* ObjProp = CastField<FObjectProperty>(Prop))
	{
		return GenerateObjectPropertyValueSlate(ArrPtr,ObjProp);
	}
	else if (FArrayProperty* ArrProp = CastField<FArrayProperty>(Prop))
	{
		return SNew(STextBlock)
		.Text(FText::FromString("Non pas 2D"))
		.AutoWrapText(false)  // don't wrap lines
		.OverflowPolicy(ETextOverflowPolicy::Ellipsis)
		.ColorAndOpacity(FLinearColor(FColor::Red));
	}
	FString PropertyName = FString("No (");
	PropertyName.Append(Prop->GetCPPType());
	PropertyName.Append(")");
	return SNew(STextBlock)
		.Text(FText::FromString(PropertyName))
		.AutoWrapText(false)  // don't wrap lines
		.OverflowPolicy(ETextOverflowPolicy::Ellipsis)
		.ColorAndOpacity(FLinearColor(FColor::Red));
}

template <typename T>
TSharedPtr<SWidget> SPropertyViewer::GenerateNumericPropertyValueSlate(T* Value)
{
	return SNew(SBox)
		.HeightOverride(24.f)
		[
			SNew(SNumericEntryBox<T>)
			.MinDesiredValueWidth(130)
			.Value_Lambda([Value]()
			{
				return *Value;
			})
			.OnValueChanged_Lambda([Value](T NewValue)
			{
				*Value = NewValue;
			})
		];
}

TSharedPtr<SWidget> SPropertyViewer::GenerateBoolPropertyValueSlate(bool* Value)
{
	return SNew(SBox)
		.HeightOverride(24.f)
		[
			SNew(SCheckBox)
			.IsChecked_Lambda([Value]()
			{
				return *Value ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
			})
			.OnCheckStateChanged_Lambda([Value](ECheckBoxState NewValue)
			{
				*Value = NewValue == ECheckBoxState::Checked;
			})
		];
}

TSharedPtr<SWidget> SPropertyViewer::GenerateObjectPropertyValueSlate(void* Value, FObjectProperty* Prop)
{
	return SNew(SBox)
		.HeightOverride(24.f)
		[
			SNew(SObjectPropertyEntryBox)
			.AllowedClass(Prop->PropertyClass) 
			.ObjectPath_Lambda([Value,Prop]()
			{
				if (Prop == nullptr || Value == nullptr)
				{
					return FString("");
				}
					return Prop->GetObjectPropertyValue(Value)->GetPathName();
			})
			.OnObjectChanged_Lambda([Value, Prop](const FAssetData&  NewValue)
			{
				Prop->SetObjectPropertyValue(Value, NewValue.GetAsset());
			})
		];
}

TSharedPtr<SWidget> SPropertyViewer::GenerateArrayPropertyValueSlate(FArrayProperty* ArrayProp, void* ArrayPtr)
{
	TSharedPtr<SVerticalBox> VBox;
	SAssignNew(VBox,SVerticalBox);

	FScriptArrayHelper ArrayHelper(ArrayProp,ArrayPtr);
	FProperty* InnerProp = ArrayProp->Inner;

	for (int i = 0; i < ArrayHelper.Num(); i++)
	{
		TSharedPtr<SWidget> Value = GeneratePropertyValueSlateFromArrayPtr(InnerProp,ArrayHelper,i);
		VBox->AddSlot()
		[
			Value.ToSharedRef()
		];
	}
	
	return SNew(SHorizontalBox)
	+SHorizontalBox::Slot()
	[
		SNew(SBox)
		.WidthOverride(0.5f)
	]
	+SHorizontalBox::Slot()
	.FillWidth(1.0f)
	[
		VBox.ToSharedRef()
	];
}




