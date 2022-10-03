// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "THProp.generated.h"

UCLASS()
class TREASUREHUNTER_API ATHProp : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATHProp();

	// Whether it's objective
	UPROPERTY(EditAnywhere)
	bool bSpecial = false;

	bool bGlowing = false;

	// Basically in kg
	UPROPERTY(EditAnywhere)
	float Mass = 1.0f;

	UPROPERTY(EditAnywhere, Category="Component")
	UStaticMeshComponent* StaticMeshComponent;

	// Whether physics is applied
	UPROPERTY(EditAnywhere)
	bool bMoveable = true;

	FTransform Preset;

	TArray<UMaterialInterface*> DefaultMaterials;

	UPROPERTY(EditAnywhere, Category="Material")
	UMaterialInterface* GlowMaterial;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Reset transform of prop (Reset completely)
	UFUNCTION(BlueprintCallable)
	void ResetTransform();

	// Reset rotation of prop
	UFUNCTION(BlueprintCallable)
	void ResetRotation(bool b_only_wake_up);

	// Change glowing state
	UFUNCTION(BlueprintCallable)
	void ChangeGlow();

	// Change Mass
	UFUNCTION(BlueprintCallable)
	void SetMassScale(float NewMass);

	// Add force to specific direction
	UFUNCTION(BlueprintCallable)
	void AddCustomForce(FVector direction, int32 force = 100);
};
