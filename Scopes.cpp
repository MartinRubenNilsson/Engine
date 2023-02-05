#include "pch.h"
#include "Scopes.h"

/*
* class ScopedPrimitiveTopology
*/

ScopedPrimitiveTopology::ScopedPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY aNewTopology)
{
	DX11_CONTEXT->IAGetPrimitiveTopology(&myOldTopology);
	DX11_CONTEXT->IASetPrimitiveTopology(aNewTopology);
}

ScopedPrimitiveTopology::~ScopedPrimitiveTopology()
{
	DX11_CONTEXT->IASetPrimitiveTopology(myOldTopology);
}
