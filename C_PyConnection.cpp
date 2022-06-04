#include "C_PyConnection.h"

AC_PyConnection::AC_PyConnection()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//PrimaryActorTick.TickInterval = 1;

	Robot_Angoli_Ricevuti.Init(0, 6);
	Robot_Angoli_Rotatori.Init(FRotator(0.0), 6);

	DatiDaRitornare.Init(0.0, 2);

}

void AC_PyConnection::BeginPlay()
{
	Super::BeginPlay();
	
}

// ------------------------------------------------------- Tick
void AC_PyConnection::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	

	/*
	//se siamo connessi procedo, oppure esco
	if (SocketConPython->GetConnectionState() != SCS_Connected)
	{
		AC_PyConnection::isConnectionOn = false; //aggiorno flag pubblico
		return;
	}
	else {
		AC_PyConnection::isConnectionOn = true; //aggiorno flag pubblico
	}*/

	if (SocketConPython && SocketConPython->GetConnectionState() == SCS_Connected)
	{	
		
		// se nel tik prima ho ricevuto dei dati, faccio rilevazioni e rispondo
		if (isPendingLettura)
		{
			PreparaRispostaDati();

			DatiDaRitornare[0] = Distanza_robot_TA;
			DatiDaRitornare[1] = Distanza_robot_TB;

			B_DatiDaRitornare = reinterpret_cast<uint8*>(DatiDaRitornare.GetData());

			//UE_LOG(LogTemp, Warning, TEXT("Dati da ritornare!!: %f"), DatiDaRitornare);

			//li spedisco
			SocketConPython->Send(B_DatiDaRitornare, 8, BytesSent);

			//GLog->Logf(TEXT("Spedisco Dati!"));

			isPendingLettura = false;
			return;
		}
	
		// se non avevo pendenze, leggo eventuali risposte
		if (SocketConPython->HasPendingData(QuantiDatiLeggere)) {

			//UE_LOG(LogTemp, Warning, TEXT("Dati da leggere!!: %d"), QuantiDatiLeggere);

			if (SocketConPython->Recv(DatiRicevuti, QuantiDatiLeggere, QuantiDatiLetti))
			{
				//UE_LOG(LogTemp, Warning, TEXT("Dati ricevuti!: %d : %d"), QuantiDatiLetti,*DatiRicevuti);
				//conversione dati ricevuti nell'array con gli angoli
				ParseAngoliArrivati(DatiRicevuti);
				isPendingLettura = true;
			
			}

		}

		
	}

	//UE_LOG(LogTemp, Warning, TEXT("Tick: %d"), ContatoreTick);
	ContatoreTick++;

}


// --------------------------------------------------------- Parsing e risposta

int AC_PyConnection::ParseAngoliArrivati(uint8* Messaggio)
{
/*	Robot_Angoli_Ricevuti[0] = *(reinterpret_cast<float*>(Messaggio));
	Robot_Angoli_Ricevuti[1] = *(reinterpret_cast<float*>(Messaggio + 4 ));
	Robot_Angoli_Ricevuti[2] = *(reinterpret_cast<float*>(Messaggio + 8 ));
	Robot_Angoli_Ricevuti[3] = *(reinterpret_cast<float*>(Messaggio + 12));
	Robot_Angoli_Ricevuti[4] = *(reinterpret_cast<float*>(Messaggio + 16));
	Robot_Angoli_Ricevuti[5] = *(reinterpret_cast<float*>(Messaggio + 20));
*/
	//UE_LOG(LogTemp, Warning, TEXT("Dati da ritornare!!: %f %f %f %f %f %f"), Robot_Angoli_Ricevuti[0], Robot_Angoli_Ricevuti[1], Robot_Angoli_Ricevuti[2],
	//																		Robot_Angoli_Ricevuti[3], Robot_Angoli_Ricevuti[4], Robot_Angoli_Ricevuti[5]);

	Robot_Angoli_Rotatori[0] = FRotator( *(reinterpret_cast<float*>(Messaggio)) ,										0.0,							0.0 );
	Robot_Angoli_Rotatori[1] = FRotator( 									0.0 , *(reinterpret_cast<float*>(Messaggio + 4)) ,							90.0);
	Robot_Angoli_Rotatori[2] = FRotator(*(reinterpret_cast<float*>(Messaggio + 8))	,									11.0 ,							0.0	);
	Robot_Angoli_Rotatori[3] = FRotator(*(reinterpret_cast<float*>(Messaggio + 12))	,									-18.0,							-90.0);
	Robot_Angoli_Rotatori[4] = FRotator(*(reinterpret_cast<float*>(Messaggio + 16))	,									0.0 ,							0.0); 
	Robot_Angoli_Rotatori[5] = FRotator(									0.0 , *(reinterpret_cast<float*>(Messaggio + 20)),							0.0); 

	/////////////////////////////// todo settare altre trasformazioni
	Robot_Start->SetActorRelativeRotation(Robot_Angoli_Rotatori[0]);
	Robot_Arm1->SetActorRelativeRotation(Robot_Angoli_Rotatori[1]);
	Robot_Arm2->SetActorRelativeRotation(Robot_Angoli_Rotatori[2]);

	return 0;
}

void AC_PyConnection::PreparaRispostaDati()
{	
	Distanza_robot_TB = FVector::Dist(Robot_End->GetActorTransform().GetLocation(), Target_B->GetActorTransform().GetLocation());

	Distanza_robot_TA = FVector::Dist(Robot_End->GetActorTransform().GetLocation(), Target_A->GetActorTransform().GetLocation());
}

// --------------------------------------------- Connessione

bool AC_PyConnection::TryConnection() {
	bool ConnectionSuccess = false;
	
	// creazione indirizzo per connettersi
	TSharedRef<FInternetAddr> PythonServerAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	FIPv4Address ip;
	FIPv4Address::Parse(PythonServerIP, ip);
	PythonServerAddr->SetIp(ip.Value);
	PythonServerAddr->SetPort(7001);

	//creazione Socket
	SocketConPython = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("Default"), false);

	//Connessione
	if (SocketConPython->Connect(*PythonServerAddr))
	{
		GLog->Logf(TEXT("Connessione Avvenuta!"));
		ConnectionSuccess = true;
	} else {
		GLog->Logf(TEXT("Connessione FALLITA"));
	}

	AC_PyConnection::isConnectionOn = ConnectionSuccess;

	return ConnectionSuccess;
}

// --------------------------------------------- Funzioni varie

bool AC_PyConnection::TestSendMessage() {
	bool Sended = false;
	if (SocketConPython && SocketConPython->GetConnectionState() == SCS_Connected)
	{
		uint8 PLUTO = 4;

		SocketConPython->Send(&PLUTO, 1, BytesSent);
		UE_LOG(LogTemp, Warning, TEXT("Mandato?"));
		Sended = true;
	}
	return Sended;
}

FVector AC_PyConnection::InitializeTargetA() {

	return FVector();
}


// ------------------------------------------- Distruttore
AC_PyConnection::~AC_PyConnection()
{

}