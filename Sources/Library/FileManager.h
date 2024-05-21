#pragma once

#include <memory>

// --- ファイルの基底クラス ---
class File {
public:
	char* name_;

public:
	virtual void load(std::ifstream&) = 0;
	virtual void save(std::ofstream&) = 0;
};


//	派生クラス
class KeyAssignFile final : public File {
public:
	KeyAssignFile() { name_ = "./Data/Save/InputAssign.txt"; }
	void load(std::ifstream& file) override;
	void save(std::ofstream& file) override;
};


// --- ファイル管理クラス ---
class FileManager {
public:
	static void save(std::unique_ptr<File> fileData);
	static void load(std::unique_ptr<File> fileData);
};