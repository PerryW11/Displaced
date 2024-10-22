// DialogueWidget.cpp

#include "UI/Dialogue/DialogueWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "UI/Dialogue/DialogueManager.h"

void UDialogueWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (CanvasPanel && DialogueTextBlock)
	{
		// Ensure the TextBlock is attached to the Canvas Panel
		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(DialogueTextBlock->Slot);
		if (!CanvasSlot)
		{
			// If the TextBlock is not already attached, add it to the CanvasPanel
			CanvasSlot = CanvasPanel->AddChildToCanvas(DialogueTextBlock);
		}

		// Set the position and size of the TextBlock
		if (CanvasSlot)
		{
			CanvasSlot->SetPosition(FVector2D(100, 100)); // Adjust position as needed
			CanvasSlot->SetSize(FVector2D(400, 50));      // Adjust size as needed
		}
	}
}

void UDialogueWidget::UpdateDialogue(const FDialogueNode& DialogueNode)
{
	if (DialogueTextBlock)
	{
		DialogueTextBlock->SetText(DialogueNode.DialogueText);
	}
}

void UDialogueWidget::InitializeDialogueWidget(ADialogueManager* DialogueManager)
{
	if (DialogueManager)
	{
		DialogueManager->OnDialogueUpdated.AddDynamic(this, &UDialogueWidget::UpdateDialogue);
	}
}
