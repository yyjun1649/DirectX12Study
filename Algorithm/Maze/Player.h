#pragma once

class Board;

class Player
{
public:
	void		Init(Board* board);
	void		Update(uint64 deltaTick);

	void		SetPosition(Pos pos) { _pos = pos; }
	Pos			GetPos() { return _pos; }

private:
	Pos			_pos = {};
	int32		_dir = DIR_UP;
	Board*		_board = nullptr;
};

