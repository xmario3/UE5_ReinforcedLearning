// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "math/vector.h"

#include "SocketSubsystem.h"
#include "Sockets.h"
#include "Networking.h"
#include "IPAddress.h"

#include "C_PyConnection.generated.h"

UCLASS()
class UE5_RL_PYCONNECT_API AC_PyConnection : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AC_PyConnection();
	~AC_PyConnection();
	
	// oggetti di rete
	UPROPERTY(BlueprintReadOnly)
	bool isConnectionOn = false;

	UPROPERTY(BlueprintReadOnly)
	int ContatorePacchettiRicevuti = 0;

	UPROPERTY(BlueprintReadOnly)
	int ContatorePacchettiMandati = 0;
	
	UFUNCTION(BlueprintCallable)
	bool TryConnection();

	UFUNCTION(BlueprintCallable)
	bool TestSendMessage();

	UPROPERTY(EditAnywhere)
	FString PythonServerIP = TEXT("127.0.0.1");

	uint8* B_DatiDaRitornare = new uint8[44];
	TArray<float> DatiDaRitornare;

	uint8* DatiRicevuti = new uint8[32];
	FSocket* SocketConPython;
	int32 QuantiDatiLetti = 0;
	uint32 QuantiDatiLeggere = 0;
	int32 BytesSent = 0;
	
	// dati tecnici
	bool isPendingLettura = false;
	float Distanza_robot_TA;
	float Distanza_robot_TB;

	UPROPERTY(BlueprintReadWrite)
	bool isCollisionOccurred = false;
	

	// dati raccolti da mandare
	FVector* Target_Coord	= new FVector(0.0, 0.0, 0.0);
	FVector* EE_Coord		= new FVector(0.0, 0.0, 0.0);
	FQuat* EE_Rotator		= new FQuat(0.0, 0.0, 0.0,0.0);
	// 0.0 ok; 1.0 Collision
	float AuxCom = 0.0; 
	
	//oggetti in scena
	UPROPERTY(EditAnywhere)
	AActor* Robot_Base;
	UPROPERTY(EditAnywhere)
	AActor* Robot_Start;
	UPROPERTY(EditAnywhere)
	AActor* Robot_Arm1;
	UPROPERTY(EditAnywhere)
	AActor* Robot_Arm2;
	UPROPERTY(EditAnywhere)
	AActor* Robot_Arm3;
	UPROPERTY(EditAnywhere)
	AActor* Robot_Arm4;
	UPROPERTY(EditAnywhere)
	AActor* Robot_End;
	UPROPERTY(EditAnywhere)
	AActor* Robot_EEffector;
	UPROPERTY(EditAnywhere)
	AActor* Target_A;

	//dati di Learning
	UPROPERTY(BlueprintReadWrite)
	TArray<float> Robot_Angoli_Ricevuti;

	TArray<FRotator> Robot_Angoli_Rotatori;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	int ParseAngoliArrivati(uint8*);
	void PreparaRispostaDati();

public:	

	UFUNCTION(BlueprintNativeEvent)
	void InitializeTarget();

	void InitializeTarget_Implementation();



	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
