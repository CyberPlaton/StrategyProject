#pragma once

// FMOD INCLUDE
#include "FMOD/fmod.hpp"
#include "FMOD/fmod_studio.hpp"
#include "FMOD/fsbank.h"
// COMMMON INCLUDE
#include "Logging.h"
#include <map>
#include <vector>


// Sometimes, Windows touches me where no one has ever been...
#ifdef PlaySound
#undef PlaySound
#endif

namespace sound
{
	class SoundSystem;


	/// @brief Generate a hash value for input string.
	/// Algorithm by Prof. Daniel J. Bernstein. Source: https://www.partow.net/programming/hashfunctions/#StringHashing 06 - DJB Hash Function.
	/// @param str 
	/// @param length 
	/// @return 
	static size_t DJBHash(const char* str, size_t length)
	{
		size_t hash = length;
		for (size_t i = 0; i < length; ++str, ++i)
		{
			hash = ((hash << 5) + hash) + (*str);
		}
		return hash;
	}

	/// @brief 
	class SoundSource
	{
	public:
		struct Data
		{
			Data() : m_X(-INT_MAX), m_Y(-INT_MAX), m_Z(-INT_MAX),
					 m_VolumeFalloffFactor(-INT_MAX), m_Radius(-INT_MAX),
					 m_Loop(false)
			{

			}

			std::string m_Soundfilepath;
			std::string m_SoundName;
			std::string m_ChannelGroup;
			float m_X, m_Y, m_Z;
			float m_VolumeFalloffFactor;
			float m_Radius;
			float m_FadeoutRadius;
			bool m_Loop;
		};


	public:
		SoundSource() : m_FMODSound(nullptr), m_FMODChannel(nullptr)
		{
		}
		~SoundSource();


		//////////////////////////////////////////////////////
		// Sound Source base functionality.
		//////////////////////////////////////////////////////
		
		/// @brief Create a Sound resource using FMOD and copy the Data.
		/// @param system Pointer to Sound System to acquire the FMOD System.
		/// @param data Pointer to the defining Data.
		/// @return True on success.
		bool Initialize(SoundSystem* system, SoundSource::Data* data);

		/// @brief Release FMOD Sound resource and free used memory.
		void Terminate();

		/// @brief Try to play FMOD sound. The Sound must belong to a Channel Group. A new Channel will be created.
		/// @param system The initialized Sound System.
		/// @param sound_source_channel_group The Channel Group to which the created Channel will be attached. By default "sound_source_channel_group" will be "Master".
		/// @param loop_sound Whether to loop the sound.
		/// @return True on success.
		bool Play(SoundSystem* system, FMOD::ChannelGroup* sound_source_channel_group, bool loop_sound);
		
		/// @brief Stop the sound from playing and delete the Channel.
		void Stop();
		
		/// @brief Just pause the sound from playing. Calling "Play" after this will play the sound again.
		void Pause();
		



		
		/////////////////////////////////////////////
		// Sound Source basic setters and getters.
		/////////////////////////////////////////////

		/// @brief Set the Volume for the Sound Source playing.
		/// Note that we set the FMOD Channel volume.
		/// @param v Volume, where 0.0f is mute and 1.0f is maximum possible.
		void SetVolume(float v);

		/// @brief Retrieve the current volume of the Sound Source.
		/// Note that it is possible, that the volume is non-zero, and the Sound Source still
		/// unhearable, because it is stopped or paused.
		/// @return Value between 0.0f and 1.0f, where 0.0f is muted and 1.0f is at max volume.
		float GetVolume();

		/// @brief Set the position of the Sound Source.
		/// @param v 3D Vector specifying the position.
		void SetPosition(FMOD_VECTOR v);

		/// @brief Retrieve the current position of the Sound Source.
		/// @return 3D Vector specifying the position.
		FMOD_VECTOR GetPosition();
		
		/// @brief Retrieve whether the Sound Source is currently playing.
		/// @return False if paused or stopped.
		bool Playing();

		DECLARE_GET_SET(float, Radius, m_SoundData.m_Radius);
		DECLARE_GET_SET(float, FadeoutRadius, m_SoundData.m_FadeoutRadius);
		DECLARE_GET_SET(float, VolumeFalloffFactor, m_SoundData.m_VolumeFalloffFactor);
		DECLARE_GET_SET(bool, Looped, m_SoundData.m_Loop);
		DECLARE_GET_SET(std::string, ChannelGroupName, m_SoundData.m_ChannelGroup);


		/////////////////////////////////////////////
		// Sound Source utility functionality.
		/////////////////////////////////////////////

		unsigned int MSTotal();
		unsigned int SecTotal();
		unsigned int MinTotal();

		unsigned int MSCurrent();
		unsigned int SecCurrent();
		unsigned int MinCurrent();


		Data m_SoundData;

	private:
		FMOD::Sound* m_FMODSound;
		FMOD::Channel* m_FMODChannel;
	};



	/// @brief
	class SoundSystem
	{
	public:
		STATIC_GET_DEL(SoundSystem, g_SoundSystem);

		//////////////////////////////////////////////////////
		// Sound System base functionality.
		//////////////////////////////////////////////////////

		/// @brief Initialize the Sound System to be ready to create and play sound sources.
		/// @return True on success.
		bool Initialize(float game_world_scale = 0.08f, float minimal_sound_source_volume = 0.001f);

		/// @brief Terminate the Sound System and release any used data.
		void Terminate();


		/// @brief Get the FMOD System.
		/// @return Pointer to FMOD System.
		FMOD::System* System();

		/// @brief Update the volume of all sound sources based on the listener position.
		/// Also update which sounds to play and which to stop.
		/// @param camerax X Position of the listener.
		/// @param cameray Y Position of the listener.
		/// @param cameraz Z Position of the listener, i.e. his Height.
		void Update(float camerax, float cameray, float cameraz);
		



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
		/// Fails if the specified Sound Source or the specified Channel Group does not exist.
		/// By default the Sound plays on "Master" and is not Looped.
		/// @param sound_source_name Name of the Sound Source.
		/// @param sound_channel_group Name of the Channel Group.
		/// @return True on success.
		bool PlaySound(const std::string& sound_source_name, const std::string& sound_channel_group = "Master", bool loop_sound = false);


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

		/// @brief The higher the scale, the faster the general Sound Source volume falloff.
		float m_GameWorldScale;

		/// @brief The minimal Sound Source volume value, after which it stops playing.
		float m_UnhearableBarrier;


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


		/// @brief Compute the 3D distance between 2 Points.
		/// @param a First Point X coordinate.
		/// @param b First Point Y coordinate.
		/// @param c First Point Z coordinate.
		/// @param x Second Point X coordinate.
		/// @param y Second Point Y coordinate.
		/// @param z Second Point Z coordinate.
		/// @return The distance value.
		float _computeDistance(float a, float b, float c, float x, float y, float z);


		/// @brief Compute the volume of a sound.
		/// @param sound_source_falloff_factor The sound sources falloff curve data.
		/// @param distance The sound sources distance between it and the listener.
		/// @return The volume clamped between 0.0 (OFF) and 1.0 (FULL).
		float _computeVolume(float sound_source_falloff_factor, float distance);
	};
}