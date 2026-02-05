#pragma once

class UDialogueEdGraph : public UEdGraph
{
#if WITH_EDITOR
public:
	void SetStartNode(class UDialogueGraphNode* EditorApp);
	class UDialogueGraphNode* GetStartNode();

	void OnNodeAdded (class UDialogueGraphBaseNode* Node);
	void OnNodeDestroyed (class UDialogueGraphBaseNode* Node);

	void InitializeData();

	
private:
	FGuid StartNodeID = FGuid();
#endif
};