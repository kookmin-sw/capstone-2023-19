#pragma once

class TextureShaderClass : public AlignedAllocationPolicy<16>
{
private:
    struct MatrixBufferType
    {
        DirectX::XMMATRIX world;
        DirectX::XMMATRIX view;;
        DirectX::XMMATRIX projection;
    };

public:
    TextureShaderClass();
    TextureShaderClass(const TextureShaderClass&);
    ~TextureShaderClass();

    bool Initialize(ID3D11Device*, HWND);
    void Shutdown();
    // World, View, Proejction matrix로 렌더링
    bool Render(ID3D11DeviceContext*, int, DirectX::XMMATRIX, DirectX::XMMATRIX, DirectX::XMMATRIX, ID3D11ShaderResourceView*);

private:
    // WCHAR* shader 경로
    bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
    void ShutdownShader();
    void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

    bool SetShaderParameters(ID3D11DeviceContext*, DirectX::XMMATRIX, DirectX::XMMATRIX, DirectX::XMMATRIX, ID3D11ShaderResourceView*);
    void RenderShader(ID3D11DeviceContext*, int);

private:
    ID3D11VertexShader* vertexShader_ = nullptr;
    ID3D11PixelShader* pixelShader_ = nullptr;
    ID3D11InputLayout* layout_ = nullptr;
    ID3D11Buffer* matrixBuffer_ = nullptr;

    ID3D11SamplerState* sampleState_ = nullptr;
};