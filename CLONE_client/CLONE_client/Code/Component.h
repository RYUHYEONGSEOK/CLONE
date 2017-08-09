#pragma once

class CComponent
{
public:
	virtual void			Update()	PURE;
private:
	virtual void			Release()	PURE;

public:
	explicit CComponent();
	virtual ~CComponent();
};
