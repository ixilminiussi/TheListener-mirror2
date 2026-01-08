#include "System/Debug/CogStoryWindow_Events.h"

#include "EventGraphActionData.h"
#include "GPE/Obji.h"

#if ENABLE_COG
#include <EventGraphRuntime/Public/EventSubsystem.h>
#include <EventGraphRuntime/Public/EventGraphConditionData.h>
#include <CogWidgets.h>
#include <GPE/Toy.h>
#include <GPE/Radio/StationDataAsset.h>
#include <Kismet/GameplayStatics.h>
#include <System/Core/PlayGameMode.h>
#include <System/Events/EventCondition.h>
#endif

void FCogStoryWindow_Events::Initialize()
{
	FCogWindow::Initialize();

#if ENABLE_COG
	bHasMenu = true;

	Config = GetConfig<UCogNarrationConfig_Events>();

	AGameModeBase* GameModeBase = UGameplayStatics::GetGameMode(GetWorld());
	if (GameModeBase == nullptr)
	{
		ImGui::TextDisabled("No GameMode Instance");
		return;
	}

	APlayGameMode* PlayGameMode = static_cast<APlayGameMode*>(GameModeBase);
	if (PlayGameMode == nullptr)
	{
		ImGui::TextDisabled("Wrong GameMode Instance (must be PlayGameMode)");
	}
#endif
}

void FCogStoryWindow_Events::RenderHelp()
{
	ImGui::Text("This window displays the current state of each Event Condition and Action."
		"It can also be used to inject inputs to help debugging.");
}

void FCogStoryWindow_Events::RenderContent()
{
	FCogWindow::RenderContent();

#if ENABLE_COG
	check(GetWorld()->GetSubsystem<UEventSubsystem>()); // window shouldnt even be initialized otherwise

	if (ImGui::BeginTabBar(""))
	{
		if (ImGui::BeginTabItem("Conditions Map"))
		{
			RenderConditionsTab();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Actions Map"))
		{
			RenderActionsTab();
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Options"))
		{
			ImGui::Checkbox("Show Header", &Config->ShowHeader);
			for (TObjectIterator<UClass> It; It; ++It)
			{
				UClass* Class = *It;
				if (It->IsChildOf(UEventGraphBaseConditionData::StaticClass()) && !It->HasAnyClassFlags(CLASS_Abstract)
					&& !It->GetName().StartsWith(TEXT("SKEL_")) && !It->GetName().StartsWith(TEXT("REINST_")))
				{
					bool bMyBool;
#if WITH_EDITORONLY_DATA
					FString Label = FString::Printf(TEXT("Show %s"), *It->GetDisplayNameText().ToString());
#else
					FString Label = FString::Printf(TEXT("Show %s"), *It->GetName());
#endif
					ImGui::Checkbox(TCHAR_TO_UTF8(*Label), &bMyBool);
				}
			}
			ImGui::EndMenu();
		}

		if (ImGui::MenuItem("Reset"))
		{
		}

		ImGui::EndMenuBar();
	}
#endif
}

void FCogStoryWindow_Events::RenderTick(float DeltaSeconds)
{
	FCogWindow::RenderTick(DeltaSeconds);
}

void FCogStoryWindow_Events::RenderConditionsTab()
{
#if ENABLE_COG
	UEventSubsystem* EventSubsystem = GetWorld()->GetSubsystem<UEventSubsystem>();
	check(EventSubsystem);

	RenderConditionsTable();

	if (InjectConditionsMap.Num() == 0)
	{
		ImGui::BeginDisabled();
	}
	if (ImGui::Button("Inject"))
	{
		for (auto& [Key, bState] : InjectConditionsMap)
		{
			if (EventSubsystem->ConditionsMap.Contains(Key))
			{
				EventSubsystem->ConditionsMap[Key] = bState;
			}
		}
		InjectConditionsMap.Empty();
		EventSubsystem->CheckCurrentEventsState();
	}
	else
	{
		if (InjectConditionsMap.Num() == 0)
		{
			ImGui::EndDisabled();
		}
		else
		{
			if (ImGui::Button("Reset"))
			{
				InjectConditionsMap.Empty();
			}
		}
	}
#endif
}

void FCogStoryWindow_Events::RenderConditionsTable()
{
#if ENABLE_COG
	constexpr ImVec4 InvalidColor(1.0f, 0.0f, 0.0f, 0.8f);

	constexpr int ColumnCount = 3;
	if (ImGui::BeginTable("##Table", ColumnCount,
	                      ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Sortable | ImGuiTableFlags_Borders))
	{
		ImGui::TableSetupColumn("Condition Type",
		                        ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_DefaultSort);
		ImGui::TableSetupColumn("Misc",
		                        ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("State", ImGuiTableColumnFlags_WidthFixed);

		if (Config->ShowHeader)
		{
			ImGui::TableHeadersRow();
		}

		UEventSubsystem* EventSubsystem = GetWorld()->GetSubsystem<UEventSubsystem>();
		check(EventSubsystem);

		// Sorts elements according to table begin
		static TArray<TPair<const FConditionKey&, bool&>> SortedConditions;
		if (ImGuiTableSortSpecs* SortSpecs = ImGui::TableGetSortSpecs(); SortSpecs->SpecsDirty)
		{
			SortedConditions.Empty();
			for (auto& [Key, bState] : EventSubsystem->ConditionsMap)
			{
				SortedConditions.Add({Key, bState});
			}

			if (SortSpecs->SpecsDirty)
			{
				SortedConditions.Sort(
					[SortSpecs](TPair<const FConditionKey&, bool&> A, TPair<const FConditionKey&, bool&> B)
					{
						for (int n = 0; n < SortSpecs->SpecsCount; n++)
						{
							const ImGuiTableColumnSortSpecs* Spec = &SortSpecs->Specs[n];
							int Cmp = 0;
							if (Spec->ColumnIndex == 0)
							{
								Cmp = A.Key.ConditionType->GetName().Compare(B.Key.ConditionType->GetName());
							}
							else if (Spec->ColumnIndex == 1)
							{
								Cmp = (Spec->SortDirection == ImGuiSortDirection_Ascending)
									      ? B.Key.ConditionType->GetName().Compare(A.Key.ConditionType->GetName())
									      : A.Key.ConditionType->GetName().Compare(B.Key.ConditionType->GetName());

								if (Cmp == 0)
								{
									Cmp = FCString::Stricmp(*A.Key.Data, *B.Key.Data);
								}
							}
							else if (Spec->ColumnIndex == 2)
							{
								Cmp = A.Value - B.Value;

								if (A.Value == B.Value)
								{
									Cmp = (Spec->SortDirection == ImGuiSortDirection_Ascending)
										      ? B.Key.ConditionType->GetName().Compare(A.Key.ConditionType->GetName())
										      : A.Key.ConditionType->GetName().Compare(B.Key.ConditionType->GetName());
								}
							}

							if (Cmp != 0)
							{
								return Spec->SortDirection == ImGuiSortDirection_Ascending ? Cmp < 0 : Cmp > 0;
							}
						}
						return false;
					});
				SortSpecs->SpecsDirty = false;
			}
		}
		// Sorts end

		int Index = 0;

		for (auto& [Key, bState] : SortedConditions)
		{
			ImGui::PushID(Index++);
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);

#if WITH_EDITORONLY_DATA
			const char* ColumnText = TCHAR_TO_UTF8(*Key.ConditionType->GetDisplayNameText().ToString());
#else
			const char* ColumnText = TCHAR_TO_UTF8(*Key.ConditionType->GetName());
#endif
			ImGui::Text(ColumnText, "");

			ImGui::TableSetColumnIndex(1);

			if (Key.ConditionType == UActiveTimeCondition::StaticClass())
			{
				ImGui::PushID(Index++);
				ImGui::Text("%f", Key.TimeRequired);
				ImGui::SameLine();
				ImGui::Text("[%s]", TCHAR_TO_UTF8(*Key.Data));
				ImGui::PopID();
			}
			else
			{
				ImGui::Text("%s", TCHAR_TO_ANSI(*Key.Data));
			}

			ImGui::TableSetColumnIndex(2);
			const bool bInjected = InjectConditionsMap.Contains(Key);
			bool bInject = bInjected ? InjectConditionsMap[Key] : bState;

			if (!bInjected)
			{
				FCogWidgets::PushBackColor(ImVec4(0.8f, 0.8f, 0.8f, 1));
			}
			if (ImGui::Checkbox("##Current", &bInject))
			{
				if (bInject == bState)
				{
					InjectConditionsMap.Remove(Key);
				}
				else
				{
					InjectConditionsMap.Add(Key, bInject);
				}
			}
			if (!bInjected)
			{
				FCogWidgets::PopBackColor();
			}

			ImGui::PopID();
		}
		ImGui::EndTable();
	}
#endif
}

void FCogStoryWindow_Events::RenderConditionTreeNode(UEventGraphConditionData* Condition, int PushID)
{
#if ENABLE_COG
	ImGui::PushID(PushID);
	constexpr ImVec4 InvalidColor(1.0f, 0.0f, 0.0f, 0.8f);
	constexpr ImVec4 ValidColor(0.0f, 1.0f, 0.0f, 0.8f);

	if (!IsValid(Condition))
	{
		ImGui::PushStyleColor(ImGuiCol_Text, InvalidColor);
		ImGui::TreeNodeEx("Undefined", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen);
		ImGui::PopStyleColor();
		ImGui::PopID();
		return;
	}

	const ImVec4 CurrentColor = Condition->IsValidated() ? ValidColor : InvalidColor;

	ImGui::PushStyleColor(ImGuiCol_Text, CurrentColor);

	ImGuiTreeNodeFlags Flags = ImGuiTreeNodeFlags_DefaultOpen;

	UEventGraphBoolConditionData* BoolCondition = Cast<UEventGraphBoolConditionData>(Condition);
	// Can absolute be nullptr

	if (BoolCondition)
	{
		Flags |= ImGuiTreeNodeFlags_Leaf;
	}
	if (ImGui::TreeNodeEx(TCHAR_TO_UTF8(*Condition->GetName()), Flags))
	{
		TArray<UEventGraphConditionData*> Conditions;
		Condition->GetLeafs(Conditions);
		Conditions.Remove(Condition);
		for (UEventGraphConditionData* SubCondition : Conditions)
		{
			RenderConditionTreeNode(SubCondition, PushID++);
		}

		if (BoolCondition)
		{
			if (FCogWidgets::BeginItemTableTooltip())
			{
				RenderConditionKeyTooltip(BoolCondition->GetKey());
				FCogWidgets::EndItemTableTooltip();
			}
		}

		ImGui::TreePop();
	}
	ImGui::PopStyleColor();
	ImGui::PopID();
#endif
}

void FCogStoryWindow_Events::RenderActionsTab() const
{
#if ENABLE_COG
	UEventSubsystem* EventSubsystem = GetWorld()->GetSubsystem<UEventSubsystem>();
	check(EventSubsystem);

	// Current Events
	int Index = 0;
	UEventGraphEventConditionData* ToSend{};
	for (UEventGraphEventConditionData* Event : EventSubsystem->CurrentEvents)
	{
		ImGui::PushID(Index++);
		if (ImGui::CollapsingHeader(TCHAR_TO_UTF8(*Event->GetEventNameAsString())))
		{
			RenderConditionTreeNode(Event->GetCondition());

			if (ImGui::BeginTable("##Table", 2))
			{
				ImGui::TableSetupColumn("Action");
				ImGui::TableSetupColumn("Inject");

				if (Config->ShowHeader)
				{
					ImGui::TableHeadersRow();
				}

				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				for (const TObjectPtr<class UEventGraphActionData>& Action : Event->GetActions())
				{
					ImGui::PushID(Index++);
					ImGui::Text(TCHAR_TO_UTF8(*Action->GetName()));
					ImGui::TableNextColumn();
					if (ImGui::Button("Launch Action"))
					{
						Action->LaunchEvent(GetWorld());
					}
					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					ImGui::PopID();
				}
				ImGui::EndTable();
			}
			if (ImGui::Button("Launch Event"))
			{
				ToSend = Event;
			}
		}
		ImGui::PopID();
	}

	if (ToSend)
	{
		EventSubsystem->LaunchEvent(ToSend);
	}

	// Completed Events =====================
	ImGui::SeparatorText("Completed Events");

	UEventGraphEventConditionData* ToRevert{};
	if (ImGui::BeginTable("##Table", 2))
	{
		ImGui::TableSetupColumn("Event Name", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("Inject", ImGuiTableColumnFlags_WidthStretch);

		ImGui::TableNextRow();
		ImGui::TableNextColumn();

		for (UEventGraphEventConditionData* Event : EventSubsystem->CompletedEvents)
		{
			ImGui::PushID(Index++);
			ImGui::Text(TCHAR_TO_UTF8(*Event->GetName()));
			ImGui::TableNextColumn();
			if (ImGui::Button("Revert"))
			{
				ToRevert = Event;
			}
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
			{
				constexpr ImVec4 InvalidColor(1.0f, 0.0f, 0.0f, 0.8f);
				ImGui::PushStyleColor(ImGuiCol_Text, InvalidColor);
				ImGui::SetTooltip("Potentially unsafe!");
				ImGui::PopStyleColor();
			}
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::PopID();
		}

		ImGui::EndTable();
	}

	if (ToRevert)
	{
		EventSubsystem->CurrentEvents.Add(ToRevert);
		EventSubsystem->CompletedEvents.Remove(ToRevert);
		EventSubsystem->CheckCurrentEventsState();
	}
#endif
}

void FCogStoryWindow_Events::RenderConditionKeyTooltip(const FConditionKey& Key)
{
#if ENABLE_COG
	if (ImGui::BeginTable("Effect", 2, ImGuiTableFlags_Borders))
	{
		ImGui::TableSetupColumn("Property");
		ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

		constexpr ImVec4 TextColor(1.0f, 1.0f, 1.0f, 0.5f);
		constexpr ImVec4 InvalidColor(1.0f, 0.0f, 0.0f, 0.8f);

		ImGui::TableNextRow();
		ImGui::TableNextColumn();

		// Condition Type
		ImGui::TextColored(TextColor, "Type");
		ImGui::TableNextColumn();
		ImGui::Text("%s", TCHAR_TO_ANSI(*Key.ConditionType->GetName()));

		if (Key.ConditionType == UInkpotCondition::StaticClass())
		{
			// Inkpot Tag
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::TextColored(TextColor, "Inkpot Tag");
			ImGui::TableNextColumn();
			ImGui::Text("%s", TCHAR_TO_ANSI(*Key.Data));
		}
		if (Key.ConditionType == UCustomCondition::StaticClass())
		{
			// Manual Tag
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::TextColored(TextColor, "Manual Name");
			ImGui::TableNextColumn();
			ImGui::Text("%s", TCHAR_TO_ANSI(*Key.Data));
		}
		if (Key.ConditionType == UDebugCondition::StaticClass())
		{
			// Manual Tag
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::TextColored(TextColor, "Manual Name");
			ImGui::TableNextColumn();
			ImGui::Text("%s", TCHAR_TO_ANSI(*Key.Data));
		}
		if (Key.ConditionType == UActiveTimeCondition::StaticClass())
		{
			// Time Tag
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::TextColored(TextColor, "Chrono Name");
			ImGui::TableNextColumn();
			ImGui::Text("%s", TCHAR_TO_ANSI(*Key.Data));

			// Time required
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::TextColored(TextColor, "Time");
			ImGui::TableNextColumn();
			ImGui::Text("%ss", TCHAR_TO_ANSI(*FString::SanitizeFloat(Key.TimeRequired)));
		}
		if (Key.ConditionType == UEventGraphEventConditionData::StaticClass())
		{
			// Event required
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::TextColored(TextColor, "Event");
			ImGui::TableNextColumn();
			ImGui::Text("%s", TCHAR_TO_ANSI(*Key.Data));
		}
		if (Key.ConditionType == UStationListenedCondition::StaticClass())
		{
			// Event required
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::TextColored(TextColor, "Station");
			ImGui::TableNextColumn();
			ImGui::Text("%s", TCHAR_TO_ANSI(*Key.Data));
		}
		if (Key.ConditionType == UStationFinishedCondition::StaticClass())
		{
			// Event required
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::TextColored(TextColor, "Station");
			ImGui::TableNextColumn();
			ImGui::Text("%s", TCHAR_TO_ANSI(*Key.Data));
		}
		if (Key.ConditionType == UToyPossessedCondition::StaticClass())
		{
			// Toy required
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::TextColored(TextColor, "Toy");
			ImGui::TableNextColumn();

			ImGui::Text("%s", TCHAR_TO_ANSI(*Key.Data));
		}
		if (Key.ConditionType == UObjiHeldCondition::StaticClass())
		{
			// Obji required
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::TextColored(TextColor, "Obji");
			ImGui::TableNextColumn();

			ImGui::Text("%s", TCHAR_TO_ANSI(*Key.Data));
		}

		ImGui::EndTable();
	}
#endif
}
