#pragma once

#include "CoreMinimal.h"
#include "UInteractableInterface.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class AEthanCharacter;

UCLASS()
class DISPLACED_API AItem : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
    
public:    
	AItem();

	UPROPERTY(EditAnywhere, Category = "Interaction")
	UStaticMeshComponent* itemMesh;

	UPROPERTY(EditAnywhere, Category = "Interaction")
	USphereComponent* sphereTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FVector PickupOffset = FVector(0.f, 0.f, 0.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FRotator PickupRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float ThrowForce = 5000.f;

	UPROPERTY(EditAnywhere)
	bool bIsPickedUp;
    
	AActor* Holder;

protected:
	virtual void BeginPlay() override;

public:    
	virtual void Tick(float DeltaTime) override;

	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual bool BIsHeldItem_Implementation() override;

	void Drop(AEthanCharacter* EthanCharacter);
	void Pickup(AEthanCharacter* EthanCharacter);

private:
	// Store the original scale of the item
	FVector OriginalScale;
};