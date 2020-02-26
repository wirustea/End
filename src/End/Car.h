//***************************************************************************************
// ����ģ��������Ч��
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
	 * @brief ǰ�����˶�
	 * @param dt(float) ֡ʱ�䣬��λ��
	 */
	void MoveForward(float dt);						

	/**
	 * @brief ת��
	 * @param rad(float) ת���Ƕ�
	 */
	void TurnTo(float rad);									

	/**
	* @brief ���٣����¼��ٶ�
	*/
	void SpeedUp();	

	/**
	* @brief ���ü��ٶ�
	*/
	void ReSetACC();	

	/**
	 * @brief ɲ��/������٣������ٶ���Ϊ������
	 */
	void brake();
	
public:

	/**
	 * @brief Ѱ�ҵ�ǰ�㼯�У�������ֵ��С��, min(P.y-L(P.x, P.z).y)
	 * @param WheelVex(vector<XMFLOAT3>)    // <in>  ��ά�㼯
	 * @param height(float&)				// <out> ��������С��ֵ���������ʾ�ڵ������£�
	 * @param pos(XMFLOAT3&)				// <out> ��С��ֵ�������
	 * @param grad(XMFLOAT3&)				// <out> ��С��ֵ�㴦�ڵ������꣨P.x,, L(P.x,P.z).y, P.z�����ķ�����
	 */
	void GetMinPoint(std::vector<DirectX::XMFLOAT3>& WheelVex, float& height, DirectX::XMFLOAT3& pos, DirectX::XMFLOAT3& grad);


	/**
	 * @brief �س���ǰ��ڶ��˶�
	 * @param dt(float) ֡ʱ�� ��λ��
	 */
	void TangentMove(float dt);										


	/**
	 * @brief �س���ǰ��ڶ��˶�
	 * @param rad(float) �Ƕ�
	 */
	void BodySwing(float rad);

	/**
	 * @brief ��̥�����ٶ�ת��
	 */
	void WheelRotate();
										
	/**
	 * @brief ���ݵ�ǰ���ٶȸ����ٶȣ���½����ʼ�տ���Ħ����
	 * @param dt(float) ֡ʱ�� ��λ��
	 */
	void UpdateSpeed(float dt);	

	/**
	 * @brief ���ص�ǰ�ٶ�
	 * @return double   �ٶ�
	 */
	double GetSpeed() const;

	/**
	 * @brief ����Effect��������ý��л���
	 * @param deviceContext(ID3D11DeviceContext *)  �豸������
	 * @param effect(BasicEffect &)                 Effect����
	 */
	void Draw(ID3D11DeviceContext * deviceContext, BasicEffect & effect);

	/**
	 * @brief ���� Depth Effect ��������ý������ֵ��ȡ
	 * @param deviceContext(ID3D11DeviceContext *)  �豸������
	 * @param effect(DepthRenderEffect &)           Depth Effect����
	 * @param view(XMFLOAT4X4)						���ֵ��ȡ�ӽǵ���ͼ����
	 * @param proj(XMFLOAT4X4)						���ֵ��ȡ�ӽǵ�ͶӰ����
	 */
	void Draw(ID3D11DeviceContext * deviceContext, DepthRenderEffect& effect, DirectX::XMFLOAT4X4 view, DirectX::XMFLOAT4X4 proj);

	/**
	 * @brief ��ȡλ��������y��Ϊ���Ϸ���Z������Ϊ����������
	 * @return XMFLOAT3
	 */
	DirectX::XMFLOAT3 GetPosition() const;

	/**
	 * @brief ��ȡ�������ĵ�λ��������y��Ϊ���Ϸ���Z������Ϊ����������
	 * @return XMFLOAT3
	 */
	DirectX::XMFLOAT3 GetActualPosition() const;

	/**
	 * @brief XZ����ƽ�ƣ�y��Ϊ���Ϸ���Z������Ϊ����������
	 * @param x(double) x ����
	 * @param z(double) z ����
	 */
	void MoveXZ(double x,double z);

	/**
	 * @brief ��ȡ����������y��Ϊ���Ϸ���Z������Ϊ����������
	 * @return XMFLOAT3
	 */
	DirectX::XMFLOAT3 GetFront() const;

	/**
	 * @brief ��ȡ�����Ϸ���y��Ϊ���Ϸ���Z������Ϊ����������
	 * @return XMFLOAT3
	 */
	DirectX::XMFLOAT3 GetUp() const;

	/**
	 * @brief ��ȡ������೵�����ĵ���� @m_Position ��ƫ�ƣ�y��Ϊ���Ϸ���Z������Ϊ����������
	 * @return XMFLOAT3
	 */
	DirectX::XMFLOAT3 GetLeftAxialCenterBias() const;

	/**
	 * @brief ��ȡ�����Ҳ೵�����ĵ���� @m_Position ��ƫ�ƣ�y��Ϊ���Ϸ���Z������Ϊ����������
	 * @return XMFLOAT3
	 */
	DirectX::XMFLOAT3 GetRightAxialCenterBias() const;

	/**
	 * @brief ��ȡ�����������y��Ϊ���Ϸ���Z������Ϊ����������
	 * @return XMMATRIX
	 */
	DirectX::XMMATRIX GetWorldMatrixXM() const;

	/**
	 * @brief ��ȡ������ͼ����y��Ϊ���Ϸ���Z������Ϊ����������
	 * @return XMFLOAT4X4
	 */
	DirectX::XMFLOAT4X4 GetViewMatrix() const;

	/**
	 * @brief ��ȡ������ͼ����y��Ϊ���Ϸ���Z������Ϊ����������
	 * @return XMMATRIX
	 */
	DirectX::XMMATRIX GetViewMatrixXM() const;

	/**
	 * @brief ��ȡ����ͶӰ����y��Ϊ���Ϸ���Z������Ϊ����������
	 * @return XMFLOAT4X4
	 */
	DirectX::XMFLOAT4X4 GetProjMatrix() const;

	/**
	 * @brief ��ȡ����ͶӰ����y��Ϊ���Ϸ���Z������Ϊ����������
	 * @return XMMATRIX
	 */
	DirectX::XMMATRIX GetProjMatrixXM() const;

	/**
	 * @brief ���ɳ�����ͼ����y��Ϊ���Ϸ���Z������Ϊ����������
	 */
	void GenerateViewMatrix();

	/**
	 * @brief ���ɳ���ͶӰ����y��Ϊ���Ϸ���Z������Ϊ����������
	 * @param fovY(float)		�ӳ�
	 * @param aspect(float)		��߱�
	 * @param nearZ(float)		����
	 * @param farZ(float)		Զ��
	 */
	void GenerateProjMatrix(float fovY, float aspect, float nearZ, float farZ);

	/**
	 * @brief ��ȡ���ƶ��󣨼���ͼ�Σ�
	 * @return GameObject&
	 */
	GameObject& GetCarLightObj();

	/**
	 * @brief ��ȡ���ƶ��󣨵ƹ⣩
	 * @return CarLight&
	 */
	CarLight& GetLights(bool left);

	void UpdateWorldMatrix();

private:
	//
	// ģ�����
	//
	GameObject m_leftWheel;				//����
	GameObject m_rightWheel;			//�ҳ���
	GameObject m_body;					//����
	GameObject m_light;					//���ƣ������壩

	//
	// �ƹ�
	//
	CarLight m_leftLight;				//�󳵵�
	CarLight m_rightLight;				//�ҳ���
	DirectX::XMVECTOR m_leftLightPos;	//�󳵵�λ��
	DirectX::XMVECTOR m_rightLightPos;	//�ҳ���λ��

	//
	// ½��
	//
	Land* m_TerrainPtr;					//½��

	//
	// ����
	//
	DirectX::XMFLOAT4X4 m_WorldMatrix;	
	DirectX::XMFLOAT4X4 m_ViewMatrix;
	DirectX::XMFLOAT4X4 m_ProjMatrix;

	//
	// ����
	//
	DirectX::XMFLOAT3 m_Position;
	DirectX::XMFLOAT3 m_ActualPositionBias;
	DirectX::XMFLOAT3 m_Right;
	DirectX::XMFLOAT3 m_Up;
	DirectX::XMFLOAT3 m_Front;

	//
	// �����Թ�ϵ����
	//

	//����
	DirectX::XMFLOAT3 m_leftAxialCenterBias;
	DirectX::XMFLOAT3 m_rightAxialCenterBias;

	//λ�ƾ�������λ�ƾ���
	DirectX::XMMATRIX m_leftTransMat;
	DirectX::XMMATRIX m_invLeftTransMat;

	DirectX::XMMATRIX m_rightTransMat;
	DirectX::XMMATRIX m_invRightTransMat;

	DirectX::XMMATRIX m_swingTransMat;
	DirectX::XMMATRIX m_invSwingTransMat;

	std::vector<DirectX::XMFLOAT3> m_leftWheelVex;		// ��೵�ֶ��㼯
	std::vector<DirectX::XMFLOAT3> m_rightWheelVex;		// �Ҳ೵�ֶ��㼯

	//
	// �˶�
	//
	float m_ACCFRICTION;	// Ħ�������ٶ�
	float m_ACC;			// ��ǰ�ٶ�
	float m_ACCAIR;			// ���м��ٶ�
	float m_ACCLAND;		// ������ٶ�
	float m_ACCG;			// �������ٶ�
	float m_V=0.0f;         // ��ǰ�ٶ�
	float m_GV = 0.0f;      // ��ǰ�����ٶ�
	STATE m_State;			// ״̬������/���棩

	float m_leftWheelAngle; // ����̥��ת�Ƕ�
	float m_rightWheelAngle;// ����̥��ת�Ƕ�

	DirectX::XMFLOAT3 m_leftWheelDirection;  // ����Ԥ���ƶ�����
	DirectX::XMFLOAT3 m_rightWheelDirection; // ����Ԥ���ƶ�����

	float m_MaxV;							 // ����ٶ�
	float m_MinV;							 // ��С�ٶ�
	DirectX::XMFLOAT3 m_leftWheelNorm;		 // ������������д��ķ�����
	DirectX::XMFLOAT3 m_rightWheelNorm;		 // �ҳ�����������д��ķ�����

	float m_axialLength;					 // ���᳤��
};

#endif