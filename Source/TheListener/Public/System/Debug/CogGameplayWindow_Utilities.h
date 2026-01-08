#pragma once

#include "CogCommon.h"

#include "CoreMinimal.h"
#include "CogCommonConfig.h"
#include "CogWindow.h"
#include "CogGameplayWindow_Utilities.generated.h"

class FCogGameplayWindow_Utilities : public FCogWindow
{
	using Super = FCogWindow;

public:
	virtual void Initialize() override;

protected:
	virtual void RenderHelp() override;

	virtual void RenderContent() override;

	virtual void RenderTick(float DeltaSeconds) override;

private:
	TObjectPtr<class UCogNarrationConfig_Utilities> Config = nullptr;
};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Config = Cog)
class UCogNarrationConfig_Utilities : public UCogCommonConfig
{
	GENERATED_BODY()

public:
	UPROPERTY(Config)
	bool ShowHeader = true;

	virtual void Reset() override
	{
		Super::Reset();
		ShowHeader = true;
	}
};
