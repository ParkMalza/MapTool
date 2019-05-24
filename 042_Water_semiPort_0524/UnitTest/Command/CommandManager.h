#pragma once

#include "Singleton/Singleton.h"

class Command;
class CommandManager : public SingletonBase<CommandManager>
{
public:
	CommandManager();
	~CommandManager();

	void SetRedoCommand(StackType type, Command* command);
	void SetUndoCommand(StackType type, Command* command);

	void SetRedo2Command(StackType type, Command* command);
	void SetUndo2Command(StackType type, Command* command);

	void Undo();
	void Redo();

	void Undo2();
	void Redo2();


	StackType GetUndoType();
	StackType GetRedoType();

	StackType GetUndo2Type();
	StackType GetRedo2Type();

private:
	vector<Command*> UndoStack;
	vector<Command*> RedoStack;
	vector<Command*> Undo2Stack;
	vector<Command*> Redo2Stack;
	vector<Command*>::iterator it;

	vector<StackType> UndoType;
	vector<StackType> RedoType;
	vector<StackType> Undo2Type;
	vector<StackType> Redo2Type;
	vector<StackType>::iterator typeIt;

	Command* command;
	StackType stackType;
};