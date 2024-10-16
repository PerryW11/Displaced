#include "Items/Item.h"

#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Ethan/EthanCharacter.h"

AItem::AItem()
{
    PrimaryActorTick.bCanEverTick = true;

    // Initialize static mesh component for the item
    itemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
    RootComponent = itemMesh;

    // Initialize sphere trigger for interaction
    sphereTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("SphereTrigger"));
    sphereTrigger->SetupAttachment(itemMesh);

    bIsPickedUp = false;
}

void AItem::Interact_Implementation(AActor* Interactor)
{
    if (!bIsPickedUp)
    {
        Pickup(Interactor);
    }
    else
    {
        Drop(Interactor);
    }
}

void AItem::Pickup(AActor* Interactor)
{
    AEthanCharacter* EthanCharacter = Cast<AEthanCharacter>(Interactor);
    if (EthanCharacter && EthanCharacter->HandComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("Ethan and HandComponent are valid"));
        // Attach the item to the player's HandComponent
        AttachToComponent(EthanCharacter->HandComponent, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true));

        // Disable physics and collision while holding the item
        if (itemMesh)
        {
            UE_LOG(LogTemp, Warning, TEXT("Item mesh is valid, disabling physics"));
            itemMesh->SetSimulatePhysics(false);
            itemMesh->SetEnableGravity(false);
            itemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        }

        bIsPickedUp = true;
        UE_LOG(LogTemp, Warning, TEXT("Item picked up by: %s"), *Interactor->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to pick up item: Interactor is not a valid AEthanCharacter or HandComponent is missing."));
    }
}


void AItem::Drop(AActor* Interactor)
{
    // Detach the item from the player's hand
    DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

    // Re-enable physics so the item interacts with the world after being dropped
    if (itemMesh)
    {
        itemMesh->SetSimulatePhysics(true);
        itemMesh->SetEnableGravity(true);
        itemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    }

    AEthanCharacter* EthanCharacter = Cast<AEthanCharacter>(Interactor);
    UCameraComponent* PlayerCamera = EthanCharacter->GetPlayerCamera();
    if(EthanCharacter && PlayerCamera)
    itemMesh->AddImpulse(PlayerCamera->GetForwardVector() * 2000);

    bIsPickedUp = false;
    UE_LOG(LogTemp, Warning, TEXT("Item dropped"));
}


void AItem::BeginPlay()
{
    Super::BeginPlay();
}

void AItem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}
