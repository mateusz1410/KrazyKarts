// Fill out your copyright notice in the Description page of Project Settings.


#include "GoKart.h"
#include "Components/InputComponent.h"
#include "WheeledVehicleMovementComponent4W.h"
#include "WheeledVehicle.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h" //add when replicated varaible
#include "GameFramework/GameStateBase.h"
#include "GoKartMovementComponent.h"
#include "GoKartMovementReplicator.h"

// Sets default values
AGoKart::AGoKart()
{
	MovementComponent = CreateDefaultSubobject<UGoKartMovementComponent>(TEXT("MovementComponent"));
	MovementReplicator = CreateDefaultSubobject<UGoKartMovementReplicator>(TEXT("MovementReplicator"));

 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Speed = 0.f;

	bReplicates = true; //how actor need to be replicated
	bReplicateMovement = false;

}

// Called when the game starts or when spawned
void AGoKart::BeginPlay()
{
	Super::BeginPlay();
	SetReplicateMovement(false);


	//UE_LOG(LogTemp, Warning, TEXT("NAME TEST %s"), *GetName());

	if (HasAuthority())
	{
		NetUpdateFrequency = 1; // 10 time per second for this actor update value from sever
	}
}


FString GetEnumText(ENetRole Role)
{
	switch (Role)
	{
	case ROLE_None: return "ROLE_None";
		break;
	case ROLE_SimulatedProxy: return "ROLE_SimulatedProxy";
		break;
	case ROLE_AutonomousProxy: return "ROLE_AutonomousProxy";
		break;
	case ROLE_Authority: return "ROLE_Authority";
		break;
	default:  return "Error";
		break;
	}
}
// Called every frame
void AGoKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//DrawDebugString(GetWorld(), FVector(0, 0, 100), GetEnumText(Role), this,FColor::White,DeltaTime);
}
/*
	void AGoKart::OnRep_ReplicatedLocation() //call every time when server update property 
	{
		UE_LOG(LogTemp, Warning, TEXT("ReplicatedLocation"));
	}
*/
/*
void AGoKart::OnRep_ReplicatedTransform() // only run on client,  when server update property 
{
	SetActorTransform(ReplicatedTransform);
}
*/

// Called to bind functionality to input
void AGoKart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AGoKart::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGoKart::MoveRight);
	PlayerInputComponent->BindAxis("LookUp");
	PlayerInputComponent->BindAxis("LookRight");

	PlayerInputComponent->BindAction("Handbrake", IE_Pressed, this, &AGoKart::OnHandbrakePressed);
	PlayerInputComponent->BindAction("Handbrake", IE_Released, this, &AGoKart::OnHandbrakeReleased);
	//PlayerInputComponent->BindAction("SwitchCamera", IE_Pressed, this, &AGoKart::OnToggleCamera);

}

void AGoKart::MoveForward(float Value)
{
	if (MovementComponent == nullptr) return;
	MovementComponent->SetThrottle(Value);
	//Server_MoveForward(Value);
}

//void AGoKart::Server_MoveForward_Implementation(float Value)
//{
//	Throttle = Value;
//	//Velocity = GetActorForwardVector() * 20 * Value;
//	//GetVehicleMovementComponent()->SetThrottleInput(Value);
//}
//bool AGoKart::Server_MoveForward_Validate(float Value)
//{
//	return Value >= -1 && Value <= 1; //FMath::(Value) <= 1;
//}

void AGoKart::MoveRight(float Value)
{
	if (MovementComponent == nullptr) return;
	MovementComponent->SetSteeringThrow(Value);
	//Server_MoveRight(Value);
}

//void AGoKart::Server_MoveRight_Implementation(float Value)
//{
//	SteeringThrow = Value;
//	//GetVehicleMovementComponent()->SetSteeringInput(Value);
//}
//bool AGoKart::Server_MoveRight_Validate(float Value)
//{
//	return Value >= -1 && Value <=1;
//}

void AGoKart::OnHandbrakePressed()
{
	//GetVehicleMovementComponent()->SetHandbrakeInput(true);
}

void AGoKart::OnHandbrakeReleased()
{
	//GetVehicleMovementComponent()->SetHandbrakeInput(false);
}
