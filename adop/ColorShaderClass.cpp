#include "Stdafx.h"
#include "ColorShaderClass.hpp"

ColorShaderClass::ColorShaderClass()
{

}

ColorShaderClass::ColorShaderClass(const ColorShaderClass&)
{

}

ColorShaderClass::~ColorShaderClass()
{

}

bool ColorShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
    return this->InitializeShader(device, hwnd, L"./Color.vs", L"./Color.ps");
}

void ColorShaderClass::Shutdown()
{
    this->ShutdownShader();
}

bool ColorShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, 
    XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
    if (!this->SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix))
    {
        // 렌더링에 사용될 매개변수 설정
        return false;
    }

    this->RenderShader(deviceContext, indexCount);

    return true;
}

bool ColorShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd,
    WCHAR* vsFilename, WCHAR* psFilename)
{
    ID3D10Blob* errorMessage = nullptr;

    ID3D10Blob* vertexShaderBuffer = nullptr;
    if (FAILED(D3DCompileFromFiile(VsFIlename, NULL, NULL, "ColorVertexShader", 
        "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage)))
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
    if (FAILED(D3DCompileFromFile(psFilename, NULL, NULL, "ColorPixelShader",
        "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage)))
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
    if (FAILED(device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
        vertexShaderBuffer->GetBufferSize(), NULL, this->vertexShader_)))
    {
        return false;
    }
    if (FAILED(device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(),
        pixelShaderBuffer->GetBufferSize(), NULL, this->vertexShader_)))
    {
        return false;   
    }

    // 정점 입력 레이아웃 구조체 설정
    // !!! ModelClass와 셰이더 VertexType 구조와 일치해야 함
    D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
    polygonLayout[0].SemanticName = "POSITION";
    polygonLayout[0].SemanticIndex = 0;
    polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[0].InputSlot = 0;
    polygonLayout[0].AlignedByteOffset = 0;
    polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[0].InstanceDataStepRate = 0;

    polygonLayout[1].SemanticName = "COLOR";
    polygonLayout[1].SemanticIndex = 0;
    polygonLayout[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[1].InputSlot = 0;
    polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[1].InstanceDataStepRate = 0;
    
    unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

    // Input Layout 생성
    if (FAILED(device->CreateInputLayout(polygonLayout, numElements,
        vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &(this->layout_))))
    {
        return false;
    }

    // 셰이더 버퍼 해제
    vertexShaderBuffer->Release();
    vertexShaderBuffer = nullptr;
    pixelShaderBuffer->Release();
    pixelShaderBuffer = nullptr;

    // Vertex Shader의 행렬 상수 버퍼의 구조체 작성
    D3D11_BUFFER_DESC matrixBufferDesc;
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;

    // 버퍼 포인터 생성
    if (FAILED(device->CreateBuffer(&matrixBufferDesc, NULL, &(this->matrixBuffer_))))
    {
        return false;
    }

    return true;
}

void ColorShaderClass::ShutdownShader()
{
    if (this->matrixBuffer_)
    {
        this->matrixBuffer_->Release();
        this->matrixBuffer_ = nullptr;
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

void ColorShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
    OutputDebugStringA(reinterpret_cast<const char*>(errorMessage->GetBufferPointer()));

    errorMessage->Release();
    errorMessage = nullptr;

    MessageBox(hwnd, L"Error compiling shader.", shaderFilename, MB_OK);
}

bool ColorShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, 
    XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
    // 셰이더에서 사용할 수 있도록 matrix를 transpose
    worldMatrix = XMMatrixTranspose(worldMatrix);
    viewMatrix = XMMatrixTranspose(viewMatrix);
    projectionMatrix = XMMatrixTranspose(projectionMatrix);

    // 상수 버퍼 내용을 쓸 수 있도록 수정
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    if (FAILED(deviceContext->Map(this->matrixBuffer_, 0,
         D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
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
    unsigned bufferNumber = 0;
    // Vertex Shader의 상수 버퍼를 바뀐 값으로 변경
    deviceContext->VSSetConstantBufferrs(bufferNumber, 1, &(this->matrixBuffer_));

    return true;
}

void ColorShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
    // Input Layout 설정
    deviceContext->IASetInputLayout(this->layout_);

    // Shader 설정
    deviceContext->VSSetShader(this->vertexShader_, NULL, 0);
    deviceContext->PSSetShader(this->pixelShader_, NULL, 0);

    deviceContext->DrawIndexed(indexCount, 0, 0);
}

