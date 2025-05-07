// Fill out your copyright notice in the Description page of Project Settings.

#include "EasyMultiplayerSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "EasyMultiplayerSession/EMSUtils.h"
#include "Online/OnlineSessionNames.h"
#include "Data/EMSSessionCreationSettingsPDA.h"

// This session plugin is ready to be tested but, still need to do somethings.
// TODO: Create a way to Invite Steam Friends.
// TODO: Create a way to enter on the a friend session.

UEasyMultiplayerSubsystem::UEasyMultiplayerSubsystem():
	// I'm using this approach because these especific delegates have their own way to be initialized and binded to events.
	// This way I can call the construction methods of all of then and start bind my subsystem events the way I need.
	// Makes the code a little harder to read but, there's nothing much I can do. -Renan
	OnCreateSessionEvent(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionEventListenerCallback)),
	OnFindSessionEvent(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionEventListenerCallback)),
	OnJoinSessionEvent(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionEventListenerCallback)),
	OnStartSessionEvent(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartSessionEventListenerCallback)),
	OnDestroySessionEvent(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionEventListenerCallback))

// The constructor function starts here. -Renan
{
	IOnlineSubsystem* onlineSubsystemReference = IOnlineSubsystem::Get();

	this->SessionCreationSettingsOverride = nullptr;

	// Validates the online subsystem to make sure the project has it. It will be a little bit hard to make a online game without the
	// online subsystem. -Renan
	if (!onlineSubsystemReference) {
		UEMSUtils::ShowDebugMessage(TEXT("Unable to get the Online Subsystem Reference from the engine source. Make sure the subsystem is available and try again"), FColor::Red);
		return;
	}

	// This is a debug message to help visualize what is the subsystem used for the online sessions.
	// Usually, playing in editor, the subsystem will be aways NULL. this is not a value but a subsystem name
	// When the developer has the steam installed, it will try to grab the steam subsystem and use it.
	// If the engine can grab the Steam subsystem, it will be used as the default. This debug log will show this on screen. -Renan
	const FString onlineSubsystemName = onlineSubsystemReference->GetSubsystemName().ToString();
	UEMSUtils::ShowDebugMessage(FString::Printf(TEXT("Current online subsystem: %s"), *onlineSubsystemName), FColor::Green);
	
	this->OnlineSubsystemSessionInterface = onlineSubsystemReference->GetSessionInterface();
}

void UEasyMultiplayerSubsystem::OverrideSessionCreationSettings(UEMSSessionCreationSettingsPDA* settingsToOverride) {
	this->SessionCreationSettingsOverride = settingsToOverride;
}

void UEasyMultiplayerSubsystem::CreateSession(int32 numberOfPublicConnections, FString matchTypeName) {
	// Check if the online subsystem session interface I have is valid. If not I'm simply returning void and
	// broadcasting the event with false value. -Renan
	if (!this->IsOnlineSubsystemInterfaceValid()) {
		this->OnSessionCreatedEvent.Broadcast(false);
		return;
	}
	
	// Clear the cached data when calling this function. -Renan
	this->CachedNumberOfPublicPlayers = 0;
	this->CachedMatchTypeName = FString("");
	
	// If there is a valid online game session already, it means that I need to destroy it to be able to create a new session.
	// This validation exists only to ensure that we have only one valid online game session. -Renan
	UEMSUtils::ShowDebugMessage(TEXT("Validating existing session"));
	FNamedOnlineSession* existingSession = this->OnlineSubsystemSessionInterface->GetNamedSession(NAME_GameSession);
	if (existingSession != nullptr) {
		UEMSUtils::ShowDebugMessage(TEXT("There is a previous session created. Destroying previous session"));
		this->bCreateSessionAfterDestroy = true;
		this->CachedNumberOfPublicPlayers = numberOfPublicConnections;
		this->CachedMatchTypeName = matchTypeName;
		this->DestroySession();
		return;
	}

	// I will store this into this delegate handle so this way I can 'unbind' this later. -Renan
	this->CreateSessionDelegateHandle = this->OnlineSubsystemSessionInterface->AddOnCreateSessionCompleteDelegate_Handle(this->OnCreateSessionEvent);

	// This settings will define the session creation. I think is a good ideia to convert all this information later in a data asset that can override
	// these basic configurations. This way I can define at any moment how these settings are defined. -Renan
	// TODO: Create a data asset to enable further configurations of these online settings. Also, make a function to override the settings that can be called at any time;
	this->OnlineSessionSettings = MakeShareable(new FOnlineSessionSettings());
	
	if (this->SessionCreationSettingsOverride != nullptr) {
		this->OnlineSessionSettings->bAllowJoinInProgress = this->SessionCreationSettingsOverride->bAllowJoinInProgress;
		this->OnlineSessionSettings->bAllowJoinViaPresence = this->SessionCreationSettingsOverride->bAllowJoinViaPresence;
		this->OnlineSessionSettings->bAllowInvites = this->SessionCreationSettingsOverride->bAllowInvites;
		this->OnlineSessionSettings->bAllowJoinViaPresenceFriendsOnly = this->SessionCreationSettingsOverride->bAllowJoinViaPresenceFriendsOnly;
		this->OnlineSessionSettings->bShouldAdvertise = this->SessionCreationSettingsOverride->bShouldAdvertise;
		this->OnlineSessionSettings->bUsesPresence = this->SessionCreationSettingsOverride->bUsesPresence;
		this->OnlineSessionSettings->bUseLobbiesIfAvailable = this->SessionCreationSettingsOverride->bUseLobbiesIfAvailable;
		this->OnlineSessionSettings->BuildUniqueId = 1;
	} else {
		this->OnlineSessionSettings->bAllowJoinInProgress = true;
		this->OnlineSessionSettings->bAllowJoinViaPresence = true;
		this->OnlineSessionSettings->bAllowInvites = true;
		this->OnlineSessionSettings->bAllowJoinViaPresenceFriendsOnly = false;
		this->OnlineSessionSettings->bShouldAdvertise = true;
		this->OnlineSessionSettings->bUsesPresence = true;
		this->OnlineSessionSettings->bUseLobbiesIfAvailable = true;
		this->OnlineSessionSettings->BuildUniqueId = 1;
	}

	this->OnlineSessionSettings->NumPublicConnections = numberOfPublicConnections;
	this->OnlineSessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
	this->OnlineSessionSettings->Set(MATCH_TYPE, matchTypeName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	
	// With the ULocalPlayer reference I can grab the preferred unique net id. It is required to create a new online session.
	// without a local player controller, it is impossible to track who is the owner of the session. -Renan
	const ULocalPlayer* hostPlayer = this->GetWorld()->GetFirstLocalPlayerFromController();
	if (!hostPlayer) {
		UEMSUtils::ShowDebugMessage(
			TEXT("No local player controller found. Aborting Session creation. A Local Player Controller is required in order to create a new Session"),
			FColor::Red
		);
		this->OnlineSubsystemSessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(this->CreateSessionDelegateHandle);
		this->OnSessionCreatedEvent.Broadcast(false);
		return;
	}

	// If the session is not created, then is probably a problem with the engine lifecycle.
	// I think this is a very impossible scenario, but we never know. Better safe then sorry. -Renan
	UEMSUtils::ShowDebugMessage(TEXT("Trying to create a new session"));
	if (!this->OnlineSubsystemSessionInterface->CreateSession(*hostPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *this->OnlineSessionSettings)) {
		UEMSUtils::ShowDebugMessage(TEXT("Session was not created, aborting process and started cleaning delegates"), FColor::Red);
		this->OnlineSubsystemSessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(this->CreateSessionDelegateHandle);
		this->OnSessionCreatedEvent.Broadcast(false);
	}
}

void UEasyMultiplayerSubsystem::FindSession(int32 maxOnlineSessionsSearchResult) {
	// Online subsystem session interface validation. It is good to aways be sure. -Renan
	if (!this->IsOnlineSubsystemInterfaceValid()) {
		TArray<FEMSOnlineSessionSearchResult> emptySearchResult = TArray<FEMSOnlineSessionSearchResult>();
		this->OnSessionFoundEvent.Broadcast(emptySearchResult, false);
		return;
	}

	// Same as the create session method. -Renan
	this->FindSessionDelegateHandle = this->OnlineSubsystemSessionInterface->AddOnFindSessionsCompleteDelegate_Handle(this->OnFindSessionEvent);

	this->OnlineSessionSearch = MakeShareable(new FOnlineSessionSearch);
	this->OnlineSessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
	this->OnlineSessionSearch->MaxSearchResults = maxOnlineSessionsSearchResult;
	this->OnlineSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	
	const ULocalPlayer* localPlayer = this->GetWorld()->GetFirstLocalPlayerFromController();

	// Same for the creation method. I need the local player controller here to be able to join sessions on the future. -Renan
	if (!localPlayer) {
		UEMSUtils::ShowDebugMessage(TEXT("No local player controller found. Aborting Find Session Process. A Local Player Controller is required in order search for sessions"), FColor::Red);
		this->OnlineSubsystemSessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(this->FindSessionDelegateHandle);
		TArray<FEMSOnlineSessionSearchResult> emptySearchResult = TArray<FEMSOnlineSessionSearchResult>();
		this->OnSessionFoundEvent.Broadcast(emptySearchResult, false);
		return;
	}

	UEMSUtils::ShowDebugMessage(TEXT("Trying to find sessions"));
	// If there is any error when the subsystem tries to retrieve the list of sessions to join, I simple send a log and broadcast a false response. -Renan
	if (!this->OnlineSubsystemSessionInterface->FindSessions(*localPlayer->GetPreferredUniqueNetId(), this->OnlineSessionSearch.ToSharedRef())) {
		this->OnlineSubsystemSessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(this->FindSessionDelegateHandle);
		TArray<FEMSOnlineSessionSearchResult> emptySearchResult = TArray<FEMSOnlineSessionSearchResult>();
		this->OnSessionFoundEvent.Broadcast(emptySearchResult, false);
	}
}

void UEasyMultiplayerSubsystem::JoinSession(const FEMSOnlineSessionSearchResult& onlineSessionSearchResult) {
	// I first validate the session search result because, if this is null or invalid there is no reason to try join this session. -Renan
	if (onlineSessionSearchResult.IsValid()) {
		 UEMSUtils::ShowDebugMessage(TEXT("Unable to Join Session, the search result is invalid"));
		this->OnSessionJoinedEvent.Broadcast(this->ConvertJoinResult(EOnJoinSessionCompleteResult::UnknownError));
		return;
	}

	// Simple Validation. -Renan
	if (!this->IsOnlineSubsystemInterfaceValid()) {
		// I think I can use this EOnJoinSessionCompleteResult::UnknownError for any generic error. Just like this one. -Renan
		this->OnSessionJoinedEvent.Broadcast(this->ConvertJoinResult(EOnJoinSessionCompleteResult::UnknownError));
		return;
	};

	// TODO: Validate if the session is full;
	// TODO: Validate if the session is private;
	// TODO: Validate if the session has password;
	
	this->JoinSessionDelegateHandle = this->OnlineSubsystemSessionInterface->AddOnJoinSessionCompleteDelegate_Handle(this->OnJoinSessionEvent);

	const ULocalPlayer* localPlayer = this->GetWorld()->GetFirstLocalPlayerFromController();
	if (!localPlayer) {
		UEMSUtils::ShowDebugMessage(TEXT("No local player controller found. Aborting Join Session Process. A Local Player Controller is required in order join a session"), FColor::Red);
		this->OnlineSubsystemSessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(this->JoinSessionDelegateHandle);
		this->OnSessionJoinedEvent.Broadcast(this->ConvertJoinResult(EOnJoinSessionCompleteResult::UnknownError));
		return;
	}

	const FOnlineSessionSearchResult foundSession = this->GetRealSessionDataFromSessionPlaceholderData(onlineSessionSearchResult);
	
	if (!this->OnlineSubsystemSessionInterface->JoinSession(*localPlayer->GetPreferredUniqueNetId(), NAME_GameSession, foundSession)) {
		UEMSUtils::ShowDebugMessage(TEXT("Unable to join session for some error"), FColor::Red);
		this->OnlineSubsystemSessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(this->JoinSessionDelegateHandle);
		this->OnSessionJoinedEvent.Broadcast(this->ConvertJoinResult(EOnJoinSessionCompleteResult::UnknownError));
		return;
	}
}

void UEasyMultiplayerSubsystem::ConnectToJoinedSession() {
	if (!this->IsOnlineSubsystemInterfaceValid()) {
		UEMSUtils::ShowDebugMessage(TEXT("Unable to connect to session. Aborting process."), FColor::Red);
		return;
	}

	// The online subsystem holds the reference to the joined session. This way I can grab the connection string address
	// at any point in time. -Renan
	FString connectionAddress;
	this->OnlineSubsystemSessionInterface->GetResolvedConnectString(NAME_GameSession, connectionAddress);

	// I validate the connection string so the user can't even try to connect to the empty address. -Renan
	if (connectionAddress.IsEmpty()) {
		UEMSUtils::ShowDebugMessage(TEXT("Unable to connect to joined session. Could not retrieve the session connection string addres."), FColor::Red);
		return;
	}
	
	if (APlayerController* playerController = this->GetGameInstance()->GetFirstLocalPlayerController()) {
		playerController->ClientTravel(connectionAddress, ETravelType::TRAVEL_Absolute);
	}
}

void UEasyMultiplayerSubsystem::StartSession() {
	// simple validation. -Renan
	if (!this->IsOnlineSubsystemInterfaceValid()) {
		this->OnSessionStartedEvent.Broadcast(false);
		return;
	}

	// Delegate handle binding. -Renan
	this->StartSessionDelegateHandle = this->OnlineSubsystemSessionInterface->AddOnStartSessionCompleteDelegate_Handle(this->OnStartSessionEvent);

	if (!this->OnlineSubsystemSessionInterface->StartSession(NAME_GameSession)) {
		UEMSUtils::ShowDebugMessage(TEXT("Unable to start the session, aborting process and cleaning delegates"), FColor::Red);
		this->OnlineSubsystemSessionInterface->ClearOnStartSessionCompleteDelegate_Handle(this->StartSessionDelegateHandle);
		this->OnSessionStartedEvent.Broadcast(false);
	}
}

void UEasyMultiplayerSubsystem::DestroySession() {
	// Simple validation. -Renan
	if (!this->IsOnlineSubsystemInterfaceValid()) {
		this->OnSessionDestroyedEvent.Broadcast(false);
		return;
	}

	// delegate handle binding. -Renan
	this->DestroySessionDelegateHandle = this->OnlineSubsystemSessionInterface->AddOnDestroySessionCompleteDelegate_Handle(this->OnDestroySessionEvent);
	
	if (!this->OnlineSubsystemSessionInterface->DestroySession(NAME_GameSession)) {
		UEMSUtils::ShowDebugMessage(TEXT("Unable to destroy session, aborting process and cleaning delegates"), FColor::Red);
		this->OnlineSubsystemSessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(this->DestroySessionDelegateHandle);
		this->OnSessionDestroyedEvent.Broadcast(false);
	}
}

void UEasyMultiplayerSubsystem::OpenGameLevelAsHostServer(FString pathToLobby) {
	UWorld* world = this->GetWorld();
	if (!world) {
		UEMSUtils::ShowDebugMessage(TEXT("Unable to retrive world information to open lobby level. Aborting process"), FColor::Red);
		return;
	}

	const FString finalPathName = FString::Printf(TEXT("/Game/%s?listen"), *pathToLobby);
	world->ServerTravel(finalPathName);
}

// Event Listeners
void UEasyMultiplayerSubsystem::OnCreateSessionEventListenerCallback(FName createdSessionName, bool bWasSuccessfullCreated) {
	if (!this->IsOnlineSubsystemInterfaceValid()) {
		this->OnSessionCreatedEvent.Broadcast(false);
		return;
	}
	
	this->OnlineSubsystemSessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(this->CreateSessionDelegateHandle);

	this->OnSessionCreatedEvent.Broadcast(bWasSuccessfullCreated);
	UEMSUtils::ShowDebugMessage(TEXT("Session created successfully!"), FColor::Green);
}

void UEasyMultiplayerSubsystem::OnFindSessionEventListenerCallback(bool bWasSuccessfull) {
	if (this->IsOnlineSubsystemInterfaceValid()) {
		TArray<FEMSOnlineSessionSearchResult> emptySearchResult = TArray<FEMSOnlineSessionSearchResult>();
		this->OnSessionFoundEvent.Broadcast(emptySearchResult, false);
		return;
	}

	this->OnlineSubsystemSessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(this->FindSessionDelegateHandle);
	
	// In case there is no session results but the process still returns a success, broadcast the event with no success because there is no list
	// of sessions to join. I don't know if broadcasting a false value is a good choice. But at this point, I think is enough. -Renan
	if (this->OnlineSessionSearch->SearchResults.Num() == 0) {
		TArray<FEMSOnlineSessionSearchResult> emptySearchResult = TArray<FEMSOnlineSessionSearchResult>();
		this->OnSessionFoundEvent.Broadcast(emptySearchResult, false);
	}
	
	// This is cached so I can use this later when the user choose a found session. Also, this is relevant to be able to use
	// unreal reflection system. -Renan
	this->CachedFindServerSearchResult = this->OnlineSessionSearch->SearchResults;

	TArray<FEMSOnlineSessionSearchResult> results;
	for (auto searchResult : this->OnlineSessionSearch->SearchResults) {
		results.Add(this->ConvertSessionResult(searchResult));
	}
	
	this->OnSessionFoundEvent.Broadcast(results, bWasSuccessfull);
	
	UEMSUtils::ShowDebugMessage(TEXT("Find Session Results returned successfully!"), FColor::Green);
}

// Join Session Callback Function
void UEasyMultiplayerSubsystem::OnJoinSessionEventListenerCallback(FName joinedSessionName, EOnJoinSessionCompleteResult::Type joinResultType) {
	if (!this->IsOnlineSubsystemInterfaceValid()) {
		this->OnSessionJoinedEvent.Broadcast(this->ConvertJoinResult(joinResultType));
		return;
	};
	
	this->OnlineSubsystemSessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(this->JoinSessionDelegateHandle);
	
	this->OnSessionJoinedEvent.Broadcast(this->ConvertJoinResult(joinResultType));
	if (joinResultType == EOnJoinSessionCompleteResult::Success) UEMSUtils::ShowDebugMessage(TEXT("Session Joined Successfully!"), FColor::Green);
}

// Start Session Callback Function
void UEasyMultiplayerSubsystem::OnStartSessionEventListenerCallback(FName sessionName, bool bWasSuccessful) {
	if (!this->IsOnlineSubsystemInterfaceValid()) {
		this->OnSessionStartedEvent.Broadcast(false);
		return;
	}
	
	this->OnlineSubsystemSessionInterface->ClearOnStartSessionCompleteDelegate_Handle(this->StartSessionDelegateHandle);

	this->OnSessionStartedEvent.Broadcast(bWasSuccessful);
	if (bWasSuccessful) UEMSUtils::ShowDebugMessage(TEXT("Session Started successfully!"), FColor::Green);
}

void UEasyMultiplayerSubsystem::OnDestroySessionEventListenerCallback(FName sessionName, bool bWasSuccessful) {
	if (!this->IsOnlineSubsystemInterfaceValid()) {
		this->OnSessionDestroyedEvent.Broadcast(false);
		return;
	}
	
	this->OnlineSubsystemSessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(this->DestroySessionDelegateHandle);

	this->OnSessionDestroyedEvent.Broadcast(bWasSuccessful);

	if (this->bCreateSessionAfterDestroy && bWasSuccessful) {
		this->bCreateSessionAfterDestroy = false;
		UEMSUtils::ShowDebugMessage(TEXT("Previos Session destroyed. Ready to create a new session."));
		this->CreateSession(this->CachedNumberOfPublicPlayers, this->CachedMatchTypeName);
		return;
	}
	
	if (bWasSuccessful) UEMSUtils::ShowDebugMessage(TEXT("Session Destroyed Successfully!"), FColor::Green);
}

bool UEasyMultiplayerSubsystem::IsOnlineSubsystemInterfaceValid() const {
	if (!this->OnlineSubsystemSessionInterface.IsValid()) {
		UEMSUtils::ShowDebugMessage(TEXT("Online Subsytem Session Interface is invalid, Something went wrong on the engine lifecycle."), FColor::Red);
		return false;
	}
	return true;
}

// This function exists to translate the session result into something that can use the blueprint reflection system.
// With this, I can aways look on the array of all the results and filter it and join the correct session result data. -Renan
FEMSOnlineSessionSearchResult UEasyMultiplayerSubsystem::ConvertSessionResult(const FOnlineSessionSearchResult& sessionResult) {
	// The result data is just converted to a FStruct that can be exposed to blueprints API. -Renan
	FString matchTypeName;
	sessionResult.Session.SessionSettings.Get(MATCH_TYPE, matchTypeName);

	FEMSOnlineSessionSearchResult result;
	result.MatchType = matchTypeName;
	result.SessionId = sessionResult.GetSessionIdStr();
	result.PingInMs = sessionResult.PingInMs;
	result.NumberOfPublicConnections = sessionResult.Session.NumOpenPublicConnections;
	result.NumberOfPrivateConnections = sessionResult.Session.NumOpenPrivateConnections;
	
	return result;
}

// Same thing here. I can use this function to make use of the blueprint reflection system. -Renan
EEMSJoinSessionCompleteResult UEasyMultiplayerSubsystem::ConvertJoinResult(const EOnJoinSessionCompleteResult::Type joinResultType) {
	// In this case is just a simple Enum translation. -Renan
	switch (joinResultType) {
		case EOnJoinSessionCompleteResult::Success: return EEMSJoinSessionCompleteResult::Success;
		case EOnJoinSessionCompleteResult::UnknownError: return EEMSJoinSessionCompleteResult::UnknownError;
		case EOnJoinSessionCompleteResult::AlreadyInSession: return EEMSJoinSessionCompleteResult::AlreadyInSession;
		case EOnJoinSessionCompleteResult::SessionIsFull: return EEMSJoinSessionCompleteResult::SessionIsFull;
		case EOnJoinSessionCompleteResult::CouldNotRetrieveAddress: return EEMSJoinSessionCompleteResult::CouldNotRetrieveAddress;
		case EOnJoinSessionCompleteResult::SessionDoesNotExist: return EEMSJoinSessionCompleteResult::SessionDoesNotExist;
		default: return EEMSJoinSessionCompleteResult::UnknownError;
	}
}

// This function is responsable to find the real session result data to join if the user so chooses a session. -Renan
FOnlineSessionSearchResult UEasyMultiplayerSubsystem::GetRealSessionDataFromSessionPlaceholderData(FEMSOnlineSessionSearchResult sessionToFind) {
	FOnlineSessionSearchResult realSessionResult;
	// This array is saved previously, when the FindSession method is called. -Renan
	for (auto cachedSessionResult : this->CachedFindServerSearchResult) {
		if (cachedSessionResult.GetSessionIdStr() == sessionToFind.SessionId) {
			realSessionResult = cachedSessionResult;
			break;
		}
	}
	return realSessionResult;
}