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

	this->OnlineSubsystemSessionInterface = onlineSubsystemReference->GetSessionInterface();
}

void UEasyMultiplayerSubsystem::CreateSession(int32 numberOfPublicConnections, FString matchType) {
	// Check if the online subsystem session interface I have is valid. If not I'm simply returning void. -Renan
	if (!this->OnlineSubsystemSessionInterface.IsValid()) {
		UEMSUtils::ShowDebugMessage(TEXT("Online Subsytem Session Interface is invalid, Something went wrong on the engine lifecycle."), FColor::Red);
		this->OnSessionCreatedEvent.Broadcast(false);
		return;
	};

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
	this->OnlineSessionSettings->Set(FName("MatchType"), matchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	// With the ULocalPlayer reference I can grab the preferred unique net id. It is required to create a new online session. -Renan
	const ULocalPlayer* hostPlayer = this->GetWorld()->GetFirstLocalPlayerFromController();
	if (!hostPlayer) {
		UEMSUtils::ShowDebugMessage(TEXT("No local player controller found. Aborting session creation for now. A Local Player Controller is required in order to create a new Session"), FColor::Red);
		this->OnlineSubsystemSessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(this->CreateSessionDelegateHandle);
		this->OnSessionCreatedEvent.Broadcast(false);
		return;
	}

	// If the session is not created, then is probably a problem with the engine lifecycle. -Renan
	if (!this->OnlineSubsystemSessionInterface->CreateSession(*hostPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *this->OnlineSessionSettings)) {
		UEMSUtils::ShowDebugMessage(TEXT("Session was not created, aborting process and cleaning delegates"));
		this->OnlineSubsystemSessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(this->CreateSessionDelegateHandle);
		this->OnSessionCreatedEvent.Broadcast(false);
	}
}

void UEasyMultiplayerSubsystem::FindSession(int32 maxOnlineSessionsSearchResult) {
	if (!this->OnlineSubsystemSessionInterface.IsValid()) {
		UEMSUtils::ShowDebugMessage(TEXT("Online Subsytem Session Interface is invalid, Something went wrong on the engine lifecycle."), FColor::Red);
		return;
	}
	
	this->FindSessionDelegateHandle = this->OnlineSubsystemSessionInterface->AddOnFindSessionsCompleteDelegate_Handle(this->OnFindSessionEvent);

	this->OnlineSessionSearch = MakeShareable(new FOnlineSessionSearch);
	this->OnlineSessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
	this->OnlineSessionSearch->MaxSearchResults = maxOnlineSessionsSearchResult;
	this->OnlineSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	const ULocalPlayer* localPlayer = this->GetWorld()->GetFirstLocalPlayerFromController();
	if (localPlayer) {
		const bool sessionsFound = this->OnlineSubsystemSessionInterface->FindSessions(
			*localPlayer->GetPreferredUniqueNetId(),
			this->OnlineSessionSearch.ToSharedRef()
		);

		if (!sessionsFound) {
			this->OnlineSubsystemSessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(this->FindSessionDelegateHandle);
			this->OnSessionFoundEvent.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		}
	}
	
}

void UEasyMultiplayerSubsystem::JoinSession(const FOnlineSessionSearchResult& onlineSessionResult) {}

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

	auto sessionResult = this->OnlineSessionSearch->SearchResults;
	this->OnSessionFoundEvent.Broadcast(sessionResult, bWasSuccessfull);
}

void UEasyMultiplayerSubsystem::OnJoinSessionEventListenerCallback(FName joinedSessionName, EOnJoinSessionCompleteResult::Type joinResultType) {
	
}

void UEasyMultiplayerSubsystem::OnStartSessionEventListenerCallback(FName sessionName, bool bWasSuccessful) {
	
}

void UEasyMultiplayerSubsystem::OnDestroySessionEventListenerCallback(FName sessionName, bool bWasSuccessful) {
	
}
