#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UInteractableInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

class DISPLACED_API IInteractableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void Interact(AActor* Interactor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	bool BIsHeldItem();

	virtual bool BIsHeldItem_Implementation()
	{
		return false;
	}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	FVector ReturnFocalPoint();

	virtual FVector ReturnFocalPoint_Implementation()
	{
		FVector vector(0,0,0);
		return vector; 
	}
};
