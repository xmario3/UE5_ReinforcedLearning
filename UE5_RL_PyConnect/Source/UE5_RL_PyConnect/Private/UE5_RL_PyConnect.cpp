// Copyright Epic Games, Inc. All Rights Reserved.

#include "UE5_RL_PyConnect.h"

#define LOCTEXT_NAMESPACE "FUE5_RL_PyConnectModule"

void FUE5_RL_PyConnectModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FUE5_RL_PyConnectModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUE5_RL_PyConnectModule, UE5_RL_PyConnect)