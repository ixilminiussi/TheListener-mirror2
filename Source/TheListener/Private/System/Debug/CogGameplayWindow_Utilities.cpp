#include "System/Debug/CogGameplayWindow_Utilities.h"

#include "AkComponent.h"
#include "CogCommon.h"

#if ENABLE_COG
#include "CogWidgets.h"
#include "GPE/Radio/PrimaryStation.h"
#include "GPE/Radio/StationDataAsset.h"
#include "Kismet/GameplayStatics.h"
#endif

void FCogGameplayWindow_Utilities::Initialize()
{
	FCogWindow::Initialize();

#if ENABLE_COG
	bHasMenu = true;

	Config = GetConfig<UCogNarrationConfig_Utilities>();
#endif
}

void FCogGameplayWindow_Utilities::RenderHelp()
{
	ImGui::Text("This window displays several gameplay utilities for debugging");
}

void FCogGameplayWindow_Utilities::RenderContent()
{
	FCogWindow::RenderContent();

#if ENABLE_COG
	const bool Open = ImGui::CollapsingHeader("InGame Stations", ImGuiTreeNodeFlags_DefaultOpen);
	if (Open && ImGui::BeginTable("Stations", 3,
	                              ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable |
	                              ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_PreciseWidths |
	                              ImGuiTableFlags_Borders))
	{
		ImGui::TableSetupColumn("Stations");
		ImGui::TableSetupColumn("Frequency");
		ImGui::TableSetupColumn("End");
		ImGui::TableHeadersRow();

		TArray<AActor*> OutActors{};
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AStation::StaticClass(), OutActors);
		int i = 0;
		for (const auto Station : OutActors)
		{
			if (const AStation* CurrentStation = Cast<AStation>(Station))
			{
				ImGui::PushID(i++);
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text(TCHAR_TO_UTF8(*CurrentStation->GetStationData()->StationName));
				ImGui::TableSetColumnIndex(1);
				ImGui::Text("%d", static_cast<int>(CurrentStation->GetStationData()->Frequency));
				ImGui::TableSetColumnIndex(2);
				if (ImGui::Button("End"))
				{
					if (CurrentStation->AkComponent)
					{
						CurrentStation->AkComponent->Stop();
					}
				}
				ImGui::PopID();
			}
		}
		ImGui::EndTable();
	}
#endif
}

void FCogGameplayWindow_Utilities::RenderTick(float DeltaSeconds)
{
	FCogWindow::RenderTick(DeltaSeconds);
}
