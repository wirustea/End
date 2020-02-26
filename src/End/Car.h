//***************************************************************************************
// 车的模型与物理效果
//***************************************************************************************


#ifndef CAR_H
#define CAR_H

#include <vector>
#include <cmath>
#include "GameObject.h"
#include "Geometry.h"
#include "terrain.h"
#include <limits.h>
#include "Land.h"
#include "params.h"

enum STATE{AIR,LAND};

class Car
{
public:
	Car();
	void init(ID3D11Device * device, Land *terrainPtr = nullptr);

	/**
	 * @brief 前后向运动
	 * @param dt(float) 帧时间，单位秒
	 */
	void MoveForward(float dt);						

	/**
	 * @brief 转向
	 * @param rad(float) 转动角度
	 */
	void TurnTo(float rad);									

	/**
	* @brief 加速，更新加速度
	*/
	void SpeedUp();	

	/**
	* @brief 重置加速度
	*/
	void ReSetACC();	

	/**
	 * @brief 刹车/后向加速，将加速度设为负方向
	 */
	void brake();
	
public:

	/**
	 * @brief 寻找当前点集中，与地面差值最小点, min(P.y-L(P.x, P.z).y)
	 * @param WheelVex(vector<XMFLOAT3>)    // <in>  三维点集
	 * @param height(float&)				// <out> 与地面的最小差值（负数则表示在地面以下）
	 * @param pos(XMFLOAT3&)				// <out> 最小差值点的坐标
	 * @param grad(XMFLOAT3&)				// <out> 最小差值点处在地面坐标（P.x,, L(P.x,P.z).y, P.z）处的法向量
	 */
	void GetMinPoint(std::vector<DirectX::XMFLOAT3>& WheelVex, float& height, DirectX::XMFLOAT3& pos, DirectX::XMFLOAT3& grad);


	/**
	 * @brief 沿车轴前后摆动运动
	 * @param dt(float) 帧时间 单位秒
	 */
	void TangentMove(float dt);										


	/**
	 * @brief 沿车轴前后摆动运动
	 * @param rad(float) 角度
	 */
	void BodySwing(float rad);

	/**
	 * @brief 轮胎根据速度转动
	 */
	void WheelRotate();
										
	/**
	 * @brief 根据当前加速度更新速度，在陆地是始终考虑摩擦力
	 * @param dt(float) 帧时间 单位秒
	 */
	void UpdateSpeed(float dt);	

	/**
	 * @brief 返回当前速度
	 * @return double   速度
	 */
	double GetSpeed() const;

	/**
	 * @brief 根据Effect对象的设置进行绘制
	 * @param deviceContext(ID3D11DeviceContext *)  设备上下文
	 * @param effect(BasicEffect &)                 Effect对象
	 */
	void Draw(ID3D11DeviceContext * deviceContext, BasicEffect & effect);

	/**
	 * @brief 根据 Depth Effect 对象的设置进行深度值获取
	 * @param deviceContext(ID3D11DeviceContext *)  设备上下文
	 * @param effect(DepthRenderEffect &)           Depth Effect对象
	 * @param view(XMFLOAT4X4)						深度值获取视角的视图矩阵
	 * @param proj(XMFLOAT4X4)						深度值获取视角的投影矩阵
	 */
	void Draw(ID3D11DeviceContext * deviceContext, DepthRenderEffect& effect, DirectX::XMFLOAT4X4 view, DirectX::XMFLOAT4X4 proj);

	/**
	 * @brief 获取位置向量，y轴为向上方向，Z负半轴为车体正方向
	 * @return XMFLOAT3
	 */
	DirectX::XMFLOAT3 GetPosition() const;

	/**
	 * @brief 获取车轴中心点位置向量，y轴为向上方向，Z负半轴为车体正方向
	 * @return XMFLOAT3
	 */
	DirectX::XMFLOAT3 GetActualPosition() const;

	/**
	 * @brief XZ方向平移（y轴为向上方向，Z负半轴为车体正方向）
	 * @param x(double) x 坐标
	 * @param z(double) z 坐标
	 */
	void MoveXZ(double x,double z);

	/**
	 * @brief 获取车体正方向（y轴为向上方向，Z负半轴为车体正方向）
	 * @return XMFLOAT3
	 */
	DirectX::XMFLOAT3 GetFront() const;

	/**
	 * @brief 获取车体上方向（y轴为向上方向，Z负半轴为车体正方向）
	 * @return XMFLOAT3
	 */
	DirectX::XMFLOAT3 GetUp() const;

	/**
	 * @brief 获取车体左侧车轮中心点距离 @m_Position 的偏移（y轴为向上方向，Z负半轴为车体正方向）
	 * @return XMFLOAT3
	 */
	DirectX::XMFLOAT3 GetLeftAxialCenterBias() const;

	/**
	 * @brief 获取车体右侧车轮中心点距离 @m_Position 的偏移（y轴为向上方向，Z负半轴为车体正方向）
	 * @return XMFLOAT3
	 */
	DirectX::XMFLOAT3 GetRightAxialCenterBias() const;

	/**
	 * @brief 获取车体世界矩阵（y轴为向上方向，Z负半轴为车体正方向）
	 * @return XMMATRIX
	 */
	DirectX::XMMATRIX GetWorldMatrixXM() const;

	/**
	 * @brief 获取车体视图矩阵（y轴为向上方向，Z负半轴为车体正方向）
	 * @return XMFLOAT4X4
	 */
	DirectX::XMFLOAT4X4 GetViewMatrix() const;

	/**
	 * @brief 获取车体视图矩阵（y轴为向上方向，Z负半轴为车体正方向）
	 * @return XMMATRIX
	 */
	DirectX::XMMATRIX GetViewMatrixXM() const;

	/**
	 * @brief 获取车体投影矩阵（y轴为向上方向，Z负半轴为车体正方向）
	 * @return XMFLOAT4X4
	 */
	DirectX::XMFLOAT4X4 GetProjMatrix() const;

	/**
	 * @brief 获取车体投影矩阵（y轴为向上方向，Z负半轴为车体正方向）
	 * @return XMMATRIX
	 */
	DirectX::XMMATRIX GetProjMatrixXM() const;

	/**
	 * @brief 生成车体视图矩阵（y轴为向上方向，Z负半轴为车体正方向）
	 */
	void GenerateViewMatrix();

	/**
	 * @brief 生成车体投影矩阵（y轴为向上方向，Z负半轴为车体正方向）
	 * @param fovY(float)		视场
	 * @param aspect(float)		宽高比
	 * @param nearZ(float)		近端
	 * @param farZ(float)		远端
	 */
	void GenerateProjMatrix(float fovY, float aspect, float nearZ, float farZ);

	/**
	 * @brief 获取车灯对象（几何图形）
	 * @return GameObject&
	 */
	GameObject& GetCarLightObj();

	/**
	 * @brief 获取车灯对象（灯光）
	 * @return CarLight&
	 */
	CarLight& GetLights(bool left);

	void UpdateWorldMatrix();

private:
	//
	// 模型零件
	//
	GameObject m_leftWheel;				//左车轮
	GameObject m_rightWheel;			//右车轮
	GameObject m_body;					//车体
	GameObject m_light;					//车灯（几何体）

	//
	// 灯光
	//
	CarLight m_leftLight;				//左车灯
	CarLight m_rightLight;				//右车灯
	DirectX::XMVECTOR m_leftLightPos;	//左车灯位置
	DirectX::XMVECTOR m_rightLightPos;	//右车灯位置

	//
	// 陆地
	//
	Land* m_TerrainPtr;					//陆地

	//
	// 矩阵
	//
	DirectX::XMFLOAT4X4 m_WorldMatrix;	
	DirectX::XMFLOAT4X4 m_ViewMatrix;
	DirectX::XMFLOAT4X4 m_ProjMatrix;

	//
	// 向量
	//
	DirectX::XMFLOAT3 m_Position;
	DirectX::XMFLOAT3 m_ActualPositionBias;
	DirectX::XMFLOAT3 m_Right;
	DirectX::XMFLOAT3 m_Up;
	DirectX::XMFLOAT3 m_Front;

	//
	// 零件相对关系向量
	//

	//车轮
	DirectX::XMFLOAT3 m_leftAxialCenterBias;
	DirectX::XMFLOAT3 m_rightAxialCenterBias;

	//位移矩阵与逆位移矩阵
	DirectX::XMMATRIX m_leftTransMat;
	DirectX::XMMATRIX m_invLeftTransMat;

	DirectX::XMMATRIX m_rightTransMat;
	DirectX::XMMATRIX m_invRightTransMat;

	DirectX::XMMATRIX m_swingTransMat;
	DirectX::XMMATRIX m_invSwingTransMat;

	std::vector<DirectX::XMFLOAT3> m_leftWheelVex;		// 左侧车轮顶点集
	std::vector<DirectX::XMFLOAT3> m_rightWheelVex;		// 右侧车轮顶点集

	//
	// 运动
	//
	float m_ACCFRICTION;	// 摩擦力加速度
	float m_ACC;			// 当前速度
	float m_ACCAIR;			// 空中加速度
	float m_ACCLAND;		// 地面加速度
	float m_ACCG;			// 重力加速度
	float m_V=0.0f;         // 当前速度
	float m_GV = 0.0f;      // 当前纵向速度
	STATE m_State;			// 状态（空中/地面）

	float m_leftWheelAngle; // 左轮胎旋转角度
	float m_rightWheelAngle;// 右轮胎旋转角度

	DirectX::XMFLOAT3 m_leftWheelDirection;  // 左轮预计移动方向
	DirectX::XMFLOAT3 m_rightWheelDirection; // 右轮预计移动方向

	float m_MaxV;							 // 最大速度
	float m_MinV;							 // 最小速度
	DirectX::XMFLOAT3 m_leftWheelNorm;		 // 左车轮与地面相切处的法向量
	DirectX::XMFLOAT3 m_rightWheelNorm;		 // 右车轮与地面相切处的法向量

	float m_axialLength;					 // 车轴长度
};

#endif