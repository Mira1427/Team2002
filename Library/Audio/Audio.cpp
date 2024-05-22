#include "Audio.h"

#include <Windows.h>
#include <winerror.h>
#include <assert.h>

#include "../Library/Misc.h"

// ===== チャンクデータ関連 ==============================================================================================================

// --- チャンクを探す ---
HRESULT FindChunk(HANDLE hFile, DWORD fourCC, DWORD& chunkSize, DWORD& chunkDataPosition)
{
	// --- ファイルポインタをファイルの先頭に移動 ---
	if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
	{
		// --- エラーが発生した場合はエラーコードを返す ---
		return HRESULT_FROM_WIN32(GetLastError());
	}

	HRESULT hr{ S_OK };

	// --- ファイルポインタの位置情報を格納する変数 ---
	DWORD chunkType;		// チャンクの種類
	DWORD chunkDataSize;	// チャンクのデータサイズ
	DWORD riffDataSize = 0;	// RIFFデータのサイズ
	DWORD bytesRead = 0;	// 読み込まれたバイト数
	DWORD offset = 0;		// チャンクの位置

	// --- ファイルを読み込みながら処理を続けるループ ---
	while (hr == S_OK)
	{
		DWORD numberOfBytesRead;	// 読み込んだバイト数

		// --- チャンクの種類を読み込む ---
		if (0 == ReadFile(hFile, &chunkType, sizeof(DWORD), &numberOfBytesRead, NULL))
		{
			// --- 読み取りエラーが発生した場合はエラーコードをセット ---
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		// --- チャンクのデータサイズを読み取る ---
		if (0 == ReadFile(hFile, &chunkDataSize, sizeof(DWORD), &numberOfBytesRead, NULL))
		{
			// --- 読み取りエラーが発生した場合はエラーコードをセット ---
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		// --- チャンクの種類に応じて処理を分岐 ---
		switch (chunkType) {
		case 'FFIR': // RIFF形式
			riffDataSize = chunkDataSize;	// RIFFデータのサイズを取得
			chunkDataSize = 4;				// チャンクのデータサイズを修正

			// --- ファイルの種類を読み取る ---
			if (0 == ReadFile(hFile, &chunkType, sizeof(DWORD), &numberOfBytesRead, NULL))
			{
				// --- 読み取りエラーが発生した場合はエラーコードをセット ---
				hr = HRESULT_FROM_WIN32(GetLastError());
			}

			break;

		default: // それ以外

			// --- チャンクのデータサイズ分ファイルポインタを移動 ---
			if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, chunkDataSize, NULL, FILE_CURRENT))
			{
				// --- エラーが発生した場合はエラーコードを返す ---
				return HRESULT_FROM_WIN32(GetLastError());
			}

			break;
		}

		offset += sizeof(DWORD) * 2;	// チャンクの位置情報を更新

		// --- 目的のFourCCと一致するチャンクを見つけた場合 ---
		if (chunkType == fourCC)
		{
			// --- チャンクのサイズと位置情報をセット ---
			chunkSize = chunkDataSize;
			chunkDataPosition = offset;
			return S_OK;
		}

		offset += chunkDataSize;	// 次のチャンクの位置情報を更新

		// --- リフデータサイズ以上のバイト数を読み込んだ場合は終了 ---
		if (bytesRead >= riffDataSize)
		{
			return S_FALSE;
		}
	}

	return S_OK;
}


// --- チャンクを読み取る ---
HRESULT ReadChunkData(HANDLE hFile, LPVOID buffer, DWORD bufferSize, DWORD bufferOffset)
{
	HRESULT hr{ S_OK };

	// --- ファイルポインタを指定されたオフセット位置に移動 ---
	if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, bufferOffset, NULL, FILE_BEGIN))
	{
		// --- エラーが発生した場合はエラーコードを返す ---
		return HRESULT_FROM_WIN32(GetLastError());
	}

	DWORD numberOfBytesRead;	// 読み取ったバイト数

	// --- ファイルからデータを読み取る ---
	if (0 == ReadFile(hFile, buffer, bufferSize, &numberOfBytesRead, NULL))
	{
		// --- 読み取りエラーが発生した場合はエラーコードをセット ---
		hr = HRESULT_FROM_WIN32(GetLastError());
	}

	return hr;
}


// ===== オーディオ関連 ===================================================================================================================

Audio::Audio(IXAudio2* xAudio2, const char* fileName, float volume, float masterVolumeRate)
{
	Load(xAudio2, fileName);				// 読み込み

	SetVolume(volume, masterVolumeRate);	// 音量の設定
}


// --- デストラクタ ---
Audio::~Audio()
{
	sourceVoice_->DestroyVoice();	// ソースボイスを解放
	delete[] buffer_.pAudioData;	// オーディオデータのバッファを解放
}


// --- 最終的な音量の取得 ---
float Audio::GetFinalVolume()
{
	float currentVolume;
	sourceVoice_->GetVolume(&currentVolume);
	return currentVolume;
}


// --- 音量の設定 ---
void Audio::SetVolume(float volume, float masterVolumeRate)
{
	volume_ = volume;	// 音量を設定

	float finalVolume = volume * masterVolumeRate;

	HRESULT hr{ S_OK };
	hr = sourceVoice_->SetVolume(finalVolume);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}


// --- 読み込み ---
void Audio::Load(IXAudio2* xAudio2, const char* fileName)
{
	HRESULT  hr;

	// --- ファイルを開く ---
	HANDLE hFile = CreateFileA(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	}

	// --- ファイルポインタをファイルの先頭に移動 ---
	if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	}

	DWORD chunkSize;		// チャンクのサイズ
	DWORD chunkPosition;	// チャンクの位置

	// --- ファイルの種類の確認 ( WAVEかXWMAのみ ) ---
	FindChunk(hFile, 1163280727/*RIFF*/, chunkSize, chunkPosition);
	DWORD fileType;
	ReadChunkData(hFile, &fileType, sizeof(DWORD), chunkPosition);
	_ASSERT_EXPR(fileType == 'EVAW'/*WAVE*/, L"waveしか対応していません");

	// --- フォーマットチャンクを読み込む ---
	FindChunk(hFile, ' tmf'/*FMT*/, chunkSize, chunkPosition);
	ReadChunkData(hFile, &wfx, chunkSize, chunkPosition);

	// --- データチャンクからオーディオバッファデータを作成 ---
	FindChunk(hFile, 'atad', chunkSize, chunkPosition);
	BYTE* data = new BYTE[chunkSize];
	ReadChunkData(hFile, data, chunkSize, chunkPosition);

	buffer_.AudioBytes = chunkSize;			// オーディオバッファのサイズ
	buffer_.pAudioData = data;				// オーディオデータを含むバッファ
	buffer_.Flags = XAUDIO2_END_OF_STREAM;	// このバッファの後にデータがないことを伝える

	// --- ソースボイスの作成 ---
	hr = xAudio2->CreateSourceVoice(&sourceVoice_, (WAVEFORMATEX*)&wfx);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}


// --- 再生 ---
void Audio::Play(int loopCount)
{
	HRESULT hr;

	// --- ソースボイスの状態を取得 ---
	XAUDIO2_VOICE_STATE voiceState = {};
	sourceVoice_->GetState(&voiceState);

	// --- ソースボイスにバッファがキューされている場合は再生しない ---
	if (voiceState.BuffersQueued)
		return;

	buffer_.LoopCount = loopCount;	// バッファのループ回数

	// --- ソースボイスにバッファを提供する ---
	hr = sourceVoice_->SubmitSourceBuffer(&buffer_);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	// --- ソースボイスの再生を開始する ---
	hr = sourceVoice_->Start(0);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}


// --- 停止 ---
void Audio::Stop(bool playTails, bool waitForBufferToUnqueue)
{
	// --- ソースボイスの状態を取得 ---
	XAUDIO2_VOICE_STATE voiceState = {};
	sourceVoice_->GetState(&voiceState);

	// --- ソースボイスにバッファがキューされていない場合は再生しない ---
	if (!voiceState.BuffersQueued)
		return;

	HRESULT hr;

	hr = sourceVoice_->Stop(playTails ? XAUDIO2_PLAY_TAILS : 0);	// ソースボイスを停止
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	hr = sourceVoice_->FlushSourceBuffers();	// ソースボイスのバッファをフラッシュ
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	// --- バッファがキューされている間、待機するオプションが指定されている場合
	while (waitForBufferToUnqueue && voiceState.BuffersQueued)
	{
		sourceVoice_->GetState(&voiceState);
	}

}


// --- 再生中かどうか ---
bool Audio::IsQueued()
{
	// --- ソースボイスの状態を取得 ---
	XAUDIO2_VOICE_STATE voiceState = {};
	sourceVoice_->GetState(&voiceState);
	return voiceState.BuffersQueued;
}


// ===== オーディオ管理クラス =============================================================================================================================

// --- コンストラクタ ---
AudioManager::AudioManager()
{
	HRESULT hr{ S_OK };

	// --- XAudio2の作成 ---
	hr = XAudio2Create(xAudio2_.GetAddressOf(), 0, XAUDIO2_DEFAULT_PROCESSOR);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	// --- マスターボイスの作成 ---
	hr = xAudio2_->CreateMasteringVoice(&masterVoice_);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));


	soundTrack_.clear();
	soundEffects_.clear();
}


// --- デストラクタ ---
AudioManager::~AudioManager()
{
	soundTrack_.clear();
	soundEffects_.clear();
}


// ===== 音楽関連 =======================================================================================================================================

// --- すべての音量の更新 ---
void AudioManager::UpdateAllVolume()
{
	// --- 音楽の音量を設定しなおす ---
	for (auto& music : soundTrack_)
		music->SetVolume(music->GetVolume(), masterVolume_ * musicMasterVolume_);

	// --- 効果音の音量も設定しなおす ---
	for (auto& sound : soundEffects_)
		sound->SetVolume(sound->GetVolume(), masterVolume_ * soundMasterVolume_);
}

// --- 読み込み ---
void AudioManager::LoadMusic(const char* fileName, float volume)
{
	soundTrack_.emplace_back(std::make_unique<Audio>(xAudio2_.Get(), fileName, volume, masterVolume_ * musicMasterVolume_));
}

// --- 再生 ---
void AudioManager::PlayMusic(int index, bool isLoop)
{
	if (index >= soundTrack_.size())
		assert(!"音楽のインデックスが範囲外です");

	soundTrack_.at(index)->Play(isLoop ? 255 : 0);
}

// --- 停止 ---
void AudioManager::StopMusic(int index)
{
	if (index >= soundTrack_.size())
		assert(!"音楽のインデックスが範囲外です");

	soundTrack_.at(index)->Stop();
}

// --- 一括停止 ---
void AudioManager::StopMusic()
{
	for (auto& music : soundTrack_)
	{
		music->Stop();
	}
}


// ===== 効果音関連 =======================================================================================================================================

// --- 読み込み ---
void AudioManager::LoadSound(const char* fileName, float volume)
{
	soundEffects_.emplace_back(std::make_unique<Sound>(xAudio2_.Get(), fileName, volume, masterVolume_ * soundMasterVolume_));
}

// --- 再生 ---
void AudioManager::playSound(int index)
{
	if (index >= soundEffects_.size())
		assert(!"効果音のインデックスが範囲外です");

	soundEffects_.at(index)->Play();
}

// --- 停止 ---
void AudioManager::StopSound(int index)
{
	if (index >= soundEffects_.size())
		assert(!"効果音のインデックスが範囲外です");

	soundEffects_.at(index)->Stop();
}

// --- 一括停止 ---
void AudioManager::StopSound()
{
	for (auto& sound : soundEffects_)
	{
		sound->Stop();
	}
}


// ===== 効果音クラス ==================================================================================================================================

// --- コンストラクタ ---
AudioManager::Sound::Sound(IXAudio2* xAudio2, const char* fileName, float volume, float masterVolumeRate)
{
	for (auto& sound : sounds_)
	{
		sound = std::make_unique<Audio>(xAudio2, fileName, volume, masterVolumeRate);
	}


}

// --- 音量の取得 ---
float AudioManager::Sound::GetVolume()
{
	return sounds_->get()->GetVolume();
}

// --- 最終的な音量の取得 ---
float AudioManager::Sound::GetFinalVolume()
{
	return sounds_->get()->GetFinalVolume();
}

// --- 音量の設定 ---
void AudioManager::Sound::SetVolume(float volume, float masterVolumeRate)
{
	for (auto& sound : sounds_)
	{
		sound->SetVolume(volume, masterVolumeRate);
	}
}

// --- 再生 ---
bool AudioManager::Sound::Play()
{
	for (auto& sound : sounds_) {

		if (!sound->IsQueued())	// 再生されていなかったら
		{
			sound->Play();		// 再生
			return true;
		}
	}

	return false;
}

// --- 停止 ---
void AudioManager::Sound::Stop()
{
	for (auto& sound : sounds_)
	{
		sound->Stop();
	}
}