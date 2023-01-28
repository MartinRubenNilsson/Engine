#pragma once

class ConstantBuffer
{
public:
	ConstantBuffer(UINT aByteWidth);

	operator bool() const { return myBuffer; }

private:
	ComPtr<ID3D11Buffer> myBuffer;
};

