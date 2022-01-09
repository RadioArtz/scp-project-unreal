

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LayoutGenerator_Main.generated.h"

UCLASS()
class SCPPU_API ALayoutGenerator_Main : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALayoutGenerator_Main();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
