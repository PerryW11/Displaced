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
	
	UPROPERTY(BlueprintAssignable)
	FOnDialogueUpdated OnDialogueUpdated;
	
	UFUNCTION(BlueprintCallable)
	void AdvanceDialogue(int32 NextIndex);

protected:
	virtual void BeginPlay() override;

private:
	// Function to load dialogue nodes from the Data Table
	void LoadDialogueData();
};
