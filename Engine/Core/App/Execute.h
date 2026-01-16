#pragma once

class IExecute abstract
{
public:
	virtual ~IExecute() = default;

public:
	virtual void Create() = 0;
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void End() = 0;
};
