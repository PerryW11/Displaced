#include "Items/Item.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
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
        // Picking up logic
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

                // Disable physics before attaching
                if (NewItem->itemMesh)
                {
                    NewItem->itemMesh->SetSimulatePhysics(false);
                    NewItem->itemMesh->SetEnableGravity(false);
                }

                // Attach the item to the player's hand component
                USceneComponent* HandComponent = Cast<USceneComponent>(Interactor->GetComponentByClass(USceneComponent::StaticClass()));
                if (HandComponent)
                {
                    // Attach the new item to the player's hand
                    NewItem->AttachToComponent(HandComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

                    // Explicitly set the location after attachment
                    NewItem->SetActorLocation(HandComponent->GetComponentLocation());

                    UE_LOG(LogTemp, Warning, TEXT("New item successfully spawned and attached to player hand at location: %s"), *NewItem->GetActorLocation().ToString());
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


