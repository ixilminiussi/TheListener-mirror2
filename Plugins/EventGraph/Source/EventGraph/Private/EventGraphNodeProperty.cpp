#include "EventGraphNodeProperty.h"

#include "PropertyCustomizationHelpers.h"
#include "Widgets/Input/SNumericEntryBox.h"

void EventGraphNodePropertyLoader::LoadProperty(UObject* Object, TSharedPtr<SVerticalBox> PropertyVerticalBox)
{
	for (TFieldIterator<FProperty> It(Object->GetClass()); It; ++It)
	{
		FProperty* Prop = *It;
		TSharedPtr<SCompoundWidget> PropertyWidget;
		if (Prop->HasAnyPropertyFlags(CPF_Edit) &&
			!Prop->HasAnyPropertyFlags(CPF_Transient | CPF_DisableEditOnTemplate | CPF_EditConst) && Prop->GetName() != FName(TEXT("EventName")))
		{
			if (FFloatProperty* CastProperty = CastField<FFloatProperty>(Prop)) { EventGraphNodePropertyLoader::CreateProperty(PropertyWidget,CastProperty,Object);}
			if (FDoubleProperty* CastProperty = CastField<FDoubleProperty>(Prop)) { EventGraphNodePropertyLoader::CreateProperty(PropertyWidget,CastProperty,Object);}
			if (FIntProperty* CastProperty = CastField<FIntProperty>(Prop)) { EventGraphNodePropertyLoader::CreateProperty(PropertyWidget,CastProperty,Object);}
			if (FNameProperty* CastProperty = CastField<FNameProperty>(Prop)) { EventGraphNodePropertyLoader::CreateProperty(PropertyWidget,CastProperty,Object);}
			if (FStrProperty* CastProperty = CastField<FStrProperty>(Prop)) { EventGraphNodePropertyLoader::CreateProperty(PropertyWidget,CastProperty,Object);}
			if (FTextProperty* CastProperty = CastField<FTextProperty>(Prop)) { EventGraphNodePropertyLoader::CreateProperty(PropertyWidget,CastProperty,Object);}
			if (FClassProperty* CastProperty = CastField<FClassProperty>(Prop)) { EventGraphNodePropertyLoader::CreateProperty(PropertyWidget,CastProperty,Object);}
			else if (FObjectProperty* CastObjProperty = CastField<FObjectProperty>(Prop)) { EventGraphNodePropertyLoader::CreateProperty(PropertyWidget,CastObjProperty,Object);}
			if (FArrayProperty* CastProperty = CastField<FArrayProperty>(Prop)) { EventGraphNodePropertyLoader::CreateProperty(PropertyWidget,CastProperty,Object);}
		}
	if ( PropertyWidget.IsValid() )
		{
			PropertyVerticalBox->AddSlot()
			[
				PropertyWidget.ToSharedRef()
			];
		}
		
	}
}

void EventGraphNodePropertyLoader::CreatePropertySlate(FProperty* Property,TSharedPtr<SCompoundWidget>& PropertyWidget,
	TSharedPtr<SCompoundWidget>& PropertyEditableWidget)
{
	SAssignNew(PropertyWidget,SBorder)
	.Padding(FMargin(2, 2.0f))
	.BorderBackgroundColor(FSlateColor(FColor(0,0,0,1)))
	[
		SNew(SHorizontalBox)
		+SHorizontalBox::Slot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		.Padding(FMargin(0.f, 0.f, 10.f, 0.f))
		.MinWidth(110.f)
		.MaxWidth(110.f)
		[
			SNew(STextBlock)
			.Text(FText::FromString(Property->GetName()))
		]
		+SHorizontalBox::Slot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		.MinWidth(100)
		.FillContentWidth(1.0f)
		[
			PropertyEditableWidget.ToSharedRef()
		]
	];
}

void EventGraphNodePropertyLoader::CreateProperty(TSharedPtr<SCompoundWidget>& PropertyWidget, FFloatProperty* Property,
	UObject* Object)
{
	TSharedPtr<SCompoundWidget> PropertyEditableWidget;
	SAssignNew(PropertyEditableWidget, SNumericEntryBox<float>)
	.Value_Lambda([Property, Object]()
	{
		float Value = 0;
		FFloatProperty* CastProperty = CastField<FFloatProperty>(Property);
		if (CastProperty)
		{
			Value = CastProperty->GetPropertyValue_InContainer(Object);
		}
		return Value;
	})
	.OnValueChanged_Lambda([Property, Object](float NewValue)
	{
		FFloatProperty* CastProperty = CastField<FFloatProperty>(Property);
		if (CastProperty)
		{
			Object->Modify();
			CastProperty->SetPropertyValue_InContainer(Object,NewValue);

			#if WITH_EDITOR
			FPropertyChangedEvent PropertyEvent(Property);
			Object->PostEditChangeProperty(PropertyEvent);
			#endif
		}
	});
	CreatePropertySlate(Property, PropertyWidget,PropertyEditableWidget);
}

void EventGraphNodePropertyLoader::CreateProperty(TSharedPtr<SCompoundWidget>& PropertyWidget, FIntProperty* Property,
	UObject* Object)
{
	TSharedPtr<SCompoundWidget> PropertyEditableWidget;
	SAssignNew(PropertyEditableWidget, SNumericEntryBox<int>)
	.Value_Lambda([Property, Object]()
	{
		int Value = 0;
		if (Property)
		{
			Value = Property->GetPropertyValue_InContainer(Object);
		}
		return Value;
	})
	.OnValueChanged_Lambda([Property, Object](int NewValue)
	{
		if (Property)
		{
			Object->Modify();
			Property->SetPropertyValue_InContainer(Object,NewValue);

			#if WITH_EDITOR
			FPropertyChangedEvent PropertyEvent(Property);
			Object->PostEditChangeProperty(PropertyEvent);
			#endif
		}
	});
	CreatePropertySlate(Property, PropertyWidget,PropertyEditableWidget);
}

void EventGraphNodePropertyLoader::CreateProperty(TSharedPtr<SCompoundWidget>& PropertyWidget,
	FDoubleProperty* Property, UObject* Object)
{
	TSharedPtr<SCompoundWidget> PropertyEditableWidget;
	SAssignNew(PropertyEditableWidget, SNumericEntryBox<double>)
	.Value_Lambda([Property, Object]()
	{
		double Value = 0;
		if (Property)
		{
			Value = Property->GetPropertyValue_InContainer(Object);
		}
		return Value;
	})
	.OnValueChanged_Lambda([Property, Object](double NewValue)
	{
		if (Property)
		{
			Object->Modify();
			Property->SetPropertyValue_InContainer(Object,NewValue);

			#if WITH_EDITOR
			FPropertyChangedEvent PropertyEvent(Property);
			Object->PostEditChangeProperty(PropertyEvent);
			#endif
		}
	});
	CreatePropertySlate(Property, PropertyWidget,PropertyEditableWidget);
}

void EventGraphNodePropertyLoader::CreateProperty(TSharedPtr<SCompoundWidget>& PropertyWidget, FNameProperty* Property,
                                                  UObject* Object)
{
	TSharedPtr<SCompoundWidget> PropertyEditableWidget;
	SAssignNew(PropertyEditableWidget, SEditableTextBox)
	.Text_Lambda([Property, Object]()
	{
		FText Value = FText::FromString("");
		if (Property)
		{
			Value = FText::FromName(Property->GetPropertyValue_InContainer(Object));
		}
		return Value;
	})
	.OnTextChanged_Lambda([Property, Object](FText NewValue)
	{
		Object->Modify();
		Property->SetPropertyValue_InContainer(Object,FName(NewValue.ToString()));

		#if WITH_EDITOR
		FPropertyChangedEvent PropertyEvent(Property);

		Object->PostEditChangeProperty(PropertyEvent);
		#endif
	});
	
	CreatePropertySlate(Property, PropertyWidget,PropertyEditableWidget);
}

void EventGraphNodePropertyLoader::CreateProperty(TSharedPtr<SCompoundWidget>& PropertyWidget, FStrProperty* Property,
	UObject* Object)
{
	TSharedPtr<SCompoundWidget> PropertyEditableWidget;
	SAssignNew(PropertyEditableWidget, SEditableTextBox)
	.Text_Lambda([Property, Object]()
	{
		FText Value = FText::FromString("");
		if (Property)
		{
			Value = FText::FromString(Property->GetPropertyValue_InContainer(Object));
		}
		return Value;
	})
	.OnTextChanged_Lambda([Property, Object](FText NewValue)
	{
		Object->Modify();
		Property->SetPropertyValue_InContainer(Object,NewValue.ToString());

		#if WITH_EDITOR
		FPropertyChangedEvent PropertyEvent(Property);

		Object->PostEditChangeProperty(PropertyEvent);
		#endif
	});
	
	CreatePropertySlate(Property, PropertyWidget,PropertyEditableWidget);
}

void EventGraphNodePropertyLoader::CreateProperty(TSharedPtr<SCompoundWidget>& PropertyWidget, FTextProperty* Property,
	UObject* Object)
{
	TSharedPtr<SCompoundWidget> PropertyEditableWidget;
	SAssignNew(PropertyEditableWidget, SEditableTextBox)
	.Text_Lambda([Property, Object]()
	{
		FText Value = FText::FromString("");
		if (Property)
		{
			Value = Property->GetPropertyValue_InContainer(Object);
		}
		return Value;
	})
	.OnTextChanged_Lambda([Property, Object](FText NewValue)
	{
		Object->Modify();
		Property->SetPropertyValue_InContainer(Object,NewValue);

		#if WITH_EDITOR
		FPropertyChangedEvent PropertyEvent(Property);

		Object->PostEditChangeProperty(PropertyEvent);
		#endif
	});
	
	CreatePropertySlate(Property, PropertyWidget,PropertyEditableWidget);
}

void EventGraphNodePropertyLoader::CreateProperty(TSharedPtr<SCompoundWidget>& PropertyWidget, FClassProperty* Property,
	UObject* Object)
{
	
	TSharedPtr<SCompoundWidget> PropertyEditableWidget;
	SAssignNew(PropertyEditableWidget, SClassPropertyEntryBox)
	.MetaClass(Property->MetaClass)
	.SelectedClass_Lambda([Property, Object]()-> const UClass* 
	{
		return Cast<UClass>(Property->GetPropertyValue_InContainer(Object));
	})
	.OnSetClass_Lambda([Property, Object](const UClass* NewClass)
	{
		Object->Modify();
		Property->SetPropertyValue_InContainer(Object,const_cast<UClass*>(NewClass));

		#if WITH_EDITOR
		FPropertyChangedEvent PropertyEvent(Property);
		Object->PostEditChangeProperty(PropertyEvent);
		#endif
	});
	
	CreatePropertySlate(Property, PropertyWidget,PropertyEditableWidget);
}

void EventGraphNodePropertyLoader::CreateProperty(TSharedPtr<SCompoundWidget>& PropertyWidget,
                                                  FObjectProperty* Property, UObject* Object)
{
	TSharedPtr<SCompoundWidget> PropertyEditableWidget;
	SAssignNew(PropertyEditableWidget, SObjectPropertyEntryBox)
	.AllowedClass(Property->PropertyClass) 
	.ObjectPath_Lambda([Property, Object]()
	{
		UObject* Value = nullptr;
		if (Property)
		{
			Value = Property->GetPropertyValue_InContainer(Object);
		}
		return Value ? Value->GetPathName() : FString();
	})
	.OnObjectChanged_Lambda([Property, Object](const FAssetData& DataValue)
	{
		Object->Modify();
		UObject* NewValue = DataValue.GetAsset();
		Property->SetPropertyValue_InContainer(Object,NewValue);

		#if WITH_EDITOR
		FPropertyChangedEvent PropertyEvent(Property);

		Object->PostEditChangeProperty(PropertyEvent);
		#endif
	});
	
	CreatePropertySlate(Property, PropertyWidget,PropertyEditableWidget);
}


void EventGraphNodePropertyLoader::CreateProperty(TSharedPtr<SCompoundWidget>& PropertyWidget, FArrayProperty* Property,
	UObject* Object)
{
	
}




