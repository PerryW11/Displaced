// DialogueManager.cpp

#include "UI/Dialogue/DialogueManager.h"
#include "Engine/DataTable.h"

ADialogueManager::ADialogueManager()
{
	// Initialize default values
	CurrentDialogueIndex = 0;
}

void ADialogueManager::BeginPlay()
{
	Super::BeginPlay();

	// Load dialogue data from the Data Table
	LoadDialogueData();

	// Start with the first dialogue node
	if (DialogueNodes.IsValidIndex(CurrentDialogueIndex))
	{
		OnDialogueUpdated.Broadcast(DialogueNodes[CurrentDialogueIndex]);
	}
}

void ADialogueManager::LoadDialogueData()
{
	if (DialogueDataTable)
	{
		static const FString ContextString(TEXT("Dialogue Context"));
		TArray<FDialogueNode*> AllRows;
		DialogueDataTable->GetAllRows<FDialogueNode>(ContextString, AllRows);

		for (FDialogueNode* Row : AllRows)
		{
			DialogueNodes.Add(*Row);
		}
	}
}

void ADialogueManager::AdvanceDialogue(int32 NextIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("At dialogue index: %d"), NextIndex);
	if (DialogueNodes.IsValidIndex(NextIndex))
	{
		OnDialogueUpdated.Broadcast(DialogueNodes[CurrentDialogueIndex]);
		CurrentDialogueIndex++;
	}
}
