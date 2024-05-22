#pragma once

#include <memory>

// --- �t�@�C���̊��N���X ---
class File {
public:
	char* name_;

public:
	virtual void load(std::ifstream&) = 0;
	virtual void save(std::ofstream&) = 0;
};


//	�h���N���X
class KeyAssignFile final : public File {
public:
	KeyAssignFile() { name_ = "./Data/Save/InputAssign.txt"; }
	void load(std::ifstream& file) override;
	void save(std::ofstream& file) override;
};


// --- �t�@�C���Ǘ��N���X ---
class FileManager {
public:
	static void save(std::unique_ptr<File> fileData);
	static void load(std::unique_ptr<File> fileData);
};