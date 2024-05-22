#pragma once

#include <xaudio2.h>
#include <vector>
#include <memory>
#include <wrl.h>

// ===== オーディオクラス ==================================================================================================================================
class Audio
{
public:
	Audio(IXAudio2* xAudio2, const char* fileName, float volume, float masterVolumeRate);	// コンストラクタ
	~Audio();	// デストラクタ

	float GetFinalVolume();					// 最終的な音量の取得
	float GetVolume() const { return volume_; }	// 音量の取得

	void SetVolume(float volume, float masterVolumeRate);	// 音量の設定

	void Load(IXAudio2* xAudio2, const char* fileName);						// 読み込み
	void Play(int loopCount = 0);											// 再生
	void Stop(bool playTails = true, bool waitForBufferToUnqueue = true);	// 停止
	bool IsQueued();														// 一時停止

private:
	WAVEFORMATEXTENSIBLE	wfx{ 0 };
	XAUDIO2_BUFFER			buffer_{ 0 };
	IXAudio2SourceVoice*	sourceVoice_;

	float volume_;	// 音量
};


// ===== オーディオ管理クラス =============================================================================================================================
class AudioManager
{
private:
	AudioManager();		// コンストラクタ
	~AudioManager();		// デストラクタ

	AudioManager(const AudioManager&) = delete;
	AudioManager& operator=(const AudioManager&) = delete;
	AudioManager(AudioManager&&) noexcept = delete;
	AudioManager& operator= (AudioManager&&) noexcept = delete;

public:
	// --- インスタンスを取得 ---
	static AudioManager& instance()
	{
		static AudioManager instance;
		return instance;
	}

	// --- 音量関連 ---
	float GetMasterVolume() { return masterVolume_; }							// オーディオ全体の音量の取得
	float GetMusicMasterVolume() { return musicMasterVolume_; }					// 音楽全体の音量の取得
	float GetSoundMasterVolume() { return soundMasterVolume_; }					// 効果音全体の音量の取得
	void  SetMasterVolume(float volume) { masterVolume_ = volume; }				// オーディオ全体の音量の設定
	void  SetMusicMasterVolume(float volume) { musicMasterVolume_ = volume; }	// 音楽全体の音量の設定
	void  SetSoundMasterVolume(float volume) { soundMasterVolume_ = volume; }	// 効果音全体の音量の設定
	void  UpdateAllVolume();

	// --- 音楽関連 ---
	float GetMusicVolume(int index) { return soundTrack_.at(index)->GetVolume(); }				// 音量の取得
	float GetMusicFinalVolume(int index) { return soundTrack_.at(index)->GetFinalVolume(); }	// 最終的な音量の取得									
	void  SetMusicVolume(int index, float volume) { soundTrack_.at(index)->SetVolume(volume, masterVolume_ * musicMasterVolume_); }	// 音量の設定
	void  SetAllMusicVolume(float volume)
	{
		for (auto& music : soundTrack_)
			music->SetVolume(volume, masterVolume_ * musicMasterVolume_);
	}

	void  LoadMusic(const char* fileName, float volume = 1.0f);	// 読み込み
	void  PlayMusic(int index, bool isLoop = true);				// 再生
	void  StopMusic(int index);									// 停止
	void  StopMusic();											// 一括停止

	// --- 効果音関連 ---
	float GetSoundVolume(int index) { return soundEffects_.at(index)->GetVolume(); }			// 音量の取得
	float GetSoundFinalVolume(int index) { return soundEffects_.at(index)->GetFinalVolume(); }	// 最終的な音量の取得									
	void  SetSoundVolume(int index, float volume) { soundEffects_.at(index)->SetVolume(volume, masterVolume_ * soundMasterVolume_); }	// 音量の設定
	void SetAllSoundVolume(float volume)
	{
		for (auto& sound : soundEffects_)
			sound->SetVolume(volume, masterVolume_ * soundMasterVolume_);
	}

	void  LoadSound(const char* fileName, float volume = 1.0f);	// 読み込み
	void  playSound(int index);									// 再生
	void  StopSound(int index);									// 停止
	void  StopSound();											// 一括停止


private:
	inline static constexpr int MAXCONCURRENT_SOUNDS = 8;	// 効果音の最大同時再生数

	// --- 効果音クラス ---
	class Sound
	{ 
	public:
		Sound(IXAudio2* xAudio2, const char* fileName, float volume, float masterVolumeRate);	// コンストラクタ
		~Sound() { for (auto& sound : sounds_) sound = nullptr; }		// デストラクタ

		float GetVolume();		// 音量の取得
		float GetFinalVolume();	// 最終的な音量の取得

		void SetVolume(float volume, float masterVolumeRate);	// 音量の設定

		bool Play();	// 再生
		void Stop();	// 停止

	private:
		std::unique_ptr<Audio> sounds_[MAXCONCURRENT_SOUNDS];
	};


	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;			// XAudio2
	IXAudio2MasteringVoice* masterVoice_ = nullptr;		// マスターボイス

	std::vector<std::unique_ptr<Audio>> soundTrack_;	// 音楽
	std::vector<std::unique_ptr<Sound>> soundEffects_;	// 効果音

	float masterVolume_ = 1.0f;			// 全体の音量
	float musicMasterVolume_ = 1.0f;	// 音楽全体の音量
	float soundMasterVolume_ = 1.0f;	// 効果音全体の音量
};