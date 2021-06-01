#include "Selection.h"

#include "ToolMain.h"

Selection::Selection()
	: isFocused(true)
	, nearestSelectedSceneObjectID(-1)
	, wasClicked(false)
{
}

Selection::~Selection()
{
}

void Selection::OnInitialise(ToolMain* toolMain)
{
	this->toolMain = toolMain;
}

void Selection::OnMouseDown()
{
	if (!isFocused)
		return;

	if (wasClicked)
		return;
	
	currentSelection = toolMain->GetRenderer().MousePicking();

	if (currentSelection.empty())
	{
		currentChunkIDs.clear();
		
		nearestSelectedSceneObjectID = NothingSelectedID;
	}
	else
	{
		currentChunkIDs.clear();
		
		nearestSelectedSceneObjectID = currentSelection.front().second;
		
		currentChunkIDs.reserve(currentSelection.size());
		for (auto& i : currentSelection)
		{
			currentChunkIDs.push_back(toolMain->GetSceneGraph().at(i.second).chunk_ID);
		}
	}

	if (toolMain->GetObjectPropertiesDialog() != nullptr)
		toolMain->GetObjectPropertiesDialog()->SetCurrentSceneObject(GetNearestSelectedSceneObject());
	
	wasClicked = true; // Prevent the selection from happening twice.
}

void Selection::OnMouseUp()
{
	wasClicked = false; // Allow the mouse to be clicked again.
}

void Selection::OnLostFocus()
{
	this->isFocused = false;
}

void Selection::OnRegainFocus()
{
	this->isFocused = true;
}

void Selection::SetCurrentlySelected(int sceneObjectID)
{
	nearestSelectedSceneObjectID = sceneObjectID;
	currentSelection.clear();
	currentSelection.push_back(std::make_pair(0.f, sceneObjectID));
}

int Selection::GetClosestSelectedSceneObjectID()
{
	return nearestSelectedSceneObjectID;
}

SceneObject* Selection::GetNearestSelectedSceneObject()
{
	if (nearestSelectedSceneObjectID == NothingSelectedID)
		return nullptr;
	
	return &toolMain->GetSceneGraph().at(nearestSelectedSceneObjectID);
}

std::vector<std::pair<float, int>> Selection::GetCurrentlySelected()
{
	return currentSelection;
}
