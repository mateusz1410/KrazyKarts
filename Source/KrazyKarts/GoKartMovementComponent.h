// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GoKartMovementComponent.generated.h"

USTRUCT()
struct FGoKartMove
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	float Throttle; // acceleration pedal

	UPROPERTY()
	float SteeringThrow; // turn rate

	UPROPERTY()
	float DeltaTime; // what was the delta

	UPROPERTY()
	float Time; // when time start
	

	bool IsValid() const
	{
		return FMath::Abs(Throttle) <= 1 && FMath::Abs(SteeringThrow) <= 1;
	}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KRAZYKARTS_API UGoKartMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGoKartMovementComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SimulateMove(const FGoKartMove& Move);


	 FORCEINLINE FVector GetVelocity() { return Velocity; }
	 FORCEINLINE void SetVelocity(FVector Val) { Velocity = Val; }

	 FORCEINLINE void SetThrottle(float Val) { Throttle = Val; }
	 FORCEINLINE void SetSteeringThrow(float Val) { SteeringThrow = Val; }

	 FORCEINLINE FGoKartMove GetLastMove() { return LastMove; }

private:

	FGoKartMove CreateMove(float DeltaTime);

	 FVector GetAirResistance();

	 FVector GetRollingResistance();

	 void ApplyRotaion(float DeltaTime, float InputSteeringThrow);

	 void UpdateLocationFromVelocity(float DeltaTime);

	 //The mass of the car(kg)
	 UPROPERTY(EditAnywhere)
	 float Mass;

	 //Force applied to the car when throttle is fully down (acceleration pedal hit flor) (Newton N)
	 UPROPERTY(EditAnywhere)
	 float MaxDrivingForce;

	 //Minimum radius of the car trurning circle at full lock (m), max tutning
	 UPROPERTY(EditAnywhere)
	 float MinTurningRadius; // turn radius, (good for driving object)

	 UPROPERTY(EditAnywhere)
	 float DragCoefficient;  // air block multiplier, higher means more drag (kg / meter)

	 UPROPERTY(EditAnywhere)
	 float RollingResistanceCoefficient; //wheel move on road, higher means more resistance


	 UPROPERTY()
	 FVector Velocity;

	 UPROPERTY(EditAnywhere)
	 float Throttle; // acceleration pedal

	 UPROPERTY(EditAnywhere)
	 float SteeringThrow; // turn rate


	 FGoKartMove LastMove;
};
