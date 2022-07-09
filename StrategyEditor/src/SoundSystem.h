#pragma once

#include "FMOD/fmod.hpp"
#include "FMOD/fmod_studio.hpp"
#include "FMOD/fsbank.h"

#include "Logging.h"

#include <map>


namespace sound
{
	class SoundSystem;


	/// @brief Generate a hash value for input string.
	/// Algorithm by Prof. Daniel J. Bernstein. Source: https://www.partow.net/programming/hashfunctions/#StringHashing 06 - DJB Hash Function.
	/// @param str 
	/// @param length 
	/// @return 
	size_t DJBHash(const char* str, size_t length)
	{
		size_t hash = length;
		for (size_t i = 0; i < length; ++str, ++i)
		{
			hash = ((hash << 5) + hash) + (*str);
		}
		return hash;
	}

	class SoundSource
	{
	public:
		struct Data
		{
			std::string m_Soundfilepath;
			std::string m_SoundName;
			std::string m_ChannelGroup;
			float m_X, m_Y, m_Z;
			float m_VolumeFalloffFactor;
			float m_Radius;
		};


	public:
		SoundSource() = default;
		~SoundSource();

		bool Initialize(SoundSystem* system, SoundSource::Data* data);
		void Terminate();

		bool Play(SoundSystem* system, FMOD::ChannelGroup* sound_source_channel_group, bool loop_sound);
		void Stop();
		void Pause();


		Data m_SoundData;

	private:
		FMOD::Sound* m_FMODSound;
		FMOD::Channel* m_FMODChannel;
	};



	class SoundSystem
	{
	public:
		STATIC_GET_DEL(SoundSystem, g_SoundSystem);

		//////////////////////////////////////////////////////
		// Sound System base functionality.
		//////////////////////////////////////////////////////

		/// @brief Initialize the Sound System to be ready to create and play sound sources.
		/// @return True on success.
		bool Initialize();

		/// @brief Terminate the Sound System and release any used data.
		void Terminate(); // TODO


		/// @brief Get the FMOD System.
		/// @return Pointer to FMOD System.
		FMOD::System* System();

		/// @brief Update the volume of all sound sources based on the listener position.
		/// Also update which sounds to play and which to stop.
		/// @param camerax X Position of the listener.
		/// @param cameray Y Position of the listener.
		/// @param cameraz Z Position of the listener, i.e. his Height.
		void Update(float camerax, float cameray, float cameraz); // TODO
		



		//////////////////////////////////////////////////////
		// Sound Source Control and Creation functionality.
		//////////////////////////////////////////////////////

		/// @brief Create a Sound Source. Will not be played yet.
		/// @param data The initialization data for the sound source.
		/// @return True on success.
		bool CreateSound(SoundSource::Data* data);
		

		/// @brief Delete a Sound Source and free the memory. If it is played it will be stopped abruptly.
		/// @param sound_source_name Name of the Sound Source.
		void DeleteSound(const std::string& sound_source_name);


		/// @brief Try to Play a Sound Source on a Channel Group. The Channel Group can be changed dynamically.
		/// @param sound_source_name Name of the Sound Source.
		/// @param sound_channel_group Name of the Channel Group.
		/// @return True on success.
		bool PlaySound(const std::string& sound_source_name, const std::string& sound_channel_group, bool loop_sound);


		/// @brief Stop a Sound Source from playing. The sound is stopped abruptly.
		/// @param sound_source_name Name of the Sound Source.
		void StopSound(const std::string& sound_source_name);


		/// @brief Pause a sound from playing. The sound is paused abruptly.
		/// @param sound_source_name Name of the Sound Source.
		void PauseSound(const std::string& sound_source_name);


	private:
		static SoundSystem* g_SoundSystem;

		FMOD::System* m_FMODSystem;

		std::map< size_t, SoundSource* > m_SoundSourceMap;

		std::map< size_t, FMOD::ChannelGroup* > m_ChannelGroupMap;


	private:
		//////////////////////////////////////////////////////
		// Internal Utility functionality.
		//////////////////////////////////////////////////////

		void _addChannelGroup(const std::string& sound_channel_group, FMOD::ChannelGroup* group);
		void _addSoundSource(const std::string& sound_source_name, SoundSource* sound_source);
		SoundSource* _getSound(const std::string& sound_source_name);
		FMOD::ChannelGroup* _getChannelGroup(const std::string& sound_channel_group);
		void _removeSoundSource(const std::string& sound_source_name);
		void _removeChannelGroup(const std::string& sound_channel_group);
	};
}