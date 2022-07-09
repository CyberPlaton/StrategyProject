#include "SoundSystem.h"

namespace sound
{
	SoundSystem* SoundSystem::g_SoundSystem;


	SoundSource::~SoundSource()
	{
		Terminate();
	}

	bool SoundSource::Initialize(SoundSystem* system, SoundSource::Data* data)
	{
		auto s = system->System();

		auto r = s->createSound(data->m_Soundfilepath.c_str(), FMOD_2D, nullptr, &m_FMODSound);
		if(r == FMOD_OK)
		{
			// Copy data.
			m_SoundData.m_Soundfilepath = data->m_Soundfilepath;
			m_SoundData.m_SoundName = data->m_SoundName;
			m_SoundData.m_ChannelGroup = data->m_ChannelGroup;
			m_SoundData.m_X = data->m_X;
			m_SoundData.m_Y = data->m_Y;
			m_SoundData.m_Z = data->m_Z;
			m_SoundData.m_VolumeFalloffFactor = data->m_VolumeFalloffFactor;
			m_SoundData.m_Radius = data->m_Radius;

			return true;
		}

		return false;
	}

	void SoundSource::Terminate()
	{
		m_FMODSound->release();
		m_FMODChannel = nullptr;
		m_FMODSound = nullptr;

		m_SoundData.m_ChannelGroup.clear();
		m_SoundData.m_SoundName.clear();
		m_SoundData.m_Soundfilepath.clear();
		m_SoundData.m_X = -INT_MAX;
		m_SoundData.m_Y = -INT_MAX;
		m_SoundData.m_Z = -INT_MAX;
		m_SoundData.m_VolumeFalloffFactor = -INT_MAX;
		m_SoundData.m_Radius = -INT_MAX;
	}


	bool SoundSource::Play(SoundSystem* system, FMOD::ChannelGroup* sound_source_channel_group, bool loop_sound)
	{
		if(m_FMODSound)
		{
			if (m_FMODChannel)
			{
				// We are Paused or Already playing.
				m_FMODChannel->setPaused(false);
			}
			else
			{
				system->System()->playSound(m_FMODSound, sound_source_channel_group, false, &m_FMODChannel);
			}

			if (loop_sound)
			{
				m_FMODSound->setMode(FMOD_LOOP_NORMAL);
			}
			
			return true;
		}

		// Not initialized.
		return false;
	}


	void SoundSource::Stop()
	{
		m_FMODChannel->stop();
		m_FMODChannel = nullptr;
	}

	void SoundSource::Pause()
	{
		m_FMODChannel->setPaused(true);
	}


	//////////////////////////////////////////////////////
	// Sound System base functionality.
	//////////////////////////////////////////////////////

	bool SoundSystem::Initialize()
	{
		auto r = FMOD::System_Create(&m_FMODSystem);
		if(r == FMOD_OK)
		{
			r = m_FMODSystem->init(64, FMOD_INIT_NORMAL, nullptr);

			if (r == FMOD_OK)
			{
				// Create the first Master Channel Group.
				FMOD::ChannelGroup* master = nullptr;
				m_FMODSystem->getMasterChannelGroup(&master);

				_addChannelGroup("Master", master);
			}
		}

		return false;
	}

	void SoundSystem::Terminate()
	{
		// TODO
	}

	FMOD::System* SoundSystem::System()
	{
		return m_FMODSystem;
	}

	void SoundSystem::Update(float camerax, float cameray, float cameraz)
	{
		// TODO
	}


	//////////////////////////////////////////////////////
	// Sound Source Control and Creation functionality.
	//////////////////////////////////////////////////////

	bool SoundSystem::CreateSound(SoundSource::Data* data)
	{
		// Create the Sound.
		auto sound = new SoundSource();
		if(sound->Initialize(this, data))
		{
			// Store the Sound.
			_addSoundSource(data->m_SoundName, sound);

			return true;
		}

		sound->Terminate();
		return false;
	}

	void SoundSystem::DeleteSound(const std::string& sound_source_name)
	{
		if (auto sound = _getSound(sound_source_name); sound)
		{
			sound->Terminate();

			_removeSoundSource(sound_source_name);
		}
	}

	bool SoundSystem::PlaySound(const std::string& sound_source_name, const std::string& sound_channel_group, bool loop_sound)
	{
		if(auto sound = _getSound(sound_source_name); sound)
		{
			if(auto group = _getChannelGroup(sound_channel_group); group)
			{
				return sound->Play(this, group, loop_sound);
			}
		}

		return false;
	}


	void SoundSystem::StopSound(const std::string& sound_source_name)
	{
		if (auto sound = _getSound(sound_source_name); sound)
		{
			sound->Stop();
		}
	}

	void SoundSystem::PauseSound(const std::string& sound_source_name)
	{
		if (auto sound = _getSound(sound_source_name); sound)
		{
			sound->Pause();
		}
	}


	//////////////////////////////////////////////////////
	// Internal Utility functionality.
	//////////////////////////////////////////////////////

	SoundSource* SoundSystem::_getSound(const std::string& sound_source_name)
	{
		auto h = DJBHash(sound_source_name.c_str(), sound_source_name.size());

		if (m_SoundSourceMap.find(h) != m_SoundSourceMap.end())
		{
			return m_SoundSourceMap[h];
		}
		return nullptr;
	}

	FMOD::ChannelGroup* SoundSystem::_getChannelGroup(const std::string& sound_channel_group)
	{
		auto h = DJBHash(sound_channel_group.c_str(), sound_channel_group.size());

		if (m_ChannelGroupMap.find(h) != m_ChannelGroupMap.end())
		{
			return m_ChannelGroupMap[h];
		}
		return nullptr;
	}


	void SoundSystem::_removeSoundSource(const std::string& sound_source_name)
	{
		auto h = DJBHash(sound_source_name.c_str(), sound_source_name.size());

		if (m_SoundSourceMap.find(h) != m_SoundSourceMap.end())
		{
			m_SoundSourceMap.erase(m_SoundSourceMap.find(h));
		}
	}


	void SoundSystem::_removeChannelGroup(const std::string& sound_channel_group)
	{
		auto h = DJBHash(sound_channel_group.c_str(), sound_channel_group.size());

		if (m_ChannelGroupMap.find(h) != m_ChannelGroupMap.end())
		{
			m_ChannelGroupMap.erase(m_ChannelGroupMap.find(h));
		}
	}


	void SoundSystem::_addChannelGroup(const std::string& sound_channel_group, FMOD::ChannelGroup* group)
	{
		auto h = DJBHash(sound_channel_group.c_str(), sound_channel_group.size());

		m_ChannelGroupMap.try_emplace(h, group);
	}


	void SoundSystem::_addSoundSource(const std::string& sound_source_name, SoundSource* sound_source)
	{
		auto h = DJBHash(sound_source_name.c_str(), sound_source_name.size());

		m_SoundSourceMap.try_emplace(h, sound_source);
	}
}
