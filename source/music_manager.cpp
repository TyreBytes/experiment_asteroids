///
/// @file
/// @details Music Manager for Asteroids to manage music track toggling between normal, chill and stress track.
///		This is meant to be additive to the current music track that is playing. So, music tracks should be layered.
///
///		Example:
///
///		PlayMusic(MusicTrack::Race); // will start 3 audio track, the base audio, the chill audio and the stress audio.
///		SetChillMusicVolume(0.0f); // will mute the chill audio
/// 	SetChillMusicVolume(0.5f); // will unmute the chill audio layer.
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#include "music_manager.hpp"
#include "logging.hpp"

//--------------------------------------------------------------------------------------------------------------------//

namespace Asteroids::MusicSystem::Implementation
{
	class MusicManagerCreator
	{
	public:
		MusicManagerCreator(void) :
			mMusicManager()
		{
		}

		~MusicManagerCreator(void)
		{
		}

		MusicManager mMusicManager;
	};
};

using namespace Asteroids::MusicSystem::Implementation;

MusicManagerCreator theMusicManagerCreator;
Asteroids::MusicSystem::MusicManager& Asteroids::MusicSystem::theMusicManager(theMusicManagerCreator.mMusicManager);

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::MusicSystem::MusicManager::MusicManager(void)
	:
	mCurrentMusicTrack(MusicTrack::None)
{
}

Asteroids::MusicSystem::MusicManager::~MusicManager(void)
{
}

//--------------------------------------------------------------------------------------------------------------------//

// Plays a music track but won't restart the track if it's already playing.
void Asteroids::MusicSystem::MusicManager::PlayMusic(MusicTrack musicTrack)
{
	if (mCurrentMusicTrack == musicTrack)
	{ // We're already playing that.
		return;
	}

	if (MusicTrack::Main == musicTrack)
	{
		StopMusic();
		PlayMusicEvent(kMainMusic);
		mCurrentMusicTrack = MusicTrack::Main;
	}
	else if (MusicTrack::Race == musicTrack)
	{
		StopMusic();
		PlayMusicEvent(kRaceMusic);
		mCurrentMusicTrack = MusicTrack::Race;
	}
	else if (MusicTrack::Opening == musicTrack)
	{
		StopMusic();
		PlayMusicEvent(kOpeningMusic);
		mCurrentMusicTrack = MusicTrack::Opening;
	}
	else if (MusicTrack::None == musicTrack)
	{
		StopMusic();
	}
	else
	{
		tb_always_log(LogAudio::Warning() << "Tried to play unknown music track.");
	}
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::MusicSystem::MusicManager::PlayMusicEvent(String musicEvent)
{
	mCurrentMusic = tbAudio::theAudioManager.PlayEvent("audio_events", musicEvent);
	mCurrentChillMusic = tbAudio::theAudioManager.PlayEvent("audio_events", musicEvent + "_chill");
	mCurrentStressMusic = tbAudio::theAudioManager.PlayEvent("audio_events", musicEvent + "_stress");

	tb_debug_log(LogAudio::Debug() << "Playing music track " + musicEvent);
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::MusicSystem::MusicManager::StopMusic(void)
{
	// 2025-10-21: for some reasons, it crashes if we don't stop the looping first.
	mCurrentMusic.StopLooping();
	mCurrentChillMusic.StopLooping();
	mCurrentStressMusic.StopLooping();

	mCurrentMusic.Stop();
	mCurrentChillMusic.Stop();
	mCurrentStressMusic.Stop();

	mCurrentMusicTrack = MusicTrack::None;

	tb_debug_log(LogAudio::Debug() << "Stopped music track.");
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::MusicSystem::MusicManager::SetChillMusicVolume(float volume)
{
	mCurrentChillMusic.FadeVolumeTo(volume, kBaseFadeTime);
}

//--------------------------------------------------------------------------------------------------------------------//

void Asteroids::MusicSystem::MusicManager::SetStressMusicVolume(float volume)
{
	mCurrentStressMusic.FadeVolumeTo(volume, kBaseFadeTime);
}

//--------------------------------------------------------------------------------------------------------------------//
