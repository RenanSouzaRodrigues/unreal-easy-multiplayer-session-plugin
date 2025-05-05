#pragma once

#include "CoreMinimal.h"
#include "EEMSJoinSessionCompleteResult.generated.h"

UENUM(BlueprintType, Blueprintable)
enum class EEMSJoinSessionCompleteResult : uint8 {
	Success,
	SessionIsFull,
	SessionDoesNotExist,
	CouldNotRetrieveAddress,
	AlreadyInSession,
	UnknownError
};
