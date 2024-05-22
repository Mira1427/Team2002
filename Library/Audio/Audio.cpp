#include "Audio.h"

#include <Windows.h>
#include <winerror.h>
#include <assert.h>

#include "../Library/Misc.h"

// ===== �`�����N�f�[�^�֘A ==============================================================================================================

// --- �`�����N��T�� ---
HRESULT FindChunk(HANDLE hFile, DWORD fourCC, DWORD& chunkSize, DWORD& chunkDataPosition)
{
	// --- �t�@�C���|�C���^���t�@�C���̐擪�Ɉړ� ---
	if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
	{
		// --- �G���[�����������ꍇ�̓G���[�R�[�h��Ԃ� ---
		return HRESULT_FROM_WIN32(GetLastError());
	}

	HRESULT hr{ S_OK };

	// --- �t�@�C���|�C���^�̈ʒu�����i�[����ϐ� ---
	DWORD chunkType;		// �`�����N�̎��
	DWORD chunkDataSize;	// �`�����N�̃f�[�^�T�C�Y
	DWORD riffDataSize = 0;	// RIFF�f�[�^�̃T�C�Y
	DWORD bytesRead = 0;	// �ǂݍ��܂ꂽ�o�C�g��
	DWORD offset = 0;		// �`�����N�̈ʒu

	// --- �t�@�C����ǂݍ��݂Ȃ��珈���𑱂��郋�[�v ---
	while (hr == S_OK)
	{
		DWORD numberOfBytesRead;	// �ǂݍ��񂾃o�C�g��

		// --- �`�����N�̎�ނ�ǂݍ��� ---
		if (0 == ReadFile(hFile, &chunkType, sizeof(DWORD), &numberOfBytesRead, NULL))
		{
			// --- �ǂݎ��G���[�����������ꍇ�̓G���[�R�[�h���Z�b�g ---
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		// --- �`�����N�̃f�[�^�T�C�Y��ǂݎ�� ---
		if (0 == ReadFile(hFile, &chunkDataSize, sizeof(DWORD), &numberOfBytesRead, NULL))
		{
			// --- �ǂݎ��G���[�����������ꍇ�̓G���[�R�[�h���Z�b�g ---
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		// --- �`�����N�̎�ނɉ����ď����𕪊� ---
		switch (chunkType) {
		case 'FFIR': // RIFF�`��
			riffDataSize = chunkDataSize;	// RIFF�f�[�^�̃T�C�Y���擾
			chunkDataSize = 4;				// �`�����N�̃f�[�^�T�C�Y���C��

			// --- �t�@�C���̎�ނ�ǂݎ�� ---
			if (0 == ReadFile(hFile, &chunkType, sizeof(DWORD), &numberOfBytesRead, NULL))
			{
				// --- �ǂݎ��G���[�����������ꍇ�̓G���[�R�[�h���Z�b�g ---
				hr = HRESULT_FROM_WIN32(GetLastError());
			}

			break;

		default: // ����ȊO

			// --- �`�����N�̃f�[�^�T�C�Y���t�@�C���|�C���^���ړ� ---
			if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, chunkDataSize, NULL, FILE_CURRENT))
			{
				// --- �G���[�����������ꍇ�̓G���[�R�[�h��Ԃ� ---
				return HRESULT_FROM_WIN32(GetLastError());
			}

			break;
		}

		offset += sizeof(DWORD) * 2;	// �`�����N�̈ʒu�����X�V

		// --- �ړI��FourCC�ƈ�v����`�����N���������ꍇ ---
		if (chunkType == fourCC)
		{
			// --- �`�����N�̃T�C�Y�ƈʒu�����Z�b�g ---
			chunkSize = chunkDataSize;
			chunkDataPosition = offset;
			return S_OK;
		}

		offset += chunkDataSize;	// ���̃`�����N�̈ʒu�����X�V

		// --- ���t�f�[�^�T�C�Y�ȏ�̃o�C�g����ǂݍ��񂾏ꍇ�͏I�� ---
		if (bytesRead >= riffDataSize)
		{
			return S_FALSE;
		}
	}

	return S_OK;
}


// --- �`�����N��ǂݎ�� ---
HRESULT ReadChunkData(HANDLE hFile, LPVOID buffer, DWORD bufferSize, DWORD bufferOffset)
{
	HRESULT hr{ S_OK };

	// --- �t�@�C���|�C���^���w�肳�ꂽ�I�t�Z�b�g�ʒu�Ɉړ� ---
	if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, bufferOffset, NULL, FILE_BEGIN))
	{
		// --- �G���[�����������ꍇ�̓G���[�R�[�h��Ԃ� ---
		return HRESULT_FROM_WIN32(GetLastError());
	}

	DWORD numberOfBytesRead;	// �ǂݎ�����o�C�g��

	// --- �t�@�C������f�[�^��ǂݎ�� ---
	if (0 == ReadFile(hFile, buffer, bufferSize, &numberOfBytesRead, NULL))
	{
		// --- �ǂݎ��G���[�����������ꍇ�̓G���[�R�[�h���Z�b�g ---
		hr = HRESULT_FROM_WIN32(GetLastError());
	}

	return hr;
}


// ===== �I�[�f�B�I�֘A ===================================================================================================================

Audio::Audio(IXAudio2* xAudio2, const char* fileName, float volume, float masterVolumeRate)
{
	Load(xAudio2, fileName);				// �ǂݍ���

	SetVolume(volume, masterVolumeRate);	// ���ʂ̐ݒ�
}


// --- �f�X�g���N�^ ---
Audio::~Audio()
{
	sourceVoice_->DestroyVoice();	// �\�[�X�{�C�X�����
	delete[] buffer_.pAudioData;	// �I�[�f�B�I�f�[�^�̃o�b�t�@�����
}


// --- �ŏI�I�ȉ��ʂ̎擾 ---
float Audio::GetFinalVolume()
{
	float currentVolume;
	sourceVoice_->GetVolume(&currentVolume);
	return currentVolume;
}


// --- ���ʂ̐ݒ� ---
void Audio::SetVolume(float volume, float masterVolumeRate)
{
	volume_ = volume;	// ���ʂ�ݒ�

	float finalVolume = volume * masterVolumeRate;

	HRESULT hr{ S_OK };
	hr = sourceVoice_->SetVolume(finalVolume);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}


// --- �ǂݍ��� ---
void Audio::Load(IXAudio2* xAudio2, const char* fileName)
{
	HRESULT  hr;

	// --- �t�@�C�����J�� ---
	HANDLE hFile = CreateFileA(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	}

	// --- �t�@�C���|�C���^���t�@�C���̐擪�Ɉړ� ---
	if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	}

	DWORD chunkSize;		// �`�����N�̃T�C�Y
	DWORD chunkPosition;	// �`�����N�̈ʒu

	// --- �t�@�C���̎�ނ̊m�F ( WAVE��XWMA�̂� ) ---
	FindChunk(hFile, 1163280727/*RIFF*/, chunkSize, chunkPosition);
	DWORD fileType;
	ReadChunkData(hFile, &fileType, sizeof(DWORD), chunkPosition);
	_ASSERT_EXPR(fileType == 'EVAW'/*WAVE*/, L"wave�����Ή����Ă��܂���");

	// --- �t�H�[�}�b�g�`�����N��ǂݍ��� ---
	FindChunk(hFile, ' tmf'/*FMT*/, chunkSize, chunkPosition);
	ReadChunkData(hFile, &wfx, chunkSize, chunkPosition);

	// --- �f�[�^�`�����N����I�[�f�B�I�o�b�t�@�f�[�^���쐬 ---
	FindChunk(hFile, 'atad', chunkSize, chunkPosition);
	BYTE* data = new BYTE[chunkSize];
	ReadChunkData(hFile, data, chunkSize, chunkPosition);

	buffer_.AudioBytes = chunkSize;			// �I�[�f�B�I�o�b�t�@�̃T�C�Y
	buffer_.pAudioData = data;				// �I�[�f�B�I�f�[�^���܂ރo�b�t�@
	buffer_.Flags = XAUDIO2_END_OF_STREAM;	// ���̃o�b�t�@�̌�Ƀf�[�^���Ȃ����Ƃ�`����

	// --- �\�[�X�{�C�X�̍쐬 ---
	hr = xAudio2->CreateSourceVoice(&sourceVoice_, (WAVEFORMATEX*)&wfx);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}


// --- �Đ� ---
void Audio::Play(int loopCount)
{
	HRESULT hr;

	// --- �\�[�X�{�C�X�̏�Ԃ��擾 ---
	XAUDIO2_VOICE_STATE voiceState = {};
	sourceVoice_->GetState(&voiceState);

	// --- �\�[�X�{�C�X�Ƀo�b�t�@���L���[����Ă���ꍇ�͍Đ����Ȃ� ---
	if (voiceState.BuffersQueued)
		return;

	buffer_.LoopCount = loopCount;	// �o�b�t�@�̃��[�v��

	// --- �\�[�X�{�C�X�Ƀo�b�t�@��񋟂��� ---
	hr = sourceVoice_->SubmitSourceBuffer(&buffer_);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	// --- �\�[�X�{�C�X�̍Đ����J�n���� ---
	hr = sourceVoice_->Start(0);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}


// --- ��~ ---
void Audio::Stop(bool playTails, bool waitForBufferToUnqueue)
{
	// --- �\�[�X�{�C�X�̏�Ԃ��擾 ---
	XAUDIO2_VOICE_STATE voiceState = {};
	sourceVoice_->GetState(&voiceState);

	// --- �\�[�X�{�C�X�Ƀo�b�t�@���L���[����Ă��Ȃ��ꍇ�͍Đ����Ȃ� ---
	if (!voiceState.BuffersQueued)
		return;

	HRESULT hr;

	hr = sourceVoice_->Stop(playTails ? XAUDIO2_PLAY_TAILS : 0);	// �\�[�X�{�C�X���~
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	hr = sourceVoice_->FlushSourceBuffers();	// �\�[�X�{�C�X�̃o�b�t�@���t���b�V��
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	// --- �o�b�t�@���L���[����Ă���ԁA�ҋ@����I�v�V�������w�肳��Ă���ꍇ
	while (waitForBufferToUnqueue && voiceState.BuffersQueued)
	{
		sourceVoice_->GetState(&voiceState);
	}

}


// --- �Đ������ǂ��� ---
bool Audio::IsQueued()
{
	// --- �\�[�X�{�C�X�̏�Ԃ��擾 ---
	XAUDIO2_VOICE_STATE voiceState = {};
	sourceVoice_->GetState(&voiceState);
	return voiceState.BuffersQueued;
}


// ===== �I�[�f�B�I�Ǘ��N���X =============================================================================================================================

// --- �R���X�g���N�^ ---
AudioManager::AudioManager()
{
	HRESULT hr{ S_OK };

	// --- XAudio2�̍쐬 ---
	hr = XAudio2Create(xAudio2_.GetAddressOf(), 0, XAUDIO2_DEFAULT_PROCESSOR);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	// --- �}�X�^�[�{�C�X�̍쐬 ---
	hr = xAudio2_->CreateMasteringVoice(&masterVoice_);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));


	soundTrack_.clear();
	soundEffects_.clear();
}


// --- �f�X�g���N�^ ---
AudioManager::~AudioManager()
{
	soundTrack_.clear();
	soundEffects_.clear();
}


// ===== ���y�֘A =======================================================================================================================================

// --- ���ׂẲ��ʂ̍X�V ---
void AudioManager::UpdateAllVolume()
{
	// --- ���y�̉��ʂ�ݒ肵�Ȃ��� ---
	for (auto& music : soundTrack_)
		music->SetVolume(music->GetVolume(), masterVolume_ * musicMasterVolume_);

	// --- ���ʉ��̉��ʂ��ݒ肵�Ȃ��� ---
	for (auto& sound : soundEffects_)
		sound->SetVolume(sound->GetVolume(), masterVolume_ * soundMasterVolume_);
}

// --- �ǂݍ��� ---
void AudioManager::LoadMusic(const char* fileName, float volume)
{
	soundTrack_.emplace_back(std::make_unique<Audio>(xAudio2_.Get(), fileName, volume, masterVolume_ * musicMasterVolume_));
}

// --- �Đ� ---
void AudioManager::PlayMusic(int index, bool isLoop)
{
	if (index >= soundTrack_.size())
		assert(!"���y�̃C���f�b�N�X���͈͊O�ł�");

	soundTrack_.at(index)->Play(isLoop ? 255 : 0);
}

// --- ��~ ---
void AudioManager::StopMusic(int index)
{
	if (index >= soundTrack_.size())
		assert(!"���y�̃C���f�b�N�X���͈͊O�ł�");

	soundTrack_.at(index)->Stop();
}

// --- �ꊇ��~ ---
void AudioManager::StopMusic()
{
	for (auto& music : soundTrack_)
	{
		music->Stop();
	}
}


// ===== ���ʉ��֘A =======================================================================================================================================

// --- �ǂݍ��� ---
void AudioManager::LoadSound(const char* fileName, float volume)
{
	soundEffects_.emplace_back(std::make_unique<Sound>(xAudio2_.Get(), fileName, volume, masterVolume_ * soundMasterVolume_));
}

// --- �Đ� ---
void AudioManager::playSound(int index)
{
	if (index >= soundEffects_.size())
		assert(!"���ʉ��̃C���f�b�N�X���͈͊O�ł�");

	soundEffects_.at(index)->Play();
}

// --- ��~ ---
void AudioManager::StopSound(int index)
{
	if (index >= soundEffects_.size())
		assert(!"���ʉ��̃C���f�b�N�X���͈͊O�ł�");

	soundEffects_.at(index)->Stop();
}

// --- �ꊇ��~ ---
void AudioManager::StopSound()
{
	for (auto& sound : soundEffects_)
	{
		sound->Stop();
	}
}


// ===== ���ʉ��N���X ==================================================================================================================================

// --- �R���X�g���N�^ ---
AudioManager::Sound::Sound(IXAudio2* xAudio2, const char* fileName, float volume, float masterVolumeRate)
{
	for (auto& sound : sounds_)
	{
		sound = std::make_unique<Audio>(xAudio2, fileName, volume, masterVolumeRate);
	}


}

// --- ���ʂ̎擾 ---
float AudioManager::Sound::GetVolume()
{
	return sounds_->get()->GetVolume();
}

// --- �ŏI�I�ȉ��ʂ̎擾 ---
float AudioManager::Sound::GetFinalVolume()
{
	return sounds_->get()->GetFinalVolume();
}

// --- ���ʂ̐ݒ� ---
void AudioManager::Sound::SetVolume(float volume, float masterVolumeRate)
{
	for (auto& sound : sounds_)
	{
		sound->SetVolume(volume, masterVolumeRate);
	}
}

// --- �Đ� ---
bool AudioManager::Sound::Play()
{
	for (auto& sound : sounds_) {

		if (!sound->IsQueued())	// �Đ�����Ă��Ȃ�������
		{
			sound->Play();		// �Đ�
			return true;
		}
	}

	return false;
}

// --- ��~ ---
void AudioManager::Sound::Stop()
{
	for (auto& sound : sounds_)
	{
		sound->Stop();
	}
}