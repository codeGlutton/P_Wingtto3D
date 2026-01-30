#pragma once

struct Transform
{
//	GEN
//
//public:
//	Transform();
//	~Transform();
//
//public:
//	virtual void Awake() override;
//	virtual void Update() override;
//
//public:
//	Vec3 GetLocalScale() const { return _localScale; }
//	void SetLocalScale(const Vec3& localScale) { _localScale = localScale; UpdateTransform(); }
//	Vec3 GetLocalRotation() const { return _localRotation; }
//	void SetLocalRotation(const Vec3& localRotation) { _localRotation = localRotation; UpdateTransform(); }
//	Vec3 GetLocalPosition() const { return _localPosition; }
//	void SetLocalPosition(const Vec3& localPosition) { _localPosition = localPosition; UpdateTransform(); }
//
//public:
//	Vec3 GetWorldScale() const { return _worldScale; }
//	void SetWorldScale(const Vec3& scale);
//	Vec3 GetWorldRotation() const { return _worldRotation; }
//	void SetWorldRotation(const Vec3& rotation);
//	Vec3 GetWorldPosition() const { return _worldPosition; }
//	void SetWorldPosition(const Vec3& position);
//
//public:
//	Vec3 GetRight() { return _matWorld.Right(); }
//	Vec3 GetUp() { return _matWorld.Up(); }
//	Vec3 GetForward() { return _matWorld.Backward(); }
//
//public:
//	Matrix GetWorldMatrix() const { return _matWorld; }
//
//public:
//	bool HasParent() const { return _parent != nullptr; }
//	std::shared_ptr<Transform> GetParent() { return _parent; }
//	void SetParent(std::shared_ptr<Transform> parent) { _parent = parent; }
//
//	const std::vector<std::shared_ptr<Transform>>& GetChildren() const { return _children; }
//	void AddChild(std::shared_ptr<Transform> child) { _children.push_back(child); }
//
//public:
//	static Vec3 ToEulerAngles(Quat quat);
//
//protected:
//	/**
//	 * 로컬 움직임 변화를 캐싱하여 저장하는 함수
//	 */
//	void UpdateTransform();
//
//private:
//	Vec3 _localPosition = { 0.f, 0.f, 0.f };
//	Vec3 _localRotation = { 0.f, 0.f, 0.f };
//	Vec3 _localScale = { 1.f, 1.f, 1.f };
//
//	// 부모 좌표계로의 SRT
//	Matrix _matLocal = Matrix::Identity;
//	// 월드 좌표계로의 SRT
//	Matrix _matWorld = Matrix::Identity;
//
//	Vec3 _worldPosition;
//	Vec3 _worldRotation;
//	Vec3 _worldScale;
//
//private:
//	std::shared_ptr<Transform> _parent;
//	std::vector<std::shared_ptr<Transform>> _children;
};



