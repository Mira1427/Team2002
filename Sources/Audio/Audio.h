#pragma once

#include <xaudio2.h>
#include <vector>
#include <memory>
#include <wrl.h>

// ===== �I�[�f�B�I�N���X ==================================================================================================================================
class Audio
{
public:
	Audio(IXAudio2* xAudio2, const char* fileName, float volume, float masterVolumeRate);	// �R���X�g���N�^
	~Audio();	// �f�X�g���N�^

	float GetFinalVolume();					// �ŏI�I�ȉ��ʂ̎擾
	float GetVolume() const { return volume_; }	// ���ʂ̎擾

	void SetVolume(float volume, float masterVolumeRate);	// ���ʂ̐ݒ�

	void Load(IXAudio2* xAudio2, const char* fileName);						// �ǂݍ���
	void Play(int loopCount = 0);											// �Đ�
	void Stop(bool playTails = true, bool waitForBufferToUnqueue = true);	// ��~
	bool IsQueued();														// �ꎞ��~

private:
	WAVEFORMATEXTENSIBLE	wfx{ 0 };
	XAUDIO2_BUFFER			buffer_{ 0 };
	IXAudio2SourceVoice*	sourceVoice_;

	float volume_;	// ����
};


// ===== �I�[�f�B�I�Ǘ��N���X =============================================================================================================================
class AudioManager
{
private:
	AudioManager();		// �R���X�g���N�^
	~AudioManager();		// �f�X�g���N�^

	AudioManager(const AudioManager&) = delete;
	AudioManager& operator=(const AudioManager&) = delete;
	AudioManager(AudioManager&&) noexcept = delete;
	AudioManager& operator= (AudioManager&&) noexcept = delete;

public:
	// --- �C���X�^���X���擾 ---
	static AudioManager& instance()
	{
		static AudioManager instance;
		return instance;
	}

	// --- ���ʊ֘A ---
	float GetMasterVolume() { return masterVolume_; }							// �I�[�f�B�I�S�̂̉��ʂ̎擾
	float GetMusicMasterVolume() { return musicMasterVolume_; }					// ���y�S�̂̉��ʂ̎擾
	float GetSoundMasterVolume() { return soundMasterVolume_; }					// ���ʉ��S�̂̉��ʂ̎擾
	void  SetMasterVolume(float volume) { masterVolume_ = volume; }				// �I�[�f�B�I�S�̂̉��ʂ̐ݒ�
	void  SetMusicMasterVolume(float volume) { musicMasterVolume_ = volume; }	// ���y�S�̂̉��ʂ̐ݒ�
	void  SetSoundMasterVolume(float volume) { soundMasterVolume_ = volume; }	// ���ʉ��S�̂̉��ʂ̐ݒ�
	void  UpdateAllVolume();

	// --- ���y�֘A ---
	float GetMusicVolume(int index) { return soundTrack_.at(index)->GetVolume(); }				// ���ʂ̎擾
	float GetMusicFinalVolume(int index) { return soundTrack_.at(index)->GetFinalVolume(); }	// �ŏI�I�ȉ��ʂ̎擾									
	void  SetMusicVolume(int index, float volume) { soundTrack_.at(index)->SetVolume(volume, masterVolume_ * musicMasterVolume_); }	// ���ʂ̐ݒ�
	void  SetAllMusicVolume(float volume)
	{
		for (auto& music : soundTrack_)
			music->SetVolume(volume, masterVolume_ * musicMasterVolume_);
	}

	void  LoadMusic(const char* fileName, float volume = 1.0f);	// �ǂݍ���
	void  PlayMusic(int index, bool isLoop = true);				// �Đ�
	void  StopMusic(int index);									// ��~
	void  StopMusic();											// �ꊇ��~

	// --- ���ʉ��֘A ---
	float GetSoundVolume(int index) { return soundEffects_.at(index)->GetVolume(); }			// ���ʂ̎擾
	float GetSoundFinalVolume(int index) { return soundEffects_.at(index)->GetFinalVolume(); }	// �ŏI�I�ȉ��ʂ̎擾									
	void  SetSoundVolume(int index, float volume) { soundEffects_.at(index)->SetVolume(volume, masterVolume_ * soundMasterVolume_); }	// ���ʂ̐ݒ�
	void SetAllSoundVolume(float volume)
	{
		for (auto& sound : soundEffects_)
			sound->SetVolume(volume, masterVolume_ * soundMasterVolume_);
	}

	void  LoadSound(const char* fileName, float volume = 1.0f);	// �ǂݍ���
	void  playSound(int index);									// �Đ�
	void  StopSound(int index);									// ��~
	void  StopSound();											// �ꊇ��~


private:
	inline static constexpr int MAXCONCURRENT_SOUNDS = 8;	// ���ʉ��̍ő哯���Đ���

	// --- ���ʉ��N���X ---
	class Sound
	{ 
	public:
		Sound(IXAudio2* xAudio2, const char* fileName, float volume, float masterVolumeRate);	// �R���X�g���N�^
		~Sound() { for (auto& sound : sounds_) sound = nullptr; }		// �f�X�g���N�^

		float GetVolume();		// ���ʂ̎擾
		float GetFinalVolume();	// �ŏI�I�ȉ��ʂ̎擾

		void SetVolume(float volume, float masterVolumeRate);	// ���ʂ̐ݒ�

		bool Play();	// �Đ�
		void Stop();	// ��~

	private:
		std::unique_ptr<Audio> sounds_[MAXCONCURRENT_SOUNDS];
	};


	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;			// XAudio2
	IXAudio2MasteringVoice* masterVoice_ = nullptr;		// �}�X�^�[�{�C�X

	std::vector<std::unique_ptr<Audio>> soundTrack_;	// ���y
	std::vector<std::unique_ptr<Sound>> soundEffects_;	// ���ʉ�

	float masterVolume_ = 1.0f;			// �S�̂̉���
	float musicMasterVolume_ = 1.0f;	// ���y�S�̂̉���
	float soundMasterVolume_ = 1.0f;	// ���ʉ��S�̂̉���
};