// DialogueWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DialogueNode.h"
#include "DialogueWidget.generated.h"

UCLASS()
class DISPLACED_API UDialogueWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Reference to the text block that displays the dialogue
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DialogueTextBlock;

	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* CanvasPanel;

	// Function to update the dialogue displayed in the widget
	UFUNCTION()
	void UpdateDialogue(const FDialogueNode& DialogueNode);

	// Function to initialize the widget and subscribe to the DialogueManager's event
	void InitializeDialogueWidget(class ADialogueManager* DialogueManager);

	virtual void NativeConstruct() override;
};
