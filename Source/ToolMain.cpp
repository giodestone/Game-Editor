#include "ToolMain.h"
#include "resource.h"
#include <vector>
#include <sstream>
#include "MFCFrame.h"

ToolMain* ToolMain::instance = nullptr;
CMyFrame* ToolMain::mainWindow = nullptr;
bool ToolMain::isInitialised = false;


//
//ToolMain Class
ToolMain::ToolMain()
	: isFocused(true)
{
	ASSERT(instance == nullptr);
	instance = this;

	isInitialised = false;
	
	currentChunk = 0;		//default value
	selectedObject = 0;	//initial selection ID
	sceneGraph.clear();	//clear the vector for the scenegraph
	databaseConnection = NULL;

	//zero input commands
	toolInputCommands.forward		= false;
	toolInputCommands.back		= false;
	toolInputCommands.left		= false;
	toolInputCommands.right		= false;
	toolInputCommands.up = false;
	toolInputCommands.down = false;

	toolInputCommands.rotUp = false;
	toolInputCommands.rotDown = false;
	toolInputCommands.rotLeft = false;
	toolInputCommands.rotRight = false;
}


ToolMain::~ToolMain()
{
	sqlite3_close(databaseConnection);		//close the database connection
}

ToolMain* ToolMain::GetInstance()
{
	ASSERT(isInitialised);
	return instance;
}

bool ToolMain::IsInitialised()
{
	return isInitialised;
}


int ToolMain::GetClosestSelectedSceneObjectID()
{
	return selection.GetClosestSelectedSceneObjectID();
}

void ToolMain::OnActionInitialise(HWND handle, int width, int height)
{	
	//window size, handle etc for directX
	width = width;
	height = height;
	
	d3dRenderer.Initialize(handle, width, height);

	//database connection establish
	int rc;
	rc = sqlite3_open_v2("database/test.db",&databaseConnection, SQLITE_OPEN_READWRITE, NULL);

	if (rc) 
	{
		TRACE("Can't open database");
		MessageBox(NULL, L"Unable to open database.", L"Error", MB_OK | MB_ICONERROR);
	}
	else 
	{
		TRACE("Opened database successfully");
	}

	OnActionLoad();

	selection.OnInitialise(this);

	isInitialised = true;
}

void ToolMain::OnActionLoad()
{
	//load current chunk and objects into lists
	if (!sceneGraph.empty())		//is the vector empty
	{
		sceneGraph.clear();		//if not, empty it
	}

	//SQL
	int rc;
	char *sqlCommand;
	char *ErrMSG = 0;
	sqlite3_stmt *pResults;								//results of the query
	sqlite3_stmt *pResultsChunk;

	//OBJECTS IN THE WORLD
	//prepare SQL Text
	sqlCommand = "SELECT * from Objects";				//sql command which will return all records from the objects table.
	//Send Command and fill result object
	rc = sqlite3_prepare_v2(databaseConnection, sqlCommand, -1, &pResults, 0 );
	
	//loop for each row in results until there are no more rows.  ie for every row in the results. We create and object
	while (sqlite3_step(pResults) == SQLITE_ROW)
	{	
		SceneObject newSceneObject;
		newSceneObject.ID = sqlite3_column_int(pResults, 0);
		newSceneObject.chunk_ID = sqlite3_column_int(pResults, 1);
		newSceneObject.SetModelPath(reinterpret_cast<const char*>(sqlite3_column_text(pResults, 2)));
		newSceneObject.SetTextureDiffusePath(reinterpret_cast<const char*>(sqlite3_column_text(pResults, 3)));
		newSceneObject.posX = sqlite3_column_double(pResults, 4);
		newSceneObject.posY = sqlite3_column_double(pResults, 5);
		newSceneObject.posZ = sqlite3_column_double(pResults, 6);
		newSceneObject.rotX = sqlite3_column_double(pResults, 7);
		newSceneObject.rotY = sqlite3_column_double(pResults, 8);
		newSceneObject.rotZ = sqlite3_column_double(pResults, 9);
		newSceneObject.scaX = sqlite3_column_double(pResults, 10);
		newSceneObject.scaY = sqlite3_column_double(pResults, 11);
		newSceneObject.scaZ = sqlite3_column_double(pResults, 12);
		newSceneObject.render = sqlite3_column_int(pResults, 13);
		newSceneObject.collision = sqlite3_column_int(pResults, 14);
		newSceneObject.SetCollisionMeshPath(reinterpret_cast<const char*>(sqlite3_column_text(pResults, 15)));
		newSceneObject.collectable = sqlite3_column_int(pResults, 16);
		newSceneObject.destructable = sqlite3_column_int(pResults, 17);
		newSceneObject.health_amount = sqlite3_column_int(pResults, 18);
		newSceneObject.editor_render = sqlite3_column_int(pResults, 19);
		newSceneObject.editor_texture_vis = sqlite3_column_int(pResults, 20);
		newSceneObject.editor_normals_vis = sqlite3_column_int(pResults, 21);
		newSceneObject.editor_collision_vis = sqlite3_column_int(pResults, 22);
		newSceneObject.editor_pivot_vis = sqlite3_column_int(pResults, 23);
		newSceneObject.pivotX = sqlite3_column_double(pResults, 24);
		newSceneObject.pivotY = sqlite3_column_double(pResults, 25);
		newSceneObject.pivotZ = sqlite3_column_double(pResults, 26);
		newSceneObject.snapToGround = sqlite3_column_int(pResults, 27);
		newSceneObject.AINode = sqlite3_column_int(pResults, 28);
		newSceneObject.SetAudioPath(reinterpret_cast<const char*>(sqlite3_column_text(pResults, 29)));
		newSceneObject.volume = sqlite3_column_double(pResults, 30);
		newSceneObject.pitch = sqlite3_column_double(pResults, 31);
		newSceneObject.pan = sqlite3_column_int(pResults, 32);
		newSceneObject.one_shot = sqlite3_column_int(pResults, 33);
		newSceneObject.play_on_init = sqlite3_column_int(pResults, 34);
		newSceneObject.play_in_editor = sqlite3_column_int(pResults, 35);
		newSceneObject.min_dist = sqlite3_column_double(pResults, 36);
		newSceneObject.max_dist = sqlite3_column_double(pResults, 37);
		newSceneObject.camera = sqlite3_column_int(pResults, 38);
		newSceneObject.path_node = sqlite3_column_int(pResults, 39);
		newSceneObject.path_node_start = sqlite3_column_int(pResults, 40);
		newSceneObject.path_node_end = sqlite3_column_int(pResults, 41);
		newSceneObject.parent_id = sqlite3_column_int(pResults, 42);
		newSceneObject.editor_wireframe = sqlite3_column_int(pResults, 43);
		newSceneObject.name = reinterpret_cast<const char*>(sqlite3_column_text(pResults, 44));

		newSceneObject.light_type = sqlite3_column_int(pResults, 45);
		newSceneObject.light_diffuse_r = sqlite3_column_double(pResults, 46);
		newSceneObject.light_diffuse_g = sqlite3_column_double(pResults, 47);
		newSceneObject.light_diffuse_b = sqlite3_column_double(pResults, 48);
		newSceneObject.light_specular_r = sqlite3_column_double(pResults, 49);
		newSceneObject.light_specular_g = sqlite3_column_double(pResults, 50);
		newSceneObject.light_specular_b = sqlite3_column_double(pResults, 51);
		newSceneObject.light_spot_cutoff = sqlite3_column_double(pResults, 52);
		newSceneObject.light_constant = sqlite3_column_double(pResults, 53);
		newSceneObject.light_linear = sqlite3_column_double(pResults, 54);
		newSceneObject.light_quadratic = sqlite3_column_double(pResults, 55);
	

		//send completed object to scenegraph
		sceneGraph.insert({newSceneObject.ID, newSceneObject });
	}

	//THE WORLD CHUNK
	//prepare SQL Text
	sqlCommand = "SELECT * from Chunks";				//sql command which will return all records from  chunks table. There is only one tho.
														//Send Command and fill result object
	rc = sqlite3_prepare_v2(databaseConnection, sqlCommand, -1, &pResultsChunk, 0);


	sqlite3_step(pResultsChunk);
	chunk.ID = sqlite3_column_int(pResultsChunk, 0);
	chunk.name = reinterpret_cast<const char*>(sqlite3_column_text(pResultsChunk, 1));
	chunk.chunk_x_size_metres = sqlite3_column_int(pResultsChunk, 2);
	chunk.chunk_y_size_metres = sqlite3_column_int(pResultsChunk, 3);
	chunk.chunk_base_resolution = sqlite3_column_int(pResultsChunk, 4);
	chunk.heightmap_path = reinterpret_cast<const char*>(sqlite3_column_text(pResultsChunk, 5));
	chunk.tex_diffuse_path = reinterpret_cast<const char*>(sqlite3_column_text(pResultsChunk, 6));
	chunk.tex_splat_alpha_path = reinterpret_cast<const char*>(sqlite3_column_text(pResultsChunk, 7));
	chunk.tex_splat_1_path = reinterpret_cast<const char*>(sqlite3_column_text(pResultsChunk, 8));
	chunk.tex_splat_2_path = reinterpret_cast<const char*>(sqlite3_column_text(pResultsChunk, 9));
	chunk.tex_splat_3_path = reinterpret_cast<const char*>(sqlite3_column_text(pResultsChunk, 10));
	chunk.tex_splat_4_path = reinterpret_cast<const char*>(sqlite3_column_text(pResultsChunk, 11));
	chunk.render_wireframe = sqlite3_column_int(pResultsChunk, 12);
	chunk.render_normals = sqlite3_column_int(pResultsChunk, 13);
	chunk.tex_diffuse_tiling = sqlite3_column_int(pResultsChunk, 14);
	chunk.tex_splat_1_tiling = sqlite3_column_int(pResultsChunk, 15);
	chunk.tex_splat_2_tiling = sqlite3_column_int(pResultsChunk, 16);
	chunk.tex_splat_3_tiling = sqlite3_column_int(pResultsChunk, 17);
	chunk.tex_splat_4_tiling = sqlite3_column_int(pResultsChunk, 18);


	//Process REsults into renderable
	d3dRenderer.BuildDisplayList(sceneGraph);
	//build the renderable chunk 
	d3dRenderer.BuildDisplayChunk(&chunk);

}

void ToolMain::OnActionSave()
{
	//SQL
	int rc;
	char *sqlCommand;
	char *ErrMSG = 0;
	sqlite3_stmt *pResults;	//results of the query

	bool hasQueryFailed = false;
	
	// Update existing objects in database with new values.
	std::wstring sqlCommand2;

	for (auto& pair : sceneGraph)
	{
		auto& sceneObject = pair.second;
		std::stringstream command;
		command << "UPDATE Objects "
			<< "SET "
			<< "mesh = '" << sceneObject.GetMeshPathStr() << "', "
			<< "tex_diffuse = '" << sceneObject.GetTextureDiffusePathStr() << "', "
			<< "position_x = " << sceneObject.posX << ", "
			<< "position_y = " << sceneObject.posY << ", "
			<< "position_z = " << sceneObject.posZ << ", "
			<< "rotation_x = " << sceneObject.rotX << ", "
			<< "rotation_y = " << sceneObject.rotY << ", "
			<< "rotation_z = " << sceneObject.rotZ << ", "
			<< "scale_x = " << sceneObject.scaX << ", "
			<< "scale_y = " << sceneObject.scaY << ", "
			<< "scale_z = " << sceneObject.scaZ << ", "
			<< "render = " << sceneObject.render << ", "
			<< "collision = " << sceneObject.collision << ", "
			<< "collision_mesh = '" << sceneObject.GetCollisionMeshPathStr() << "', "
			<< "collectable = " << sceneObject.collectable << ", "
			<< "destructable = " << sceneObject.destructable << ", "
			<< "health_amount = " << sceneObject.health_amount << ", "
			<< "editor_render = " << sceneObject.editor_render << ", "
			<< "editor_texture_vis = " << sceneObject.editor_texture_vis << ", "
			<< "editor_normals_vis = " << sceneObject.editor_normals_vis << ", "
			<< "editor_collision_vis = " << sceneObject.editor_collision_vis << ", "
			<< "editor_pivot_vis = " << sceneObject.editor_pivot_vis << ", "
			<< "pivot_x = " << sceneObject.pivotX << ", "
			<< "pivot_y = " << sceneObject.pivotY << ", "
			<< "pivot_z = " << sceneObject.pivotZ << ", "
			<< "snap_to_ground = " << sceneObject.snapToGround << ","
			<< "AI_node = " << sceneObject.AINode << ", "
			<< "audio_file = '" << sceneObject.GetAudioPathStr() << "', "
			<< "volume = " << sceneObject.volume << ", "
			<< "pitch = " << sceneObject.pitch << ", "
			<< "pan = " << sceneObject.pan << ", "
			<< "one_shot = " << sceneObject.one_shot << ", "
			<< "play_on_init = " << sceneObject.play_on_init << ", "
			<< "play_in_editor = " << sceneObject.play_in_editor << ","
			<< "min_dist = " << sceneObject.min_dist << ", "
			<< "max_dist = " << sceneObject.max_dist << ", "
			<< "camera = " << sceneObject.camera << ", "
			<< "path_node = " << sceneObject.path_node << ", "
			<< "path_node_start = " << sceneObject.path_node_start << ", "
			<< "path_node_end = " << sceneObject.path_node_end << ", "
			<< "parent_ID = " << sceneObject.parent_id << ", "
			<< "editor_wireframe = " << sceneObject.editor_wireframe << ", "
			<< "name = '" << sceneObject.name << "', "
			<< "light_type = " << sceneObject.light_type << ", "
			<< "light_diffuse_r = " << sceneObject.light_diffuse_r << ", "
			<< "light_diffuse_g = " << sceneObject.light_diffuse_g << ", "
			<< "light_diffuse_b = " << sceneObject.light_diffuse_b << ", "
			<< "light_specular_r = " << sceneObject.light_specular_r << ", "
			<< "light_specular_g = " << sceneObject.light_specular_g << ", "
			<< "light_specular_b = " << sceneObject.light_specular_b << ", "
			<< "light_spot_cutoff = " << sceneObject.light_spot_cutoff << ", "
			<< "light_constant = " << sceneObject.light_constant << ", "
			<< "light_linear = " << sceneObject.light_linear << ", "
			<< "light_quadratic = " << sceneObject.light_quadratic << " "
  		
			<< "WHERE ID IS " << sceneObject.ID << ";";

		rc = sqlite3_prepare_v2(databaseConnection, command.str().c_str(), -1, &pResults, 0);
		if (rc != SQLITE_OK)
		{
			OutputDebugStringA(command.str().c_str());
			std::wstringstream errorss;
			errorss << "Failed to save object ID: " << sceneObject.ID << ".";
			MessageBox(NULL, errorss.str().c_str(), L"Error", MB_OK | MB_ICONERROR);
			hasQueryFailed = true;
		}
		sqlite3_step(pResults);
 	}

	// Not being able to save isn't fatal.
	if (!hasQueryFailed)
		MessageBox(NULL, L"Objects saved successfully.", L"Notification", MB_OK | MB_ICONINFORMATION);
	else
		MessageBox(NULL, L"Some objects not saved.", L"Error", MB_OK | MB_ICONERROR);
}

void ToolMain::OnActionSaveTerrain()
{
	d3dRenderer.SaveDisplayChunk(&chunk);
}

void ToolMain::Tick(MSG *msg)
{
	//Renderer Update Call
	d3dRenderer.Tick(&toolInputCommands);
}

void ToolMain::UpdateInput(MSG * msg)
{

	switch (msg->message)
	{
		//Global inputs,  mouse position and keys etc
	case WM_KEYDOWN:
		keyArray[msg->wParam] = true;
		break;

	case WM_KEYUP:
		keyArray[msg->wParam] = false;
		break;

	case WM_MOUSEMOVE:		
		toolInputCommands.mouseX = GET_X_LPARAM(msg->lParam);
		toolInputCommands.mouseY = GET_Y_LPARAM(msg->lParam);
		break;

	case WM_LBUTTONDOWN:
		OnLeftMouseDown();
		break;

	case WM_LBUTTONUP:
		OnLeftMouseUp();
		break;

	case WM_RBUTTONDOWN:
		OnRightMouseDown();
		break;

	case WM_RBUTTONUP:
		OnRightMouseUp();
		break;
	}
	//here we update all the actual app functionality that we want.  This information will either be used int toolmain, or sent down to the renderer (Camera movement etc

	UpdateInputCommands();
}

void ToolMain::UpdateInputCommands()
{	
	if (keyArray['W'])
	{
		toolInputCommands.forward = true;
	}
	else toolInputCommands.forward = false;

	if (keyArray['S'])
	{
		toolInputCommands.back = true;
	}
	else toolInputCommands.back = false;
	if (keyArray['A'])
	{
		toolInputCommands.left = true;
	}
	else toolInputCommands.left = false;
	if (keyArray['D'])
	{
		toolInputCommands.right = true;
	}
	else toolInputCommands.right = false;
	if (keyArray['R'])
	{
		toolInputCommands.up = true;
	}
	else toolInputCommands.up = false;
	if (keyArray['F'])
	{
		toolInputCommands.down = true;
	}
	else toolInputCommands.down = false;

	//rotation
	if (keyArray['E'])
	{
		toolInputCommands.rotRight = true;
	}
	else toolInputCommands.rotRight = false;
	if (keyArray['Q'])
	{
		toolInputCommands.rotLeft = true;
	}
	else toolInputCommands.rotLeft = false;
	if (keyArray['Z'])
	{
		toolInputCommands.rotUp = true;
	}
	else toolInputCommands.rotUp = false;
	if (keyArray['X'])
	{
		toolInputCommands.rotDown = true;
	}
	else toolInputCommands.rotDown = false;

	if (keyArray[VK_SHIFT])
		toolInputCommands.speedUp = true;
	else
		toolInputCommands.speedUp = false;
}

void ToolMain::OnLeftMouseDown()
{
	toolInputCommands.mouseLMBDown = true;
	
	selection.OnMouseDown();
	

}

void ToolMain::OnLeftMouseUp()
{
	toolInputCommands.mouseLMBDown = false;

	selection.OnMouseUp();
	
}

void ToolMain::OnRightMouseDown()
{
	if (IsMouseInRenderer())
		GetRenderer().GetCamera().OnMouseLook();
	else
		GetRenderer().GetCamera().OnEndMouseLook();
}

void ToolMain::OnRightMouseUp()
{
	GetRenderer().GetCamera().OnEndMouseLook();
}

bool ToolMain::IsMouseInRenderer()
{
	if (mainWindow == nullptr)
		return false;
	
	RECT rect;
	GetWindowRect(mainWindow->m_hWnd, &rect);
	
	POINT mousePoint;
	mousePoint.x = toolInputCommands.mouseX;
	mousePoint.y = toolInputCommands.mouseY;
	ClientToScreen(mainWindow->m_hWnd, &mousePoint);
	
	return PtInRect(&rect, mousePoint);
}

void ToolMain::OnMainWindowLostFocus()
{
	isFocused = false;
	selection.OnLostFocus();
	d3dRenderer.GetCamera().OnLostFocus();
}

void ToolMain::OnMainWindowRegainFocus()
{
	isFocused = true;
	selection.OnRegainFocus();
	d3dRenderer.GetCamera().OnRegainFocus();
}
