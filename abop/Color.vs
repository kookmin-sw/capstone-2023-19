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

// TYPEDEFS
struct VertexInputType
{
    float4 position : POSITION;
    float4 color : COLOR;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
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

    output.color = input.color;

    return output;
}