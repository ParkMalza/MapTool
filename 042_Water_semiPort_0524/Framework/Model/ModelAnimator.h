#pragma once
#define MAX_MODEL_KEYFRAMES 250

class ModelAnimator
{
public:
	ModelAnimator(Shader* shader, Model* model);
	~ModelAnimator();

	void Update();
	void Render();

	void Pass(UINT val);
	int Clip(UINT instance) { return tweens[instance].Curr.Clip; }
	void Speed(int val, float speed) {this-> speed[val] = speed; }
	float Speed(int val) { return speed[val]; }

	UINT GetDrawCount() { return drawCount; }

	UINT AddTransform();
	Transform* GetTransform(UINT index);

	void UpdateTransform();

	void PlayNextClip(UINT instance, UINT clip, float tweenTime = 0.18f);
private:
	void CreateAnimTransform(UINT index);

private:
	Model* model; //외부에서 모델 받아와서 렌더
	UINT drawCount;
	Transform* transforms[MAX_MODEL_INSTANCE];
	D3DXMATRIX worlds[MAX_MODEL_INSTANCE];
	VertexBuffer* instanceBuffer;

private:
		struct BoneTransform
		{
			Matrix** Transform; //힙 용량이 넘어서면 오버플로우가 일어나므로 동적 할당을 할거임

			BoneTransform()
			{
				Transform = new Matrix*[MAX_MODEL_KEYFRAMES];

				for (UINT i = 0; i < MAX_MODEL_KEYFRAMES; i++)
					Transform[i] = new Matrix[MAX_MODEL_TRANSFORMS]; //여기까지 해야.. 2차원 배열 동적할당..
			}
			

			~BoneTransform()
			{
				for (UINT i = 0; i < MAX_MODEL_KEYFRAMES; i++)
					SafeDeleteArray(Transform[i]);

				SafeDeleteArray(Transform);
			}
		};

	BoneTransform* boneTransforms;
	ID3D11Texture2D* texture;
	ID3D11ShaderResourceView* srv;

	vector<int> saveDeleteIndexes;
	
private:
	struct KeyframeDesc
	{
		int Clip = 0;

		UINT CurrFrame = 0;
		UINT NextFrame = 0;

		float Time = 0.0f;
		float RunningTime = 0.0f;

		float Padding0;
		float Padding1;
		float Padding2;
	};

	struct TweenDesc
	{
		float TakeTweenTime = 1.0f; //얼마만큼의 시간동안 변화될것이냐
		float TweenTime = 0.0f;
		float Padding[2];

		KeyframeDesc Curr;
		KeyframeDesc Next;

		TweenDesc()
		{
			Curr.Clip = 0;
			Next.Clip = -1;
		}
	};

	TweenDesc tweens[MAX_MODEL_KEYFRAMES]; // 각각이 다른 프레임으로 플레이 되어야 하므로
	ConstantBuffer* frameBuffer;

	float speed[2000];
//private:
//	struct InstanceDesc
//	{
//		UINT index;
//		Transform* Transform;
//	};
//	vector<InstanceDesc> updateTransforms;
};