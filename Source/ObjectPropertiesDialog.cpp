// ObjectPropertiesDialog.cpp : implementation file
//

#include "ObjectPropertiesDialog.h"
#include "COleVariantHelpers.h"

#include <afxshellmanager.h>
#include <afxvisualmanagerwindows.h>

#include "afxdialogex.h"
#include "resource.h"
#include "afxpropertygridctrl.h"

#include <locale>
#include <codecvt>
#include <filesystem>

#include "ToolMain.h"

using namespace COleVariantHelpers;

// ObjectPropertiesDialog dialog

IMPLEMENT_DYNAMIC(ObjectPropertiesDialog, CDialogEx)

ObjectPropertiesDialog::ObjectPropertiesDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_OBJECTPROPERTIESDIALOG, pParent)
	, currentSceneObject(nullptr)
	, toolMain(nullptr)
	, isUpdatingFieldsFromCurrentlySelectedObject(false)
{
}

ObjectPropertiesDialog::~ObjectPropertiesDialog()
{
	if (toolMain != nullptr)
		toolMain->SetObjectPropertiesDialogReference(nullptr);
}

void ObjectPropertiesDialog::OnRecievedEvent()
{
	UpdateData();
}

void ObjectPropertiesDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NAMEEDIT, nameEdit);
	DDX_Control(pDX, IDC_POSXEDIT, posXEdit);
	DDX_Control(pDX, IDC_POSYEDIT, posYEdit);
	DDX_Control(pDX, IDC_POSZEDIT, posZEdit);
	DDX_Control(pDX, IDC_ROTXEDIT, rotXEdit);
	DDX_Control(pDX, IDC_rotyedit, rotYEdit);
	DDX_Control(pDX, IDC_ROTZEDIT, rotZEdit);
	DDX_Control(pDX, IDC_SCALEXEDIT, scaleXEdit);
	DDX_Control(pDX, IDC_SCALEYEDIT, scaleYEdit);
	DDX_Control(pDX, IDC_SCALEZEDIT, scaleZEdit);
	DDX_Control(pDX, IDC_OBJECTPROPERTIESGRID, propertiesGrid);

	if (currentSceneObject != nullptr)
	{
		// DDX was made before std::string - workaround for that.
		auto tempName = StringToCString(currentSceneObject->name);
		DDX_Text(pDX, IDC_NAMEEDIT, tempName);
		currentSceneObject->name = CStringToString(tempName);
		DDX_Text(pDX, IDC_POSXEDIT, currentSceneObject->posX);
		DDX_Text(pDX, IDC_POSYEDIT, currentSceneObject->posY);
		DDX_Text(pDX, IDC_POSZEDIT, currentSceneObject->posZ);

		DDX_Text(pDX, IDC_ROTXEDIT, currentSceneObject->rotX);
		DDX_Text(pDX, IDC_rotyedit, currentSceneObject->rotY);
		DDX_Text(pDX, IDC_ROTZEDIT, currentSceneObject->rotZ);

		DDX_Text(pDX, IDC_SCALEXEDIT, currentSceneObject->scaX);
		DDX_Text(pDX, IDC_SCALEYEDIT, currentSceneObject->scaY);
		DDX_Text(pDX, IDC_SCALEZEDIT, currentSceneObject->scaZ);
		
		currentSceneObject->MarkModified();
	}
}

BOOL ObjectPropertiesDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	InitialisePropertyGrid();
	InitialisePropertyToDatabaseNameLookup();
	InitialiseFloatEdits();
	
	if (currentSceneObject == nullptr) 
		SetAllFieldsEnableState(false);

	toolMain->SetObjectPropertiesDialogReference(this);

	
	return true;
}

void ObjectPropertiesDialog::InitialiseFloatEdits()
{
	posXEdit.SetOwner(this);
	posYEdit.SetOwner(this);
	posZEdit.SetOwner(this);

	rotXEdit.SetOwner(this);
	rotYEdit.SetOwner(this);
	rotZEdit.SetOwner(this);

	scaleXEdit.SetOwner(this);
	scaleYEdit.SetOwner(this);
	scaleZEdit.SetOwner(this);
	
	posXEdit.SubscribeReciever(this);
	posYEdit.SubscribeReciever(this);
	posZEdit.SubscribeReciever(this);
	
	rotXEdit.SubscribeReciever(this);
	rotYEdit.SubscribeReciever(this);
	rotZEdit.SubscribeReciever(this);
	
	scaleXEdit.SubscribeReciever(this);
	scaleYEdit.SubscribeReciever(this);
	scaleZEdit.SubscribeReciever(this);
}

void ObjectPropertiesDialog::InitialisePropertyGrid()
{
	propertiesGrid.EnableDescriptionArea();
	propertiesGrid.SetVSDotNetLook();
	propertiesGrid.MarkModifiedProperties();

	static TCHAR BASED_CODE szFilter[] = _T("All Files(*.*)|*.*||"); // Any file filter picker because I'm unsure of file formats this may need.
	static TCHAR BASED_CODE cmoSzFilter[] = _T("CMO Files(*.cmo)|*.cmo||"); // Any file filter picker because I'm unsure of file formats this may need.

	databaseNameToProperty.insert({ "id",  new CMFCPropertyGridProperty(_T("ID"), COleVariant((long)0), _T("The objects' ID.")) });
	auto* id = databaseNameToProperty["id"];
	id->AllowEdit(false);
	propertiesGrid.AddProperty(id);

	databaseNameToProperty.insert({ "chunk_id", new CMFCPropertyGridProperty(_T("Chunk ID"), COleVariant((long)0), _T("The objects' ChunkID.")) });
	auto* chunkID = databaseNameToProperty["chunk_id"];
	chunkID->AllowEdit(false);
	propertiesGrid.AddProperty(chunkID);

	databaseNameToProperty.insert({ "ai_node", new CMFCPropertyGridProperty(_T("AI Node"), COleVariant((short)VARIANT_TRUE, VT_BOOL), _T("Whether this object is an AI node.")) });
	auto* aiNode = databaseNameToProperty["ai_node"];
	propertiesGrid.AddProperty(aiNode);

	databaseNameToProperty.insert({ "camera", new CMFCPropertyGridProperty(_T("Camera"), COleVariant((short)VARIANT_FALSE, VT_BOOL), _T("Whether the object is a camera.")) });
	auto* camera = databaseNameToProperty["camera"];
	propertiesGrid.AddProperty(camera);
	
	
	auto* renderingGroup = new CMFCPropertyGridProperty(_T("Rendering Properties"));

	databaseNameToProperty.insert({ "tex_diffuse", new CMFCPropertyGridFileProperty(_T("Texture Diffuse"), TRUE, COleVariant(_T("/../../.")), _T(".*"), 0, szFilter, _T("Texture diffuse path.")) });
	auto* texDiffuse = databaseNameToProperty["tex_diffuse"];
	renderingGroup->AddSubItem(texDiffuse);

	databaseNameToProperty.insert({ "mesh", new CMFCPropertyGridFileProperty(_T("Mesh Path"), TRUE, COleVariant(_T("/../../.")), _T(".*"), 0, cmoSzFilter, _T("Path of the mesh model. Must be a .cmo file which can be converted by using DirectX meshconvert.")) });
	auto* mesh = databaseNameToProperty["mesh"];
	renderingGroup->AddSubItem(mesh);

	databaseNameToProperty.insert({ "render", new CMFCPropertyGridProperty(_T("Render"), COleVariant((short)VARIANT_TRUE, VT_BOOL), _T("Whether the object be rendered.")) });
	auto* render = databaseNameToProperty["render"];
	renderingGroup->AddSubItem(render);
	
	propertiesGrid.AddProperty(renderingGroup);


	auto* collisionGroup = new CMFCPropertyGridProperty(_T("Collision Properties"));

	databaseNameToProperty.insert({ "collision", new CMFCPropertyGridProperty(_T("Collide"), COleVariant((short)VARIANT_TRUE, VT_BOOL), _T("Whether the object should collide with others.")) });
	auto* collision = databaseNameToProperty["collision"];
	collisionGroup->AddSubItem(collision);

	databaseNameToProperty.insert({ "collision_mesh", new CMFCPropertyGridFileProperty(_T("Collision Mesh"), TRUE, COleVariant(_T("/../../.")), _T(".*"), 0, szFilter, _T("Collision mesh path.")) });
	auto* collisionMesh = databaseNameToProperty["collision_mesh"];
	collisionGroup->AddSubItem(collisionMesh);

	databaseNameToProperty.insert({ "collectable", new CMFCPropertyGridProperty(_T("Collectable"), COleVariant((short)VARIANT_TRUE, VT_BOOL), _T("Whether the object is a collectable.")) });
	auto* collectable = databaseNameToProperty["collectable"];
	collisionGroup->AddSubItem(collectable);

	databaseNameToProperty.insert({ "destructable", new CMFCPropertyGridProperty(_T("Destructable"), COleVariant((short)VARIANT_TRUE, VT_BOOL), _T("Whether the object is destructable.")) });
	auto* destructable = databaseNameToProperty["destructable"];
	collisionGroup->AddSubItem(destructable);

	databaseNameToProperty.insert({ "health_amount", new CMFCPropertyGridProperty(_T("Health Amount"), COleVariant((long)0),_T("How much health the object has.")) });
	auto* healthAmount = databaseNameToProperty["health_amount"];
	collisionGroup->AddSubItem(healthAmount);

	propertiesGrid.AddProperty(collisionGroup);


	auto* editorGroup = new CMFCPropertyGridProperty(_T("Editor Properties"));
	
	databaseNameToProperty.insert({ "editor_render", new CMFCPropertyGridProperty(_T("Editor Render"), COleVariant((short)VARIANT_TRUE, VT_BOOL), _T("Whether to show the object in the editor.")) });
	auto* editorRender = databaseNameToProperty["editor_render"];
	editorGroup->AddSubItem(editorRender);

	databaseNameToProperty.insert({ "editor_texture_vis", new CMFCPropertyGridProperty(_T("Texture Visible in Editor"), COleVariant((short)VARIANT_TRUE, VT_BOOL), _T("Whether the texture is visible in the editor.")) });
	auto* editorTextureVis = databaseNameToProperty["editor_texture_vis"];
	editorGroup->AddSubItem(editorTextureVis);

	databaseNameToProperty.insert({ "editor_normals_vis", new CMFCPropertyGridProperty(_T("Normals Visible in Editor"), COleVariant((short)VARIANT_TRUE, VT_BOOL), _T("Whether the normals are visible in the editor.")) });
	auto* editorNormalsVis = databaseNameToProperty["editor_normals_vis"];
	editorGroup->AddSubItem(editorNormalsVis);

	databaseNameToProperty.insert({ "editor_collision_vis", new CMFCPropertyGridProperty(_T("Collisions Visible in Editor"), COleVariant((short)VARIANT_TRUE, VT_BOOL), _T("Whether the normals are visible in the editor.")) });
	auto* editorCollisionVis = databaseNameToProperty["editor_collision_vis"];
	editorGroup->AddSubItem(editorCollisionVis);

	databaseNameToProperty.insert({ "editor_pivot_vis", new CMFCPropertyGridProperty(_T("Pivot Point Visible in Editor"), COleVariant((short)VARIANT_TRUE, VT_BOOL), _T("Whether the pivot location is visible in the editor.")) });
	auto* editorPivotVis = databaseNameToProperty["editor_pivot_vis"];
	editorGroup->AddSubItem(editorPivotVis);

	databaseNameToProperty.insert({ "snap_to_ground", new CMFCPropertyGridProperty(_T("Snap To Ground"), COleVariant((short)VARIANT_TRUE, VT_BOOL), _T("Whether the placed object should snap to ground.")) });
	auto* snapToGround = databaseNameToProperty["snap_to_ground"];
	editorGroup->AddSubItem(snapToGround);
	
	databaseNameToProperty.insert({ "play_in_editor", new CMFCPropertyGridProperty(_T("Play Audio In Editor"), COleVariant((short)VARIANT_TRUE, VT_BOOL), _T("Whether the audio file shoudl play in editor.")) });
	auto* playInEditor = databaseNameToProperty["play_in_editor"];
	editorGroup->AddSubItem(playInEditor);

	databaseNameToProperty.insert({ "editor_wireframe", new CMFCPropertyGridProperty(_T("Display Wireframe in Editor"), COleVariant((short)VARIANT_TRUE, VT_BOOL), _T("Whether this object should appear as wireframe in the editor.")) });
	auto* editorWireframe = databaseNameToProperty["editor_wireframe"];
	editorGroup->AddSubItem(editorWireframe);

	propertiesGrid.AddProperty(editorGroup);
	
	
	auto* pivotGroup = new CMFCPropertyGridProperty( _T("Pivot Coordinates"));

	databaseNameToProperty.insert({ "pivot_x", new CMFCPropertyGridProperty(_T("Pivot X"), COleVariant(0.f), _T("X coordinate of the pivot value.")) });
	auto* pivotX = databaseNameToProperty["pivot_x"];
	pivotGroup->AddSubItem(pivotX);

	databaseNameToProperty.insert({ "pivot_y", new CMFCPropertyGridProperty(_T("Pivot Y"), COleVariant(0.f), _T("Y coordinate of the pivot value.")) });
	auto* pivotY = databaseNameToProperty["pivot_y"];
	pivotGroup->AddSubItem(pivotY);

	databaseNameToProperty.insert({ "pivot_z", new CMFCPropertyGridProperty(_T("Pivot Z"), COleVariant(0.f), _T("Z coordinate of the pivot value.")) });
	auto* pivotZ = databaseNameToProperty["pivot_z"];
	pivotGroup->AddSubItem(pivotZ);

	propertiesGrid.AddProperty(pivotGroup);

	
	auto* audioGroup = new CMFCPropertyGridProperty(_T("Audio Properties"));
	
	databaseNameToProperty.insert({ "audio_file", new CMFCPropertyGridFileProperty(_T("Audio File Path"), TRUE, COleVariant(_T("/../../.")), _T(".*"), 0, szFilter, _T("Path of the Audio File.")) });
	auto* audioFile = databaseNameToProperty["audio_file"];
	audioGroup->AddSubItem(audioFile);

	databaseNameToProperty.insert({ "volume", new CMFCPropertyGridProperty(_T("Volume"), COleVariant(0.f), _T("Volume of the audio file when played.")) });
	auto* volume = databaseNameToProperty["volume"];
	audioGroup->AddSubItem(volume);

	databaseNameToProperty.insert({ "pitch", new CMFCPropertyGridProperty(_T("Pitch"), COleVariant(0.f), _T("Pitch of the audio file when played.")) });
	auto* pitch = databaseNameToProperty["pitch"];
	audioGroup->AddSubItem(pitch);

	databaseNameToProperty.insert({ "pan", new CMFCPropertyGridProperty(_T("Pan"), COleVariant(0.f), _T("Pan of the left/right channel when played.")) });
	auto* pan = databaseNameToProperty["pan"];
	audioGroup->AddSubItem(pan);

	databaseNameToProperty.insert({ "one_shot", new CMFCPropertyGridProperty(_T("One Shot"), COleVariant((short)VARIANT_TRUE, VT_BOOL), _T("Whether the audio file is one shot.")) });
	auto* oneShot = databaseNameToProperty["one_shot"];
	audioGroup->AddSubItem(oneShot);

	databaseNameToProperty.insert({ "play_on_init", new CMFCPropertyGridProperty(_T("Play On Init"), COleVariant((short)VARIANT_TRUE, VT_BOOL), _T("Whether the audio file should play the sound when initialized.")) });
	auto* playOnInit = databaseNameToProperty["play_on_init"];
	audioGroup->AddSubItem(playOnInit);

	databaseNameToProperty.insert({ "min_dist", new CMFCPropertyGridProperty(_T("Minimum Distance"), COleVariant((long)0), _T("Minimum falloff distance of the sound.")) });
	auto* minDist = databaseNameToProperty["min_dist"];
	minDist->EnableSpinControl();
	audioGroup->AddSubItem(minDist);

	databaseNameToProperty.insert({ "max_dist", new CMFCPropertyGridProperty(_T("Maximum Distance"), COleVariant((long)0), _T("Maximum falloff distance of the sound.")) });
	auto* maxDist = databaseNameToProperty["max_dist"];
	maxDist->EnableSpinControl();
	audioGroup->AddSubItem(maxDist);
	
	propertiesGrid.AddProperty(audioGroup);


	auto* pathGroup = new CMFCPropertyGridProperty(_T("Path Properties"));

	databaseNameToProperty.insert({ "path_node", new CMFCPropertyGridProperty(_T("Path Node"), COleVariant((short)VARIANT_TRUE, VT_BOOL), _T("Whether the object is part of a node.")) });
	auto* pathNode = databaseNameToProperty["path_node"];
	pathGroup->AddSubItem(pathNode);
	
	databaseNameToProperty.insert({ "path_node_start", new CMFCPropertyGridProperty(_T("Path Node Start"), COleVariant((short)VARIANT_TRUE, VT_BOOL), _T("Whether the object is the start of a node.")) });
	auto* pathNodeStart = databaseNameToProperty["path_node_start"];
	pathGroup->AddSubItem(pathNodeStart);

	databaseNameToProperty.insert({ "path_node_end", new CMFCPropertyGridProperty(_T("Path Node End"), COleVariant((short)VARIANT_TRUE, VT_BOOL), _T("Whether the object is the end of a node.")) });
	auto* pathNodeEnd = databaseNameToProperty["path_node_end"];
	pathGroup->AddSubItem(pathNodeEnd);

	databaseNameToProperty.insert({ "parent_id", new CMFCPropertyGridProperty(_T("Parent ID"), COleVariant((long)0), _T("ID of the parent node.")) });
	auto* parentID = databaseNameToProperty["parent_id"];
	pathGroup->AddSubItem(parentID);

	propertiesGrid.AddProperty(pathGroup);


	auto* lightGroup = new CMFCPropertyGridProperty(_T("Light Properties"));

	databaseNameToProperty.insert({ "light_type", new CMFCPropertyGridProperty(_T("Light Type"), COleVariant((long)0), _T("Type of light this light is.")) });
	auto* lightType = databaseNameToProperty["light_type"];
	lightType->EnableSpinControl();
	lightGroup->AddSubItem(lightType);
	
	// Light diffuse + light specular handled differently! Not as three inidivudal items but rather as one!
	databaseNameToProperty.insert({ "light_diffuse", new CMFCPropertyGridColorProperty(_T("Light Diffuse"), RGB(0, 0, 0), NULL, _T("Diffuse colour of the light.")) });
	auto* lightDiffuse = databaseNameToProperty["light_diffuse"];
	lightGroup->AddSubItem(lightDiffuse);

	databaseNameToProperty.insert({ "light_specular", new CMFCPropertyGridColorProperty(_T("Light Specular"), RGB(0, 0, 0), NULL, _T("Specular colour of the light.")) });
	auto* lightSpecular = databaseNameToProperty["light_specular"];
	lightGroup->AddSubItem(lightSpecular);

	databaseNameToProperty.insert({ "light_spot_cutoff", new CMFCPropertyGridProperty(_T("Light Spot Cutoff"), COleVariant(0.f), _T("Spotlight cutoff value.")) });
	auto* lightSpotCutoff = databaseNameToProperty["light_spot_cutoff"]; // IDK what this would do. Spotlight angle?
	lightGroup->AddSubItem(lightSpotCutoff);
	
	databaseNameToProperty.insert({ "light_constant",  new CMFCPropertyGridProperty(_T("Light Constant Falloff"), COleVariant(0.f), _T("Constant falloff of the light.")) });
	auto* lightConstant = databaseNameToProperty["light_constant"];
	lightGroup->AddSubItem(lightConstant);

	databaseNameToProperty.insert({ "light_linear", new CMFCPropertyGridProperty(_T("Light Linear Falloff"), COleVariant(0.f), _T("Linear falloff of the light.")) });
	auto* lightLinear = databaseNameToProperty["light_linear"];
	lightGroup->AddSubItem(lightLinear);

	databaseNameToProperty.insert({ "light_quadratic", new CMFCPropertyGridProperty(_T("Light Quadratic Falloff"), COleVariant(0.f), _T("Quadratic falloff of the light.")) });
	auto* lightQuadratic = databaseNameToProperty["light_quadratic"];
	lightGroup->AddSubItem(lightQuadratic);
	
	propertiesGrid.AddProperty(lightGroup);

	// Fix values and properties grid being well sized.
	CRect rect;
	propertiesGrid.GetWindowRect(&rect);
	propertiesGrid.PostMessage(WM_SIZE, 0, MAKELONG(rect.Width(), rect.Height()));
}

void ObjectPropertiesDialog::InitialisePropertyToDatabaseNameLookup()
{
	ASSERT(!databaseNameToProperty.empty());
	
	for (auto& kvp : databaseNameToProperty)
	{
		propertyToDatabaseName.insert({ kvp.second, kvp.first });
	}
}

void ObjectPropertiesDialog::UpdateFieldsWithDataFromCurrentSceneObject()
{
	SetAllFieldsEnableState(false);

	if (currentSceneObject == nullptr)
		return;

	isUpdatingFieldsFromCurrentlySelectedObject = true;

	nameEdit.SetWindowTextW(StringToCString(currentSceneObject->name)); // Silly conversion byt CString takes wchar.
	
	posXEdit.SetWindowTextW(CString(std::to_wstring(currentSceneObject->posX).c_str()));
	posYEdit.SetWindowTextW(CString(std::to_wstring(currentSceneObject->posY).c_str()));
	posZEdit.SetWindowTextW(CString(std::to_wstring(currentSceneObject->posZ).c_str()));
	
	rotXEdit.SetWindowTextW(CString(std::to_wstring(currentSceneObject->rotX).c_str()));
	rotYEdit.SetWindowTextW(CString(std::to_wstring(currentSceneObject->rotY).c_str()));
	rotZEdit.SetWindowTextW(CString(std::to_wstring(currentSceneObject->rotZ).c_str()));
	
	scaleXEdit.SetWindowTextW(CString(std::to_wstring(currentSceneObject->scaX).c_str()));
	scaleYEdit.SetWindowTextW(CString(std::to_wstring(currentSceneObject->scaY).c_str()));
	scaleZEdit.SetWindowTextW(CString(std::to_wstring(currentSceneObject->scaZ).c_str()));

	// In database field order, excluding name, position, rotation, and scale.
	databaseNameToProperty["id"]->SetValue(COleVariant((long)currentSceneObject->ID));
	databaseNameToProperty["chunk_id"]->SetValue(COleVariant((long)currentSceneObject->chunk_ID));
	databaseNameToProperty["mesh"]->SetValue(COleVariant(currentSceneObject->GetModelPath()));
	databaseNameToProperty["tex_diffuse"]->SetValue(COleVariant(currentSceneObject->GetTextureDiffusePath()));
	// pos, rot, sca would go here.
	databaseNameToProperty["render"]->SetValue(COleVariant((short)BoolToVARIANTBOOL(currentSceneObject->render), VT_BOOL));
	databaseNameToProperty["collision"]->SetValue(COleVariant((short)BoolToVARIANTBOOL(currentSceneObject->collision), VT_BOOL));
	databaseNameToProperty["collision_mesh"]->SetValue(COleVariant(currentSceneObject->GetCollisionMeshPath()));
	databaseNameToProperty["collectable"]->SetValue(COleVariant((short)BoolToVARIANTBOOL(currentSceneObject->collectable), VT_BOOL));
	databaseNameToProperty["destructable"]->SetValue(COleVariant((short)BoolToVARIANTBOOL(currentSceneObject->destructable), VT_BOOL));
	databaseNameToProperty["health_amount"]->SetValue(COleVariant((long)currentSceneObject->health_amount));
	databaseNameToProperty["editor_render"]->SetValue(COleVariant((short)BoolToVARIANTBOOL(currentSceneObject->editor_render), VT_BOOL));
	databaseNameToProperty["editor_texture_vis"]->SetValue(COleVariant((short)BoolToVARIANTBOOL(currentSceneObject->editor_texture_vis), VT_BOOL));
	databaseNameToProperty["editor_normals_vis"]->SetValue(COleVariant((short)BoolToVARIANTBOOL(currentSceneObject->editor_normals_vis), VT_BOOL));
	databaseNameToProperty["editor_collision_vis"]->SetValue(COleVariant((short)BoolToVARIANTBOOL(currentSceneObject->editor_collision_vis), VT_BOOL));
	databaseNameToProperty["editor_pivot_vis"]->SetValue(COleVariant((short)BoolToVARIANTBOOL(currentSceneObject->editor_pivot_vis), VT_BOOL));
	databaseNameToProperty["pivot_x"]->SetValue(COleVariant(currentSceneObject->pivotX));
	databaseNameToProperty["pivot_y"]->SetValue(COleVariant(currentSceneObject->pivotY));
	databaseNameToProperty["pivot_z"]->SetValue(COleVariant(currentSceneObject->pivotZ));
	databaseNameToProperty["snap_to_ground"]->SetValue(COleVariant((short)BoolToVARIANTBOOL(currentSceneObject->snapToGround), VT_BOOL));
	databaseNameToProperty["ai_node"]->SetValue(COleVariant((short)BoolToVARIANTBOOL(currentSceneObject->AINode), VT_BOOL));
	databaseNameToProperty["audio_file"]->SetValue(COleVariant(currentSceneObject->GetAudioPath()));
	databaseNameToProperty["volume"]->SetValue(COleVariant(currentSceneObject->volume));
	databaseNameToProperty["pitch"]->SetValue(COleVariant(currentSceneObject->pitch));
	databaseNameToProperty["pan"]->SetValue(COleVariant(currentSceneObject->pan));
	databaseNameToProperty["one_shot"]->SetValue(COleVariant((short)BoolToVARIANTBOOL(currentSceneObject->one_shot), VT_BOOL));
	databaseNameToProperty["play_on_init"]->SetValue(COleVariant((short)BoolToVARIANTBOOL(currentSceneObject->play_on_init), VT_BOOL));
	databaseNameToProperty["play_in_editor"]->SetValue(COleVariant((short)BoolToVARIANTBOOL(currentSceneObject->play_in_editor), VT_BOOL));
	databaseNameToProperty["min_dist"]->SetValue(COleVariant((long)currentSceneObject->min_dist));
	databaseNameToProperty["max_dist"]->SetValue(COleVariant((long)currentSceneObject->max_dist));
	databaseNameToProperty["camera"]->SetValue(COleVariant((short)BoolToVARIANTBOOL(currentSceneObject->camera), VT_BOOL));
	databaseNameToProperty["path_node"]->SetValue(COleVariant((short)BoolToVARIANTBOOL(currentSceneObject->path_node), VT_BOOL));
	databaseNameToProperty["path_node_start"]->SetValue(COleVariant((short)BoolToVARIANTBOOL(currentSceneObject->path_node_start), VT_BOOL));
	databaseNameToProperty["path_node_end"]->SetValue(COleVariant((short)BoolToVARIANTBOOL(currentSceneObject->path_node_end), VT_BOOL));
	databaseNameToProperty["parent_id"]->SetValue(COleVariant((long)currentSceneObject->parent_id));
	databaseNameToProperty["editor_wireframe"]->SetValue(COleVariant((short)BoolToVARIANTBOOL(currentSceneObject->editor_wireframe), VT_BOOL));
	// name would go here
	databaseNameToProperty["light_type"]->SetValue(COleVariant((long)currentSceneObject->light_type));
	// Light diffuse + light specular handled differently!
	reinterpret_cast<CMFCPropertyGridColorProperty*>(databaseNameToProperty["light_diffuse"])->SetColor(RGB(currentSceneObject->light_diffuse_r, currentSceneObject->light_diffuse_g, currentSceneObject->light_diffuse_b));
	reinterpret_cast<CMFCPropertyGridColorProperty*>(databaseNameToProperty["light_specular"])->SetColor(RGB(currentSceneObject->light_specular_r, currentSceneObject->light_specular_g, currentSceneObject->light_specular_b));
	databaseNameToProperty["light_spot_cutoff"]->SetValue(COleVariant(currentSceneObject->light_spot_cutoff));
	databaseNameToProperty["light_constant"]->SetValue(COleVariant(currentSceneObject->light_constant));
	databaseNameToProperty["light_linear"]->SetValue(COleVariant(currentSceneObject->light_linear));
	databaseNameToProperty["light_quadratic"]->SetValue(COleVariant(currentSceneObject->light_quadratic));

	isUpdatingFieldsFromCurrentlySelectedObject = false;
	
	SetAllFieldsEnableState(true);
}

void ObjectPropertiesDialog::SetAllFieldsEnableState(bool state)
{
	nameEdit.EnableWindow(state);
	posXEdit.EnableWindow(state);
	posYEdit.EnableWindow(state);
	posZEdit.EnableWindow(state);
	rotXEdit.EnableWindow(state);
	rotYEdit.EnableWindow(state);
	rotZEdit.EnableWindow(state);
	scaleXEdit.EnableWindow(state);
	scaleYEdit.EnableWindow(state);
	scaleZEdit.EnableWindow(state);
	
	for (auto i = 0; i < propertiesGrid.GetPropertyCount(); ++i)
	{
		auto* currentProperty = propertiesGrid.GetProperty(i);

		currentProperty->Enable(state);

		for (auto j = 0; j < currentProperty->GetSubItemsCount(); j++)
		{
			currentProperty->GetSubItem(j)->Enable(state);
		}
	}
}

void ObjectPropertiesDialog::SetCurrentSceneObject(SceneObject* sceneObject)
{
	if (currentSceneObject != nullptr)
		UpdateData();
	
	this->currentSceneObject = sceneObject;

	UpdateFieldsWithDataFromCurrentSceneObject();
}

void ObjectPropertiesDialog::ClearCurrentSceneObject()
{
	SetCurrentSceneObject(nullptr);
}

afx_msg LRESULT ObjectPropertiesDialog::OnPropertiesGridPropertyUpdated(WPARAM wParam, LPARAM lParam)
{
	OutputDebugStringW(L"Property box changed!");

	// wparam = The control ID of the property list.
	// lparam = A pointer to the property (CMFCPropertyGridProperty) that changed.

	if (currentSceneObject == nullptr)
		return 0;
	
	auto* currentProperty = reinterpret_cast<CMFCPropertyGridProperty*>(lParam);

	if (propertyToDatabaseName[currentProperty] == "light_diffuse")
	{
		auto diffuseColor = reinterpret_cast<CMFCPropertyGridColorProperty*>(currentProperty)->GetColor();
		currentSceneObject->light_diffuse_r = GetRValue(diffuseColor);
		currentSceneObject->light_diffuse_g = GetGValue(diffuseColor);
		currentSceneObject->light_diffuse_b = GetBValue(diffuseColor);
	}
	else if (propertyToDatabaseName[currentProperty] == "light_specular")
	{
		auto specularColor = reinterpret_cast<CMFCPropertyGridColorProperty*>(currentProperty)->GetColor();
		currentSceneObject->light_specular_r = GetRValue(specularColor);
		currentSceneObject->light_specular_g = GetGValue(specularColor);
		currentSceneObject->light_specular_b = GetBValue(specularColor);
	}

	// In order of database, excluding name, pos, rot, scl (they are seperate fields, set in DoDataExchange()), light_diffuse, light_specular rgb (which are above).
	// Ideally this would be a switch statement but C++ is antique and doesnt allow for switch statements when looking up a dictionary.
	// File paths are converted from absolute to relative paths before being applied.
	else if (propertyToDatabaseName[currentProperty] == "mesh")
		SetPathPropertyAndUpdateCurrentSceneObjectField(reinterpret_cast<CMFCPropertyGridFileProperty*>(currentProperty), currentSceneObject->GetModelPath(), L"Invalid mesh path. Placeholder will be displayed.", L"Invalid Mesh Path");
	else if (propertyToDatabaseName[currentProperty] == "tex_diffuse")
		SetPathPropertyAndUpdateCurrentSceneObjectField(reinterpret_cast<CMFCPropertyGridFileProperty*>(currentProperty), currentSceneObject->GetTextureDiffusePath(), L"Invalid diffuse texture path. A placeholder texture will be used instead.", L"Invalid Diffuse Texture Path");
	else if (propertyToDatabaseName[currentProperty] == "render")
		currentSceneObject->render = VARIANTBOOLToBool(currentProperty->GetValue().boolVal);
	else if (propertyToDatabaseName[currentProperty] == "collision")
		currentSceneObject->collision = VARIANTBOOLToBool(currentProperty->GetValue().boolVal);
	else if (propertyToDatabaseName[currentProperty] == "collision_mesh") // Less checks on collision mesh because a file type is not specified.
		SetPathPropertyAndUpdateCurrentSceneObjectField(reinterpret_cast<CMFCPropertyGridFileProperty*>(currentProperty), currentSceneObject->GetCollisionMeshPath(), L"Invalid collision mesh path.", L"Invalid Collision Mesh Path");
	else if (propertyToDatabaseName[currentProperty] == "collectable")
		currentSceneObject->collectable = VARIANTBOOLToBool(currentProperty->GetValue().boolVal);
	else if (propertyToDatabaseName[currentProperty] == "destructable")
		currentSceneObject->destructable = VARIANTBOOLToBool(currentProperty->GetValue().boolVal);
	else if (propertyToDatabaseName[currentProperty] == "health_amount")
		currentSceneObject->health_amount = currentProperty->GetValue().intVal;
	else if (propertyToDatabaseName[currentProperty] == "editor_render")
		currentSceneObject->editor_render = VARIANTBOOLToBool(currentProperty->GetValue().boolVal);
	else if (propertyToDatabaseName[currentProperty] == "editor_texture_vis")
		currentSceneObject->editor_texture_vis = VARIANTBOOLToBool(currentProperty->GetValue().boolVal);
	else if (propertyToDatabaseName[currentProperty] == "editor_normals_vis")
		currentSceneObject->editor_normals_vis = VARIANTBOOLToBool(currentProperty->GetValue().boolVal);
	else if (propertyToDatabaseName[currentProperty] == "editor_collision_vis")
		currentSceneObject->editor_collision_vis = VARIANTBOOLToBool(currentProperty->GetValue().boolVal);
	else if (propertyToDatabaseName[currentProperty] == "editor_pivot_vis")
		currentSceneObject->editor_pivot_vis = VARIANTBOOLToBool(currentProperty->GetValue().boolVal);
	else if (propertyToDatabaseName[currentProperty] == "pivot_x")
		currentSceneObject->pivotX = currentProperty->GetValue().fltVal;
	else if (propertyToDatabaseName[currentProperty] == "pivot_y")
		currentSceneObject->pivotY = currentProperty->GetValue().fltVal;
	else if (propertyToDatabaseName[currentProperty] == "pivot_z")
		currentSceneObject->pivotZ = currentProperty->GetValue().fltVal;
	else if (propertyToDatabaseName[currentProperty] == "snap_to_ground")
		currentSceneObject->snapToGround = VARIANTBOOLToBool(currentProperty->GetValue().boolVal);
	else if (propertyToDatabaseName[currentProperty] == "ai_node")
		currentSceneObject->AINode = VARIANTBOOLToBool(currentProperty->GetValue().boolVal);
	else if (propertyToDatabaseName[currentProperty] == "audio_file")
		SetPathPropertyAndUpdateCurrentSceneObjectField(reinterpret_cast<CMFCPropertyGridFileProperty*>(currentProperty), currentSceneObject->GetAudioPath(), L"Invalid audio file path.", L"Invalid Audio File Path");
	else if (propertyToDatabaseName[currentProperty] == "volume")
		currentSceneObject->volume = currentProperty->GetValue().fltVal;
	else if (propertyToDatabaseName[currentProperty] == "pitch")
		currentSceneObject->pitch = currentProperty->GetValue().fltVal;
	else if (propertyToDatabaseName[currentProperty] == "pan")
		currentSceneObject->pan = currentProperty->GetValue().fltVal;
	else if (propertyToDatabaseName[currentProperty] == "one_shot")
		currentSceneObject->one_shot = VARIANTBOOLToBool(currentProperty->GetValue().boolVal);
	else if (propertyToDatabaseName[currentProperty] == "play_on_init")
		currentSceneObject->play_on_init = VARIANTBOOLToBool(currentProperty->GetValue().boolVal);
	else if (propertyToDatabaseName[currentProperty] == "play_in_editor")
		currentSceneObject->play_in_editor = VARIANTBOOLToBool(currentProperty->GetValue().boolVal);
	else if (propertyToDatabaseName[currentProperty] == "min_dist")
		currentSceneObject->min_dist = currentProperty->GetValue().intVal;
	else if (propertyToDatabaseName[currentProperty] == "max_dist")
		currentSceneObject->max_dist = currentProperty->GetValue().intVal;
	else if (propertyToDatabaseName[currentProperty] == "camera")
		currentSceneObject->camera = VARIANTBOOLToBool(currentProperty->GetValue().boolVal);
	else if (propertyToDatabaseName[currentProperty] == "path_node")
		currentSceneObject->path_node = VARIANTBOOLToBool(currentProperty->GetValue().boolVal);
	else if (propertyToDatabaseName[currentProperty] == "path_node_start")
		currentSceneObject->path_node_start = VARIANTBOOLToBool(currentProperty->GetValue().boolVal);
	else if (propertyToDatabaseName[currentProperty] == "path_node_end")
		currentSceneObject->path_node_end = VARIANTBOOLToBool(currentProperty->GetValue().boolVal);
	else if (propertyToDatabaseName[currentProperty] == "parent_id")
		currentSceneObject->parent_id = currentProperty->GetValue().intVal;
	else if (propertyToDatabaseName[currentProperty] == "editor_wireframe")
		currentSceneObject->editor_wireframe = VARIANTBOOLToBool(currentProperty->GetValue().boolVal);
	else if (propertyToDatabaseName[currentProperty] == "light_type")
		currentSceneObject->light_type = currentProperty->GetValue().intVal;
	else if (propertyToDatabaseName[currentProperty] == "light_spot_cutoff")
		currentSceneObject->light_spot_cutoff = currentProperty->GetValue().fltVal;
	else if (propertyToDatabaseName[currentProperty] == "light_constant")
		currentSceneObject->light_constant = currentProperty->GetValue().fltVal;
	else if (propertyToDatabaseName[currentProperty] == "light_linear")
		currentSceneObject->light_linear = currentProperty->GetValue().fltVal;
	else if (propertyToDatabaseName[currentProperty] == "light_quadratic")
		currentSceneObject->light_quadratic = currentProperty->GetValue().fltVal;

	currentSceneObject->MarkModified();
	
	return 0;
}

bool ObjectPropertiesDialog::VerifyContentsAreFloat(CEdit& field, float previousValue, float& outValue)
{
	CString contents;
	field.GetWindowTextW(contents);

	try
	{
		const float finalValue = std::stof(static_cast<LPCTSTR>(contents));
		outValue = finalValue;
		return true;
	}
	catch (...)
	{
		field.SetWindowTextW(CString(std::to_wstring(previousValue).c_str()));
		return false;
	}
}

bool ObjectPropertiesDialog::IsPathValid(std::wstring path)
{
	return std::filesystem::exists(path);
}

void ObjectPropertiesDialog::SetPathPropertyAndUpdateCurrentSceneObjectField(CMFCPropertyGridFileProperty* prop, CString& sceneObjectFileProperty,
	std::wstring warningBody, std::wstring warningTitle)
{
	auto propValue = std::wstring(prop->GetValue().bstrVal);

	if (propValue.empty())
	{
		sceneObjectFileProperty = CString(propValue.c_str());
	}
	else if (IsPathValid(ToRelativePath(propValue)))
	{
		sceneObjectFileProperty = CString(ToRelativePath(propValue).c_str());
	}
	else
	{
		MessageBox(warningBody.c_str(), warningTitle.c_str(), MB_OK | MB_ICONWARNING);
		sceneObjectFileProperty = CString(propValue.c_str());
	}

	prop->SetValue(COleVariant(sceneObjectFileProperty));
}


BEGIN_MESSAGE_MAP(ObjectPropertiesDialog, CDialogEx)
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertiesGridPropertyUpdated)
	ON_EN_CHANGE(IDC_NAMEEDIT, &ObjectPropertiesDialog::OnChangeOrUpdateNameEdit)
	ON_EN_UPDATE(IDC_NAMEEDIT, &ObjectPropertiesDialog::OnChangeOrUpdateNameEdit)
//	ON_MESSAGE(FIELD_VALUE_CONFIRM, &ObjectPropertiesDialog::OnFieldValueConfirm)
END_MESSAGE_MAP()


void ObjectPropertiesDialog::OnChangeOrUpdateNameEdit()
{
	if (currentSceneObject == nullptr || isUpdatingFieldsFromCurrentlySelectedObject)
		return;
	
	CString contents;
	nameEdit.GetWindowTextW(contents);

	currentSceneObject->name = CStringToString(contents);
}


//BOOL ObjectPropertiesDialog::PreTranslateMessage(MSG* pMsg)
//{
//	if (pMsg->message == WM_KEYDOWN)
//	{
//		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
//		{
//			return TRUE;                // Do not process further
//		}
//	}
//
//	return CDialogEx::PreTranslateMessage(pMsg);
//}


//afx_msg LRESULT ObjectPropertiesDialog::OnFieldValueConfirm(WPARAM wParam, LPARAM lParam)
//{
//	return 0;
//}
