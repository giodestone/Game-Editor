#pragma once

#include "d3d11.h"
#include <SimpleMath.h>

#include "StepTimer.h"

class Camera
{
	float moveRate;
	float speedUpMultiplier;
	float camRotRate;
	float mouseLookSensitivity;
	
	DirectX::SimpleMath::Vector3 position;
	DirectX::SimpleMath::Vector3 rotation;
	DirectX::SimpleMath::Vector3 lookAt;
	DirectX::SimpleMath::Vector3 lookDirection;
	DirectX::SimpleMath::Vector3 right;

	DirectX::SimpleMath::Matrix viewMatrix;

	bool isFocused;
	bool isMouseLookEnabled;
	bool wasMouseLookEnabled;
	bool ignoreInitialChange;
	int mouseXOnBeginMouseLook;
	int mouseYOnBeginMouseLook;

	/// <summary>
	/// Update the look based on the mouse look.
	/// </summary>
	/// <param name="timer"></param>
	void UpdateMouseLook(DX::StepTimer const& timer);

	/// <summary>
	/// Update the current position and rotation based on keyboard commands.
	/// </summary>
	/// <param name="timer"></param>
	void UpdatePositionAndRotation(DX::StepTimer const& timer);

	/// <summary>
	/// Update the view matrix based on current camera position.
	/// </summary>
	void UpdateViewMatrix();
public:
	Camera();
	~Camera();

	/// <summary>
	/// Get the current position.
	/// </summary>
	/// <returns>Current position.</returns>
	DirectX::SimpleMath::Vector3 GetPosition() { return position; }
	
	/// <summary>
	/// Update function. Should be called when the renderer is updated.
	/// </summary>
	/// <param name="timer"></param>
	void Update(DX::StepTimer const& timer);

	/// <summary>
	/// Should be called when the window that the renderer is a part of loses focus.
	/// </summary>
	void OnLostFocus();

	/// <summary>
	/// Should be called when the window that the render is a part of regains focus.
	/// </summary>
	void OnRegainFocus();

	/// <summary>
	/// Should be called when the user is actively trying to mouse look.
	/// </summary>
	void OnMouseLook();

	/// <summary>
	/// Should be called when mouse look should be terminated.
	/// </summary>
	void OnEndMouseLook();

	/// <summary>
	/// Get the current view matrix.
	/// </summary>
	/// <returns></returns>
	DirectX::SimpleMath::Matrix GetCameraViewMatrix() { return viewMatrix; }
};

