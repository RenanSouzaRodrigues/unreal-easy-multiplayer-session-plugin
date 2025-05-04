// Fill out your copyright notice in the Description page of Project Settings.

#include "EasyMultiplayerSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "EasyMultiplayerSession/EMSUtils.h"
#include "Online/OnlineSessionNames.h"

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

void UEasyMultiplayerSubsystem::CreateSession(int32 numberOfPublicConnections, FString matchTypeName) {
	// Check if the online subsystem session interface I have is valid. If not I'm simply returning void and
	// broadcasting the event with false value. -Renan
	if (!this->IsOnlineSubsystemInterfaceValid()) {
		this->OnSessionCreatedEvent.Broadcast(false);
		return;
	}

	// If there is a valid online game session already, it means that I need to destroy it to be able to create a new session.
	// This validation exists only to ensure that we have only one valid online game session. -Renan
	FNamedOnlineSession* existingSession = this->OnlineSubsystemSessionInterface->GetNamedSession(NAME_GameSession);
	if (existingSession != nullptr) this->OnlineSubsystemSessionInterface->DestroySession(NAME_GameSession);

	// I will store this into this delegate handle so this way I can 'unbind' this later. -Renan
	this->CreateSessionDelegateHandle = this->OnlineSubsystemSessionInterface->AddOnCreateSessionCompleteDelegate_Handle(this->OnCreateSessionEvent);

	// This settings will define the session creation. I think is a good ideia to convert all this information later in a data asset that can override
	// these basic configurations. This way I can define at any moment how these settings are defined. -Renan
	// TODO: Create a data asset to enable further configurations of these online settings. Also, make a function to override the settings that can be called at any time;
	this->OnlineSessionSettings = MakeShareable(new FOnlineSessionSettings());
	
	this->OnlineSessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
	this->OnlineSessionSettings->NumPublicConnections = numberOfPublicConnections;
	this->OnlineSessionSettings->bAllowJoinInProgress = true;
	this->OnlineSessionSettings->bAllowJoinViaPresence = true;
	this->OnlineSessionSettings->bAllowInvites = true;
	this->OnlineSessionSettings->bAllowJoinViaPresenceFriendsOnly = false;
	this->OnlineSessionSettings->bShouldAdvertise = true;
	this->OnlineSessionSettings->bUsesPresence = true;
	this->OnlineSessionSettings->bUseLobbiesIfAvailable = true;
	this->OnlineSessionSettings->BuildUniqueId = 1;
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
	if (!this->OnlineSubsystemSessionInterface->CreateSession(*hostPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *this->OnlineSessionSettings)) {
		UEMSUtils::ShowDebugMessage(TEXT("Session was not created, aborting process and cleaning delegates"));
		this->OnlineSubsystemSessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(this->CreateSessionDelegateHandle);
		this->OnSessionCreatedEvent.Broadcast(false);
	}
}

void UEasyMultiplayerSubsystem::FindSession(int32 maxOnlineSessionsSearchResult) {
	// Online subsystem session interface validation. It is good to aways be sure. -Renan
	if (!this->IsOnlineSubsystemInterfaceValid()) {
		this->OnSessionFoundEvent.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
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
		this->OnSessionFoundEvent.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		return;
	}

	// If there is any error when the subsystem tries to retrieve the list of sessions to join, I simple send a log and broadcast a false response. -Renan
	if (!this->OnlineSubsystemSessionInterface->FindSessions(*localPlayer->GetPreferredUniqueNetId(), this->OnlineSessionSearch.ToSharedRef())) {
		this->OnlineSubsystemSessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(this->FindSessionDelegateHandle);
		this->OnSessionFoundEvent.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
	}
}

void UEasyMultiplayerSubsystem::JoinSession(const FOnlineSessionSearchResult& onlineSessionSearchResult) {
	// I first validate the session search result because, if this is null or invalid there is no reason to try join this session. -Renan
	if (!onlineSessionSearchResult.IsValid()) {
		 UEMSUtils::ShowDebugMessage(TEXT("Unable to Join Session, the search result is invalid"));
		this->OnSessionJoinedEvent.Broadcast(EOnJoinSessionCompleteResult::SessionDoesNotExist);
		return;
	}

	// Simple Validation. -Renan
	if (!this->IsOnlineSubsystemInterfaceValid()) {
		// I think I can use this EOnJoinSessionCompleteResult::UnknownError for any generic error. Just like this one. -Renan
		this->OnSessionJoinedEvent.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	};

	// TODO: Validate if the session is full;

	// TODO: Validate if the session is private;
	
	this->JoinSessionDelegateHandle = this->OnlineSubsystemSessionInterface->AddOnJoinSessionCompleteDelegate_Handle(this->OnJoinSessionEvent);

	const ULocalPlayer* localPlayer = this->GetWorld()->GetFirstLocalPlayerFromController();
	if (!localPlayer) {
		UEMSUtils::ShowDebugMessage(TEXT("No local player controller found. Aborting Join Session Process. A Local Player Controller is required in order join a session"), FColor::Red);
		this->OnlineSubsystemSessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(this->JoinSessionDelegateHandle);
		this->OnSessionJoinedEvent.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	}
	
	if (!this->OnlineSubsystemSessionInterface->JoinSession(*localPlayer->GetPreferredUniqueNetId(), NAME_GameSession, onlineSessionSearchResult)) {
		UEMSUtils::ShowDebugMessage(TEXT("Unable to join session for some error"), FColor::Red);
		this->OnlineSubsystemSessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(this->JoinSessionDelegateHandle);
		this->OnSessionJoinedEvent.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	}
}

void UEasyMultiplayerSubsystem::StartSession() {}

void UEasyMultiplayerSubsystem::DestroySession() {}

// Event Listeners
void UEasyMultiplayerSubsystem::OnCreateSessionEventListenerCallback(FName createdSessionName, bool bWasSuccessfullCreated) {
	if (this->OnlineSubsystemSessionInterface) {
		this->OnlineSubsystemSessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(this->CreateSessionDelegateHandle);
	}
	
	this->OnSessionCreatedEvent.Broadcast(bWasSuccessfullCreated);
	UEMSUtils::ShowDebugMessage(TEXT("Session created successfully!"), FColor::Green);
}

void UEasyMultiplayerSubsystem::OnFindSessionEventListenerCallback(bool bWasSuccessfull) {
	if (this->OnlineSubsystemSessionInterface) {
		this->OnlineSubsystemSessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(this->FindSessionDelegateHandle);
	}

	// In case there is no session results but the process still returns a success, broadcast the event with no success because there is no list
	// of sessions to join. I don't know if broadcasting a false value is a good choice. But at this point, I think is enough. -Renan
	if (this->OnlineSessionSearch->SearchResults.Num() == 0) this->OnSessionFoundEvent.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
	
	this->OnSessionFoundEvent.Broadcast(this->OnlineSessionSearch->SearchResults, bWasSuccessfull);
	UEMSUtils::ShowDebugMessage(TEXT("Find Session Results returned successfully!"), FColor::Green);
}

void UEasyMultiplayerSubsystem::OnJoinSessionEventListenerCallback(FName joinedSessionName, EOnJoinSessionCompleteResult::Type joinResultType) {
	if (!this->IsOnlineSubsystemInterfaceValid()) return;
	this->OnlineSubsystemSessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(this->JoinSessionDelegateHandle);
	this->OnSessionJoinedEvent.Broadcast(joinResultType);
}

void UEasyMultiplayerSubsystem::OnStartSessionEventListenerCallback(FName sessionName, bool bWasSuccessful) {
	
}

void UEasyMultiplayerSubsystem::OnDestroySessionEventListenerCallback(FName sessionName, bool bWasSuccessful) {
	
}

bool UEasyMultiplayerSubsystem::IsOnlineSubsystemInterfaceValid() const {
	if (this->OnlineSubsystemSessionInterface.IsValid()) {
		UEMSUtils::ShowDebugMessage(TEXT("Online Subsytem Session Interface is invalid, Something went wrong on the engine lifecycle."), FColor::Red);
		return false;
	}
	return true;
}
