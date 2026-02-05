#pragma once

#include "Widgets/SCompoundWidget.h"

class SPropertyViewer : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SPropertyViewer) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	TSharedPtr<SVerticalBox> PropertiesVerticalBox;
	TArray<FString> HiddenPropertyNames;
	
public:
	void SetObject(UObject* Data);
	void SetObject(UObject* Data, TArray<FString>);

	TSharedPtr<SHorizontalBox> GeneratePropertySlate(FProperty* Prop, UObject* Data);
	TSharedPtr<SWidget> GeneratePropertyValueSlate(FProperty* Prop, UObject* Data);
	TSharedPtr<SWidget> GeneratePropertyValueSlateFromArrayPtr(FProperty* Prop, FScriptArrayHelper ArrayHelper, int index);

	template<typename T>
	TSharedPtr<SWidget> GenerateNumericPropertyValueSlate(T* Value);
	TSharedPtr<SWidget> GenerateBoolPropertyValueSlate(bool* Value);
	TSharedPtr<SWidget> GenerateObjectPropertyValueSlate(void* Value, FObjectProperty* Prop);
	TSharedPtr<SWidget> GenerateArrayPropertyValueSlate(FArrayProperty* ArrayProp, void* ArrayPtr);
	//Mising FName String ... property
};


