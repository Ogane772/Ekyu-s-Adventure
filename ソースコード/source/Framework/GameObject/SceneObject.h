#pragma once
#include <Windows.h>
#include <memory>
#include <string>
enum class ESoundIndex;
enum class ETextureType;
struct ID3D11ShaderResourceView;
class CPlayer;
class CCamera;
class CMouseCursor;
class CScene;

class CSceneObject
{
public:
	virtual ~CSceneObject() {};

	virtual void Init() {};
	virtual void UnInit() {};
	virtual void Update(float DeltaTime) {};
	virtual void Draw() {};
	virtual void ShadowDraw() {};//影を付けるモデルの描画
	//オブジェクトのUpdate、Draw前に呼び出される共通処理
	virtual void PreUpdate(float DeltaTime) { Update(DeltaTime); };
	virtual void PreDraw() { Draw(); }
	virtual void PreShadowDraw() { ShadowDraw(); };

	void SetDestroy() { m_IsDestroy = true; }
	//エディタモード中のオブジェクト削除処理
	virtual void SetDestroyEdit() { m_IsDestroy = true; }
	bool Destroy();
	UINT PlaySoundIndex(const ESoundIndex SoundIndex);
	void PlaySoundID(const UINT Id);
	void StopSoundIndex(const ESoundIndex SoundIndex);
	void StopSoundID(const UINT Id);
	void DeleteSoundIndex(const ESoundIndex SoundIndex);
	void DeleteSoundID(const UINT Id);
	void ReplayBGM();
	void StopBGM();
	void DeleteBGM();
	void AllSoundStop();
	void AllSoundReplay();

	//テクスチャの読み込み
	std::weak_ptr<ID3D11ShaderResourceView> LoadTexture(const ETextureType TextureType);
	std::weak_ptr<ID3D11ShaderResourceView> LoadTexture(const std::string& FileName);
	//テクスチャの解放
	void ReleaseTexture(std::weak_ptr<ID3D11ShaderResourceView> Texture);

protected:
	bool m_IsDestroy = false;

	CPlayer* GetPlayer() const;
	CCamera* GetCamera() const;
	CMouseCursor* GetMouseCursor() const;
	std::weak_ptr<CScene> GetScene() const;
	bool IsEditMode() const;
	void SetEvent(const bool Type);
	bool IsEvent() const;
};