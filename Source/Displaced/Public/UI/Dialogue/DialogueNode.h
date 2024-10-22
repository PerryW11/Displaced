// DialogueManager.h or a dedicated header file for structs
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DialogueNode.generated.h"

USTRUCT(BlueprintType)
struct FDialogueNode : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DialogueIndex; // The index for this dialogue node

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DialogueText; // The actual dialogue text to be displayed

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText CharacterName; // The owner of the dialogue story-wise
	
};
