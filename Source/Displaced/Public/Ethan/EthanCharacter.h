// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "EthanCharacter.generated.h"

class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class USphereComponent;

UENUM(BlueprintType)
enum class EPlayerMode : uint8
{
	Normal,
	Dialogue
};


UCLASS()
class DISPLACED_API AEthanCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEthanCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Base Gameplay Input
	
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* EthanContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MovementAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* InteractAction;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Interact();

	//Dialogue Gameplay Input
	
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* DialogueContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* DialogueLookAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* DialogueInteractAction;

	void DialogueLook(const FInputActionValue& Value);
	void DialogueInteract();

	FVector DialogueLookLocation;
	FRotator DialogueLookDirection;
	float MaxLookAngle;

	UPROPERTY(EditAnywhere)
	UCameraComponent* PlayerCamera;

	UPROPERTY(EditAnywhere);
	float InteractLineTraceDistance = 1000;

	void InterpolateCameraToTarget(float DeltaTime);
	void EndDialogue();

	UFUNCTION()
	void DialogueMove(const FInputActionValue& Value);
	
	FVector2D LastLookInput;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USphereComponent* HandComponent;
	
	// Variable to store the currently held item
	AActor* CurrentlyHeldItem;

	EPlayerMode CurrentPlayerMode; 

	UCameraComponent* GetPlayerCamera();

	bool bIsInterpolatingCamera;

private:
	

};
