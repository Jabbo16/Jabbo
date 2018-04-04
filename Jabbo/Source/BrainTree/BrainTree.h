// BrainTree - C++ behavior tree single header library
// Copyright 2017 Pär Arvidsson. All rights reserved.
// Licensed under the MIT license (https://github.com/arvidsson/BrainTree/blob/master/LICENSE).

#pragma once

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

namespace BrainTree
{
	class Node
	{
	public:
		enum class Status
		{
			Invalid,
			Success,
			Failure,
			Running,
		};

		virtual ~Node() {}

		virtual Status update() = 0;
		virtual void initialize() {}
		virtual void terminate(Status s) {}

		Status tick()
		{
			if (status != Status::Running) {
				initialize();
			}

			status = update();

			if (status != Status::Running) {
				terminate(status);
			}

			return status;
		}

		bool isSuccess() const { return status == Status::Success; }
		bool isFailure() const { return status == Status::Failure; }
		bool isRunning() const { return status == Status::Running; }
		bool isTerminated() const { return isSuccess() || isFailure(); }

		void reset() { status = Status::Invalid; }

		using Ptr = std::shared_ptr<Node>;

	protected:
		Status status = Status::Invalid;
	};

	class Blackboard
	{
	public:
		void setBool(std::string key, bool value) { bools[key] = value; }
		bool getBool(std::string key)
		{
			if (bools.find(key) == bools.end()) {
				bools[key] = false;
			}
			return bools[key];
		}
		bool hasBool(std::string key) const { return bools.find(key) != bools.end(); }

		void setInt(std::string key, int value) { ints[key] = value; }
		int getInt(std::string key)
		{
			if (ints.find(key) == ints.end()) {
				ints[key] = 0;
			}
			return ints[key];
		}
		bool hasInt(std::string key) const { return ints.find(key) != ints.end(); }

		void setFloat(std::string key, float value) { floats[key] = value; }
		float getFloat(std::string key)
		{
			if (floats.find(key) == floats.end()) {
				floats[key] = 0.0f;
			}
			return floats[key];
		}
		bool hasFloat(std::string key) const { return floats.find(key) != floats.end(); }

		void setDouble(std::string key, double value) { doubles[key] = value; }
		double getDouble(std::string key)
		{
			if (doubles.find(key) == doubles.end()) {
				doubles[key] = 0.0f;
			}
			return doubles[key];
		}
		bool hasDouble(std::string key) const { return doubles.find(key) != doubles.end(); }

		void setString(std::string key, std::string value) { strings[key] = value; }
		std::string getString(std::string key)
		{
			if (strings.find(key) == strings.end()) {
				strings[key] = "";
			}
			return strings[key];
		}
		bool hasString(std::string key) const { return strings.find(key) != strings.end(); }

		using Ptr = std::shared_ptr<Blackboard>;

	protected:
		std::unordered_map<std::string, bool> bools;
		std::unordered_map<std::string, int> ints;
		std::unordered_map<std::string, float> floats;
		std::unordered_map<std::string, double> doubles;
		std::unordered_map<std::string, std::string> strings;
	};

	class Leaf : public Node
	{
	public:
		Leaf() {}
		virtual ~Leaf() {}
		Leaf(Blackboard::Ptr blackboard) : blackboard(blackboard) {}

		virtual Status update() = 0;

	protected:
		Blackboard::Ptr blackboard;
	};

	class Composite : public Node
	{
	public:
		virtual ~Composite() {}

		void addChild(Node::Ptr child) { children.push_back(child); }
		bool hasChildren() const { return !children.empty(); }
		int getIndex() const { return index; }

	protected:
		std::vector<Node::Ptr> children;
		int index = 0;
	};

	class Decorator : public Node
	{
	public:
		virtual ~Decorator() {}

		void setChild(Node::Ptr node) { child = node; }
		bool hasChild() const { return child != nullptr; }

	protected:
		Node::Ptr child = nullptr;
	};

	class BehaviorTree : public Node
	{
	public:
		BehaviorTree() : blackboard(std::make_shared<Blackboard>()) {}
		BehaviorTree(const Node::Ptr &rootNode) : BehaviorTree() { root = rootNode; }
		BehaviorTree(const Blackboard::Ptr &shared) : BehaviorTree() { sharedBlackboard = shared; }

		Status update() { return root->tick(); }

		void setRoot(const Node::Ptr &node) { root = node; }
		Blackboard::Ptr getBlackboard() const { return blackboard; }
		Blackboard::Ptr getSharedBlackboard() const { return sharedBlackboard; }
		void setSharedBlackboard(const Blackboard::Ptr &shared) { sharedBlackboard = shared; }

	private:
		Node::Ptr root = nullptr;
		Blackboard::Ptr blackboard = nullptr;
		Blackboard::Ptr sharedBlackboard = nullptr;
	};

	// The Selector composite ticks each child node in order, and remembers what child it prevously tried to tick.
	// If a child succeeds or runs, the sequence returns the same status.
	// In the next tick, it will try to run each child in order again.
	// If all children fails, only then does the selector fail.
	class MemSelector : public Composite
	{
	public:
		Status update() override
		{
			if (!hasChildren()) {
				return Status::Success;
			}

			// Keep going until a child behavior says it's running.
			while (1) {
				auto &child = children.at(index);
				auto status = child->tick();

				// If the child succeeds, or keeps running, do the same.
				if (status != Status::Failure) {
					return status;
				}

				// Hit the end of the array, it didn't end well...
				if (++index == children.size()) {
					index = 0;
					return Status::Failure;
				}
			}
		}
	};

	// The MemSequence composite ticks each child node in order, and remembers what child it prevously tried to tick.
	// If a child fails or runs, the sequence returns the same status.
	// In the next tick, it will try to run each child in order again.
	// If all children succeeds, only then does the sequence succeed.
	class MemSequence : public Composite
	{
	public:
		Status update() override
		{
			if (!hasChildren()) {
				return Status::Success;
			}

			// Keep going until a child behavior says it's running.
			while (1) {
				auto &child = children.at(index);
				auto status = child->tick();

				// If the child fails, or keeps running, do the same.
				if (status != Status::Success) {
					return status;
				}

				// Hit the end of the array, job done!
				if (++index == children.size()) {
					index = 0;
					return Status::Success;
				}
			}
		}
	};

	class ParallelSequence : public Composite
	{
	public:
		ParallelSequence(bool successOnAll = true, bool failOnAll = true) : useSuccessFailPolicy(true), successOnAll(successOnAll), failOnAll(failOnAll) {}
		ParallelSequence(int minSuccess, int minFail) : minSuccess(minSuccess), minFail(minFail) {}

		Status update() override
		{
			int minimumSuccess = minSuccess;
			int minimumFail = minFail;

			if (useSuccessFailPolicy) {
				if (successOnAll) {
					minimumSuccess = children.size();
				}
				else {
					minimumSuccess = 1;
				}

				if (failOnAll) {
					minimumFail = children.size();
				}
				else {
					minimumFail = 1;
				}
			}

			int total_success = 0;
			int total_fail = 0;

			for (auto &child : children) {
				auto status = child->tick();
				if (status == Status::Success) {
					total_success++;
				}
				if (status == Status::Failure) {
					total_fail++;
				}
			}

			if (total_success >= minimumSuccess) {
				return Status::Success;
			}
			if (total_fail >= minimumFail) {
				return Status::Failure;
			}

			return Status::Running;
		}

	private:
		bool useSuccessFailPolicy = false;
		bool successOnAll = true;
		bool failOnAll = true;
		int minSuccess = 0;
		int minFail = 0;
	};

	// The Selector composite ticks each child node in order.
	// If a child succeeds or runs, the sequence returns the same status.
	// In the next tick, it will try to run each child in order again.
	// If all children fails, only then does the selector fail.
	class Selector : public Composite
	{
	public:
		void initialize() override
		{
			index = 0;
		}

		Status update() override
		{
			if (!hasChildren()) {
				return Status::Success;
			}

			// Keep going until a child behavior says it's running.
			while (1) {
				auto &child = children.at(index);
				auto status = child->tick();

				// If the child succeeds, or keeps running, do the same.
				if (status != Status::Failure) {
					return status;
				}

				// Hit the end of the array, it didn't end well...
				if (++index == children.size()) {
					return Status::Failure;
				}
			}
		}
	};

	// The Sequence composite ticks each child node in order.
	// If a child fails or runs, the sequence returns the same status.
	// In the next tick, it will try to run each child in order again.
	// If all children succeeds, only then does the sequence succeed.
	class Sequence : public Composite
	{
	public:
		void initialize() override
		{
			index = 0;
		}

		Status update() override
		{
			if (!hasChildren()) {
				return Status::Success;
			}

			// Keep going until a child behavior says it's running.
			while (1) {
				auto &child = children.at(index);
				auto status = child->tick();

				// If the child fails, or keeps running, do the same.
				if (status != Status::Success) {
					return status;
				}

				// Hit the end of the array, job done!
				if (++index == children.size()) {
					return Status::Success;
				}
			}
		}
	};

	// The Failer decorator returns failure, regardless of what happens to the child.
	class Failer : public Decorator
	{
	public:
		Status update() override
		{
			child->tick();
			return Status::Failure;
		}
	};

	// The Inverter decorator inverts the child node's status, i.e. failure becomes success and success becomes failure.
	// If the child runs, the Inverter returns the status that it is running too.
	class Inverter : public Decorator
	{
	public:
		Status update() override
		{
			auto s = child->tick();

			if (s == Status::Success) {
				return Status::Failure;
			}
			else if (s == Status::Failure) {
				return Status::Success;
			}

			return s;
		}
	};

	// The Repeater decorator repeats infinitely or to a limit until the child returns success.
	class Repeater : public Decorator
	{
	public:
		Repeater(int limit = 0) : limit(limit) {}

		void initialize() override
		{
			counter = 0;
		}

		Status update() override
		{
			while (1) {
				auto s = child->tick();

				if (s == Status::Running) {
					return Status::Running;
				}

				if (s == Status::Failure) {
					return Status::Failure;
				}

				if (limit > 0 && ++counter == limit) {
					return Status::Success;
				}

				child->reset();
			}
		}

	protected:
		int limit;
		int counter = 0;
	};

	// The Succeeder decorator returns success, regardless of what happens to the child.
	class Succeeder : public Decorator
	{
	public:
		Status update() override
		{
			child->tick();
			return Status::Success;
		}
	};

	// The UntilFail decorator repeats until the child returns fail and then returns success.
	class UntilFail : public Decorator
	{
	public:
		Status update() override
		{
			while (1) {
				auto status = child->tick();

				if (status == Status::Failure) {
					return Status::Success;
				}
			}
		}
	};

	// The UntilSuccess decorator repeats until the child returns success and then returns success.
	class UntilSuccess : public Decorator
	{
	public:
		Status update() override
		{
			while (1) {
				auto status = child->tick();

				if (status == Status::Success) {
					return Status::Success;
				}
			}
		}
	};
} // namespace BrainTree
