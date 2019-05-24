#pragma once
#include "Systems/IExecute.h" //인터페이스

class Main : public IExecute
{
public:
	//컨트롤 . 해서 IExecute 에 있는 모든 순수 가상함수 끌어오기
	// IExecute을(를) 통해 상속됨
	virtual void Initialize() override;
	virtual void Ready() override;
	virtual void Destroy() override;
	virtual void Update() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;
	virtual void ResizeScreen() override;

private:
	void Push(IExecute* exceute);

private:
	vector<IExecute *> executes;


};