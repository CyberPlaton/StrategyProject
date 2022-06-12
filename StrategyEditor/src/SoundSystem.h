#ifndef _SOUND_SYSTEM_H_
#define _SOUND_SYSTEM_H_

#include "FMOD/fmod.hpp"
#include "FMOD/fmod_studio.hpp"
#include "FMOD/fsbank.h"

#include "Logging.h"

#include <map>
#include <vector>
#include <string>

#define STATIC_GET_DEL(CLASS, STATIC_MEMBER) \
static CLASS* get() \
{ if(!STATIC_MEMBER){STATIC_MEMBER = new CLASS();} return STATIC_MEMBER;} \
static void del() \
{if(STATIC_MEMBER){delete STATIC_MEMBER; STATIC_MEMBER = nullptr;}}

#define CONCAT_STRING(a, b) a##b

#define DECLARE_GET_SET(returntype, name, member) \
returntype CONCAT_STRING(Get, name)() \
{ \
	return member; \
} \
void CONCAT_STRING(Set, name)(returntype value) \
{ \
	member = value; \
} \

#define SOUND_DEFAULT_DISTANCE_FACTOR 1.0f // 1 Meter Unit.
#define SOUND_DEFAULT_ROLLOFF_FACTOR 0.0f  // Use FMOD_3D_LINEARSQUAREROLLOFF instead.
#define SOUND_DEFAULT_CHANNEL_COUNT 256
#define SOUND_DEFAULT_MIN_DISTANCE 0.1f
#define SOUND_DEFAULT_MAX_DISTANCE 256 * 128.0 * SOUND_DEFAULT_DISTANCE_FACTOR


/// @brief Managing a single channel and its related sound.
struct SoundChannel
{
	static bool LoadSoundToChannel(SoundChannel* channel, const std::string& filepath, bool sound_2d = false);
	static void UnloadSoundFromChannel(SoundChannel* channel);
	static void AddChannelToGroup(SoundChannel* channel, const std::string& group_name);

	struct Data
	{
		/// @brief A linear volume level. 0.0 = silent, 1.0 = full volume. Default = 1.0. Negative volumes and amplification (> 1.0) are supported.
		float m_volume;

		/// @brief Pitch value, 0.5 = half pitch, 2.0 = double pitch, etc default = 1.0.
		float m_pitch;
		
		/// @brief A left/right pan level, from -1.0 to 1.0 inclusive. -1.0 = Full left, 0.0 = center, 1.0 = full right. Default = 0.0.
		float m_pan;

		/// @brief 3D only. The position in game space.
		FMOD_VECTOR m_position;

		/// @brief 3D only. The velocity of the sound.
		FMOD_VECTOR m_velocity;

		/// @brief Whether playing mode is looping.
		bool m_looped;

		/// @brief Whether currently playing.
		bool m_played;

		/// @brief Whether we have successfully loaded an FMOD::Sound.
		bool m_hasSound;

		/// @brief Either 2D or 3D.
		bool m_2d;

		/// @brief The Sound which was created by FMOD.
		FMOD::Sound* m_sound;
		
		/// @brief The underlying Channel on which this sound is played.
		FMOD::Channel* m_channel;

		/// @brief To which ChannelGroup this sound belongs. 
		FMOD::ChannelGroup* m_group;

		/// @brief The user given name of the sound source.
		std::string m_name;
	};


	void Release()
	{
		if (GetIsPlayed()) { Stop(); }

		if (GetHasSound()) { UnloadSoundFromChannel(this); }

		// Apparently FMOD::SoundChannel* does not need to be released.
		m_data.m_channel = nullptr;
	}


	/// @brief Comman FMOD to play this sound channel with specified options in data. 
	void Play();

	/// @brief If played pause sound. 
	void Pause();
	
	/// @brief If played stop the sound. On next play it will start from beginning.
	void Stop();


	DECLARE_GET_SET(float, Volume, m_data.m_volume);
	DECLARE_GET_SET(float, Pitch, m_data.m_pitch);
	DECLARE_GET_SET(float, Pan, m_data.m_pan);
	DECLARE_GET_SET(FMOD_VECTOR, Velocity, m_data.m_velocity);
	DECLARE_GET_SET(bool, Looped, m_data.m_looped);
	DECLARE_GET_SET(const std::string&, Name, m_data.m_name);
	DECLARE_GET_SET(bool, HasSound, m_data.m_hasSound);
	DECLARE_GET_SET(FMOD::Channel*, Channel, m_data.m_channel);
	DECLARE_GET_SET(FMOD::ChannelGroup*, ChannelGroup, m_data.m_group);
	DECLARE_GET_SET(FMOD::Sound*, Sound, m_data.m_sound);
	DECLARE_GET_SET(bool, Is2D, m_data.m_2d);


	void SetPosition(FMOD_VECTOR position);
	FMOD_VECTOR GetPosition() { return m_data.m_position; }

	bool GetIsPlayed();

private:
	Data m_data;
};


/// @brief Managing (Loading, releasing etc.) channel groups, channels and sound data with FMOD.
class SoundSystem
{
public:
	STATIC_GET_DEL(SoundSystem, g_SoundSystem);

	FMOD::System* System() { return m_system; }

	bool Initialize();
	void Terminate();

	/// @brief 
	void Update();

	/// @brief Set data of the main listener position. This is used to simulate 3D sound.
	/// @param x 
	/// @param y 
	/// @param z 
	void SetListenerPositionVector(float x, float y, float z);

	/// @brief Create a channel group.
	/// @param name 
	/// @return 
	bool CreateChannelGroup(const std::string& name, const std::string& parent = "Master");
	bool CreateMasterChannelGroup(const std::string& name = "Master");
	void ReleaseAllChannelGroups();
	void ReleaseChannelGroup(FMOD::ChannelGroup* group);

	bool CreateSoundOnChannel(const std::string& filepath, const std::string& sound_name, const std::string& channel_group_name, bool loop, bool is2d, FMOD_VECTOR position, float vol, float pitch, float pan, bool start_playing_directly);
	bool CreateSoundOnChannel(const std::string& filepath, const std::string& name, const std::string& channel_group_name, bool sound_2d = false, FMOD_VECTOR position = {0.0f, 0.0f, 0.0f});


	SoundChannel* GetSound(const std::string& sound);


	FMOD::ChannelGroup* GetChannelGroup(const std::string& name);

private:
	static SoundSystem* g_SoundSystem;
	FMOD::System* m_system;

	float m_interfaceUpdateTime = 50;

	/// @brief Listeners positional value.
	float m_listenerX;
	float m_listenerY;
	float m_listenerZ;

	/// @brief  Listener previous position for computing velocity.
	float m_listenerXBefore;
	float m_listenerYBefore;
	float m_listenerZBefore;

	std::vector< FMOD::ChannelGroup* > m_channelGroupVec;
	std::vector< SoundChannel* > m_soundChannelVec;

private:

	uint64_t m_nextHashValue = 0;
	std::map< std::string, uint64_t > m_hashValueMap;
	uint64_t GetHashValue(const std::string& name);
};

#endif