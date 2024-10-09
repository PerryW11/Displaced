// Fill out your copyright notice in the Description page of Project Settings.


#include "Ethan/EthanCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Items/Item.h"
#include "Items/UInteractableInterface.h"

// Sets default values
AEthanCharacter::AEthanCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Player Camera"));
	PlayerCamera->SetupAttachment(RootComponent);
	PlayerCamera->bUsePawnControlRotation = true;

	HandComponent = CreateDefaultSubobject<USceneComponent>(TEXT("HandComponent"));
	HandComponent->SetupAttachment(PlayerCamera);

}

// Called when the game starts or when spawned
void AEthanCharacter::BeginPlay()
{
	Super::BeginPlay();

	if(APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if(UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(EthanContext, 0);
		}
	}
	
}

void AEthanCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
	
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
	
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardDirection, MovementVector.Y);
	
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void AEthanCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerPitchInput(LookAxisVector.Y);
	AddControllerYawInput(LookAxisVector.X);
}

void AEthanCharacter::Interact()
{
	UE_LOG(LogTemp, Warning, TEXT("Ethan Interact called"));
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		FVector CameraLocation;
		FRotator CameraRotation;
		PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

		FVector LineTraceEnd = CameraLocation + (CameraRotation.Vector() * InteractLineTraceDistance);

		FCollisionQueryParams TraceParams;
		TraceParams.AddIgnoredActor(this);

		FHitResult Hit;
		bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, CameraLocation, LineTraceEnd, ECC_Visibility, TraceParams);

		if (bHit)
		{
			AActor* HitActor = Hit.GetActor();
			if (HitActor && HitActor->Implements<UInteractableInterface>())
			{
				// Call the Interact function on the hit actor
				IInteractableInterface::Execute_Interact(HitActor, this);
			}
		} else
		{
			UE_LOG(LogTemp, Warning, TEXT("%s is not a valid interactable object"), Hit.GetActor());
		}
	}
}

// Called every frame
void AEthanCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEthanCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if(UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &AEthanCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AEthanCharacter::Look);
		/*EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &AEthanCharacter::Interact);*/
		// Bind interact action with a log to check if InteractAction is valid
		if (InteractAction)
		{
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AEthanCharacter::Interact);
			UE_LOG(LogTemp, Warning, TEXT("InteractAction bound successfully"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("InteractAction is NULL! Make sure it's assigned."));
		}
	}

}

