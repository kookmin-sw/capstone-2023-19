#include "Stdafx.h"
#include "LightShaderClass.hpp"

LightShaderClass::LightShaderClass()
{

}

LightShaderClass::LightShaderClass(const LightShaderClass& other)
{

}

LightShaderClass::~LightShaderClass()
{

}

bool LightShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
    WCHAR vs[] = L"./Light.vs";
    WCHAR ps[] = L"./Light.ps";

    return this->InitializeShader(device, hwnd, vs, ps);
}

void LightShaderClass::Shutdown()
{
    this->ShutdownShader();
}

bool LightShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount,
    DirectX::XMMATRIX worldMatrix, DirectX::XMMATRIX viewMatrix, DirectX::XMMATRIX projectionMatrix,
    ID3D11ShaderResourceView* texture, DirectX::XMFLOAT3 lightDirection, DirectX::XMFLOAT4 ambientColor,
    DirectX::XMFLOAT4 diffuseColor)
{
    if (!this->SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix,
        texture, lightDirection, ambientColor, diffuseColor))
    {
        // 렌더링에 사용될 매개변수 설정
        return false;
    }

    this->RenderShader(deviceContext, indexCount);

    return true;
}

bool LightShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd,
    WCHAR* vsFilename, WCHAR* psFilename)
{
    HRESULT result;
    ID3D10Blob* errorMessage = nullptr;

    ID3D10Blob* vertexShaderBuffer = nullptr;
    result = D3DCompileFromFile(vsFilename, NULL, NULL, "LightVertexShader",
        "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
    if (FAILED(result))
    {
        if (errorMessage)
        {
            // VertexShader 컴파일 실패시 오류 메시지 출력
            this->OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
        }
        else
        {
            // vsFilename이 잘못된 경우
            MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
        }

        return false;
    }

    ID3D10Blob* pixelShaderBuffer = nullptr;
    result = D3DCompileFromFile(psFilename, NULL, NULL, "LightPixelShader",
        "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
    if (FAILED(result))
    {
        if (errorMessage)
        {
            // PixelShader 컴파일 실패시 오류 메시지 출력
            this->OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
        }
        else
        {
            // psFilename이 잘못된 경우
            MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
        }

        return false;
    }

    // 버퍼로부터 셰이더 생성
    result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
        vertexShaderBuffer->GetBufferSize(), NULL, &(this->vertexShader_));
    if (FAILED(result))
    {
        return false;
    }

    result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(),
        pixelShaderBuffer->GetBufferSize(), NULL, &(this->pixelShader_));
    if (FAILED(result))
    {
        return false;
    }

    // 정점 입력 레이아웃 구조체 설정
    // ModelClass와 셰이더 VertexType 구조와 일치해야 함
    D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
    polygonLayout[0].SemanticName = "POSITION";
    polygonLayout[0].SemanticIndex = 0;
    polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[0].InputSlot = 0;
    polygonLayout[0].AlignedByteOffset = 0;
    polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[0].InstanceDataStepRate = 0;

    polygonLayout[1].SemanticName = "TEXCOORD";
    polygonLayout[1].SemanticIndex = 0;
    polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    polygonLayout[1].InputSlot = 0;
    polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[1].InstanceDataStepRate = 0;

    polygonLayout[2].SemanticName = "NORMAL";
    polygonLayout[2].SemanticIndex = 0;
    polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[2].InputSlot = 0;
    polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[2].InstanceDataStepRate = 0;

    unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

    // Input Layout 생성
    result = device->CreateInputLayout(polygonLayout, numElements,
        vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &(this->layout_));
    if (FAILED(result))
    {
        return false;
    }

    // 셰이더 버퍼 해제
    vertexShaderBuffer->Release();
    vertexShaderBuffer = nullptr;
    pixelShaderBuffer->Release();
    pixelShaderBuffer = nullptr;

    // Texture sampler 구조체 작성
    D3D11_SAMPLER_DESC samplerDesc;
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.BorderColor[0] = 0;
    samplerDesc.BorderColor[1] = 0;
    samplerDesc.BorderColor[2] = 0;
    samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    result = device->CreateSamplerState(&samplerDesc, &(this->sampleState_));
    if (FAILED(result))
    {
        return false;
    }

    // Vertex Shader의 행렬 상수 버퍼의 구조체 작성
    D3D11_BUFFER_DESC matrixBufferDesc;
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;

    // 버퍼 포인터 생성
    result = device->CreateBuffer(&matrixBufferDesc, NULL, &(this->matrixBuffer_));
    if (FAILED(result))
    {
        return false;
    }

    // Pixel Shader에 있는 광원 동적 상수 버퍼 description 설정
    // D3D11_BIND_CONSTANT_BUFFER를 사용하면 ByteWidth는 항상 16의 배수여야 함
    D3D11_BUFFER_DESC lightBufferDesc;
    lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    lightBufferDesc.ByteWidth = sizeof(LightBufferType);
    lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    lightBufferDesc.MiscFlags = 0;
    lightBufferDesc.StructureByteStride = 0;
    
    result = device->CreateBuffer(&lightBufferDesc, NULL, &(this->lightBuffer_));
    if (FAILED(result))
    {
        return false;
    }

    return true;
}

void LightShaderClass::ShutdownShader()
{
    if (this->lightBuffer_)
    {
        this->lightBuffer_->Release();
        this->lightBuffer_ = nullptr;
    }

    if (this->matrixBuffer_)
    {
        this->matrixBuffer_->Release();
        this->matrixBuffer_ = nullptr;
    }

    if (this->sampleState_)
    {
        this->sampleState_->Release();
        this->sampleState_ = nullptr;
    }

    if (this->layout_)
    {
        this->layout_->Release();
        this->layout_ = nullptr;
    }

    if (this->pixelShader_)
    {
        this->pixelShader_->Release();
        this->pixelShader_ = nullptr;
    }

    if (this->vertexShader_)
    {
        this->vertexShader_->Release();
        this->vertexShader_ = nullptr;
    }
}

void LightShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
    OutputDebugStringA(reinterpret_cast<const char*>(errorMessage->GetBufferPointer()));

    errorMessage->Release();
    errorMessage = nullptr;

    MessageBox(hwnd, L"Error compiling shader.", shaderFilename, MB_OK);
}

bool LightShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext,
    DirectX::XMMATRIX worldMatrix, DirectX::XMMATRIX viewMatrix, DirectX::XMMATRIX projectionMatrix,
    ID3D11ShaderResourceView* texture, DirectX::XMFLOAT3 lightDirection, DirectX::XMFLOAT4 ambientColor,
    DirectX::XMFLOAT4 diffuseColor)
{
    // 셰이더에서 사용할 수 있도록 matrix를 transpose
    worldMatrix = XMMatrixTranspose(worldMatrix);
    viewMatrix = XMMatrixTranspose(viewMatrix);
    projectionMatrix = XMMatrixTranspose(projectionMatrix);

    // 상수 버퍼 내용을 쓸 수 있도록 수정
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    if (FAILED(deviceContext->Map(this->matrixBuffer_, 0, D3D11_MAP_WRITE_DISCARD,
        0, &mappedResource)))
    {
        return false;
    }

    MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;

    // 상수 버퍼로 데이터 전달
    dataPtr->world = worldMatrix;
    dataPtr->view = viewMatrix;
    dataPtr->projection = projectionMatrix;

    deviceContext->Unmap(this->matrixBuffer_, 0);

    // Vertex Shader의 상수 버퍼 위치 설정
    unsigned int bufferNumber = 0;
    // Vertex Shader의 상수 버퍼를 바뀐 값으로 변경
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &(this->matrixBuffer_));
    // Pixel Shader에서 ShaderTexture 리소스 설정
    deviceContext->PSSetShaderResources(0, 1, &texture);

    //lighterBuffer를 작성할 수 있도록 수정
    if (FAILED(deviceContext->Map(this->lightBuffer_, 0, D3D11_MAP_WRITE_DISCARD,
        0, &mappedResource)))
    {
        return false;
    }

    LightBufferType* dataPtr2 = (LightBufferType*)mappedResource.pData;

    // light 변수를 상수 버퍼에 복사
    dataPtr2->ambientColor = ambientColor;
    dataPtr2->diffuseColor = diffuseColor;
    dataPtr2->lightDirection = lightDirection;
    dataPtr2->padding = 0.0f;

    deviceContext->Unmap(this->lightBuffer_, 0);

    bufferNumber = 0;

    deviceContext->PSSetConstantBuffers(bufferNumber, 1, &(this->lightBuffer_));
    
    return true;
}

void LightShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
    // Input Layout 설정
    deviceContext->IASetInputLayout(this->layout_);

    // Shader 설정
    deviceContext->VSSetShader(this->vertexShader_, NULL, 0);
    deviceContext->PSSetShader(this->pixelShader_, NULL, 0);

    // Pixel Shader에서 샘플러 상태 설정
    deviceContext->PSSetSamplers(0, 1, &this->sampleState_);

    deviceContext->DrawIndexed(indexCount, 0, 0);
}

