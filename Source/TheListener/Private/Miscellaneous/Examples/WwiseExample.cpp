#include "Miscellaneous/Examples/WwiseExample.h"
#include "AkComponent.h"
#include "AkAudio/Classes/AkGameplayStatics.h"

// Sets default values
AWwiseExample::AWwiseExample()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AkComponent = CreateDefaultSubobject<UAkComponent>(TEXT("AkComponent"));
	AkComponent->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AWwiseExample::BeginPlay()
{
	Super::BeginPlay();

	// Methode 1 : Passer par un UAkAudioComponent
	if(AkComponent)
	{
		FOnAkPostEventCallback nullCallback;
		AkComponent->PostAkEvent(MusicEvent, int32(0), nullCallback);
	}

	// Methode 2 : Passer par la classe statique UAkGameplayStatics
	FOnAkPostEventCallback nullCallback;
	UAkGameplayStatics::PostEvent(MusicEvent, this, int32(0), nullCallback);
}

// Called every frame
void AWwiseExample::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}