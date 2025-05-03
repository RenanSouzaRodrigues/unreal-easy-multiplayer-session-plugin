// Fill out your copyright notice in the Description page of Project Settings.


#include "EasyMultiplayerSubsystem.h"
#include "EasyMultiplayerSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "EasyMultiplayerSession/EMSUtils.h"

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

	this->onlineSubsystemSessionInterface = onlineSubsystemReference->GetSessionInterface();
}

void UEasyMultiplayerSubsystem::CreateSession(int32 numberOfPublicConnections, FString matchType) {
	// Check if the online subsystem session interface I have is valid. If not I'm simply returning void. -Renan
	if (!this->onlineSubsystemSessionInterface.IsValid()) {
		UEMSUtils::ShowDebugMessage(TEXT("Online Subsytem Session Interface is invalid, Something went wrong on the engine lifecycle."), FColor::Red);
		return;
	};

	// If there is a valid online game session already, this mean that I need to destroy it to be able create a new session.
	// This validation exists only to assure that we have only one valid online game session. -Renan
	auto existingSession = this->onlineSubsystemSessionInterface->GetNamedSession(NAME_GameSession);
	if (existingSession != nullptr) this->onlineSubsystemSessionInterface->DestroySession(NAME_GameSession);

	// I will store this into its delegate handle so this way I can 'unbind' this later. -Renan
	this->CreateSessionDelegateHandle = this->onlineSubsystemSessionInterface->AddOnCreateSessionCompleteDelegate_Handle(this->OnCreateSessionEvent);

	// This settings will define the session creation. I think is a good ideia to convert all this information after in a data asset that can override
	// these basic configurations. This way I can define at any moment how these settings are defined. -Renan
	// TODO: Create a data asset to enable further configurations of these online settings;
	this->lastOnlineSessionSettings = MakeShareable(new FOnlineSessionSettings());
	this->lastOnlineSessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
	this->lastOnlineSessionSettings->NumPublicConnections = numberOfPublicConnections;
	this->lastOnlineSessionSettings->bAllowJoinInProgress = true;
	this->lastOnlineSessionSettings->bAllowJoinViaPresence = true;
	this->lastOnlineSessionSettings->bAllowInvites = true;
	this->lastOnlineSessionSettings->bAllowJoinViaPresenceFriendsOnly = false;
	this->lastOnlineSessionSettings->bShouldAdvertise = true;
	this->lastOnlineSessionSettings->bUsesPresence = true;
	this->lastOnlineSessionSettings->Set(FName("MatchType"), matchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	// With the ULocalPlayer reference I can grab the preferred unique net id. It is required to create a new online session.
	// This function also returns void if the session was not correctly created, but this function as a whole could return a boolean
	// This way the developer can validate if the session was created on the right way. But at the same time I will be exposing a lot
	// more of this API than necessary. Let's see if this argument still stands after a while. -Renan
	if (const ULocalPlayer* hostServerPlayer = this->GetWorld()->GetFirstLocalPlayerFromController()) {
		const bool createdSession = this->onlineSubsystemSessionInterface->CreateSession(
			*hostServerPlayer->GetPreferredUniqueNetId(),
			NAME_GameSession,
			*this->lastOnlineSessionSettings
		);

		if (!createdSession) {
			UEMSUtils::ShowDebugMessage(TEXT("Session was not created, aborting process and cleaning delegates"));
			// If the session was not created, Here I can remove the delegate previously binded, this way I can aways call it again. -Renan
			this->onlineSubsystemSessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(this->CreateSessionDelegateHandle);
		}
	}
}

void UEasyMultiplayerSubsystem::FindSession(int32 maxOnlineSessionsSearchResult) {}

void UEasyMultiplayerSubsystem::JoinSession(const FOnlineSessionSearchResult& onlineSessionResult) {}

void UEasyMultiplayerSubsystem::StartSession() {}

void UEasyMultiplayerSubsystem::DestroySession() {}

// Event Listeners
void UEasyMultiplayerSubsystem::OnCreateSessionEventListenerCallback(FName createdSessionName, bool bWasSuccessfullCreated) {}

void UEasyMultiplayerSubsystem::OnFindSessionEventListenerCallback(bool bWasSuccessfull) {}

void UEasyMultiplayerSubsystem::OnJoinSessionEventListenerCallback(FName joinedSessionName, EOnJoinSessionCompleteResult::Type joinResultType) {}

void UEasyMultiplayerSubsystem::OnStartSessionEventListenerCallback(FName sessionName, bool bWasSuccessful) {}

void UEasyMultiplayerSubsystem::OnDestroySessionEventListenerCallback(FName sessionName, bool bWasSuccessful) {}
