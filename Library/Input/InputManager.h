#pragma once

#include <map>
#include <list>
#include <string>

#include <Keyboard.h>
#include <Mouse.h>
#include <DirectXMath.h>
#include <GamePad.h>
#include <Windows.h>

#include "../Math/Vector.h"

class InputManager;

namespace Input {

	//	�r�b�g�p�̃L�[���x��
	inline static constexpr uint64_t LEFT	 = 1;
	inline static constexpr uint64_t RIGHT	 = 2;
	inline static constexpr uint64_t UP		 = 4;
	inline static constexpr uint64_t DOWN	 = 8;
	inline static constexpr uint64_t LMB	 = 16;
	inline static constexpr uint64_t RMB	 = 32;
	inline static constexpr uint64_t MMB	 = 64;
	inline static constexpr uint64_t DELETE_ = 128;
	inline static constexpr uint64_t ENTER	 = 256;
	inline static constexpr uint64_t ESCAPE  = 512;
	inline static constexpr uint64_t CONFIRM = 1024;
	inline static constexpr uint64_t PAUSE   = 2048;
	inline static constexpr uint64_t LEFT_ATTACK   = 4096;
	inline static constexpr uint64_t RIGHT_ATTACK   = 8192;


	class GamePad {
	public:
		enum Controlls {
			UP,
			DOWN,
			RIGHT,
			LEFT,
			A,
			B,
			X,
			Y,
			L3,
			R3,
			L1,
			R1,
			BACK,
			START,
			L2,
			R2,
			LSTICK_UP,
			LSTICK_DOWN,
			LSTICK_RIGHT,
			LSTICK_LEFT,
			RSTICK_UP,
			RSTICK_DOWN,
			RSTICK_RIGHT,
			RSTICK_LEFT,
			MAX
		};
	};
}


struct InputAssign {

	std::string	key_;
	int64_t		bit_;
	int			code_;
};


struct KeyState {

	DirectX::Keyboard::State state_;
	std::list<InputAssign> assign_;
};


struct MouseState {

	int prevPosX_, prevPosY_;
	int curPosX_, curPosY_;
	int deltaX_, deltaY_;
	bool isMove_;
	float deadZone_ = 0.3f;

	float wheelDelta_;
	int wheel_;

	DirectX::Mouse::State state_;
	std::list<InputAssign> assign_;

	//	�R�[�h�ɉ��������͂����m����
	bool isClicked(const int& code) {

		switch (code) {

		case VK_LBUTTON: return state_.leftButton;  break;
		case VK_RBUTTON: return state_.rightButton; break;
		case VK_MBUTTON: return state_.middleButton; break;
		}

		return false;
	}
};


//	�X�e�B�b�N
struct Stick {

	//	�X�� ( -1.0 ~ 1.0 )
	float leftX_, leftY_;
	float rightX_, rightY_;
	float leftDeadZone_, rightDeadZone_;

	//	���X�e�B�b�N���X�������ǂ���
	bool isLStickLeft()		{ return leftX_ < -leftDeadZone_; }
	bool isLStickRight()	{ return leftX_ >  leftDeadZone_; }
	bool isLStickUp()		{ return leftY_ >  leftDeadZone_; }
	bool isLStickDown()		{ return leftY_ < -leftDeadZone_; }

	//	�E�X�e�B�b�N���X�������ǂ���
	bool isRStickLeft()		{ return rightX_ < -rightDeadZone_; }
	bool isRStickRight()	{ return rightX_ >  rightDeadZone_; }
	bool isRStickUp()		{ return rightY_ >  rightDeadZone_; }
	bool isRStickDown()		{ return rightY_ < -rightDeadZone_; }

	//	�R�[�h�ɉ��������͂����m����
	bool isTilted(const int& code) {

		switch (code) {

			//	���X�e�B�b�N
		case Input::GamePad::LSTICK_UP:		return isLStickUp();	break;
		case Input::GamePad::LSTICK_DOWN:	return isLStickDown();	break;
		case Input::GamePad::LSTICK_RIGHT:	return isLStickRight(); break;
		case Input::GamePad::LSTICK_LEFT:	return isLStickLeft();	break;

			//	�E�X�e�B�b�N
		case Input::GamePad::RSTICK_UP:		return isRStickUp();	break;
		case Input::GamePad::RSTICK_DOWN:	return isRStickDown();	break;
		case Input::GamePad::RSTICK_RIGHT:	return isRStickRight(); break;
		case Input::GamePad::RSTICK_LEFT:	return isRStickLeft();	break;
		}

		return false;
	}
};


//	�g���K�[
struct Trigger {

	//	�������� ( 0.0 ~ 1.0 )
	float left_, right_;
	float leftDeadZone_, rightDeadZone_;

	bool isL2Pressed() { return left_  > leftDeadZone_; }
	bool isR2Pressed() { return right_ > rightDeadZone_; }

	//	�R�[�h�ɉ��������͂����m����
	bool isTriggered(const int& code) {

		switch (code) {

		case Input::GamePad::L2:
			return isL2Pressed(); break;

		case Input::GamePad::R2:
			return isR2Pressed(); break;
		}

		return false;
	}
};


struct PadState {

	Stick stick_;
	Trigger trigger_;
	float LMotor_, RMotor_;

	DirectX::GamePad::State state_;
	std::list<InputAssign> padAssign_;
};



class InputManager {
public:
	//	�L�[�{�[�h
	std::unique_ptr<DirectX::Keyboard> keyboard_;
	DirectX::Keyboard::KeyboardStateTracker keyboardStateTracker_;
	
	//	�}�E�X
	std::unique_ptr<DirectX::Mouse> mouse_;

	//	�R���g���[���[
	std::shared_ptr<DirectX::GamePad> gamePad_;

	//	�L�[���蓖�ėp
	std::map<std::string, uint64_t> inputBindings_;

public:
	static InputManager& Instance() { 

		static InputManager instance_;
		return instance_;
	}


	~InputManager();
	void initialize();
	void update();
	void loadAssignment();
	void saveAssignment();
	void bindInputAssign();


	struct Bit {

		uint64_t state_;
		uint64_t prevState_;
		uint64_t trigger_;
		uint64_t release_;
	}bit_[4];


	//	���͂̃r�b�g��Ԃ�
	uint64_t state(size_t playerNum);
	uint64_t down(size_t playerNum);
	uint64_t up(size_t playerNum);


	//	�L�[�{�[�h
	KeyState k_;

	DirectX::Keyboard::State getKeyState() const { return k_.state_; }
	DirectX::Keyboard::State isKeyDown() const { return keyboardStateTracker_.pressed; }
	DirectX::Keyboard::State isKeyUp() const { return keyboardStateTracker_.released; }


	//	�}�E�X
	MouseState m_;

	DirectX::Mouse::State getMouseState() const { return m_.state_; }


	//	�R���g���[���[
	PadState p_[4];

	DirectX::GamePad::State getPadState(int padNum) { return gamePad_->GetState(padNum); }
	bool isConnected(int padNum) const { return gamePad_->GetState(padNum).connected; }

	//	�J�[�\���̍��W
	int getCursorPosX() const { return m_.state_.x; }
	int getCursorPosY() const { return m_.state_.y; }
	DirectX::XMFLOAT2 getCursorPos() { 
		return DirectX::XMFLOAT2(
			static_cast<float>(m_.state_.x),
			static_cast<float>(m_.state_.y)
		);
	}

	// --- �J�[�\���̕ω��� ---
	int getCursorDeltaX() const { return m_.deltaX_; }
	int getCursorDeltaY() const { return m_.deltaY_; }
};