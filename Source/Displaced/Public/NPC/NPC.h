// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Items/UInteractableInterface.h"
#include "NPC.generated.h"

class UDialogueWidget;
class ADialogueManager;

UCLASS()
class DISPLACED_API ANPC : public ACharacter, public IInteractableInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANPC();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Interact_Implementation(AActor* Interactor) override;

	// Reference to the Dialogue Widget class to spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UDialogueWidget> DialogueWidgetClass;

	// Reference to the Dialogue Manager that manages the NPC's dialogue
	/**
	 * 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	ADialogueManager* DialogueManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText CharacterName; 
	
};
