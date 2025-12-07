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

#ifndef Asteroids_MusicManager_hpp
#define Asteroids_MusicManager_hpp

#include "asteroids.hpp"

namespace Asteroids::MusicSystem
{
	namespace Implementation
	{
		class MusicManagerCreator;
	}

	enum MusicTrack
	{
		None,
		Opening,
		Main,
		Race
	};


	namespace
	{
		String kOpeningMusic = "music";
		String kMainMusic = "music";
		String kRaceMusic = "music";
		const float kBaseTrackVolume = 0.7f;
		const int kBaseFadeTime = 500;
	}

	class MusicManager : public tbCore::Noncopyable
	{
	public:
		void PlayMusic(MusicTrack musicTrack);
		void SetChillMusicVolume(float volume = kBaseTrackVolume);
		void SetStressMusicVolume(float volume = kBaseTrackVolume);

		void StopMusic(void);

		MusicManager(void);
		~MusicManager(void);

	private:
		void PlayMusicEvent(String musicEvent);

		MusicTrack mCurrentMusicTrack;

		tbAudio::AudioController mCurrentMusic;
		tbAudio::AudioController mCurrentChillMusic;
		tbAudio::AudioController mCurrentStressMusic;

		friend class Implementation::MusicManagerCreator;
	}; //namespace AudioManager

	extern MusicManager& theMusicManager;
};	//namespace Asteroids

#endif /* Asteroids_MusicManager_hpp */
