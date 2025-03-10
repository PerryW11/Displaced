// DialogueManager.cpp

#include "UI/Dialogue/DialogueManager.h"
#include "Engine/DataTable.h"

ADialogueManager::ADialogueManager()
{
	CurrentDialogueIndex = 0;
	CurrentConversationID = "";
	ActiveDialogueWidget = nullptr;
}

void ADialogueManager::BeginPlay()
{
	Super::BeginPlay();

	// Load dialogue data from the Data Table
	LoadDialogueData();
}

void ADialogueManager::LoadDialogueData()
{
    if (DialogueDataTable)
    {
        static const FString ContextString(TEXT("Dialogue Context"));
        TArray<FDialogueNode*> AllRows;
        DialogueDataTable->GetAllRows<FDialogueNode>(ContextString, AllRows);

        DialogueNodes.Empty();
        for (FDialogueNode* Row : AllRows)
        {
            DialogueNodes.Add(*Row);
        }
    }
}

void ADialogueManager::StartConversation(const FString& NewConversationID)
{
    // Only start if the conversation hasn’t been completed
    if (!CompletedConversations.Contains(NewConversationID))
    {
        CurrentConversationID = NewConversationID;
        CurrentDialogueIndex = -1;
        AdvanceDialogue(CurrentDialogueIndex + 1);
        UE_LOG(LogTemp, Warning, TEXT("Started conversation: %s"), *NewConversationID);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Conversation %s already completed, cannot restart"), *NewConversationID);
    }
}

bool ADialogueManager::IsConversationFinished() const
{
    int32 NextIndex = CurrentDialogueIndex + 1;
    if (!DialogueNodes.IsValidIndex(NextIndex))
    {
        return true;
    }
    return DialogueNodes[NextIndex].ConversationID != CurrentConversationID;
}

void ADialogueManager::AdvanceDialogue(int32 NextIndex)
{
    if (DialogueNodes.IsValidIndex(NextIndex))
    {
        CurrentDialogueIndex = NextIndex;
        UE_LOG(LogTemp, Warning, TEXT("At dialogue index: %d, ConversationID: %s"), CurrentDialogueIndex, *DialogueNodes[CurrentDialogueIndex].ConversationID);

        if (DialogueNodes[CurrentDialogueIndex].ConversationID == CurrentConversationID)
        {
            OnDialogueUpdated.Broadcast(DialogueNodes[CurrentDialogueIndex]);
            UE_LOG(LogTemp, Warning, TEXT("Broadcasted dialogue: %s"), *DialogueNodes[CurrentDialogueIndex].DialogueText.ToString());
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Next index %d is invalid, conversation may be at end"), NextIndex);
    }
}

bool ADialogueManager::HasConversationBeenCompleted(const FString& ConversationID) const
{
    return CompletedConversations.Contains(ConversationID);
}