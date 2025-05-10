// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Enums/EEMSJoinSessionCompleteResult.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Structs/FEMSOnlineSessionSearchResult.h"
#include "EasyMultiplayerSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEasyMultiplayerDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEasyMultiplayerSessionCreatedDelegate, bool, bSessionCreated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEasyMultiplayerFindSessionDelegate, const TArray<FEMSOnlineSessionSearchResult>&, sessionResult, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEasyMultiplayerJoinSessionDelegate, EEMSJoinSessionCompleteResult,  joinResult);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEasyMultiplayerStartSessionDeletage, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEasyMultiplayerDestroySessionDelegate, bool, bSuccess);


UCLASS()
class EASYMULTIPLAYERSESSION_API UEasyMultiplayerSubsystem : public UGameInstanceSubsystem {
	GENERATED_BODY()

	// ===================================================
	// Unreal Methods
	// ===================================================
public:
	UEasyMultiplayerSubsystem();

	
	
	// ===================================================
	// Online Subsystem
	// ===================================================
private:
	// IOnlineSessionPtr is just a typedef for some sort of TMap. This property must be validated all the time.
	// If this value is null at some point, everything can break causing a crash. -Renan
	IOnlineSessionPtr OnlineSubsystemSessionInterface;

	// This method is just a usefull way to validate and send a debug message to the user. The OnlineSubsystemInterface
	// must be validated all the time. -Renan
	bool IsOnlineSubsystemInterfaceValid() const;

	
	
	// ===================================================
	// Create Session
	// ===================================================
private:
	// This property can hold the configurations to override when the user wants to create a new session. -Renan
	class UEMSSessionCreationSettingsPDA* SessionCreationSettingsOverride;

	// This shared pointer is use not only for store the session creation settings, but to store the session creation result.
	// The shared pointer is used by the OnlineSubsystemInteface to store the result. -Renan
	TSharedPtr<FOnlineSessionSettings> OnlineSessionSettings;
	
public:
	/**
	 * Method that can be used to override the default session creation settings.
	 * @param settingsToOverride A Data Asset object reference based on UEMSSessionCreationSettingsPDA
	 */
	UFUNCTION(BlueprintCallable)
	void OverrideSessionCreationSettings(UEMSSessionCreationSettingsPDA* settingsToOverride);
	
	/**
	 * Method that creates a new online multiplayer session. This method also fires up an event in case the session was
	 * successfully created or not. You can bind a Custom Event Listeners to the OnSessionCreatedEvent to handle any use case.
	 * @param numberOfPublicConnections Number of public available slot for players to join.
	 * @param matchTypeName Name of the Session Match Type
	 */
	UFUNCTION(BlueprintCallable)
	void CreateSession(int32 numberOfPublicConnections = 16, FString matchTypeName = "FreeForAll");
private:
	// This delegate is called by the online subsystem when it finishes the session creation. -Renan
	FOnCreateSessionCompleteDelegate OnCreateSessionEvent;
	
	// This handle is used to store the delegate bind of OnCreateSessionEvent. -Renan
	FDelegateHandle CreateSessionDelegateHandle;

	// This callback method is called when OnCreateSessionEvent broadcasts. -Renan
	void OnCreateSessionEventListenerCallback(FName createdSessionName, bool bWasSuccessfullCreated);
	
public:
	// This is the event called when the hole process is done. This can also be used inside Blueprints. -Renan
	UPROPERTY(BlueprintAssignable)
	FEasyMultiplayerSessionCreatedDelegate OnSessionCreatedEvent;

	

	// ===================================================
	// Find Session
	// ===================================================
private:
	// This session search is used to configure and retain everything that returns from the find session. -Renan
	TSharedPtr<FOnlineSessionSearch> OnlineSessionSearch;
	
public:
	/**
	 * Method to look for available sessions. This method broadcast a delegate with all the found sessions. You can bind a
	 * Custom Event Listener to the OnSessionFoundEvent to handle the find session result.
	 * @param maxOnlineSessionsSearchResult Max number of sessions to return.
	 */
	UFUNCTION(BlueprintCallable)
	void FindSession(int32 maxOnlineSessionsSearchResult, float timeoutInSeconds = 120);

private:
	// This delegate is called by the online subsystem when it finishes the session finding. -Renan
	FOnFindSessionsCompleteDelegate OnFindSessionEvent;

	// This handle is used to store the delegate bind of OnFindSessionEvent. -Renan
	FDelegateHandle FindSessionDelegateHandle;

	void OnFindSessionEventListenerCallback(bool bWasSuccessfull);
	
public:
	// This is the event called when the Find Sessions process is done. This can also be used inside Blueprints. -Renan
	UPROPERTY(BlueprintAssignable)
	FEasyMultiplayerFindSessionDelegate OnSessionFoundEvent;

private:
	// This method handles the translation of the OnFindSessionEvent result to something that can be used by blueprints.
	// Normally this can't be done because the OnlineSubsystemInterface dispatch a result that is not compatible with the
	// blueprints reflection API. So I need to convert it to something usefull inside blueprints. -Renan
	FEMSOnlineSessionSearchResult ConvertSessionResult(FOnlineSessionSearchResult sessionResult);



	// ===================================================
	// Join Session
	// ===================================================
public:
	/**
	 * Method used to join a session. This method broadcast a delegate with the session join result. You can bind
	 * a Custom Event Listener to the OnSessionJoinedEvent to handle the join session result.
	 * @param onlineSessionSearchResult The session result that can be found using EasyMultiplayerSession::FindSessions method
	 */
	UFUNCTION(BlueprintCallable)
	void JoinSession(const FEMSOnlineSessionSearchResult& onlineSessionSearchResult);

private:
	// This delegate is called by the online subsystem when it finishes the session joining. -Renan
	FOnJoinSessionCompleteDelegate OnJoinSessionEvent;

	// This handle is used to store the delegate bind of OnJoinSessionEvent. -Renan
	FDelegateHandle JoinSessionDelegateHandle;

	

	// ===================================================
	// Start Session
	// ===================================================



	// ===================================================
	// Destroy Session
	// ===================================================



	
	
	
	
	
public:
	UPROPERTY(BlueprintAssignable)
	FEasyMultiplayerJoinSessionDelegate OnSessionJoinedEvent;

	UPROPERTY(BlueprintAssignable)
	FEasyMultiplayerStartSessionDeletage OnSessionStartedEvent;

	UPROPERTY(BlueprintAssignable)
	FEasyMultiplayerDestroySessionDelegate OnSessionDestroyedEvent;
	
private:
	
	// These events or delegates are called when the functions finish their execution. They need to be constructed first -Renan
	
	
	
	FOnStartSessionCompleteDelegate OnStartSessionEvent;
	FOnDestroySessionCompleteDelegate OnDestroySessionEvent;

	// These delegate handles will be used when the main delegates are not needed anymore. -Renan
	
	
	FDelegateHandle StartSessionDelegateHandle;
	FDelegateHandle DestroySessionDelegateHandle;

	// This variables are used to create the session after destroing a existing session when the user tries to
	// create a session. If there is a current existing session, I need to destroy it first. And after the destroy event is called
	// I can create a brand new session. -Renan
	bool bCreateSessionAfterDestroy { false };
	int32 CachedNumberOfPublicPlayers;
	FString CachedMatchTypeName;
	
	
public:
	

	
	

	

	/**
	 * Method used to connect to a joined session. If there is a valid joined session cached on the subsystem, this method will handle
	 * the Client travel. Does not broadcast delegates.
	 */
	UFUNCTION(BlueprintCallable)
	void ConnectToJoinedSession();
	
	UFUNCTION(BlueprintCallable)
	void StartSession();

	/**
	* Method to destroy the current active session. This method broadcast a delegate with the destroy session result. You can bind
	 * a Custom Event Listener to the OnSessionDestroyedEvent to handle the destroy session result.
	 */
	UFUNCTION(BlueprintCallable)
	void DestroySession();

	/**
	 * Method to Open a level as the host server. This method must be used after the host creates a session.
	 * @param pathToLobby String with the map name to open as server. Example: If your map is located under 'Content/Maps/MyMap.umap' just provide
	 * the following string: 'Maps/MyMap'
	 */
	UFUNCTION(BlueprintCallable)
	void OpenGameLevelAsHostServer(FString pathToLobby);


	UFUNCTION(BlueprintCallable)
	bool FilterSessionResultsByMatchType(const TArray<FEMSOnlineSessionSearchResult>& Results, const FString& MatchTypeName, FEMSOnlineSessionSearchResult& OutResult);

protected:
	// Internal callbacks for the events that ared called by the session funcions. -Renan
	
	void OnJoinSessionEventListenerCallback(FName joinedSessionName, EOnJoinSessionCompleteResult::Type joinResultType);
	void OnStartSessionEventListenerCallback(FName sessionName, bool bWasSuccessful);
	void OnDestroySessionEventListenerCallback(FName sessionName, bool bWasSuccessful);

private:
	
	
	EEMSJoinSessionCompleteResult ConvertJoinResult(const EOnJoinSessionCompleteResult::Type joinResultType);
};
