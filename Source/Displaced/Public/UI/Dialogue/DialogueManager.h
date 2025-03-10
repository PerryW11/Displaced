// DialogueManager.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DialogueNode.h"
#include "DialogueManager.generated.h"

// Delegate for broadcasting dialogue updates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogueUpdated, const FDialogueNode&, DialogueNode);

UCLASS()
class DISPLACED_API ADialogueManager : public AActor
{
	GENERATED_BODY()

public:
	ADialogueManager();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	UDataTable* DialogueDataTable;
	
	UPROPERTY(BlueprintReadWrite)
	TArray<FDialogueNode> DialogueNodes;
	
	UPROPERTY(BlueprintReadWrite)
	int32 CurrentDialogueIndex;

	UPROPERTY(BlueprintReadWrite)
	FString CurrentConversationID;
	
	UPROPERTY(BlueprintAssignable)
	FOnDialogueUpdated OnDialogueUpdated;
	
	UFUNCTION(BlueprintCallable)
	void AdvanceDialogue(int32 NextIndex);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void StartConversation(const FString& NewConversationID);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	bool IsConversationFinished() const;

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	bool HasConversationBeenCompleted(const FString& ConversationID) const;

	class UDialogueWidget* ActiveDialogueWidget;

	UPROPERTY()
	TSet<FString> CompletedConversations;

protected:
	virtual void BeginPlay() override;

private:
	// Function to load dialogue nodes from the Data Table
	void LoadDialogueData();
};
