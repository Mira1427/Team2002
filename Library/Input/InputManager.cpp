#include "InputManager.h"

#include <Windows.h>

#include "../Library/FileManager.h"



//	�L�[�{�[�h�̊��蓖��
std::list<InputAssign> keyAssign = {
	{"Left",	Input::LEFT,	DirectX::Keyboard::A},
	{"Right",	Input::RIGHT,	DirectX::Keyboard::D},
	{"Up",		Input::UP,		DirectX::Keyboard::W},
	{"Down",	Input::DOWN,	DirectX::Keyboard::S},
	{"Delete",	Input::DELETE_, DirectX::Keyboard::Delete},
	{"Enter",	Input::ENTER,	DirectX::Keyboard::Enter},
	{"Escape",	Input::ESCAPE,	DirectX::Keyboard::Escape},
	{"Space",	Input::CONFIRM, DirectX::Keyboard::Enter},
	{"Escape",	Input::PAUSE,	DirectX::Keyboard::Escape},
	{"LeftAttack",	Input::LEFT_ATTACK,	DirectX::Keyboard::J},
	{"RightAttack",	Input::RIGHT_ATTACK,	DirectX::Keyboard::L},
	{"---keyAssignEnd---", -1, -1}
};


//	�Q�[���p�b�h�̊��蓖��
std::list<InputAssign> padAssign = {
	{"Up", Input::UP, Input::GamePad::LSTICK_UP},
	{"Up", Input::UP, Input::GamePad::UP},
	{"Down", Input::DOWN, Input::GamePad::LSTICK_DOWN},
	{"Down", Input::DOWN, Input::GamePad::DOWN},
	{"Left", Input::LEFT, Input::GamePad::L1},
	{"Right", Input::RIGHT, Input::GamePad::R1},
	{"LeftAttack", Input::LEFT_ATTACK, Input::GamePad::L2},
	{"RightAttack", Input::RIGHT_ATTACK, Input::GamePad::R2},
	{"Confirm", Input::CONFIRM, Input::GamePad::A},
	{"Pause", Input::PAUSE, Input::GamePad::START},
	{"Escape", Input::ESCAPE, Input::GamePad::B},
	{"---PadAssignEnd---", -1, -1}
};


//	�}�E�X�̊��蓖��
std::list<InputAssign> mouseAssign = {
	{"LMB",		Input::LMB,		VK_LBUTTON},
	{"RMB",		Input::RMB,		VK_RBUTTON},
	{"MMB",		Input::MMB,		VK_MBUTTON},
	{"---MouseAssignEnd---", -1, -1}
};


InputManager::~InputManager() {
	
	//	�I�����ɕۑ�
	saveAssignment();

	k_.assign_.clear();

	for (auto& pad : p_)
		pad.padAssign_.clear();

	m_.assign_.clear();

	inputBindings_.clear();
}


void InputManager::initialize() {

	keyboard_	= std::make_unique<DirectX::Keyboard>();
	mouse_		= std::make_unique<DirectX::Mouse>();
	gamePad_	= std::make_shared<DirectX::GamePad>();


	//	�L�[���蓖�Ă̓ǂݍ���
	loadAssignment();
}


//	���蓖�Ă̓ǂݍ���
void InputManager::loadAssignment() {

	//	�������񏉊���
	k_.assign_.clear();

	for(auto& pad : p_)
		pad.padAssign_.clear();
	
	m_.assign_.clear();

	//	�ǂݍ���
	FileManager::load(std::make_unique<KeyAssignFile>());

	//	���蓖�Ă��Ȃ���΃f�t�H���g��ݒ�
	for (size_t i = 0; i < 4; i++)
	{
		if (k_.assign_.empty() || p_[i].padAssign_.empty() || m_.assign_.empty()) {

			if(k_.assign_.empty()) 
				k_.assign_ = keyAssign;

			if (p_[i].padAssign_.empty())
				p_[i].padAssign_ = padAssign;

			if (m_.assign_.empty())
				m_.assign_ = mouseAssign;

			//	�ݒ��ۑ�
			saveAssignment();
		}
	}


	//	���蓖�Ă��o�C���h����
	bindInputAssign();
}


//	���蓖�Ă̕ۑ�
void InputManager::saveAssignment() {

	FileManager::save(std::make_unique<KeyAssignFile>());
}


//	���蓖�Ă�ꂽ �L�[(�{�^��) ���o�C���h����
void InputManager::bindInputAssign() {

	inputBindings_.clear();

	//	�L�[�{�[�h
	if (!k_.assign_.empty()/*���蓖�čς�*/) {

		for (auto& assign : k_.assign_) {

			inputBindings_[assign.key_] |= assign.bit_;
		}
	}


	//	�Q�[���p�b�h
	for (size_t i = 0; i < 4; i++)
	{
		if (!p_[i].padAssign_.empty()/*���蓖�čς�*/ && isConnected(static_cast<int>(i))/*�ڑ��ς�*/) {

			for (auto& assign : p_[i].padAssign_) {

				inputBindings_[assign.key_] |= assign.bit_;
			}
		}
	}


	//	�}�E�X
	if (!m_.assign_.empty()/*���蓖�čς�*/) {

		for (auto& assign : m_.assign_) {

			inputBindings_[assign.key_] |= assign.bit_;
		}
	}
}



void InputManager::update()
{
	//	���͂̏�Ԃ̍X�V
	for (auto& pad : bit_)
		pad.prevState_ = pad.state_;


	//	��Ԃ����Z�b�g
	for (auto& pad : bit_)
	{
		pad.state_   = 0;
		pad.trigger_ = 0;
		pad.release_ = 0;
	}


	//	�e���̓f�o�C�X�̏�Ԃ��擾
	k_.state_ = keyboard_->GetState();
	m_.state_ = mouse_->GetState();

	for (size_t i = 0; i < 4; i++)
	{
		p_[i].state_ = gamePad_->GetState(static_cast<int>(i));
	}


	//	�L�[�{�[�h
	if (!k_.assign_.empty()/*���蓖�čς�*/) {

		for (auto& assign : k_.assign_) {

			//	�I���R�[�h�̊m�F
			if (assign.bit_ < 0) break;

			//	������Ă�����r�b�g�𗧂Ă�
			if (k_.state_.IsKeyDown(static_cast<DirectX::Keyboard::Keys>(assign.code_)))
				bit_[0].state_ |= inputBindings_[assign.key_];
		}
	}


	//	�Q�[���p�b�h
	//	���X�e�B�b�N
	float LStickX = 0, LStickY = 0;

	LStickX = p_[0].state_.thumbSticks.leftX;
	LStickY = p_[0].state_.thumbSticks.leftY;

	//	�L�[�{�[�h�̓��͏���
	switch (bit_[0].state_ & (Input::LEFT | Input::RIGHT)) {
	case Input::LEFT:	LStickX = -1.0f;	break;
	case Input::RIGHT:	LStickX = 1.0f;		break;
	}

	switch (bit_[0].state_ & (Input::UP | Input::DOWN)) {
	case Input::UP:		LStickY = 1.0f;		break;
	case Input::DOWN:	LStickY = -1.0f;	break;
	}

	p_[0].stick_.leftX_ = LStickX;
	p_[0].stick_.leftY_ = LStickY;


	for (size_t i = 1; i < 4; i++)
	{
		p_[i].stick_.leftX_ = p_[i].state_.thumbSticks.leftX;
		p_[i].stick_.leftY_ = p_[i].state_.thumbSticks.leftY;
	}


	//	�E�X�e�B�b�N
	for (size_t i = 0; i < 4; i++)
	{
		p_[i].stick_.rightX_ = p_[i].state_.thumbSticks.rightX;
		p_[i].stick_.rightY_ = p_[i].state_.thumbSticks.rightY;

		//	�g���K�[
		p_[i].trigger_.left_ = p_[i].state_.triggers.left;
		p_[i].trigger_.right_ = p_[i].state_.triggers.right;
	}


	for (size_t i = 0; i < 4; i++)
	{
		if (!p_[i].padAssign_.empty()/*���蓖�čς�*/)
		{
			if (isConnected(static_cast<int>(i))/*�ڑ��ς�*/)
			{
				for (auto& assign : p_[i].padAssign_)
				{

					//	�I���R�[�h�̊m�F
					if (assign.bit_ < 0) break;

					struct InputData { bool data[10]; } inputData{};

					//	�X�e�B�b�N
					if (assign.code_ >= Input::GamePad::LSTICK_UP) {

						//	������Ă�����r�b�g�𗧂Ă�
						if (p_[i].stick_.isTilted(assign.code_))
							bit_[i].state_ |= inputBindings_[assign.key_];
					}

					//	�g���K�[
					else if (assign.code_ >= Input::GamePad::L2) {

						//	������Ă�����r�b�g�𗧂Ă�
						if (p_[i].trigger_.isTriggered(assign.code_))
							bit_[i].state_ |= inputBindings_[assign.key_];
					}

					//	�{�^��
					else if (assign.code_ >= Input::GamePad::A) {

						//	�{�^���̍\���̂̃f�[�^���R�s�[
						std::memcpy(&inputData, &p_[i].state_.buttons, sizeof(DirectX::GamePad::Buttons));

						//	������Ă�����r�b�g�𗧂Ă�
						if (inputData.data[assign.code_ - Input::GamePad::A])
							bit_[i].state_ |= inputBindings_[assign.key_];
					}

					//	�\���L�[
					else {

						//	�\���L�[�̍\���̂̃f�[�^���R�s�[
						std::memcpy(&inputData, &p_[i].state_.dpad, sizeof(DirectX::GamePad::DPad));

						//	������Ă�����r�b�g�𗧂Ă�
						if (inputData.data[assign.code_])
							bit_[i].state_ |= inputBindings_[assign.key_];
					}
				}

				gamePad_->SetVibration(static_cast<int>(i), p_[i].LMotor_, p_[i].RMotor_);
			}
		}
	}


	//	�}�E�X
	
	//	�O��̃J�[�\���̍��W��ۑ�
	m_.prevPosX_ = m_.curPosX_;
	m_.prevPosY_ = m_.curPosY_;

	//	�J�[�\���̍��W���擾
	m_.curPosX_ = m_.state_.x;
	m_.curPosY_ = m_.state_.y;

	m_.deltaX_ = m_.curPosX_ - m_.prevPosX_;
	m_.deltaY_ = m_.curPosY_ - m_.prevPosY_;

	m_.isMove_ = (std::abs(m_.curPosX_ - m_.prevPosX_) > m_.deadZone_ && std::abs(m_.curPosY_ - m_.prevPosY_) > m_.deadZone_) ? true : false;

	m_.assign_ = mouseAssign;

	if (!m_.assign_.empty()/*���蓖�čς�*/) {

		for (auto& assign : m_.assign_) {

			if (GetAsyncKeyState(assign.code_))
				bit_[0].state_ |= inputBindings_[assign.key_];
		}
	}



	for (size_t i = 0; i < 4; i++)
	{
		bit_[i].trigger_ = ~bit_[i].prevState_ & bit_[i].state_;
		bit_[i].release_ = bit_[i].prevState_ & ~bit_[i].state_;
	}
}



//	���͂̃r�b�g��Ԃ�
uint64_t InputManager::state(size_t playerNum)
{
	return bit_[playerNum].state_;
}

uint64_t InputManager::down(size_t playerNum) 
{
	return bit_[playerNum].trigger_;
}

uint64_t InputManager::up(size_t playerNum)
{
	return bit_[playerNum].release_;
}