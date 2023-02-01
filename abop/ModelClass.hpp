#pragma once

class ModelClass : public AlignedAllocationPolicy<16>
{
protected:
    struct VertexType
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT4 color;
    };

public:
    ModelClass();
    ModelClass(const ModelClass&);
    ~ModelClass();

    bool Initialize(ID3D11Device*, Model);
    void Shutdown();
    void Render(ID3D11DeviceContext*);

    void SetPosition(const float&, const float&, const float&);
    void SetPosition(const Vector3&);
    void SetRotation(const float&, const float&, const float&);
    void SetRotation(const Vector3&);
    void SetScale(const float&, const float&, const float&);
    void SetScale(const Vector3&);

    int GetIndexCount();
    Vector3 GetPosition();
    Vector3 GetRotation();

protected:
    // ModelClass에서 재정의
    bool InitializeBuffers(ID3D11Device*);
    bool InitializeBuffers(ID3D11Device*, Model);
    void ShutdownBuffers();
    void RenderBuffers(ID3D11DeviceContext*);

protected:
    ID3D11Buffer* vertexBuffer_ = nullptr;
    ID3D11Buffer* indexBuffer_ = nullptr;
    int vertexCount_ = 0;
    int indexCount_ = 0;

    Vector3 position_ = { 0.0f, 0.0f, 0.0f };
    Vector3 rotation_ = { 0.0f, 0.0f, 0.0f };
    Vector3 scale_ = { 1.0f, 1.0f, 1.0f };
    DirectX::XMFLOAT4 color_ = { 0.0f, 0.0f, 0.0f, 0.0f };
};