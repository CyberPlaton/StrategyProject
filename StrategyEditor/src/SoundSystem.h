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
#define SOUND_DEFAULT_MAX_DISTANCE 256 * 128.0f


/// @brief Managing a single channel and its related sound.
struct SoundChannel
{
	static SoundChannel* LoadSoundToChannel(const std::string& filepath, bool sound_2d = false);
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

		FMOD_VECTOR m_position;

		FMOD_VECTOR m_velocity;

		bool m_looped;

		bool m_played;

		bool m_hasSound;

		FMOD::Sound* m_sound;

		FMOD::Channel* m_channel;

		FMOD::ChannelGroup* m_group;

		std::string m_name;
	};


	void Release()
	{
		// if HasSound(){ UnloadSoundFromChannel( this ); }

		// m_channel->release();
	}


	void Play();
	void Pause();
	void Stop();


	DECLARE_GET_SET(float, Volume, m_data.m_volume);
	DECLARE_GET_SET(float, Pitch, m_data.m_pitch);
	DECLARE_GET_SET(float, Pan, m_data.m_pan);
	DECLARE_GET_SET(FMOD_VECTOR, Position, m_data.m_position);
	DECLARE_GET_SET(FMOD_VECTOR, Velocity, m_data.m_velocity);
	DECLARE_GET_SET(bool, Looped, m_data.m_looped);
	DECLARE_GET_SET(const std::string&, Name, m_data.m_name);
	DECLARE_GET_SET(bool, HasSound, m_data.m_hasSound);
	DECLARE_GET_SET(FMOD::Channel*, Channel, m_data.m_channel);
	DECLARE_GET_SET(FMOD::ChannelGroup*, ChannelGroup, m_data.m_group);
	DECLARE_GET_SET(FMOD::Sound*, Sound, m_data.m_sound);
	DECLARE_GET_SET(bool, IsPlayed, m_data.m_played);

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