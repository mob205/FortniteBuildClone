// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/FBCGameMode.h"


#if WITH_GAMELIFT
#include "GameLiftServerSDK.h"
#include "GameLiftServerSDKModels.h"
#endif

DEFINE_LOG_CATEGORY(LogFBCGameMode);

AFBCGameMode::AFBCGameMode() = default;

void AFBCGameMode::BeginPlay()
{
	Super::BeginPlay();

#if WITH_GAMELIFT
	InitGameLift();
#endif
}

void AFBCGameMode::ConfigureServerParameters(FServerParameters& ServerParametersForAnywhere)
{
#if WITH_GAMELIFT
    UE_LOG(LogFBCGameMode, Log, TEXT("Configuring server parameters for Anywhere..."));

    FString GlAnywhereWebSocketUrl = "";
    if (FParse::Value(FCommandLine::Get(), TEXT("GlAnywhereWebSocketUrl="), GlAnywhereWebSocketUrl))
    {
        ServerParametersForAnywhere.m_webSocketUrl = TCHAR_TO_UTF8(*GlAnywhereWebSocketUrl);
    }

    FString GlAnywhereFleetId = "";
    if (FParse::Value(FCommandLine::Get(), TEXT("GlAnywhereFleetId="), GlAnywhereFleetId))
    {
        ServerParametersForAnywhere.m_fleetId = TCHAR_TO_UTF8(*GlAnywhereFleetId);
    }

    FString GlAnywhereProcessId = "";
    if (FParse::Value(FCommandLine::Get(), TEXT("GlAnywhereProcessId="), GlAnywhereProcessId))
    {
        ServerParametersForAnywhere.m_processId = TCHAR_TO_UTF8(*GlAnywhereProcessId);
    }
    else
    {
        // If no ProcessId is passed as a command line argument, generate a randomized unique string.
        FString TimeString = FString::FromInt(std::time(nullptr));
        FString ProcessId = "ProcessId_" + TimeString;
        ServerParametersForAnywhere.m_processId = TCHAR_TO_UTF8(*ProcessId);
    }

    FString GlAnywhereHostId = "";
    if (FParse::Value(FCommandLine::Get(), TEXT("GlAnywhereHostId="), GlAnywhereHostId))
    {
        ServerParametersForAnywhere.m_hostId = TCHAR_TO_UTF8(*GlAnywhereHostId);
    }

    FString GlAnywhereAuthToken = "";
    if (FParse::Value(FCommandLine::Get(), TEXT("GlAnywhereAuthToken="), GlAnywhereAuthToken))
    {
        ServerParametersForAnywhere.m_authToken = TCHAR_TO_UTF8(*GlAnywhereAuthToken);
    }

    FString GlAnywhereAwsRegion = "";
    if (FParse::Value(FCommandLine::Get(), TEXT("GlAnywhereAwsRegion="), GlAnywhereAwsRegion))
    {
        ServerParametersForAnywhere.m_awsRegion = TCHAR_TO_UTF8(*GlAnywhereAwsRegion);
    }

    FString GlAnywhereAccessKey = "";
    if (FParse::Value(FCommandLine::Get(), TEXT("GlAnywhereAccessKey="), GlAnywhereAccessKey))
    {
        ServerParametersForAnywhere.m_accessKey = TCHAR_TO_UTF8(*GlAnywhereAccessKey);
    }

    FString GlAnywhereSecretKey = "";
    if (FParse::Value(FCommandLine::Get(), TEXT("GlAnywhereSecretKey="), GlAnywhereSecretKey))
    {
        ServerParametersForAnywhere.m_secretKey = TCHAR_TO_UTF8(*GlAnywhereSecretKey);
    }

    FString GlAnywhereSessionToken = "";
    if (FParse::Value(FCommandLine::Get(), TEXT("GlAnywhereSessionToken="), GlAnywhereSessionToken))
    {
        ServerParametersForAnywhere.m_sessionToken = TCHAR_TO_UTF8(*GlAnywhereSessionToken);
    }

    UE_LOG(LogFBCGameMode, SetColor, TEXT("%s"), COLOR_YELLOW);
    UE_LOG(LogFBCGameMode, Log, TEXT(">>>> WebSocket URL: %s"), *ServerParametersForAnywhere.m_webSocketUrl);
    UE_LOG(LogFBCGameMode, Log, TEXT(">>>> Fleet ID: %s"), *ServerParametersForAnywhere.m_fleetId);
    UE_LOG(LogFBCGameMode, Log, TEXT(">>>> Process ID: %s"), *ServerParametersForAnywhere.m_processId);
    UE_LOG(LogFBCGameMode, Log, TEXT(">>>> Host ID (Compute Name): %s"), *ServerParametersForAnywhere.m_hostId);
    UE_LOG(LogFBCGameMode, Log, TEXT(">>>> Auth Token: %s"), *ServerParametersForAnywhere.m_authToken);
    UE_LOG(LogFBCGameMode, Log, TEXT(">>>> Aws Region: %s"), *ServerParametersForAnywhere.m_awsRegion);
    UE_LOG(LogFBCGameMode, Log, TEXT(">>>> Access Key: %s"), *ServerParametersForAnywhere.m_accessKey);
    UE_LOG(LogFBCGameMode, Log, TEXT(">>>> Secret Key: %s"), *ServerParametersForAnywhere.m_secretKey);
    UE_LOG(LogFBCGameMode, Log, TEXT(">>>> Session Token: %s"), *ServerParametersForAnywhere.m_sessionToken);
    UE_LOG(LogFBCGameMode, SetColor, TEXT("%s"), COLOR_NONE);
#endif
}

int32 AFBCGameMode::GetPort()
{
    // Search command line arguments for port
    TArray<FString> CommandLineTokens;
    TArray<FString> CommandLineSwitches;

    // GameServer.exe -port=7777 LOG=server.mylog
    FCommandLine::Parse(FCommandLine::Get(), CommandLineTokens, CommandLineSwitches);

    for (FString SwitchStr : CommandLineSwitches)
    {
        FString Key;
        FString Value;

        if (SwitchStr.Split("=", &Key, &Value))
        {
            if (Key.Equals(TEXT("port"), ESearchCase::IgnoreCase))
            {
                return FCString::Atoi(*Value);
            }
        }
    }

    // No command line argument found - use Unreal's default port
    return FURL::UrlConfig.DefaultPort;
}

void AFBCGameMode::InitGameLift()
{
#if WITH_GAMELIFT

	UE_LOG(LogFBCGameMode, Log, TEXT("Initializing the GameLift Server"));

	FGameLiftServerSDKModule* GameLiftSDKModule = &FModuleManager::LoadModuleChecked<FGameLiftServerSDKModule>(FName("GameLiftServerSDK"));

	// Server Parameters are only needed for GameLift Anywhere fleets. Not needed for GameLift managed EC2 fleet
	FServerParameters ServerParams{};

    UE_LOG(LogFBCGameMode, Log, TEXT("Calling InitGameLift..."));

    FGameLiftServerSDKModule* GameLiftSdkModule = &FModuleManager::LoadModuleChecked<FGameLiftServerSDKModule>(FName("GameLiftServerSDK"));

    // Define the server parameters for a GameLift Anywhere fleet. These are not needed for a GameLift managed EC2 fleet.
    FServerParameters ServerParametersForAnywhere;

    bool bIsAnywhereActive = false;
    if (FParse::Param(FCommandLine::Get(), TEXT("GlAnywhere")))
    {
        bIsAnywhereActive = true;
    }

    if (bIsAnywhereActive)
    {
        ConfigureServerParameters(ServerParametersForAnywhere);
    }

    UE_LOG(LogFBCGameMode, Log, TEXT("Initializing the GameLift Server..."));

    // InitSDK will establish a local connection with GameLift's agent to enable further communication.
    FGameLiftGenericOutcome InitSdkOutcome = GameLiftSdkModule->InitSDK(ServerParametersForAnywhere);
    if (InitSdkOutcome.IsSuccess())
    {
        UE_LOG(LogFBCGameMode, SetColor, TEXT("%s"), COLOR_GREEN);
        UE_LOG(LogFBCGameMode, Log, TEXT("GameLift InitSDK succeeded!"));
        UE_LOG(LogFBCGameMode, SetColor, TEXT("%s"), COLOR_NONE);
    }
    else
    {
        UE_LOG(LogFBCGameMode, SetColor, TEXT("%s"), COLOR_RED);
        UE_LOG(LogFBCGameMode, Log, TEXT("ERROR: InitSDK failed : ("));
        FGameLiftError GameLiftError = InitSdkOutcome.GetError();
        UE_LOG(LogFBCGameMode, Log, TEXT("ERROR: %s"), *GameLiftError.m_errorMessage);
        UE_LOG(LogFBCGameMode, SetColor, TEXT("%s"), COLOR_NONE);
        return;
    }

    ProcessParameters = MakeShared<FProcessParameters>();

    // When a game session is created, Amazon GameLift Servers sends an activation request to the game server and passes along the game session object containing game properties and other settings.
    // Here is where a game server should take action based on the game session object.
    // Once the game server is ready to receive incoming player connections, it should invoke GameLiftServerAPI.ActivateGameSession()
    ProcessParameters->OnStartGameSession.BindLambda([=](Aws::GameLift::Server::Model::GameSession InGameSession)
        {
            FString GameSessionId = FString(InGameSession.GetGameSessionId());
            UE_LOG(LogFBCGameMode, Log, TEXT("GameSession Initializing: %s"), *GameSessionId);
            GameLiftSdkModule->ActivateGameSession();
        });

    // OnProcessTerminate callback. Amazon GameLift Servers will invoke this callback before shutting down an instance hosting this game server.
    // It gives this game server a chance to save its state, communicate with services, etc., before being shut down.
    // In this case, we simply tell Amazon GameLift Servers we are indeed going to shutdown.
    ProcessParameters->OnTerminate.BindLambda([=]()
        {
            UE_LOG(LogFBCGameMode, Log, TEXT("Game Server Process is terminating"));
            GameLiftSdkModule->ProcessEnding();
        });

    // This is the HealthCheck callback.
    // Amazon GameLift Servers will invoke this callback every 60 seconds or so.
    // Here, a game server might want to check the health of dependencies and such.
    // Simply return true if healthy, false otherwise.
    // The game server has 60 seconds to respond with its health status. Amazon GameLift Servers will default to 'false' if the game server doesn't respond in time.
    // In this case, we're always healthy!
    ProcessParameters->OnHealthCheck.BindLambda([]()
        {
            UE_LOG(LogFBCGameMode, Log, TEXT("Performing Health Check"));
            return true;
        });
    
    ProcessParameters->port = GetPort();

    // Here, the game server tells Amazon GameLift Servers where to find game session log files.
    // At the end of a game session, Amazon GameLift Servers uploads everything in the specified 
    // location and stores it in the cloud for access later.
    TArray<FString> Logfiles;
    Logfiles.Add(TEXT("FortniteBuildClone/Saved/Logs/FortniteBuildClone.log"));
    ProcessParameters->logParameters = Logfiles;

    // The game server calls ProcessReady() to tell Amazon GameLift Servers it's ready to host game sessions.
    UE_LOG(LogFBCGameMode, Log, TEXT("Calling Process Ready..."));
    FGameLiftGenericOutcome ProcessReadyOutcome = GameLiftSdkModule->ProcessReady(*ProcessParameters);

    if (ProcessReadyOutcome.IsSuccess())
    {
        UE_LOG(LogFBCGameMode, SetColor, TEXT("%s"), COLOR_GREEN);
        UE_LOG(LogFBCGameMode, Log, TEXT("Process Ready!"));
        UE_LOG(LogFBCGameMode, SetColor, TEXT("%s"), COLOR_NONE);
    }
    else
    {
        UE_LOG(LogFBCGameMode, SetColor, TEXT("%s"), COLOR_RED);
        UE_LOG(LogFBCGameMode, Log, TEXT("ERROR: Process Ready Failed!"));
        FGameLiftError ProcessReadyError = ProcessReadyOutcome.GetError();
        UE_LOG(LogFBCGameMode, Log, TEXT("ERROR: %s"), *ProcessReadyError.m_errorMessage);
        UE_LOG(LogFBCGameMode, SetColor, TEXT("%s"), COLOR_NONE);
    }

    UE_LOG(LogFBCGameMode, Log, TEXT("InitGameLift completed!"));
#endif
}
