#pragma once


class Graphics;

class BufferResource
{
public:
	virtual ~BufferResource() = default;
	virtual void BindAsBuffer(Graphics&) noxnd = 0;
	virtual void BindAsBuffer(Graphics&, BufferResource*) noxnd = 0;
	virtual void Clear(Graphics&) noxnd = 0;
};