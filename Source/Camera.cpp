#include "stdafx.h"

#include "Camera.h"

#include "ToolMain.h"

Camera::Camera()
	: moveRate(18.f)
	, speedUpMultiplier(2.f)
	, camRotRate(180.f)
	, mouseLookSensitivity(1.f / 6.f)
	, position(0.f, 3.7f, -3.5f)
	, rotation(0.f, 0.f, 0.f)
	, isFocused(true)
	, isMouseLookEnabled(false)
	, wasMouseLookEnabled(false)
	, ignoreInitialChange(false)
	, mouseXOnBeginMouseLook(0)
	, mouseYOnBeginMouseLook(0)
{
}

Camera::~Camera()
{
}

void Camera::UpdateMouseLook(DX::StepTimer const& timer)
{
	auto inputCommands = ToolMain::GetInstance()->GetInputCommands();

	if (!isMouseLookEnabled)
		return;

	if (ignoreInitialChange)
	{
		ignoreInitialChange = false;
		return;
	}

	POINT mousePt;
	mousePt.x = inputCommands.mouseX;
	mousePt.y = inputCommands.mouseY;
	
	ClientToScreen(ToolMain::GetInstance()->GetRenderer().GetOwningWindow(), &mousePt);
	
	auto deltaX = mousePt.x - mouseXOnBeginMouseLook;
	auto deltaY = mousePt.y - mouseYOnBeginMouseLook;

	rotation.x -= static_cast<float>(deltaY) * mouseLookSensitivity;
	rotation.y -= static_cast<float>(deltaX) * mouseLookSensitivity;

	SetCursorPos(mouseXOnBeginMouseLook, mouseYOnBeginMouseLook);
}

void Camera::UpdatePositionAndRotation(DX::StepTimer const& timer)
{
	using namespace DirectX;

	auto inputCommands = ToolMain::GetInstance()->GetInputCommands();

	float speedMultiplier = 1.f;
	if (inputCommands.speedUp)
		speedMultiplier = speedUpMultiplier;

	if (inputCommands.forward)
	{
		auto radians = rotation.y * 0.0174532f;

		// Update the position.
		position.x -= sinf(radians) * moveRate * static_cast<float>(timer.GetElapsedSeconds()) * speedMultiplier;
		position.z -= cosf(radians) * moveRate * static_cast<float>(timer.GetElapsedSeconds()) * speedMultiplier;
	}
	if (inputCommands.back)
	{
		auto radians = rotation.y * 0.0174532f;

		// Update the position.
		position.x += sinf(radians) * moveRate * static_cast<float>(timer.GetElapsedSeconds() * speedMultiplier);
		position.z += cosf(radians) * moveRate * static_cast<float>(timer.GetElapsedSeconds() * speedMultiplier);
	}
	if (inputCommands.right)
	{
		// Convert degrees to radians.
		auto radians = rotation.y * 0.0174532f;

		// Update the position.
		position.z -= sinf(radians) * moveRate * static_cast<float>(timer.GetElapsedSeconds() * speedMultiplier);
		position.x += cosf(radians) * moveRate * static_cast<float>(timer.GetElapsedSeconds() * speedMultiplier);
	}
	if (inputCommands.left)
	{
		// Convert degrees to radians.
		auto radians = rotation.y * 0.0174532f;

		// Update the position.
		position.z += sinf(radians) * moveRate * static_cast<float>(timer.GetElapsedSeconds() * speedMultiplier);
		position.x -= cosf(radians) * moveRate * static_cast<float>(timer.GetElapsedSeconds() * speedMultiplier);
	}
	if (inputCommands.up)
	{
		position.y += moveRate * static_cast<float>(timer.GetElapsedSeconds() * speedMultiplier);
	}
	if (inputCommands.down)
	{
		position.y -= moveRate * static_cast<float>(timer.GetElapsedSeconds() * speedMultiplier);
	}


	if (inputCommands.rotRight)
	{
		rotation.y -= camRotRate * static_cast<float>(timer.GetElapsedSeconds());
	}
	if (inputCommands.rotLeft)
	{
		rotation.y += camRotRate * static_cast<float>(timer.GetElapsedSeconds());
	}
	if (inputCommands.rotUp)
	{
		rotation.x += camRotRate * static_cast<float>(timer.GetElapsedSeconds());
	}
	if (inputCommands.rotDown)
	{
		rotation.x -= camRotRate * static_cast<float>(timer.GetElapsedSeconds());
	}
}

void Camera::UpdateViewMatrix()
{
	using namespace DirectX;

	XMVECTOR up, positionv, lookAt;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	// Setup the vectors
	up = XMVectorSet(0.0f, 1.0, 0.0, 1.0f);
	positionv = XMLoadFloat3(&position);
	lookAt = XMVectorSet(0.0, 0.0, 1.0f, 1.0f);

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	pitch = rotation.x * 0.0174532f;
	yaw = rotation.y * 0.0174532f;
	roll = rotation.z * 0.0174532f;

	// Create the rotation matrix from the yaw, pitch, and roll values.
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	lookAt = XMVector3TransformCoord(lookAt, rotationMatrix);
	up = XMVector3TransformCoord(up, rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	lookAt = positionv + lookAt;

	// Finally create the view matrix from the three updated vectors.
	viewMatrix = XMMatrixLookAtLH(positionv, lookAt, up);
}

void Camera::Update(DX::StepTimer const& timer)
{
	if (isFocused)
	{
		UpdatePositionAndRotation(timer);
		UpdateMouseLook(timer);	
	}
	
	UpdateViewMatrix();	
}

void Camera::OnLostFocus()
{
	isFocused = false;
	OnEndMouseLook();
}

void Camera::OnRegainFocus()
{
	isFocused = true;
}

void Camera::OnMouseLook()
{
	if (!isFocused)
		return;
	
	if (!wasMouseLookEnabled)
	{
		RECT rect;
		GetClientRect(ToolMain::GetInstance()->GetRenderer().GetOwningWindow(), &rect);
		POINT mousePt;
		mousePt.x = rect.right / 2.f;
		mousePt.y = rect.bottom / 2.f;
		
		ClientToScreen(ToolMain::GetInstance()->GetRenderer().GetOwningWindow(), &mousePt);

		SetCursorPos(mousePt.x, mousePt.y);
		mouseXOnBeginMouseLook = mousePt.x;
		mouseYOnBeginMouseLook = mousePt.y;
		wasMouseLookEnabled = true;
		ignoreInitialChange = true;
	}
	
	isMouseLookEnabled = true;
}

void Camera::OnEndMouseLook()
{
	isMouseLookEnabled = false;
	wasMouseLookEnabled = false;
}
