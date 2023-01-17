#pragma once

class ModelClass : public AlignedAllocationPolicy<16>
{
private:
    struct VertexType
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT4 color;
    };
public:
    ModelClass();
    ModelClass(const ModelClass&);
    ~ModelClass();

    bool Initialize(ID3D11Device*, int, float*, int);
    void Shutdown();
    void Render(ID3D11DeviceContext*);

    int GetIndexCount();

private:
    bool InitializeBuffers(ID3D11Device*);      // not used
    bool InitializeBuffers(ID3D11Device*, int, float*, int);
    void ShutdownBuffers();
    void RenderBuffers(ID3D11DeviceContext*);

private:
    ID3D11Buffer* vertexBuffer_ = nullptr;
    ID3D11Buffer* indexBuffer_ = nullptr;
    int vertexCount_ = 0;
    int indexCount_ = 0;
};