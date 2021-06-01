#pragma once

#include <afx.h>
#include <afxstr.h>
#include <string>


//This object should accurately and totally reflect the information stored in the object table


class SceneObject
{
	bool isModified;

	CString model_path;
	CString tex_diffuse_path;
	CString collision_mesh;
	CString audio_path;

public:
	int ID;
	int chunk_ID;
	float posX, posY, posZ;
	float rotX, rotY, rotZ;
	float scaX, scaY, scaZ;
	bool render, collision;
	bool collectable, destructable;
	int health_amount;
	bool editor_render, editor_texture_vis;
	bool editor_normals_vis, editor_collision_vis, editor_pivot_vis;
	float pivotX, pivotY, pivotZ;
	bool snapToGround;
	bool AINode;
	float volume;
	float pitch;
	float pan;
	bool one_shot;
	bool play_on_init;
	bool play_in_editor;
	int min_dist;
	int max_dist;
	bool camera;
	bool path_node;
	bool path_node_start;
	bool path_node_end;
	int parent_id;
	bool editor_wireframe;
	std::string name;
	int light_type;
	float light_diffuse_r, light_diffuse_g, light_diffuse_b;
	float light_specular_r, light_specular_g, light_specular_b;
	float light_spot_cutoff;
	float light_constant;
	float light_linear;
	float light_quadratic;


	SceneObject();
	~SceneObject();

	
	CString& GetModelPath() { return model_path; }
	CString& GetTextureDiffusePath() { return tex_diffuse_path; }
	CString& GetCollisionMeshPath() { return collision_mesh; }
	CString& GetAudioPath() { return audio_path; }
	std::string GetMeshPathStr() const;
	std::string GetTextureDiffusePathStr() const;
	std::string GetCollisionMeshPathStr() const;
	std::string GetAudioPathStr() const;

	void SetModelPath(CString mp) { model_path = mp; }
	void SetTextureDiffusePath(CString tdp) { tex_diffuse_path = tdp; }
	void SetCollisionMeshPath(CString cmp) { collision_mesh = cmp; }
	void SetAudioPath(CString ap) { audio_path = ap; }
	
	/// <summary>
	/// Check whether the object is modified and must be redrawn.
	/// </summary>
	/// <returns>true if modified; false if not.</returns>
	bool IsModified() { return isModified; }
	
	/// <summary>
	/// Mark the object as modified, therefore needing updating for the renderer.
	/// </summary>
	void MarkModified();

	/// <summary>
	/// Unmark as modified - no longer needing to be updated.
	/// </summary>
	void UnmarkModified();
};

