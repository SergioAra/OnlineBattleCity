// Copyright Epic Games, Inc. All Rights Reserved.

#include "pch.h"
#include "Menu.h"
#include "Level.h"
#include "Game.h"
#include "Console.h"

FGame::FGame() noexcept(false) :
	FBaseGame()
{
	Menu = std::make_shared<FMenu>(Console);
	Level = std::make_unique<FLevel>();
	CustomInvites = std::make_unique<FCustomInvites>();

	CreateConsoleCommands();
}

FGame::~FGame()
{
}

void FGame::CreateConsoleCommands()
{
	FBaseGame::CreateConsoleCommands();
	if (Console)
	{
		const std::vector<const wchar_t*> ExtraHelpMessageLines =
		{
			L" SIMULATECUSTOMINVITE - simulates an incoming custom invite, debug testing only",
		};
		AppendHelpMessageLines(ExtraHelpMessageLines);

#if defined(_DEBUG) || defined(_TEST)
		Console->AddCommand(L"SIMULATECUSTOMINVITE", [](const std::vector<std::wstring>& args)
		{
			FGame::Get().GetCustomInvites()->HandleCustomInviteReceived("TEST PAYLOAD", "inviteidINVITEIDinviteid", FAccountHelpers::ProductUserIDFromString("fakeRemoteUser-1234"));
		});
#endif
	}
}

void FGame::OnGameEvent(const FGameEvent& Event)
{
	CustomInvites->OnGameEvent(Event);
	FBaseGame::OnGameEvent(Event);
}
