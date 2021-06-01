#pragma once
#include <afxdialogex.h>

#include <afxpropertygridctrl.h>
#include <unordered_map>


#include "SceneObject.h"
#include "CFloatEdit.h"


// ObjectPropertiesDialog dialog

class ObjectPropertiesDialog : public CDialogEx, public IEventReciever
{
	DECLARE_DYNAMIC(ObjectPropertiesDialog)

public:
	ObjectPropertiesDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~ObjectPropertiesDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OBJECTPROPERTIESDIALOG };
#endif

protected:
	class ToolMain* toolMain;

	CMFCPropertyGridCtrl propertiesGrid;
	
	CEdit nameEdit;
	CFloatEdit posXEdit;
	CFloatEdit posYEdit;
	CFloatEdit posZEdit;
	CFloatEdit rotXEdit;
	CFloatEdit rotYEdit;
	CFloatEdit rotZEdit;
	CFloatEdit scaleXEdit;
	CFloatEdit scaleYEdit;
	CFloatEdit scaleZEdit;

	bool isUpdatingFieldsFromCurrentlySelectedObject;

	std::unordered_map<std::string, CMFCPropertyGridProperty*> databaseNameToProperty;
	std::unordered_map<CMFCPropertyGridProperty*, std::string> propertyToDatabaseName;

	SceneObject* currentSceneObject;


	/// <summary>
	/// When event is recieved, call DoDataExchange() to update curentSceneObject with values in the edit fields.
	/// </summary>
	virtual void OnRecievedEvent() override;
	
	/// <summary>
	/// Update fields/variables they represent based on respective contents.
	/// </summary>
	/// <param name="pDX"></param>
	virtual void DoDataExchange(CDataExchange* pDX) override;

	virtual BOOL OnInitDialog() override;

	/// <summary>
	/// Setup event this object to receive updates when float edits are updated.
	/// </summary>
	void InitialiseFloatEdits();
	
	/// <summary>
	/// Add properties to propertiesGrid based on elements of SceneObject.
	/// </summary>
	void InitialisePropertyGrid();

	/// <summary>
	/// Initialise propertyToDatabaseName lookup. Must be called after populating databaseNameToProperty via InitialisePropertyGrid().
	/// </summary>
	void InitialisePropertyToDatabaseNameLookup();

	/// <summary>
	/// Update the data of the editable fields with information from the current scene object.
	/// </summary>
	void UpdateFieldsWithDataFromCurrentSceneObject();

	/// <summary>
	/// Enable/disable of all fields.
	/// </summary>
	/// <param name="state"></param>
	void SetAllFieldsEnableState(bool state);

	/// <summary>
	/// Verify that contents are the type double, if they are not the contents of field to previousValue.
	/// </summary>
	/// <param name="field">The field to check.</param>
	/// <param name="previousValue">The value of the field before applying.</param>
	/// <returns>True if a double; false if not.</returns>
	bool VerifyContentsAreFloat(CEdit& field, float previousValue, float& outValue);

	/// <summary>
	/// Check whether a path is valid.
	/// </summary>
	/// <param name="path"></param>
	/// <returns>True if the path exists; false if it doesn't.</returns>
	bool IsPathValid(std::wstring path);

	/// <summary>
	/// Set the value of a path property, and update a CString value that is a part of the scene object file.
	/// It will also display an error if the value is empty and the path is invalid. The path will also be converted
	/// to be relative if it is valid.
	/// </summary>
	/// <param name="prop">Path property.</param>
	/// <param name="sceneObjectFileProperty">The property to update with the converted value.</param>
	/// <param name="warningBody">The warning MessageBox's body text.</param>
	/// <param name="warningTitle">The warning MessageBox's title.</param>
	void SetPathPropertyAndUpdateCurrentSceneObjectField(CMFCPropertyGridFileProperty* prop, CString& sceneObjectFileProperty, std::wstring warningBody, std::wstring warningTitle);

public:

	/// <summary>
	/// Setup reference to ToolMain, must be set up otherwise picking won't work.
	/// </summary>
	/// <param name="toolMain"></param>
	void SetReferenceToToolMain(class ToolMain* toolMain)
	{
		this->toolMain = toolMain;
	}

	/// <summary>
	/// Update the scene object's properties that should be displayed by the ObjectPropertiesDialog.
	/// </summary>
	/// <param name="sceneObject"></param>
	void SetCurrentSceneObject(SceneObject* sceneObject);

	/// <summary>
	/// Clear the current selection and update the properties box to be disabled.
	/// </summary>
	void ClearCurrentSceneObject();

	DECLARE_MESSAGE_MAP()

	afx_msg void OnChangeOrUpdateNameEdit();
	
	/// <summary>
	/// Event callback for when propertiesGrid property is updated. Called by MFC. Updates the changes onto the object that is being modified.
	/// </summary>
	/// <param name="wParam">The control ID of the property list.</param>
	/// <param name="lParam">A pointer to the property (CMFCPropertyGridProperty) that changed.</param>
	afx_msg LRESULT OnPropertiesGridPropertyUpdated(WPARAM wParam, LPARAM lParam);
};
