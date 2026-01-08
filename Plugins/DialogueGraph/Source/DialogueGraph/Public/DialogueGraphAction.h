#pragma once
#include  "CoreMinimal.h"
#include "AssetTypeActions_Base.h"

class DialogueGraphAction : public FAssetTypeActions_Base
{
	public:
	DialogueGraphAction(EAssetTypeCategories::Type Category);
	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual uint32 GetCategories() override;

	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;

private:
	EAssetTypeCategories::Type AssetCategory;
};