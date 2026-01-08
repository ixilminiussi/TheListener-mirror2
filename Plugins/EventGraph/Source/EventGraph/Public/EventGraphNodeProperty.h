#pragma once

class EventGraphNodePropertyLoader
{
	public:
	static void LoadProperty(UObject* Object, TSharedPtr<SVerticalBox> PropertyVerticalBox);
	
	static void CreatePropertySlate(FProperty* Property, TSharedPtr<SCompoundWidget>& PropertyWidget, TSharedPtr<SCompoundWidget>& PropertyEditableWidget);
	
	static void CreateProperty(TSharedPtr<SCompoundWidget>& PropertyWidget, FFloatProperty* Property, UObject* Object);
	static void CreateProperty(TSharedPtr<SCompoundWidget>& PropertyWidget, FIntProperty* Property, UObject* Object);
	static void CreateProperty(TSharedPtr<SCompoundWidget>& PropertyWidget, FDoubleProperty* Property, UObject* Object);
	static void CreateProperty(TSharedPtr<SCompoundWidget>& PropertyWidget, FNameProperty* Property, UObject* Object);
	static void CreateProperty(TSharedPtr<SCompoundWidget>& PropertyWidget, FStrProperty* Property, UObject* Object);
	static void CreateProperty(TSharedPtr<SCompoundWidget>& PropertyWidget, FTextProperty* Property, UObject* Object);
	static void CreateProperty(TSharedPtr<SCompoundWidget>& PropertyWidget, FClassProperty* Property, UObject* Object);
	static void CreateProperty(TSharedPtr<SCompoundWidget>& PropertyWidget, FObjectProperty* Property, UObject* Object);
	static void CreateProperty(TSharedPtr<SCompoundWidget>& PropertyWidget, FArrayProperty* Property, UObject* Object);
};


