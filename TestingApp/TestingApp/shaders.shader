cbuffer LightBuffer
{
    float4 directionalLightVector;
    float4 directionalLightColor;
    float4 ambientLightColor;
}

cbuffer MatricesBuffer
{
    float4x4 finalTransformationMatrix;
    float4x4 rotationMatrix;
}

Texture2D Texture;
SamplerState samplerState;

struct VOut
{
    float4 color : COLOR;
    float2 textureCoordinates : TEXCOORD;
    float4 position : SV_POSITION;
};

VOut VShader(float4 position : POSITION, float4 normal : NORMAL, float2 textureCoordinates : TEXCOORD)
{
    VOut output;
    output.position = mul(finalTransformationMatrix, position);
    output.color = ambientLightColor;
    float4 rotatedNormal = normalize(mul(rotationMatrix, normal));
    float directionalBrightness = saturate(dot(rotatedNormal, directionalLightVector));
    output.color += directionalLightColor * directionalBrightness;
    output.textureCoordinates = textureCoordinates;
    return output;
}

float4 PShader(float4 color : COLOR, float2 textureCoordinates : TEXCOORD) : SV_TARGET
{
    float4 texturedColor = color * Texture.Sample(samplerState, textureCoordinates);
    return texturedColor;
}