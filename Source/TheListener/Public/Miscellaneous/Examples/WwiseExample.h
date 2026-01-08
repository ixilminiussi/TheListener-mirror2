#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WwiseExample.generated.h"

UCLASS()
class THELISTENER_API AWwiseExample : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWwiseExample();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	class UAkComponent *AkComponent;

	UPROPERTY(EditAnywhere, Category = "Audio")
	class UAkAudioEvent *MusicEvent;
};