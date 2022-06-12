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

	result = m_system->init(SOUND_DEFAULT_CHANNEL_COUNT, FMOD_3D_LINEARROLLOFF | FMOD_INIT_NORMAL | FMOD_LOWMEM, 0);
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

	// Update Audio Source positions according to listener positions.
	FMOD_VECTOR position = {0, 0, 0};
	FMOD_VECTOR velocity = {0, 0, 0};
	for (auto& sound : m_soundChannelVec)
	{
		FMOD_VECTOR p = sound->GetPosition();

		position = { p.x - (m_listenerX - m_listenerXBefore), p.y - (m_listenerY - m_listenerYBefore), p.z - (m_listenerZ - m_listenerZBefore) };

		sound->GetChannel()->set3DAttributes(&position, &velocity);
	}

	// Update FMOD.
	system->update();
}

bool SoundSystem::ChangeSoundSourceName(const std::string& sound_source, const std::string& new_sound_source_name)
{
	for (auto& sc : m_soundChannelVec)
	{
		if (sc->GetName().compare(sound_source) == 0)
		{
			sc->SetName(new_sound_source_name);
			return true;
		}
	}
	return false;
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

	SoundChannel* sc = new SoundChannel();
	if (SoundChannel::LoadSoundToChannel(sc,  filepath, sound_2d))
	{
		SoundChannel::AddChannelToGroup(sc, channel_group_name);

		sc->SetPosition(position);
		sc->SetChannelGroup(m_channelGroupVec[index]);
		sc->SetName(name);

		m_soundChannelVec.push_back(sc);

		return true;
	}


	// Clean up on failure.
	delete sc;
	sc = nullptr;
	return false;
}

bool SoundSystem::CreateSoundOnChannel(const std::string& filepath, const std::string& sound_name, const std::string& channel_group_name, bool loop, bool is2d, FMOD_VECTOR position, float vol, float pitch, float pan, bool start_playing_directly)
{
	if (CreateSoundOnChannel(filepath, sound_name, channel_group_name, is2d, position))
	{
		auto s = GetSound(sound_name);

		// Loop
		if (loop)
		{
			s->GetSound()->setMode(FMOD_LOOP_NORMAL);
			s->SetLooped(loop);
		}

		// Vol
		s->SetVolume(vol);

		// Pitch
		s->SetPitch(pitch);

		// Pan
		s->SetPan(pan);

		if (start_playing_directly) s->Play();

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




bool SoundChannel::LoadSoundToChannel(SoundChannel* channel, const std::string& filepath, bool sound_2d)
{
	auto system = SoundSystem::get()->System();

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
		channel->SetIs2D(sound_2d);

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

		channel->m_data.m_sound = nullptr;
		
		channel->SetHasSound(false);
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

	// Set data of SoundChannel
	m_data.m_channel->setVolume(m_data.m_volume); // Volume. Done on Channel.
	if (!m_data.m_2d)
	{
		m_data.m_channel->set3DAttributes(&m_data.m_position, &m_data.m_velocity); // 3D data. Done on Channel.

		//m_data.m_sound->set3DConeSettings(100.0f, 260.0f, 0.1f);
		m_data.m_sound->set3DMinMaxDistance(SOUND_DEFAULT_MIN_DISTANCE, SOUND_DEFAULT_MAX_DISTANCE);
	}
	m_data.m_channel->setPan(m_data.m_pan); // Pan. Done on Channel.
	m_data.m_channel->setPitch(m_data.m_pitch); // Pitch. Done on Channel.
	if (m_data.m_looped)
	{
		m_data.m_sound->setMode(FMOD_LOOP_NORMAL); // Loop. Done on Sound.
	}

	m_data.m_channel->setChannelGroup(m_data.m_group); // ChannelGroup

	// Play SoundChannel
	auto result = system->playSound(m_data.m_sound, m_data.m_group, false, &m_data.m_channel);
	if (result == FMOD_OK)
	{
		m_data.m_played = true;
	}
}

void SoundChannel::Pause()
{
	if (!m_data.m_played) return;


	m_data.m_played = false;
}

void SoundChannel::Stop()
{
	if (!m_data.m_played) return;

	m_data.m_channel->stop();

	m_data.m_played = false;
}

bool SoundChannel::GetIsPlayed()
{
	m_data.m_channel->isPlaying(&m_data.m_played);
	return m_data.m_played;
}

void SoundChannel::SetPosition(FMOD_VECTOR position)
{
	m_data.m_position.x = position.x;
	m_data.m_position.y = position.y;
	m_data.m_position.z = position.z;

	FMOD_VECTOR vel = { 0, 0, 0 };

	m_data.m_channel->set3DAttributes(&m_data.m_position, &vel);
}