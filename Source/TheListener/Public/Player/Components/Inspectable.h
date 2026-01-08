#pragma once

#include "Inspectable.generated.h"

UINTERFACE(MinimalAPI)
class UInspectable : public UInterface
{
	GENERATED_BODY()
};

class IInspectable
{
	GENERATED_BODY()

public:
	virtual bool IsInspectableVisible() = 0;
	
	virtual void ShowInspectable() = 0;

	virtual void HideInspectable() = 0;

	virtual class UInspectWidget* GetInspectWidget() = 0;
};

