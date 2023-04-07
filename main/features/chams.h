
enum EPlayerMaterials : int {
	MAT_DEFAULT,
	MAT_FLAT,
	MAT_PULSE,
	MAT_METALLIC,
	MAT_GLOW,
	MAT_MAX
};

class IMaterial;
namespace TModelChams {
	void Initialize();
	IMaterial* CreateMaterial(std::string, std::string, std::string = ("vgui/white"), std::string = "", bool = false, bool = false, std::string_view = "");
	IMaterial* CreateOverlay(int num);
	bool OnRenderModel( DrawModelResults_t* pResults, const DrawModelInfo_t& info, matrix3x4_t* pBoneToWorld, float* flFlexWeights, float* flFlexDelayedWeights, const Vector3D& vecModelOrigin, int nFlags);
	
	inline bool bMaterialInitialized = false;
	inline IMaterial* matMaterials[MAT_MAX] = {};
	inline IMaterial* matMaterialsDME[MAT_MAX] = {};
}