// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/NPC.h"

#include "Blueprint/UserWidget.h"
#include "Ethan/EthanCharacter.h"
#include "UI/Dialogue/DialogueWidget.h"
#include "UI/Dialogue/DialogueManager.h"

// Sets default values
ANPC::ANPC()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
}

// Called when the game starts or when spawned
void ANPC::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ANPC::Interact_Implementation(AActor* Interactor)
{
	AEthanCharacter* EthanCharacter = Cast<AEthanCharacter>(Interactor);
	if (EthanCharacter)
	{
		APlayerController* PlayerController = Cast<APlayerController>(EthanCharacter->GetController());
		if (PlayerController && DialogueWidgetClass)
		{
			UDialogueWidget* DialogueWidget = CreateWidget<UDialogueWidget>(PlayerController, DialogueWidgetClass);
			if (DialogueWidget)
			{
				DialogueWidget->AddToViewport();
				
				if (DialogueManager)
				{
					DialogueWidget->InitializeDialogueWidget(DialogueManager);
				}
				
				DialogueManager->AdvanceDialogue(0); 
			}
		}
	}
}

