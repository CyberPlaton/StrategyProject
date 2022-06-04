#include "SoundSystem.h"

SoundSystem* SoundSystem::g_SoundSystem = nullptr;

uint64_t SoundSystem::GetHashValue(const std::string& name)
{
	if (m_hashValueMap.find(name) == m_hashValueMap.end())
	{
		m_hashValueMap.try_emplace(name, m_nextHashValue++);
	}

	return m_hashValueMap[name];
}

bool SoundSystem::Initialize()
{
	// Initialize System.
	auto result = FMOD::System_Create(&m_system);
	if (result != FMOD_OK) return false;

	result = m_system->init(SOUND_DEFAULT_CHANNEL_COUNT, FMOD_3D_LINEARSQUAREROLLOFF | FMOD_INIT_NORMAL, 0);
	if (result != FMOD_OK) return false;

	m_channelGroupVec.resize(1024);

	// Create Master channel.
	FMOD::ChannelGroup* cg = nullptr;
	m_system->getMasterChannelGroup(&cg);

	auto index = GetHashValue("Master");
	m_channelGroupVec[index] = cg;


	// Set initial position.
	m_listenerX = 0;
	m_listenerY = 0;
	m_listenerZ = 0;
	m_listenerXBefore = 0;
	m_listenerYBefore = 0;
	m_listenerZBefore = 0;

	m_system->set3DSettings(0.0f, SOUND_DEFAULT_DISTANCE_FACTOR, SOUND_DEFAULT_ROLLOFF_FACTOR);

	return true;
}

void SoundSystem::Terminate()
{
	// Release all sounds.
	while (m_soundChannelVec.size() > 0)
	{
		auto sound = m_soundChannelVec[m_soundChannelVec.size() - 1];
		m_soundChannelVec.pop_back();

		sound->Release();
		delete sound; 
		sound = nullptr;
	}

	// Release all groups.
	while (m_channelGroupVec.size() > 0)
	{
		auto group = m_channelGroupVec[m_channelGroupVec.size() - 1];
		m_channelGroupVec.pop_back();

		group->release();
	}

	// Close and release system.
	m_system->close();
	m_system->release();
}

void SoundSystem::Update()
{
	auto system = SoundSystem::get()->System();

	// Update listener position and velocity.
	FMOD_VECTOR up = { 0.0f, 1.0f, 0.0f };
	FMOD_VECTOR forward = { 0.0f, 0.0f, 1.0f };

	// Velocity is needed to produce a doppler effect. We dont require one.
	FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f };
	//vel.x = (m_listenerX - m_listenerXBefore) * (1000 / m_interfaceUpdateTime);
	//vel.y = (m_listenerY - m_listenerYBefore) * (1000 / m_interfaceUpdateTime);
	//vel.z = (m_listenerZ - m_listenerZBefore) * (1000 / m_interfaceUpdateTime);

	m_listenerXBefore = m_listenerX;
	m_listenerYBefore = m_listenerY;
	m_listenerZBefore = m_listenerZ;

	FMOD_VECTOR lastpos = {m_listenerXBefore, m_listenerYBefore, m_listenerZBefore };
	FMOD_VECTOR listenerpos = { m_listenerX, m_listenerY, m_listenerZ };
	system->set3DListenerAttributes(0, &listenerpos, &vel, &forward, &up);


	// Update FMOD.
	system->update();
}

void SoundSystem::SetListenerPositionVector(float x, float y, float z)
{
	m_listenerX = x;
	m_listenerY = y;
	m_listenerZ = z;
}

bool SoundSystem::CreateChannelGroup(const std::string& name, const std::string& parent)
{
	FMOD::ChannelGroup* cg = nullptr;
	m_system->createChannelGroup(name.c_str(), &cg);

	// Add new ChannelGroup to storage under specific index.
	auto index = GetHashValue(name);
	m_channelGroupVec[index] = cg;

	// Get master ChannelGroup and add group to it.
	index = GetHashValue(parent);
	m_channelGroupVec[index]->addGroup(cg);


	return cg != nullptr;
}

bool SoundSystem::CreateMasterChannelGroup(const std::string& name /*= "Master"*/)
{
	FMOD::ChannelGroup* cg = nullptr;
	m_system->getMasterChannelGroup(&cg);

	auto index = GetHashValue("Master");
	m_channelGroupVec[index] = cg;

	return cg != nullptr;
}

void SoundSystem::ReleaseAllChannelGroups()
{
	for (int i = 0; i < m_channelGroupVec.size(); i++)
	{
		auto group = m_channelGroupVec[i];
		if (group)
		{
			ReleaseChannelGroup(group);
			m_channelGroupVec[i] = nullptr;
		}
	}

	m_hashValueMap.clear();
	m_nextHashValue = 0;
}

void SoundSystem::ReleaseChannelGroup(FMOD::ChannelGroup* group)
{
	group->release();
	group = nullptr;
}

bool SoundSystem::CreateSoundOnChannel(const std::string& filepath, const std::string& name, const std::string& channel_group_name, bool sound_2d, FMOD_VECTOR position)
{
	auto index = GetHashValue(channel_group_name);

	SoundChannel* sc = nullptr;
	if (sc = SoundChannel::LoadSoundToChannel(filepath, sound_2d); sc != nullptr)
	{
		SoundChannel::AddChannelToGroup(sc, channel_group_name);

		sc->SetPosition(position);
		sc->SetChannelGroup(m_channelGroupVec[index]);
		sc->SetName(name);

		m_soundChannelVec.push_back(sc);

		return true;
	}

	return false;
}

SoundChannel* SoundSystem::GetSound(const std::string& sound)
{
	for (auto& s : m_soundChannelVec)
	{
		auto n = s->GetName();

		if (n.compare(sound) == 0)
		{
			return s;
		}
	}

	return nullptr;
}

FMOD::ChannelGroup* SoundSystem::GetChannelGroup(const std::string& name)
{
	auto index = GetHashValue(name);
	return m_channelGroupVec[index];
}




SoundChannel* SoundChannel::LoadSoundToChannel(const std::string& filepath, bool sound_2d)
{
	auto system = SoundSystem::get()->System();
	auto channel = new SoundChannel();

	if (channel->GetHasSound())
	{
		SoundChannel::UnloadSoundFromChannel(channel);
	}

	auto mode = FMOD_3D;
	if (sound_2d) mode = FMOD_2D;

	auto result = system->createSound(filepath.c_str(), mode, 0, &channel->m_data.m_sound);

	if (result == FMOD_OK)
	{
		channel->SetHasSound(true);

		channel->m_data.m_sound->set3DMinMaxDistance(SOUND_DEFAULT_MIN_DISTANCE, SOUND_DEFAULT_MAX_DISTANCE);

		system->playSound(channel->GetSound(), 0, true, &channel->m_data.m_channel);

		return channel;
	}

	return nullptr;
}

void SoundChannel::UnloadSoundFromChannel(SoundChannel* channel)
{
	if (channel->GetHasSound())
	{
		channel->GetSound()->release();
	}
}

void SoundChannel::AddChannelToGroup(SoundChannel* channel, const std::string& group_name)
{
	auto system = SoundSystem::get();

	auto group = system->GetChannelGroup(group_name);

	channel->GetChannel()->setChannelGroup(group);
}

void SoundChannel::Play()
{
	if (m_data.m_played) return;

	auto system = SoundSystem::get()->System();

	m_data.m_channel->setVolume(m_data.m_volume);

	system->playSound(m_data.m_sound, m_data.m_group, false, &m_data.m_channel);

	m_data.m_channel->set3DAttributes(&m_data.m_position, &m_data.m_velocity);


	m_data.m_played = true;
}

void SoundChannel::Pause()
{
	if (!m_data.m_played) return;


	m_data.m_played = false;
}

void SoundChannel::Stop()
{
	if (!m_data.m_played) return;


	m_data.m_played = false;
}
