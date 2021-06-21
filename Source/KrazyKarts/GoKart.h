// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GoKartMovementComponent.h"
#include "GoKart.generated.h"


UCLASS()
class KRAZYKARTS_API AGoKart : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGoKart();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;



	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UGoKartMovementComponent* MovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UGoKartMovementReplicator* MovementReplicator;

private:

/* 
* Replicate 
	
	//UPROPERTY(Replicated) // normal replictation GetLifetimeReplicatedProps, check value ervery  NetUpdateFrequency = 10; // 10 time per second
	//FVector ReplicatedLocation;

	//	----!!!!----
	UPROPERTY(ReplicatedUsing = OnRep_ReplicatedLocation)  // ReplicatedUsing Replicated using  owne function
	FVector ReplicatedLocation;

	
	UFUNCTION()
	void OnRep_ReplicatedLocation(); // !!!---------------!!!----------!!!

	UPROPERTY(Replicated) // mark variable to replicate, need to set property only in .CPP file
	FRotator ReplicatedRotation;
	//UPROPERTY(ReplicatedUsing = OnRep_ReplicatedTransform) //calling only on client
	//FTransform ReplicatedTransform;

	//UFUNCTION()
	//void OnRep_ReplicatedTransform();

	

	//UPROPERTY(EditAnywhere) //trun in place use for spaceship
	//float MaxDegreesPerSecond; // max turn value per sec (dergees /sec) (good for flying object)


//	UFUNCTION(Server, Reliable, WithValidation) //Server - call on server, Reliable - is important, try as long as need to replicate, WithValidation - check that client doesn't cheating
	//void Server_MoveForward(float Value);

	//UFUNCTION(Server, Reliable, WithValidation) // if server need to be WithValidation
	//void Server_MoveRight(float Value);
	*/ 

private:

	float Speed;

	void OnHandbrakePressed();
	void OnHandbrakeReleased();

	void MoveForward(float Value);
	void MoveRight(float Value);

};
