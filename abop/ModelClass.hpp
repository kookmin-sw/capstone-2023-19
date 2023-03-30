#pragma once

class TextureClass;

class ModelClass : public AlignedAllocationPolicy<16>
{
protected:
    struct VertexType
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT2 texture;
        DirectX::XMFLOAT3 normal;
        DirectX::XMFLOAT4 color;
    };

    struct ModelType
    {
        float x, y, z;
        float tu, tv;
        float nx, ny, nz;
    };

public:
    ModelClass();
    ModelClass(const ModelClass&);
    ~ModelClass();

    // Custom model
    bool Initialize(ID3D11Device*, Model);
    // Texture
    bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*, char*);
    void Shutdown();
    void Render(ID3D11DeviceContext*);

    void SetPosition(const float&, const float&, const float&);
    void SetPosition(const Vector3&);
    void SetRotation(const float&, const float&, const float&);
    void SetRotation(const Vector3&);
    void SetQuaternion(const float&, const float&, const float&, const float&);
    void SetScale(const float&, const float&, const float&);
    void SetScale(const Vector3&);

    void SetColor(const float& r, const float& g, const float& b, const float& a);

    int GetIndexCount();
    Vector3 GetPosition();
    Vector3 GetRotation();
    DirectX::XMFLOAT4 GetQuaternion();

    ID3D11ShaderResourceView* GetTexture();

protected:
    // ModelClass에서 재정의
    bool InitializeBuffers(ID3D11Device*);
    bool InitializeBuffers(ID3D11Device*, Model);
    void ShutdownBuffers();
    void RenderBuffers(ID3D11DeviceContext*);

    bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*);
    void ReleaseTexture();

    bool LoadModel(char*);
    void ReleaseModel();

protected:
    ID3D11Buffer* vertexBuffer_ = nullptr;
    ID3D11Buffer* indexBuffer_ = nullptr;
    int vertexCount_ = 0;
    int indexCount_ = 0;

    Vector3 position_ = { 0.0f, 0.0f, 0.0f };
    Vector3 rotation_ = { 0.0f, 0.0f, 0.0f };
    Vector3 scale_ = { 1.0f, 1.0f, 1.0f };
    DirectX::XMFLOAT4 quaternion_ = { 0.0f, 0.0f, 0.0f, 1.0f };
    DirectX::XMFLOAT4 color_ = { 0.0f, 0.0f, 0.0f, 0.0f };

    TextureClass* texture_ = nullptr;
    ModelType* model_ = nullptr;
};