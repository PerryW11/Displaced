#include "Items/Item.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Ethan/EthanCharacter.h"

AItem::AItem()
{
    PrimaryActorTick.bCanEverTick = true;

    itemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
    RootComponent = itemMesh;

    sphereTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("SphereTrigger"));
    sphereTrigger->SetupAttachment(itemMesh);

    bIsPickedUp = false;
    OriginalScale = FVector(1.f, 1.f, 1.f); // Default, overridden in BeginPlay
}

void AItem::Interact_Implementation(AActor* Interactor)
{
    AEthanCharacter* EthanCharacter = Cast<AEthanCharacter>(Interactor);
    if (EthanCharacter)
    {
        if (!EthanCharacter->CurrentlyHeldItem)
        {
            Pickup(EthanCharacter);
        }
        else if (EthanCharacter->CurrentlyHeldItem == this)
        {
            Drop(EthanCharacter);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("EthanCharacter is already holding another item"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("EthanCharacter pointer is null"));
    }
}

bool AItem::BIsHeldItem_Implementation()
{
    return bIsPickedUp;
}

void AItem::Pickup(AEthanCharacter* EthanCharacter)
{
    if (EthanCharacter && EthanCharacter->HandComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("Ethan and HandComponent are valid"));

        // Log initial scale
        FVector CurrentScale = GetActorScale3D();
        UE_LOG(LogTemp, Warning, TEXT("Current item scale: %s"), *CurrentScale.ToString());

        // Attach the item to the HandComponent
        AttachToComponent(EthanCharacter->HandComponent, 
            FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), 
            NAME_None);

        // Log scale after attachment
        FVector PostAttachScale = GetActorScale3D();
        UE_LOG(LogTemp, Warning, TEXT("Scale after attachment: %s"), *PostAttachScale.ToString());

        // Calculate required relative scale to maintain OriginalScale
        FVector ParentScale = EthanCharacter->HandComponent->GetComponentScale();
        UE_LOG(LogTemp, Warning, TEXT("HandComponent scale: %s"), *ParentScale.ToString());
        FVector RelativeScale = OriginalScale / ParentScale; // Element-wise division

        // Set the relative transform
        FTransform AttachTransform;
        AttachTransform.SetLocation(PickupOffset);
        AttachTransform.SetRotation(PickupRotation.Quaternion());
        AttachTransform.SetScale3D(RelativeScale);
        SetActorRelativeTransform(AttachTransform);

        // Log final scale
        FVector FinalScale = GetActorScale3D();
        UE_LOG(LogTemp, Warning, TEXT("Scale after transform: %s"), *FinalScale.ToString());

        // Disable physics and collision
        if (itemMesh)
        {
            UE_LOG(LogTemp, Warning, TEXT("Item mesh is valid, disabling physics"));
            itemMesh->SetSimulatePhysics(false);
            itemMesh->SetEnableGravity(false);
            itemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        }

        bIsPickedUp = true;
        Holder = EthanCharacter;
        EthanCharacter->CurrentlyHeldItem = this;
        UE_LOG(LogTemp, Warning, TEXT("Item picked up by: %s"), *EthanCharacter->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to pick up item: Interactor is not a valid AEthanCharacter or HandComponent is missing."));
    }
}

void AItem::Drop(AEthanCharacter* EthanCharacter)
{
    DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

    if (itemMesh)
    {
        itemMesh->SetSimulatePhysics(true);
        itemMesh->SetEnableGravity(true);
        itemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    }

    if (EthanCharacter && EthanCharacter->GetPlayerCamera())
    {
        UCameraComponent* PlayerCamera = EthanCharacter->GetPlayerCamera();
        itemMesh->AddImpulse(PlayerCamera->GetForwardVector() * ThrowForce);
        UE_LOG(LogTemp, Warning, TEXT("Item thrown with force: %f"), ThrowForce);
    }

    bIsPickedUp = false;
    Holder = nullptr;
    EthanCharacter->CurrentlyHeldItem = nullptr;
    UE_LOG(LogTemp, Warning, TEXT("Item dropped"));
}

void AItem::BeginPlay()
{
    Super::BeginPlay();
    OriginalScale = GetActorScale3D(); // Capture the item's initial scale
    UE_LOG(LogTemp, Warning, TEXT("Original item scale set: %s"), *OriginalScale.ToString());
}

void AItem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}