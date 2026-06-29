#pragma once
//テクスチャの読み込みや管理を行うクラス
#include <d3d11.h>
#include <string>
#include <memory>
#include <unordered_map>
#include "../Singleton/Singleton.h"
#include "TextureTypes.h"

class CTextureLoader : public CSingleton<CTextureLoader>
{
private:
	//UIやエフェクト等のテクスチャタイプに紐づくパスリスト
	std::unordered_map<ETextureType, std::string> m_TexturePathDataList;
	//テクスチャパスをキーとしたロードしたテクスチャのリスト
	std::unordered_map<std::string, TextureData> m_TextureLoadDataList;
	//生成したテクスチャリソースをキーとしたテクスチャパスのリスト
	std::unordered_map<ID3D11ShaderResourceView*, std::string> m_TextureResourceDataList;

public:
	CTextureLoader();
	~CTextureLoader();

	std::weak_ptr<ID3D11ShaderResourceView> Load(const ETextureType TextureType);
	std::weak_ptr<ID3D11ShaderResourceView> Load(const std::string& FileName);

	void Release(std::weak_ptr<ID3D11ShaderResourceView>& Texture);
	void Release(ID3D11ShaderResourceView* Texture);

	UINT GetWidth(const ETextureType TextureType) const;
	UINT GetHeight(const ETextureType TextureType) const;
};