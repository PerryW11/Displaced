// Fill out your copyright notice in the Description page of Project Settings.


#include "Ethan/EthanCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
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

    // Check if the player is holding an item
    if (CurrentlyHeldItem)
    {
        // Drop the item
        UE_LOG(LogTemp, Warning, TEXT("Dropping held item: %s"), *CurrentlyHeldItem->GetName());

        // Detach the item from the player and keep its current world transform
        CurrentlyHeldItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

        // Re-enable physics and collision for the dropped item
        if (UStaticMeshComponent* ItemMesh = CurrentlyHeldItem->FindComponentByClass<UStaticMeshComponent>())
        {
            // Ensure physics and gravity are enabled
            ItemMesh->SetSimulatePhysics(true);
            ItemMesh->SetEnableGravity(true);
            ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

            // Optionally, apply a small force to make the item "drop" more naturally
            FVector DropImpulse = PlayerCamera->GetForwardVector() * 200.0f;  // Adjust force as needed
            ItemMesh->AddImpulse(DropImpulse);
        }

        // Clear the reference to the held item
        CurrentlyHeldItem = nullptr;
    }
    else
    {
        // Perform the line trace to interact with objects
        APlayerController* PlayerController = Cast<APlayerController>(GetController());
        if (PlayerController)
        {
            FVector CameraLocation;
            FRotator CameraRotation;
            PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

            FVector LineTraceEnd = CameraLocation + (CameraRotation.Vector() * InteractLineTraceDistance);

            // Draw debug line to visualize the line trace
            DrawDebugLine(GetWorld(), CameraLocation, LineTraceEnd, FColor::Green, false, 2.0f, 0, 2.0f);

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

                    // If the hit actor has a StaticMeshComponent, treat it as the item
                    if (UStaticMeshComponent* ItemMesh = HitActor->FindComponentByClass<UStaticMeshComponent>())
                    {
                        CurrentlyHeldItem = HitActor;
                        UE_LOG(LogTemp, Warning, TEXT("Picking up item: %s"), *HitActor->GetName());

                        // Attach the item to the HandComponent
                        CurrentlyHeldItem->AttachToComponent(HandComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

                        // Disable physics and collision while holding the item
                        ItemMesh->SetSimulatePhysics(false);
                        ItemMesh->SetEnableGravity(false);
                        ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
                    }
                }
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("No valid interactable object hit"));
            }
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

