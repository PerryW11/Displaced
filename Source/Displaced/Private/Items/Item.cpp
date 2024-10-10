#include "Items/Item.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Ethan/EthanCharacter.h"
#include "Kismet/GameplayStatics.h"

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
        UE_LOG(LogTemp, Warning, TEXT("Item picked up, spawning new item"));

        AItem* OriginalItem = this;
        FTransform OriginalTransform = OriginalItem->GetActorTransform();  // Save the original transform

        // Spawn a new instance of the same object
        UWorld* World = GetWorld();
        if (World)
        {
            TSubclassOf<AItem> ItemClass = OriginalItem->GetClass();
            
            // Spawn the new item at the original location (or adjust if needed)
            AItem* NewItem = World->SpawnActor<AItem>(ItemClass, OriginalTransform);

            if (NewItem)
            {
                // Log the root component to ensure it's correct
                UE_LOG(LogTemp, Warning, TEXT("Root Component of New Item: %s"), *NewItem->GetRootComponent()->GetName());

                // Set the item's owner to the player character
                NewItem->SetOwner(Interactor);

                // Disable physics and reset velocities before attaching
                if (NewItem->itemMesh)
                {
                    NewItem->itemMesh->SetSimulatePhysics(false);
                    NewItem->itemMesh->SetEnableGravity(false);
                    NewItem->itemMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);  // Reset velocity
                    NewItem->itemMesh->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);  // Reset angular velocity
                }

                // Cast Interactor to your character class
                AEthanCharacter* EthanCharacter = Cast<AEthanCharacter>(Interactor);
                if (EthanCharacter && EthanCharacter->HandComponent)
                {
                    // Log the name of the component to ensure it's the HandComponent
                    UE_LOG(LogTemp, Warning, TEXT("Attaching to HandComponent: %s"), *EthanCharacter->HandComponent->GetName());

                    // Attach the new item to the player's HandComponent
                    NewItem->AttachToComponent(EthanCharacter->HandComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

                    // Reset the item's relative location, scale, and rotation
                    NewItem->SetActorRelativeLocation(FVector::ZeroVector);
                    NewItem->SetActorRelativeRotation(FRotator::ZeroRotator);

                    UE_LOG(LogTemp, Warning, TEXT("New item successfully spawned and attached to HandComponent"));
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("HandComponent not found on EthanCharacter!"));
                }

                // Now that the new item is attached, destroy the original item
                OriginalItem->Destroy();
            }
        }
    }
    else
    {
        // Drop logic here
        UE_LOG(LogTemp, Warning, TEXT("Item dropped by: %s"), *Interactor->GetName());
        bIsPickedUp = false;
        Holder = nullptr;

        UWorld* World = GetWorld();
        if (World)
        {
            FTransform DropTransform = GetActorTransform();  // Get the current transform

            // Spawn a new instance of the item at the drop location
            TSubclassOf<AItem> ItemClass = this->GetClass();
            AItem* DroppedItem = World->SpawnActor<AItem>(ItemClass, DropTransform);

            if (DroppedItem)
            {
                // Re-enable physics so the item interacts with the world after being dropped
                if (DroppedItem->itemMesh)
                {
                    DroppedItem->itemMesh->SetSimulatePhysics(true);
                    DroppedItem->itemMesh->SetEnableGravity(true);  // Ensure gravity is enabled
                }

                UE_LOG(LogTemp, Warning, TEXT("New item successfully dropped in the world"));
            }

            // Destroy the instance that was attached to the player's hand
            Destroy();  // Remove the hand-attached version of the item
        }
    }
}


void AItem::BeginPlay()
{
	Super::BeginPlay();
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


