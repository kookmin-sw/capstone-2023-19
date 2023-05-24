// Fill out your copyright notice in the Description page of Project Settings.


#include "Leaf.h"

// Sets default values
ALeaf::ALeaf()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESH"));

	// Mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM(TEXT("StaticMesh'/Game/SM_Leaf.SM_Leaf'"));
	if (SM.Succeeded())
	{
		Mesh->SetStaticMesh(SM.Object);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid basic cylinder mesh"));
	}

}

// Called when the game starts or when spawned
void ALeaf::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALeaf::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

