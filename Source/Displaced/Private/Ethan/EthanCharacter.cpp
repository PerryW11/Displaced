// Fill out your copyright notice in the Description page of Project Settings.


#include "Ethan/EthanCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Items/UInteractableInterface.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Dialogue/DialogueManager.h"
#include "UI/Dialogue/DialogueWidget.h"
#include "Math/Quat.h"


AEthanCharacter::AEthanCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
    
	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Player Camera"));
	PlayerCamera->SetupAttachment(RootComponent);
	PlayerCamera->bUsePawnControlRotation = true;

	HandComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Hand"));
	HandComponent->SetupAttachment(PlayerCamera);

	CurrentPlayerMode = EPlayerMode::Normal;
	MaxLookAngle = 30.0f;
	LastLookInput = FVector2D::ZeroVector;
}

// Called when the game starts or when spawned
void AEthanCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(EthanContext, 0);
			UE_LOG(LogTemp, Warning, TEXT("Input mapping context added"));
		}
	}
}

void AEthanCharacter::Move(const FInputActionValue& Value)
{
	if (CurrentPlayerMode == EPlayerMode::Dialogue) return;

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
	if (CurrentPlayerMode == EPlayerMode::Dialogue) return;

	const FVector2D LookAxisVector = Value.Get<FVector2D>();
	AddControllerPitchInput(LookAxisVector.Y);
	AddControllerYawInput(LookAxisVector.X);
}

void AEthanCharacter::Interact()
{
    UE_LOG(LogTemp, Warning, TEXT("Ethan Interact called"));

    if (CurrentPlayerMode == EPlayerMode::Dialogue)
    {
        UE_LOG(LogTemp, Warning, TEXT("Advancing dialogue"));
        DialogueInteract();
        return;
    }
    
    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    if (!PlayerController) return;

    FVector CameraLocation;
    FRotator CameraRotation;
    PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

    FVector LineTraceEnd = CameraLocation + (CameraRotation.Vector() * InteractLineTraceDistance);
    DrawDebugLine(GetWorld(), CameraLocation, LineTraceEnd, FColor::Green, false, 2.0f, 0, 2.0f);

    FCollisionQueryParams TraceParams;
    TraceParams.AddIgnoredActor(this);

    FHitResult Hit;
    bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, CameraLocation, LineTraceEnd, ECC_Visibility, TraceParams);

    if (bHit && Hit.GetActor() && Hit.GetActor()->Implements<UInteractableInterface>())
    {
        AActor* HitActor = Hit.GetActor();
        UE_LOG(LogTemp, Warning, TEXT("Object is interactable: %s"), *HitActor->GetName());
        IInteractableInterface::Execute_Interact(HitActor, this);

        for (const auto& Tag : HitActor->Tags)
        {
            UE_LOG(LogTemp, Warning, TEXT("Actor has tag: %s"), *Tag.ToString());
        }

        if (HitActor->ActorHasTag(FName("Item")))
        {
            UE_LOG(LogTemp, Warning, TEXT("Object has item tag"));
            if (IInteractableInterface::Execute_BIsHeldItem(HitActor))
            {
                CurrentlyHeldItem = HitActor;
                UE_LOG(LogTemp, Warning, TEXT("Picked up item: %s"), *HitActor->GetName());
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Item can't be picked up: %s"), *HitActor->GetName());
            }
        }
        else if (HitActor->ActorHasTag(FName("NPC")))
        {
            ADialogueManager* DialogueManager = Cast<ADialogueManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ADialogueManager::StaticClass()));
            if (DialogueManager)
            {
                FString ConversationID = "test";
                if (DialogueManager->HasConversationBeenCompleted(ConversationID))
                {
                    UE_LOG(LogTemp, Warning, TEXT("Conversation %s already completed, skipping dialogue mode"), *ConversationID);
                    return;
                }
            }

            UE_LOG(LogTemp, Warning, TEXT("Interacting with NPC: %s"), *HitActor->GetName());
            CurrentPlayerMode = EPlayerMode::Dialogue;
            bIsInterpolatingCamera = true;
            PlayerCamera->bUsePawnControlRotation = false;

            TArray<UActorComponent*> HeadComponents = HitActor->GetComponentsByTag(USphereComponent::StaticClass(), FName("Head"));
            if (HeadComponents.Num() > 0)
            {
                if (USphereComponent* HeadSphere = Cast<USphereComponent>(HeadComponents[0]))
                {
                    DialogueLookLocation = HeadSphere->GetComponentLocation();
                    UE_LOG(LogTemp, Warning, TEXT("DialogueLookLocation set to Head sphere: %s"), *DialogueLookLocation.ToString());
                }
            }
            else if (UCapsuleComponent* Capsule = HitActor->FindComponentByClass<UCapsuleComponent>())
            {
                DialogueLookLocation = Capsule->GetComponentLocation() + FVector(0.f, 0.f, Capsule->GetScaledCapsuleHalfHeight());
                UE_LOG(LogTemp, Warning, TEXT("DialogueLookLocation set to capsule top: %s"), *DialogueLookLocation.ToString());
            }
            else
            {
                DialogueLookLocation = HitActor->GetActorLocation() + FVector(0.f, 0.f, 60.f);
                UE_LOG(LogTemp, Warning, TEXT("DialogueLookLocation set to fallback: %s"), *DialogueLookLocation.ToString());
            }
        }
    }
    else if (CurrentlyHeldItem)
    {
        IInteractableInterface::Execute_Interact(CurrentlyHeldItem, this);
        CurrentlyHeldItem = nullptr;
        UE_LOG(LogTemp, Warning, TEXT("Dropped the held item"));
    }
}

void AEthanCharacter::InterpolateCameraToTarget(float DeltaTime)
{
	static float InterpolationTime = 0.f;
	static FRotator StartRotation;

	if (bIsInterpolatingCamera && InterpolationTime == 0.f)
	{
		StartRotation = PlayerCamera->GetComponentRotation();
		PlayerCamera->bUsePawnControlRotation = false;
	}

	FVector CameraLocation = PlayerCamera->GetComponentLocation();
	FVector DirectionToTarget = (DialogueLookLocation - CameraLocation).GetSafeNormal();
	FRotator TargetRotation = FRotationMatrix::MakeFromX(DirectionToTarget).Rotator();

	InterpolationTime += DeltaTime;
	float Alpha = FMath::Clamp(InterpolationTime / 0.5f, 0.f, 1.f);
	FRotator NewRotation = FMath::Lerp(StartRotation, TargetRotation, Alpha);

	PlayerCamera->SetWorldRotation(NewRotation);

	if (Alpha >= 1.0f)
	{
		bIsInterpolatingCamera = false;
		InterpolationTime = 0.f;
		UE_LOG(LogTemp, Warning, TEXT("Interpolation complete, switching to clamped DialogueLook"));
	}
}

// Called every frame
void AEthanCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsInterpolatingCamera)
    {
        InterpolateCameraToTarget(DeltaTime);
        DrawDebugSphere(GetWorld(), DialogueLookLocation, 10.f, 12, FColor::Red, false, -1.f, 0, 1.f);
    }
    else if (CurrentPlayerMode == EPlayerMode::Dialogue)
    {
        UE_LOG(LogTemp, Warning, TEXT("Dialogue mode active, LastLookInput: %s"), *LastLookInput.ToString());

        if (LastLookInput != FVector2D::ZeroVector)
        {
            FVector CameraLocation = PlayerCamera->GetComponentLocation();
            FVector DirectionToTarget = (DialogueLookLocation - CameraLocation).GetSafeNormal();
            FRotator TargetRotation = FRotationMatrix::MakeFromX(DirectionToTarget).Rotator();

            FRotator CurrentRotation = PlayerCamera->GetComponentRotation();
            float LookSpeed = 100.f;

            FRotator NewRotation = CurrentRotation;
            NewRotation.Yaw += LastLookInput.X * DeltaTime * LookSpeed;
            NewRotation.Pitch += LastLookInput.Y * DeltaTime * LookSpeed;

            // Use FQuat with Quaternion() conversion
            FQuat TargetQuat = TargetRotation.Quaternion();
            FQuat NewQuat = NewRotation.Quaternion();
            float AngleBetween = FMath::Acos(TargetQuat | NewQuat); // Use operator| for dot product

            float MaxAngleRadians = FMath::DegreesToRadians(MaxLookAngle);
            if (AngleBetween > MaxAngleRadians)
            {
                FQuat ClampedQuat = FQuat::Slerp(TargetQuat, NewQuat, MaxAngleRadians / AngleBetween);
                NewRotation = ClampedQuat.Rotator();
            }

            PlayerCamera->SetWorldRotation(NewRotation);

            LastLookInput = FVector2D::ZeroVector;
        }
    }
}

// Called to bind functionality to input
void AEthanCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &AEthanCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AEthanCharacter::Look);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AEthanCharacter::Interact);
		EnhancedInputComponent->BindAction(DialogueLookAction, ETriggerEvent::Triggered, this, &AEthanCharacter::DialogueLook);
		EnhancedInputComponent->BindAction(DialogueInteractAction, ETriggerEvent::Started, this, &AEthanCharacter::DialogueInteract);
        
		UE_LOG(LogTemp, Warning, TEXT("Input bindings set up: Move, Look, Interact, DialogueLook, DialogueInteract"));
	}
}

UCameraComponent* AEthanCharacter::GetPlayerCamera()
{
	return PlayerCamera;
}

void AEthanCharacter::DialogueLook(const FInputActionValue& Value)
{
	if (CurrentPlayerMode != EPlayerMode::Dialogue)
	{
		UE_LOG(LogTemp, Warning, TEXT("DialogueLook ignored: Not in dialogue mode"));
		return;
	}

	LastLookInput = Value.Get<FVector2D>();
	UE_LOG(LogTemp, Warning, TEXT("DialogueLook input: %s"), *LastLookInput.ToString());
}

void AEthanCharacter::EndDialogue()
{
	if (CurrentPlayerMode == EPlayerMode::Dialogue)
	{
		FRotator CurrentCameraRotation = PlayerCamera->GetComponentRotation();
		if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
		{
			PlayerController->SetControlRotation(CurrentCameraRotation);
		}
		CurrentPlayerMode = EPlayerMode::Normal;
		bIsInterpolatingCamera = false;
		PlayerCamera->bUsePawnControlRotation = true;
		UE_LOG(LogTemp, Warning, TEXT("Dialogue mode ended, camera rotation preserved"));
	}
}

void AEthanCharacter::DialogueInteract()
{
	if (CurrentPlayerMode != EPlayerMode::Dialogue) return;

	ADialogueManager* DialogueManager = Cast<ADialogueManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ADialogueManager::StaticClass()));
	if (!DialogueManager) return;

	UE_LOG(LogTemp, Warning, TEXT("Dialogue Manager exists, advancing dialogue"));

	if (DialogueManager->IsConversationFinished())
	{
		UE_LOG(LogTemp, Warning, TEXT("Conversation finished, exiting dialogue mode"));
		DialogueManager->CompletedConversations.Add(DialogueManager->CurrentConversationID);
		UE_LOG(LogTemp, Warning, TEXT("Marked conversation %s as completed"), *DialogueManager->CurrentConversationID);
		EndDialogue();

		if (DialogueManager->ActiveDialogueWidget)
		{
			DialogueManager->ActiveDialogueWidget->RemoveFromParent();
			DialogueManager->ActiveDialogueWidget = nullptr;
		}
	}
	else
	{
		DialogueManager->AdvanceDialogue(DialogueManager->CurrentDialogueIndex + 1);
	}
}

void AEthanCharacter::DialogueMove(const FInputActionValue& Value)
{
	if (CurrentPlayerMode != EPlayerMode::Dialogue)
	{
		return;
	}

	const FVector2D MovementVector = Value.Get<FVector2D>();
	if (MovementVector.IsNearlyZero())
	{
		return;
	}

	// Get the direction to the NPC's head
	FVector CameraLocation = PlayerCamera->GetComponentLocation();
	FVector DirectionToTarget = (DialogueLookLocation - CameraLocation).GetSafeNormal();
	FRotator TargetRotation = FRotationMatrix::MakeFromX(DirectionToTarget).Rotator();

	// Calculate the desired movement direction based on input
	const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
	FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	FVector DesiredDirection = (ForwardDirection * MovementVector.Y + RightDirection * MovementVector.X).GetSafeNormal();

	// Clamp the movement direction relative to the NPC
	float MaxMoveAngle = 45.0f;
	float AngleToTarget = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(DesiredDirection, DirectionToTarget)));
	if (AngleToTarget > MaxMoveAngle)
	{
		// Project the desired direction onto the allowed cone
		FVector ClampedDirection = DirectionToTarget.RotateAngleAxis(MaxMoveAngle * (AngleToTarget > 0 ? 1 : -1), FVector::UpVector);
		DesiredDirection = ClampedDirection;
	}

	// Apply movement (reduced speed during dialogue)
	AddMovementInput(DesiredDirection, MovementVector.Size() * 0.5f);
}







