#pragma once
#include <vector>

#include "InputCommands.h"
#include "SceneObject.h"

class ToolMain;

class Selection
{
	ToolMain* toolMain;

	bool isFocused;
	
	std::vector<std::pair<float, int>> currentSelection;
	int nearestSelectedSceneObjectID;
	std::vector<int> currentChunkIDs;

	bool wasClicked;
public:
	Selection();
	~Selection();

	static constexpr int NothingSelectedID = -1;
	
	/// <summary>
	/// Should be called before use once to set up references.
	/// </summary>
	/// <param name="toolMain"></param>
	void OnInitialise(ToolMain* toolMain);

	/// <summary>
	/// Should be called when the MOUSEDOWN event happens. Updates current selection.
	/// </summary>
	void OnMouseDown();

	/// <summary>
	/// Should be called when the MOUSEUP event happens.
	/// </summary>
	void OnMouseUp();

	/// <summary>
	/// Should be called when WM_ACTIVATE with WM_INACTIVE on the window that the camera is part of comes in.
	/// </summary>
	void OnLostFocus();

	/// <summary>
	/// Should be called when the WM_ACTIVATE with WA_ACTIVE or WA_CLICKACTIVE command comes in for the window that the camera is a part of. By default the window is considered in focus.
	/// </summary>
	void OnRegainFocus();

	
	// Properties

	/// <summary>
	/// Get the ID of the closest selected object.
	/// </summary>
	/// <returns>-1 (value of NothingSelectedID) if nothing is selected; ID of the item selected.</returns>
	int GetClosestSelectedSceneObjectID();

	/// <summary>
	/// Get the Scene object that is currently selected.
	/// </summary>
	/// <returns>nullptr if nothing is selected; the pointer to the SceneObject if it is (do not modify, should modify SceneGraph collection instead if needed).</returns>
	SceneObject* GetNearestSelectedSceneObject();

	/// <summary>
	/// Get all the currently selected objects, as sorted by the distance away from camera. If distance to camera is zero the current selection was set manually.
	/// </summary>
	/// <returns>Empty if none; Currently selected objects, as sorted by the distance away from camera.</returns>
	std::vector<std::pair<float, int>> GetCurrentlySelected();

	/// <summary>
	/// Set the currently selected object based on the scene graph index.
	/// </summary>
	/// <param name="sceneObjectID"></param>
	void SetCurrentlySelected(int sceneObjectID);
};

