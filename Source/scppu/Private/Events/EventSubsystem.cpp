#include "Events/EventSubsystem.h"

void UGameEventSubsystem::Initialize()
{

}

void UGameEventSubsystem::Deinitialize()
{

}

FGameEventHandle UGameEventSubsystem::ExecuteNormalEvent(UNormalGameEvent* EventAsset, UGameEventPayload* Payload, FTransform WorldTransform)
{
	return FGameEventHandle();
}

FGameEventHandle UGameEventSubsystem::ExecutePeriodicEvent(UPeriodicGameEvent* EventAsset, UGameEventPayload* Payload)
{
	return FGameEventHandle();
}

AGameEventExecutor* UGameEventSubsystem::GetEventExecutor(FGameEventHandle Handle) const
{
	return nullptr;
}

EGameEventStatus UGameEventSubsystem::GetEventStatus(FGameEventHandle Handle) const
{
	return EGameEventStatus::Invalid;
}

UGameEventPayload* UGameEventSubsystem::GetEventPayload(FGameEventHandle Handle) const
{
	return nullptr;
}

void UGameEventSubsystem::SetEventPayload(FGameEventHandle Handle, UGameEventPayload* NewPayload)
{
}


UBaseGameEvent* UGameEventSubsystem::GetEventAsset(FGameEventHandle Handle) const
{
	return nullptr;
}

void UGameEventSubsystem::CancelEvent(FGameEventHandle Handle, bool bForceCancel)
{
}
