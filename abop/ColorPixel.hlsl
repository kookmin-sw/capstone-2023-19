//////////////////////////////////////////////////////
// Color.ps  - Color Pixel Shader
//////////////////////////////////////////////////////

cbuffer LightBuffer
{
    float4 ambientColor;
    float4 diffuseColor;
    float3 lightDirection;
    float specularPower;
    float4 specularColor;
};

// TYPEDEFS
struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float3 normal : NORMAL;
    float3 viewDirection : TEXCOORD1;
};

// Pixel Shader
float4 ColorPixelShader(PixelInputType input) : SV_TARGET
{
    float4 outputColor;

    float3 lightDir;
    float lightIntensity;
    float4 specular;
    float reflection;

    // 주변광 설정
    outputColor = input.color * ambientColor;
    
    // 난반사광
    lightDir = -lightDirection;
    lightIntensity = saturate(dot(input.normal, lightDir));

    if (lightIntensity > 0.0f)
    {
        outputColor += (diffuseColor * lightIntensity);
        outputColor = saturate(outputColor);

        // 빛의 강도, 법선 벡터 및 빛의 방향에 따라 반사 벡터 계산
        reflection = normalize(2 * lightIntensity * input.normal - lightDir);

        specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower);
    }

    outputColor = saturate(outputColor + specular);

    // !!! DISABLE
    return input.color;
}