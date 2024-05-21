#include "FileManager.h"

#include "../Input/InputManager.h"

#include <fstream>


//	��������
void FileManager::save(std::unique_ptr<File> fileData) {

	std::ofstream file(fileData->name_);

	if (file.is_open()) {

		fileData->save(file);

		file.close();

		fileData = nullptr;
	}

	else {

		_ASSERT(!L"output�Ńt�@�C�����J���܂���ł����B");
	}
}


//	�ǂݍ���
void FileManager::load(std::unique_ptr<File> fileData) {

	std::ifstream file(fileData->name_);

	if (file.is_open()) {

		fileData->load(file);

		file.close();

		fileData = nullptr;
	}

	else
	{
		_ASSERT(!L"input�Ńt�@�C�����J���܂���ł����B");
	}
}



//========================================
//			�t�@�C������֘A
//========================================
// 
//	���蓖�Ă̓ǂݍ���
void KeyAssignFile::load(std::ifstream& file) {

	InputAssign assign = {};

	//	�L�[�{�[�h
	while(1) {

		//	�f�[�^�����[�h
		file >> assign.key_ >> assign.bit_ >> assign.code_;

		//	�����l�Ȃ�ǉ������I��
		if (assign.bit_ == 0) break;

		//	���X�g�ɒǉ�
		InputManager::instance().k_.assign_.emplace_back(assign);

		//	�I���t���O���ǂ����̊m�F
		if (assign.bit_ < 0) break;
	};


	//	�Q�[���p�b�h
	while(1) {

		//	�f�[�^�����[�h
		file >> assign.key_ >> assign.bit_ >> assign.code_;

		//	�����l�Ȃ�ǉ������I��
		if (assign.bit_ == 0) break;

		//	���X�g�ɒǉ�
		InputManager::instance().p_[0].padAssign_.emplace_back(assign);

		//	�I���t���O���ǂ����̊m�F
		if (assign.bit_ < 0) break;
	};


	//	�}�E�X
	while(1) {

		//	�f�[�^�����[�h
		file >> assign.key_ >> assign.bit_ >> assign.code_;

		//	�����l�Ȃ�ǉ������I��
		if (assign.bit_ == 0) break;

		//	���X�g�ɒǉ�
		InputManager::instance().m_.assign_.emplace_back(assign);

		//	�I���t���O���ǂ����̊m�F
		if (assign.bit_ < 0) break;
	};
}


//	���蓖�Ă̕ۑ�
void KeyAssignFile::save(std::ofstream& file) {

	//	�L�[���蓖�Ă�ۑ�
	for (auto& it : InputManager::instance().k_.assign_) {

		file << it.key_ << " " << it.bit_ << " " << it.code_ << '\n';
	}

	//	�Q�[���p�b�h�̊��蓖�Ă�ۑ�
	for (auto& it : InputManager::instance().p_[0].padAssign_) {

		file << it.key_ << " " << it.bit_ << " " << it.code_ << '\n';
	}

	//	�}�E�X�̊��蓖�Ă�ۑ�
	for (auto& it : InputManager::instance().m_.assign_) {

		file << it.key_ << " " << it.bit_ << " " << it.code_ << '\n';
	}
}