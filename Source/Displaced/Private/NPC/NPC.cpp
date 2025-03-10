// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/NPC.h"

#include "Blueprint/UserWidget.h"
#include "Ethan/EthanCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Dialogue/DialogueWidget.h"
#include "UI/Dialogue/DialogueManager.h"
#include "Components/SphereComponent.h"

// Sets default values
ANPC::ANPC()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Head = CreateDefaultSubobject<USphereComponent>(TEXT("Head"));
	Head->SetupAttachment(RootComponent);
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
		if (EthanCharacter->CurrentPlayerMode == EPlayerMode::Dialogue)
		{
			UE_LOG(LogTemp, Warning, TEXT("Player already in dialogue mode, letting player handle dialogue advancement"));
			return;
		}

		APlayerController* PlayerController = Cast<APlayerController>(EthanCharacter->GetController());
		if (PlayerController && DialogueWidgetClass)
		{
			ADialogueManager* DialogueManager = Cast<ADialogueManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ADialogueManager::StaticClass()));
			if (!DialogueManager)
			{
				UE_LOG(LogTemp, Error, TEXT("DialogueManager not found in the world."));
				return;
			}

			// Check if the conversation has already been completed
			FString ConversationID = "test"; // Replace with dynamic logic if needed
			if (DialogueManager->HasConversationBeenCompleted(ConversationID))
			{
				UE_LOG(LogTemp, Warning, TEXT("Conversation %s already completed, interaction blocked"), *ConversationID);
				return;
			}

			UDialogueWidget* DialogueWidget = CreateWidget<UDialogueWidget>(PlayerController, DialogueWidgetClass);
			if (DialogueWidget)
			{
				DialogueWidget->AddToViewport();
				DialogueWidget->InitializeDialogueWidget(DialogueManager);
				DialogueManager->ActiveDialogueWidget = DialogueWidget;
				DialogueManager->StartConversation(ConversationID);
			}
		}
	}
}

