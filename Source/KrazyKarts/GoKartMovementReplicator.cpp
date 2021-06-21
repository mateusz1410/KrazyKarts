// Fill out your copyright notice in the Description page of Project Settings.


#include "GoKartMovementReplicator.h"
#include "Net/UnrealNetwork.h" //add when replicated varaible
#include "GameFramework/Actor.h"
#include "Engine/World.h"


// Sets default values for this component's properties
UGoKartMovementReplicator::UGoKartMovementReplicator()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicated(true);

	// ...
}


// Called when the game starts
void UGoKartMovementReplicator::BeginPlay()
{
	Super::BeginPlay();	

	MovementComponent = GetOwner()->FindComponentByClass<UGoKartMovementComponent>();
}


// Called every frame
void UGoKartMovementReplicator::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (MovementComponent == nullptr) return;

	FGoKartMove LastMove = MovementComponent->GetLastMove();

	if (GetOwnerRole() == ROLE_AutonomousProxy)
	{
		UnacknowledgeMoves.Add(LastMove); //array of moves
		Server_SendMove(LastMove);
	}

	//APawn* Owner = Cast<APawn>(GetOwner());
	//if (!Owner) return;
	// we are the server and in control of the pawn 
	if (GetOwner()->GetRemoteRole() == ROLE_SimulatedProxy/*Owner->IsLocallyControlled()*/)
	{
		UpdateServerState(LastMove);
		//FGoKartMove Move = MovementComponent->CreateMove(DeltaTime);
		//Server_SendMove(Move);
	}

	if (GetOwner()->Role == ROLE_SimulatedProxy)
	{
		//MovementComponent->SimulateMove(ServerState.LastMove);
		ClientTick(DeltaTime);
	}

	//	!!!-------------!!!----------------!!!
	//everything works many time (when 2 player + server) -  player0 all check: checkPlayer0 -rep, checkPlayer2 -rep, checkServer (no Authotithy - so copy /rep value) 
	//for server all check: check player0 - setvalue, player2 - setValue, Server - setvalue (Authority to all set variable everywhere)
	//	!!!-------------!!!----------------!!!

	//if (HasAuthority()) // role authority server, check exec for every instance of game for each playere 
	//{
		//specific actor, not localy controlled 
		//(not server, but each client owne location)
		//ServerState.LastMove =
	//}
}
void UGoKartMovementReplicator::ClientTick(float DeltaTime)
{
	ClientTimeSinceUpdate += DeltaTime;

	if (ClientTimeBetweenLastUpdate < KINDA_SMALL_NUMBER) return; // KINDA_SMALL_NUMBER - constant value close to 0  (0,0001) //SMALL_NUMBER  - constant value close to 0 smaller than KINDA_SMALL_NUMBER (0,00000001)
	if (MovementComponent == nullptr) return;

	float LerpRatio = ClientTimeSinceUpdate / ClientTimeBetweenLastUpdate;
	FHermiteCubicSpline Spline = CreateSpline();
	
	//FVector NewLocation = FMath::LerpStable(StartLocation, TargetLocation, LerpRatio);
	InterpolateLocation(Spline, LerpRatio);
	
	InterpolateVelocity(Spline, LerpRatio);

	InterpolateRotation(LerpRatio);

}


FHermiteCubicSpline UGoKartMovementReplicator::CreateSpline()
{

	FHermiteCubicSpline Spline;
	// lerp location
	Spline.TargetLocation = ServerState.Transform.GetLocation();
	Spline.StartLocation = ClientStartTransform.GetLocation();

	Spline.StartDerivative = ClientStartVelocity * GetVelocityToDerivative();
	Spline.TargetDerivative = ServerState.Velocity * GetVelocityToDerivative(); //velocity is in m/2 but location is cm

	return Spline;
}

void UGoKartMovementReplicator::InterpolateRotation(float LerpRatio)
{
	// lerp rotation
	FQuat TargetRotation = ServerState.Transform.GetRotation();
	FQuat StartRotation = ClientStartTransform.GetRotation();

	FQuat NewRotation = FQuat::Slerp(StartRotation, TargetRotation, LerpRatio);

	if (MeshOffsetRoot != nullptr)
	{
		MeshOffsetRoot->SetWorldRotation(NewRotation);

	}
}

void UGoKartMovementReplicator::InterpolateVelocity(const FHermiteCubicSpline &Spline, float LerpRatio)
{
	FVector NewDerivative = Spline.InterpolateDerivative(LerpRatio);
	FVector NewVelocity = NewDerivative / GetVelocityToDerivative();
	MovementComponent->SetVelocity(NewVelocity); //smoth change velocity when turn
}

void UGoKartMovementReplicator::InterpolateLocation(const FHermiteCubicSpline &Spline, float LerpRatio)
{
	FVector NewLocation = Spline.InterpolateLocation(LerpRatio);

	if (MeshOffsetRoot != nullptr)
	{
		MeshOffsetRoot->SetWorldLocation(NewLocation);

	}
}

float UGoKartMovementReplicator::GetVelocityToDerivative()
{
	return ClientTimeBetweenLastUpdate * 100;
}

void UGoKartMovementReplicator::UpdateServerState(const FGoKartMove& Move)
{
	ServerState.LastMove = Move;
	ServerState.Transform = GetOwner()->GetActorTransform();
	ServerState.Velocity = MovementComponent->GetVelocity();
}


//replicated property only i .CPP not int .h, no need, unless use UPROPERTY(ReplicatedUsing = OnRep_...)
void UGoKartMovementReplicator::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps); // need to be always

	/*
	DOREPLIFETIME(AGoKart, ReplicatedLocation);
	DOREPLIFETIME(AGoKart, ReplicatedRotation); //what and how replicated ( DOREPLIFETIME className, VariableName)
	*/
	//DOREPLIFETIME(AGoKart, ReplicatedTransform);
	//DOREPLIFETIME(AGoKart, Velocity);

	DOREPLIFETIME(UGoKartMovementReplicator, ServerState);

}

void UGoKartMovementReplicator::OnRep_ServerState()
{

	switch (GetOwnerRole())
	{
	case ROLE_AutonomousProxy: Autonomous_OnRep_ServerState();
		break;
	
	case ROLE_SimulatedProxy: SimulatedProxy_OnRep_ServerState();
		break;

	default:
		break;
	}
	
	
}

void UGoKartMovementReplicator::Autonomous_OnRep_ServerState()
{
	if (MovementComponent == nullptr) return;

	GetOwner()->SetActorTransform(ServerState.Transform);
	MovementComponent->SetVelocity(ServerState.Velocity);

	ClearAcknowledgeMoves(ServerState.LastMove);

	for (const FGoKartMove& Move : UnacknowledgeMoves)
	{
		MovementComponent->SimulateMove(Move);
	}
}
void UGoKartMovementReplicator::SimulatedProxy_OnRep_ServerState()
{
	if (MovementComponent == nullptr) return;
	ClientTimeBetweenLastUpdate = ClientTimeSinceUpdate;
	ClientTimeSinceUpdate = 0;

	if (MeshOffsetRoot != nullptr)
	{
		ClientStartTransform.SetLocation(MeshOffsetRoot->GetComponentLocation());
		ClientStartTransform.SetRotation(MeshOffsetRoot->GetComponentQuat());
	}
	ClientStartVelocity = MovementComponent->GetVelocity();

	GetOwner()->SetActorTransform(ServerState.Transform);
}


void UGoKartMovementReplicator::ClearAcknowledgeMoves(FGoKartMove LastMove)
{
	TArray<FGoKartMove> NewMoves;
	for (const FGoKartMove& Move : UnacknowledgeMoves)
	{
		if (Move.Time > LastMove.Time)
		{
			NewMoves.Add(Move);
		}
	}

	UnacknowledgeMoves = NewMoves;
}


void UGoKartMovementReplicator::Server_SendMove_Implementation(FGoKartMove Move)
{
	if (MovementComponent == nullptr) return;

	ClientSimulatedTime += Move.DeltaTime;
	MovementComponent->SimulateMove(Move);

	UpdateServerState(Move);
}

bool UGoKartMovementReplicator::Server_SendMove_Validate(FGoKartMove Move)
{
	float ProposedTime = ClientSimulatedTime + Move.DeltaTime;

	bool ClientNotRunningAhead = ProposedTime < GetWorld()->TimeSeconds;
	if (!ClientNotRunningAhead) 
	{
		UE_LOG(LogTemp, Error, TEXT("Client is running too fast."));
		return false;
	}
	if (!Move.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Received invalid move."));
		return false;
	}

	return true;
}