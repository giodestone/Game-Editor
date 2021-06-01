#pragma once

struct InputCommands
{
	bool forward;
	bool back;
	bool right;
	bool left;
	bool up;
	bool down;

	bool speedUp;
	
	bool rotRight;
	bool rotLeft;
	bool rotUp;
	bool rotDown;

	// Mouse
	int mouseX = 0;
	int mouseY = 0;
	bool mouseLMBDown = false;
	

	/// <summary>
	/// Clear any commands to be false. Does not affect mouse.
	/// </summary>
	void ClearCommands()
	{
		forward = false;
		back = false;
		right = false;
		left = false;
		up = false;
		down = false;

		speedUp = false;

		rotRight = false;
		rotLeft = false;
		rotUp = false;
		rotDown = false;
	}
};
