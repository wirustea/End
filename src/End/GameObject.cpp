
#include "GameObject.h"
#include "d3dUtil.h"
#include "DXTrace.h"
using namespace DirectX;

struct InstancedData
{
	XMMATRIX world;
	XMMATRIX worldInvTranspose;
};

GameObject::GameObject()
	: m_Capacity(),
	m_WorldMatrix(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f),
	m_Up(0.0f, 1.0f, 0.0f),
	m_Right(1.0f, 0.0f, 0.0f),
	m_Front(0.0f, 0.0f, 1.0f),
	m_Position(0.0f, 0.0f, 0.0f)
{
}

void GameObject::SetY(float y)
{
	m_Position.y = y;
}

XMFLOAT3 GameObject::GetPosition() const
{
	return m_Position;
}

XMFLOAT3 GameObject::GetUp() const
{
	return m_Up;
}

XMFLOAT3 GameObject::GetRight() const
{
	return m_Right;
}

XMFLOAT3 GameObject::GetLookAt() const
{
	return m_Front;
}

BoundingBox GameObject::GetBoundingBox() const
{
	BoundingBox box;
	m_Model.boundingBox.Transform(box, XMLoadFloat4x4(&m_WorldMatrix));
	return box;
}



BoundingOrientedBox GameObject::GetBoundingOrientedBox() const
{
	BoundingOrientedBox box;
	BoundingOrientedBox::CreateFromBoundingBox(box, m_Model.boundingBox);
	box.Transform(box, XMLoadFloat4x4(&m_WorldMatrix));
	return box;
}


BoundingBox GameObject::GetLocalBoundingBox() const
{
	return m_Model.boundingBox;
}

size_t GameObject::GetCapacity() const
{
	return m_Capacity;
}

void GameObject::ResizeBuffer(ID3D11Device * device, size_t count)
{
	// 设置实例缓冲区描述
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = (UINT)count * sizeof(InstancedData);
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	// 创建实例缓冲区
	HR(device->CreateBuffer(&vbd, nullptr, m_pInstancedBuffer.ReleaseAndGetAddressOf()));

	// 重新调整m_Capacity
	m_Capacity = count;
}




void GameObject::SetModel(Model && model)
{
	std::swap(m_Model, model);
	model.modelParts.clear();
	model.boundingBox = BoundingBox();
}

void GameObject::SetModel(const Model & model)
{
	m_Model = model;
}

void GameObject::SetWorldMatrix(const XMFLOAT4X4 & world)
{
	m_WorldMatrix = world;
}

void XM_CALLCONV GameObject::SetWorldMatrix(FXMMATRIX world)
{
	XMStoreFloat4x4(&m_WorldMatrix, world);
}

void GameObject::UpdateWorldMatrix()
{
	GameObject::m_WorldMatrix = {
	GameObject::m_Right.x, GameObject::m_Right.y, GameObject::m_Right.z, 0.0f,
	GameObject::m_Up.x, GameObject::m_Up.y, GameObject::m_Up.z, 0.0f,
	GameObject::m_Front.x, GameObject::m_Front.y, GameObject::m_Front.z, 0.0f,
	GameObject::m_Position.x, GameObject::m_Position.y, GameObject::m_Position.z, 1.0f
	};
}


void GameObject::RotateY(float rad)
{
	XMMATRIX R = XMMatrixRotationY(rad);

	XMStoreFloat3(&m_Right, XMVector3TransformNormal(XMLoadFloat3(&m_Right), R));
	XMStoreFloat3(&m_Up, XMVector3TransformNormal(XMLoadFloat3(&m_Up), R));
	XMStoreFloat3(&m_Front, XMVector3TransformNormal(XMLoadFloat3(&m_Front), R));
}

void GameObject::MoveForward(float d)
{
	XMVECTOR Pos = XMLoadFloat3(&m_Position);
	XMVECTOR Look = XMLoadFloat3(&m_Front);
	XMVECTOR Dist = XMVectorReplicate(d);
	// DestPos = Dist * Look + SrcPos
	XMStoreFloat3(&m_Position, XMVectorMultiplyAdd(Dist, Look, Pos));
}

void GameObject::Pitch(float rad)
{
	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&m_Right), rad);
	XMVECTOR Up = XMVector3TransformNormal(XMLoadFloat3(&m_Up), R);
	XMVECTOR Look = XMVector3TransformNormal(XMLoadFloat3(&m_Front), R);
	float cosPhi = XMVectorGetY(Look);
	// 将上下视野角度Phi限制在[2pi/9, 7pi/9]，
	// 即余弦值[-cos(2pi/9), cos(2pi/9)]之间
	if (fabs(cosPhi) > cosf(XM_2PI / 9))
		return;

	XMStoreFloat3(&m_Up, Up);
	XMStoreFloat3(&m_Front, Look);
}

void GameObject::Draw(ID3D11DeviceContext * deviceContext, BasicEffect & effect)
{
	UINT strides = m_Model.vertexStride;
	UINT offsets = 0;

	for (auto& part : m_Model.modelParts)
	{
		// 设置顶点/索引缓冲区
		deviceContext->IASetVertexBuffers(0, 1, part.vertexBuffer.GetAddressOf(), &strides, &offsets);
		deviceContext->IASetIndexBuffer(part.indexBuffer.Get(), part.indexFormat, 0);

		// 更新数据并应用
		effect.SetWorldMatrix(XMLoadFloat4x4(&m_WorldMatrix));
		effect.SetTextureDiffuse(part.texDiffuse.Get());
		effect.SetMaterial(part.material);

		effect.Apply(deviceContext);

		deviceContext->DrawIndexed(part.indexCount, 0, 0);
	}
}

void GameObject::Draw(ID3D11DeviceContext * deviceContext, DepthRenderEffect& effect, XMFLOAT4X4 view, XMFLOAT4X4 proj)
{
	UINT strides = m_Model.vertexStride;
	UINT offsets = 0;

	for (auto& part : m_Model.modelParts)
	{
		// 设置顶点/索引缓冲区
		deviceContext->IASetVertexBuffers(0, 1, part.vertexBuffer.GetAddressOf(), &strides, &offsets);
		deviceContext->IASetIndexBuffer(part.indexBuffer.Get(), part.indexFormat, 0);

		// 更新数据并应用
		effect.SetWorldViewProjMatrix(XMLoadFloat4x4(&m_WorldMatrix), XMLoadFloat4x4(&view), XMLoadFloat4x4(&proj));

		effect.Apply(deviceContext);

		deviceContext->DrawIndexed(part.indexCount, 0, 0);
	}
}


void GameObject::DrawInstanced(ID3D11DeviceContext * deviceContext, BasicEffect & effect, const std::vector<DirectX::XMMATRIX>& data)
{
	D3D11_MAPPED_SUBRESOURCE mappedData;
	UINT numInsts = (UINT)data.size();
	// 若传入的数据比实例缓冲区还大，需要重新分配
	if (numInsts > m_Capacity)
	{
		ComPtr<ID3D11Device> device;
		deviceContext->GetDevice(device.GetAddressOf());
		ResizeBuffer(device.Get(), numInsts);
	}

	HR(deviceContext->Map(m_pInstancedBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));

	InstancedData * iter = reinterpret_cast<InstancedData *>(mappedData.pData);
	for (auto& mat : data)
	{
		iter->world = XMMatrixTranspose(mat);
		iter->worldInvTranspose = XMMatrixInverse(nullptr, mat);	// 两次转置抵消
		iter++;
	}

	deviceContext->Unmap(m_pInstancedBuffer.Get(), 0);

	UINT strides[2] = { m_Model.vertexStride, sizeof(InstancedData) };
	UINT offsets[2] = { 0, 0 };
	ID3D11Buffer * buffers[2] = { nullptr, m_pInstancedBuffer.Get() };
	for (auto& part : m_Model.modelParts)
	{
		buffers[0] = part.vertexBuffer.Get();

		// 设置顶点/索引缓冲区
		deviceContext->IASetVertexBuffers(0, 2, buffers, strides, offsets);
		deviceContext->IASetIndexBuffer(part.indexBuffer.Get(), part.indexFormat, 0);

		// 更新数据并应用
		effect.SetTextureDiffuse(part.texDiffuse.Get());
		effect.SetMaterial(part.material);
		effect.Apply(deviceContext);

		deviceContext->DrawIndexedInstanced(part.indexCount, numInsts, 0, 0, 0);
	}
}

void GameObject::SetDebugObjectName(const std::string& name)
{
#if (defined(DEBUG) || defined(_DEBUG)) && (GRAPHICS_DEBUGGER_OBJECT_NAME)

	m_Model.SetDebugObjectName(name);
	std::string instBufferName = name + ".InstancedBuffer";
	if (m_pInstancedBuffer)
	{
		m_pInstancedBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(instBufferName.length()), instBufferName.c_str());
	}

#endif
}


