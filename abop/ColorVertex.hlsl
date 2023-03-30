//////////////////////////////////////////////////////
// Color.vs  - Color Vertex Shader
//////////////////////////////////////////////////////

// GLOBALS
cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer CameraBuffer
{
    float3 cameraPosition;
    float padding;
};

// TYPEDEFS
struct VertexInputType
{
    float4 position : POSITION;
    float4 color : COLOR;
    float3 normal : NORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float3 normal : NORMAL;
    float3 viewDirection : TEXCOORD1;
};

// Vertex Shader
PixelInputType ColorVertexShader(VertexInputType input)
{
    PixelInputType output;

    // 적절한 행렬 계산을 위해 위치 벡터는 4 단위로 수정
    input.position.w = 1.0f;

    // 월드, 뷰, 투영 행렬에 대한 정점의 위치를 계산
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // 월드 행렬에 대한 법선 벡터 계산
    output.normal = mul(input.normal, (float3x3)worldMatrix);
    output.normal = normalize(output.normal);

    float4 worldPosition = mul(input.position, worldMatrix);
    
    // 월드 행렬 기준으로 카메라 방향 계산
    output.viewDirection = cameraPosition.xyz - worldPosition;
    output.viewDirection = normalize(output.viewDirection);

    output.color = input.color;

    return output;
}