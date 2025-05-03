// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "EasyMultiplayerSubsystem.generated.h"

UCLASS()
class EASYMULTIPLAYERSESSION_API UEasyMultiplayerSubsystem : public UGameInstanceSubsystem {
	GENERATED_BODY()

private:
	// As I can see this is just a typedef of some sort of TMap. But this is fine. -Renan
	IOnlineSessionPtr onlineSubsystemSessionInterface;

	// This settings are used to store the session settings of the last online session I create. -Renan
	TSharedPtr<FOnlineSessionSettings> lastOnlineSessionSettings;

private:
	// These events or delegates are called when the functions finish their execution. -Renan
	FOnCreateSessionCompleteDelegate OnCreateSessionEvent;
	FOnFindSessionsCompleteDelegate OnFindSessionEvent;
	FOnJoinSessionCompleteDelegate OnJoinSessionEvent;
	FOnStartSessionCompleteDelegate OnStartSessionEvent;
	FOnDestroySessionCompleteDelegate OnDestroySessionEvent;

	// These delegate handles will be use when the main delegates are not needed anymore. -Renan
	FDelegateHandle CreateSessionDelegateHandle;
	FDelegateHandle FindSessionDelegateHandle;
	FDelegateHandle JoinSessionDelegateHandle;
	FDelegateHandle StartSessionDelegateHandle;
	FDelegateHandle DestroySessionDelegateHandle;
	
public:
	UEasyMultiplayerSubsystem();

	void CreateSession(int32 numberOfPublicConnections, FString matchType);
	void FindSession(int32 maxOnlineSessionsSearchResult);
	void JoinSession(const FOnlineSessionSearchResult& onlineSessionResult);
	void StartSession();
	void DestroySession();

protected:
	// Internal callbacks for the events that ared called by the session funcions. -Renan
	void OnCreateSessionEventListenerCallback(FName createdSessionName, bool bWasSuccessfullCreated);
	void OnFindSessionEventListenerCallback(bool bWasSuccessfull);
	void OnJoinSessionEventListenerCallback(FName joinedSessionName, EOnJoinSessionCompleteResult::Type joinResultType);
	void OnStartSessionEventListenerCallback(FName sessionName, bool bWasSuccessful);
	void OnDestroySessionEventListenerCallback(FName sessionName, bool bWasSuccessful);
};
