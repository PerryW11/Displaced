// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UInteractableInterface.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class USphereComponent;
class USceneComponent;

UCLASS()
class DISPLACED_API AItem : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

	UPROPERTY(EditAnywhere, Category = "Interaction")
	UStaticMeshComponent* itemMesh;

	UPROPERTY(EditAnywhere, Category = "Interaction")
	USphereComponent* sphereTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FVector PickupOffset;

	UPROPERTY(EditAnywhere)
	bool bIsPickedUp;
	
	AActor* Holder;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Implement Interact_PickUp from the interface
	virtual void Interact_Implementation(AActor* Interactor) override;

};
