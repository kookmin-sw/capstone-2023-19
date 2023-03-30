#pragma once

class ColorShaderClass : public AlignedAllocationPolicy<16>
{
private:
    struct MatrixBufferType
    {
        DirectX::XMMATRIX world;
        DirectX::XMMATRIX view;;
        DirectX::XMMATRIX projection;
    };

    class CameraBufferType
    {
    public:
        DirectX::XMFLOAT3 cameraPosition;
        float padding;
    };

    struct LightBufferType
    {
        DirectX::XMFLOAT4 ambientColor;
        DirectX::XMFLOAT4 diffuseColor;
        DirectX::XMFLOAT3 lightDirection;
        float specularPower;
        DirectX::XMFLOAT4 specularColor;
    };

public:
    ColorShaderClass();
    ColorShaderClass(const ColorShaderClass& other);
    ~ColorShaderClass();

    bool Initialize(ID3D11Device* device, HWND hwnd);
    void Shutdown();
    // World, View, Proejction matrix로 렌더링
    bool Render(ID3D11DeviceContext* deviceContext, int indexCount, 
                DirectX::XMMATRIX worldMatrix, 
                DirectX::XMMATRIX viewMatrix, 
                DirectX::XMMATRIX projectionMatrix,
                DirectX::XMFLOAT3 lightDirection,
                DirectX::XMFLOAT4 ambientColor,
                DirectX::XMFLOAT4 diffuseColor,
                DirectX::XMFLOAT3 cameraPosition,
                DirectX::XMFLOAT4 specularColor,
                float specularPower);

private:
    // WCHAR* shader 경로
    bool InitializeShader(ID3D11Device* device, HWND hwnd, 
                          WCHAR* vsFilename, WCHAR* psFilename);
    void ShutdownShader();
    void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename);

    bool SetShaderParameters(ID3D11DeviceContext* deviceContext, 
                             DirectX::XMMATRIX worldMatrix, 
                             DirectX::XMMATRIX viewMatrix,
                             DirectX::XMMATRIX projectionMAtrix,
                             DirectX::XMFLOAT3 lightDirection,
                             DirectX::XMFLOAT4 ambientColor,
                             DirectX::XMFLOAT4 diffuseColor,
                             DirectX::XMFLOAT3 cameraPosition,
                             DirectX::XMFLOAT4 specularColor,
                             float specularPower);
    void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);

private:
    ID3D11VertexShader* vertexShader_ = nullptr;
    ID3D11PixelShader* pixelShader_ = nullptr;
    ID3D11InputLayout* layout_ = nullptr;

    ID3D11Buffer* matrixBuffer_ = nullptr;
    ID3D11Buffer* cameraBuffer_ = nullptr;
    ID3D11Buffer* lightBuffer_ = nullptr;
};