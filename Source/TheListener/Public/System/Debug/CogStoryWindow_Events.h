#pragma once

#include "CogCommon.h"
#include "CoreMinimal.h"
#include "CogCommonConfig.h"
#include "CogWindow.h"
#include "EventGraphRuntime/Public/EventSubsystem.h"
#include "CogStoryWindow_Events.generated.h"


class FCogStoryWindow_Events : public FCogWindow
{
	using Super = FCogWindow;

public:
	virtual void Initialize() override;

protected:
	virtual void RenderHelp() override;

	virtual void RenderContent() override;

	virtual void RenderTick(float DeltaSeconds) override;

private:
	void RenderConditionsTab();
	void RenderConditionsTable();
	void RenderActionsTab() const;
	static void RenderConditionTreeNode(class UEventGraphConditionData*, int PushID = 0);
	static void RenderConditionKeyTooltip(const FConditionKey&);

	TMap<FConditionKey, bool> InjectConditionsMap;

	TObjectPtr<class UCogNarrationConfig_Events> Config = nullptr;
};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Config = Cog)
class UCogNarrationConfig_Events : public UCogCommonConfig
{
	GENERATED_BODY()

public:
	UPROPERTY(Config)
	bool ShowHeader = true;

	virtual void Reset() override
	{
		Super::Reset();

		// RepeatPeriod = 0.5f;
		ShowHeader = true;
	}
};
