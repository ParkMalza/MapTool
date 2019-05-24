#include "stdafx.h"
#include "CommandManager.h"

CommandManager::CommandManager()
{
}

CommandManager::~CommandManager()
{
}

void CommandManager::SetRedoCommand(StackType type, Command * command)
{
	if (RedoStack.empty())
		return;

	RedoStack.push_back(command);
	RedoType.push_back(type);
}

void CommandManager::SetUndoCommand(StackType type, Command * command)
{
	UndoStack.push_back(command);
	UndoType.push_back(type);
	// UndoStack에 하나라도 다시 쌓이면 RedoStack은
	// 비워져서 다시 이전 상태로 돌아갈 수 없다.
	if (RedoStack.size() > 0)
	{
		RedoStack.clear();
		RedoType.clear();
	}
}

void CommandManager::SetRedo2Command(StackType type, Command * command)
{
	if (Redo2Stack.empty())
		return;

	Redo2Stack.push_back(command);
	Redo2Type.push_back(type);
}

void CommandManager::SetUndo2Command(StackType type, Command * command)
{
	Undo2Stack.push_back(command);
	Undo2Type.push_back(type);
	// UndoStack에 하나라도 다시 쌓이면 RedoStack은
	// 비워져서 다시 이전 상태로 돌아갈 수 없다.
	if (Redo2Stack.size() > 0)
	{
		Redo2Stack.clear();
		Redo2Type.clear();
	}
}

void CommandManager::Undo()  // c + z
{
	if (UndoStack.empty())
		return;

	it = UndoStack.end() - 1;  //undo스택의 맨위값
	command = *it;
	command->Undo();  //쌓인거 확인해서 해당 자식의 undo 실행
	UndoStack.erase(it);  //

	RedoStack.push_back(command);

	typeIt = UndoType.end() - 1;
	stackType = *typeIt;
	UndoType.erase(typeIt);

	RedoType.push_back(stackType);
}

void CommandManager::Redo() // c + y
{
	if (RedoStack.empty())
		return;

	it = RedoStack.end() - 1;
	command = *it;
	command->Redo();  //쌓인거 확인해서 해당 자식의 redo 실행
	RedoStack.erase(it);

	UndoStack.push_back(command);

	typeIt = RedoType.end() - 1;
	stackType = *typeIt;
	RedoType.erase(typeIt);

	UndoType.push_back(stackType);
}

void CommandManager::Undo2()
{
	if (Undo2Stack.empty())
		return;

	it = Undo2Stack.end() - 1;  //undo스택의 맨위값
	command = *it;
	//command->Undo2();  //쌓인거 확인해서 해당 자식의 undo 실행
	Undo2Stack.erase(it);  //

	Redo2Stack.push_back(command);

	typeIt = Undo2Type.end() - 1;
	stackType = *typeIt;
	Undo2Type.erase(typeIt);

	Redo2Type.push_back(stackType);

}

void CommandManager::Redo2()
{
	if (Redo2Stack.empty())
		return;

	it = Redo2Stack.end() - 1;
	command = *it;
	//command->Redo2();  //쌓인거 확인해서 해당 자식의 redo 실행
	Redo2Stack.erase(it);

	Undo2Stack.push_back(command);

	typeIt = Redo2Type.end() - 1;
	stackType = *typeIt;
	Redo2Type.erase(typeIt);

	Undo2Type.push_back(stackType);
}

StackType CommandManager::GetUndoType()
{
	typeIt = UndoType.end() - 1;

	return *typeIt;
}

StackType CommandManager::GetRedoType()
{
	typeIt = RedoType.end() - 1;

	return *typeIt;
}

