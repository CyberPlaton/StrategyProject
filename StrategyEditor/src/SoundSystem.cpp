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
		if(m_FMODChannel)
		{
			m_FMODChannel->stop();
			m_FMODChannel = nullptr;
		}
	}

	void SoundSource::Pause()
	{
		m_FMODChannel->setPaused(true);
	}


	void SoundSource::Volume(float v)
	{
		if(m_FMODChannel)
		{
			m_FMODChannel->setVolume(v);
		}
	}

	float SoundSource::Volume()
	{
		if (m_FMODChannel)
		{
			float v;
			m_FMODChannel->getVolume(&v);
			return v;
		}

		return -INT_MAX;
	}

	bool SoundSource::Playing()
	{
		if(m_FMODChannel)
		{
			bool v;
			m_FMODChannel->isPlaying(&v);
			return v;
		}

		return false;
	}

	//////////////////////////////////////////////////////
	// Sound System base functionality.
	//////////////////////////////////////////////////////

	bool SoundSystem::Initialize(float game_world_scale, float minimal_sound_source_volume)
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

				m_GameWorldScale = game_world_scale;
				m_UnhearableBarrier = minimal_sound_source_volume;

				return true;
			}
		}

		return false;
	}

	void SoundSystem::Terminate()
	{
		// No need to release or delete Channel Groups.
		m_ChannelGroupMap.clear();

		// Release Sound Sources.
		for(auto& pair: m_SoundSourceMap)
		{
			pair.second->Terminate();
		}

		// Close and Release FMOD from Duty.
		m_FMODSystem->close();
		m_FMODSystem->release();
	}

	FMOD::System* SoundSystem::System()
	{
		return m_FMODSystem;
	}

	void SoundSystem::Update(float camerax, float cameray, float cameraz)
	{
		// Simulate the listener and audio movement by hand.
		for(auto& pair: m_SoundSourceMap)
		{
			auto sound = pair.second;
			auto data = sound->m_SoundData;
			LOG_DBG_INFO("[{:.4f}][SoundSystem::Update] Processing: \"{}\"!", APP_RUN_TIME, data.m_SoundName);


			// Compute Distance.
			float dist = _computeDistance(camerax, cameray, cameraz, data.m_X, data.m_Y, data.m_Z);
			LOG_DBG_WARN("[{:.4f}][SoundSystem::Update] Distance: \"{}\"; Radius: \"{}\"!", APP_RUN_TIME, dist, data.m_Radius);


			if(dist <= data.m_Radius)
			{
				// Compute volume.
				float vol = 1.0f - _computeVolume(data.m_VolumeFalloffFactor, dist);
				LOG_DBG_WARN("[{:.4f}][SoundSystem::Update] Volume: \"{}\"!", APP_RUN_TIME, vol);


				if(sound->Playing())
				{
					// Adjust volume.
					if (vol > m_UnhearableBarrier)
					{
						sound->Volume(vol);
					}
				}
				else
				{
					// Adjust volume and start if barely hearable.
					if (vol >= m_UnhearableBarrier)
					{
						LOG_DBG_CRITICAL("[{:.4f}][SoundSystem::Update] Starting Sound Source: \"{}\"!", APP_RUN_TIME, data.m_SoundName);
						sound->Play(this, _getChannelGroup(data.m_ChannelGroup), data.m_Loop);
					}
					// Start the new Sound Source with the minimal available volume.
					sound->Volume(0.0f);
				}
			}
			else if(dist <= data.m_Radius + data.m_FadeoutRadius) // Barely outside of Radius.
			{
				// Simulate Fadeout.
				float fadeout_volume = sound->Volume();
				sound->Volume(fadeout_volume - 0.001f);


				if (sound->Volume() == 0.0f)
				{
					LOG_DBG_CRITICAL("[{:.4f}][SoundSystem::Update] Stopping Sound Source: \"{}\"!", APP_RUN_TIME, data.m_SoundName);
					sound->Stop();
				}
			}
			else // Hard outside of Radius.
			{
				sound->Stop();
			}
		}
		
		// FMOD update.
		m_FMODSystem->update();
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

			LOG_DBG_INFO("[{:.4f}][SoundSystem::CreateSound] Created Sound Source: Name:\"{}\", Path:\"{}\"!", APP_RUN_TIME, data->m_SoundName, data->m_Soundfilepath);
			LOG_FILE_INFO("[{:.4f}][SoundSystem::CreateSound] Created Sound Source: Name:\"{}\";, Path:\"{}\"!", APP_RUN_TIME, data->m_SoundName, data->m_Soundfilepath);

			return true;
		}

		LOG_DBG_ERROR("[{:.4f}][SoundSystem::CreateSound] Failed creating Sound Source: Name:\"{}\", Path:\"{}\"!", APP_RUN_TIME, data->m_SoundName, data->m_Soundfilepath);
		LOG_FILE_ERROR("[{:.4f}][SoundSystem::CreateSound] Failed creating Sound Source: Name:\"{}\";, Path:\"{}\"!", APP_RUN_TIME, data->m_SoundName, data->m_Soundfilepath);
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
				LOG_DBG_INFO("[{:.4f}][SoundSystem::PlaySound] Playing Sound Source \"{}\" on Channel Group \"{}\"!", APP_RUN_TIME, sound_source_name, sound_channel_group);
				LOG_FILE_INFO("[{:.4f}][SoundSystem::PlaySound] Playing Sound Source \"{}\" on Channel Group \"{}\"!", APP_RUN_TIME, sound_source_name, sound_channel_group);

				return sound->Play(this, group, loop_sound);
			}

			LOG_DBG_ERROR("[{:.4f}][SoundSystem::PlaySound] Failed playing Sound Source \"{}\": Channel Group \"{}\" not found!", APP_RUN_TIME, sound_source_name, sound_channel_group);
			LOG_FILE_ERROR("[{:.4f}][SoundSystem::PlaySound] Failed playing Sound Source \"{}\": Channel Group \"{}\" not found!", APP_RUN_TIME, sound_source_name, sound_channel_group);
		}

		LOG_DBG_ERROR("[{:.4f}][SoundSystem::PlaySound] Failed playing Sound Source \"{}\": Sound Source \"{}\" not found!", APP_RUN_TIME, sound_source_name, sound_source_name);
		LOG_FILE_ERROR("[{:.4f}][SoundSystem::PlaySound] Failed playing Sound Source \"{}\": Sound Source \"{}\" not found!", APP_RUN_TIME, sound_source_name, sound_source_name);

		return false;
	}


	void SoundSystem::StopSound(const std::string& sound_source_name)
	{
		if (auto sound = _getSound(sound_source_name); sound)
		{
			sound->Stop();

			LOG_DBG_INFO("[{:.4f}][SoundSystem::StopSound] Stop Sound Source \"{}\"!", APP_RUN_TIME, sound_source_name);
			LOG_FILE_INFO("[{:.4f}][SoundSystem::StopSound] Stop Sound Source \"{}\"!", APP_RUN_TIME, sound_source_name);
		}
	}

	void SoundSystem::PauseSound(const std::string& sound_source_name)
	{
		if (auto sound = _getSound(sound_source_name); sound)
		{
			sound->Pause();

			LOG_DBG_INFO("[{:.4f}][SoundSystem::PauseSound] Pause Sound Source \"{}\"!", APP_RUN_TIME, sound_source_name);
			LOG_FILE_INFO("[{:.4f}][SoundSystem::PauseSound] Pause Sound Source \"{}\"!", APP_RUN_TIME, sound_source_name);
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


	float SoundSystem::_computeDistance(float a, float b, float c, float x, float y, float z)
	{
		// We compute 3D euclidian distance.
		float q = (a - x) * (a - x);
		float r = (b - y) * (b - y);
		float s = (c - z) * (c - z);

		float d = std::sqrt(q + r + s);

		return d;
	}

	float SoundSystem::_computeVolume(float sound_source_falloff_factor, float distance)
	{
		// The Base.
		float b = (1 / sound_source_falloff_factor);

		// The power to which to raise.
		// This can be adjusted in order to match the scale of the Game World.
		float x = distance * m_GameWorldScale;


		// Return the Inverse. But clamp it between 0 and 1.
		float y = 1.0f - pow(b, x);

		float r = y;
		if (y > 1.0f) r = 1.0f;
		if (y < 0.0f) r = 0.0f;

		LOG_DBG_WARN("[{:.4f}][SoundSystem::_computeVolume] Formula: 1 - power( ({}), {} ) = {}.", APP_RUN_TIME, b, x, y);
		LOG_DBG_WARN("[{:.4f}][SoundSystem::_computeVolume] Clamped: {}.", APP_RUN_TIME, r);

		return r;
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
