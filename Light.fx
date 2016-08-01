struct VS_INPUT
{
	float4 Position : POSITION;
	float4 normal:NORMAL;
	float2 tex0:TEXCOORD;
};
struct VS_OUTPUT
{
	float4 Position:POSITION;
		float2 tex0:TEXCOORD0;
	float4 colorA:TEXCOORD1;
	float4 colorD:TEXCOORD2;
	float4 colorS:TEXCOORD3;
};
VS_OUTPUT vs_main(VS_INPUT input)
{
	VS_OUTPUT Output;
	Output.Position = mul(input.Position, matWorldViewProjection);
		Output.tex0 = input.tex0;
		//①     这里定义了VS的输入结构，需要说明的是“POSITION”是一个semantics(语意)。
		//	语意的作用就是标识寄存器。这行代码表示：VS_ INPUT的Position成员将从POSI TION寄存器中获得。而寄存器中的数值已经在Shader调用之前由D3D准备好了。相应的VS_ OUTPUT中的语意则表示Shader执行完毕之后运算结果将放到哪个寄存器中。这里的运算结果将在之后的阶段里在顶点之间执行线性插值，然后插值结果将被PS使用。
		//②     这里是VS主过程的声明，定义了输入输出的类型。
		//③     将顶点变换到投影空间。3D投影是十分复杂的过程，幸运的是D3D已经将这个过程做的很好，只要几行代码就可以生成正确的矩阵，并完成设置。这里的matWorldViewProjection是由应用程序在渲染过程开始之前通过ID3DXEFFECT接口设置的。它是包含了世界、观察、投影变换的矩阵。
		//④     直接将贴图坐标返回。
		//⑤     由于所有的光照运算都是在世界空间进行的，而默认情况下法线是在模型空间中的，所以需要将法线变换到世界空间中。该行将世界矩阵的位移量清0，（_41, _42, _43分别表示矩阵的第4行第1、2、3列，这正是保存位移量的地方）这是因为法线是向量而不是顶点，对向量进行位移变换的错误的。
		//⑥     这里执行了法线和光线的点乘。Normalize将向量规格化，dot执行点乘。它们都是HLSL的内置函数。
		//⑦     计算光线的反射向量。使用公式：(参考[7]：P 53 Programing VertexShaders : RacorX3)
		//R = 2 * (N.L)*N - L。式中：N——法线向量; L——光线向量; R——反射向量。

		Output.colorA = ka*lightAmbient*MaterialAmbient;
		Output.colorD = kd*lightdiffuse*(max(0, -DotNLInWorld))*MaterialDiffuse;
		float DotRV = dot(normalize(posinw - vViewPosition), Vreflect);
		Output.colorS = ks*lightspecular*pow(max(0, DotRV), ns)*max(0, -DotNLInWorld / abs(DotNLInWorld))*MaterialSpecular; ⑩
		return(Output);
}

struct PS_INPUT
{
	float2 tex0:TEXCOORD0;
	float4 colorA:TEXCOORD1;
	float4 colorD:TEXCOORD2;
	float4 colorS:TEXCOORD3;
};
float4 ps_main(PS_INPUT input) : COLOR0
{
	float4 finalcolor;
float4 diffusecolor = tex2D(DiffuseSampler,input.tex0);
finalcolor = ((input.colorA + input.colorD)*diffusecolor + input.colorS)*lightpower;
return finalcolor;
}
