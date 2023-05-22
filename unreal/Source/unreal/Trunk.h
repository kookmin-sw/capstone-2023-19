// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Trunk.generated.h"

UENUM(BlueprintType)
enum class TrunkMaterialType : uint8
{
	None = 0,
	Beech_I,
	Beech_II,
	Beech_III,
	Beech_IV,
	Beech_V,
	Beech_VI,
	Birch_I,
	Birch_II,
	Birch_III,
	Birch_IV,
	Cheery_I,
	Cheery_II,
	Cheery_III,
	Cheery_IV,
	Cheery_V,
	Chestnut_I,
	Chestnut_II,
	Fir_I,
	Fir_II,
	Fir_III,
	Fir_IV,
	Fir_V,
	Oak_I,
	Oak_II,
	Oak_III,
	Pine_I,
	Pine_II,
	Pine_III,
	Pine_IV,
	Pine_V,
	Populus_I,
	Populus_II,
};

UCLASS()
class UNREAL_API ATrunk : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATrunk();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	wchar_t* GetTrunkMaterialPath() const;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SetTrunkMaterial(TrunkMaterialType type);

private:
	TrunkMaterialType TrunkType;
	const wchar_t* TrunkMaterialPath;

	UStaticMeshComponent* Mesh;
};
